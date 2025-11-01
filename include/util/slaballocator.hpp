#ifndef _SLAB_ALLOCATOR_H
#define _SLAB_ALLOCATOR_H

#include <sys/mman.h>

#include <concepts>
#include <list>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "singleton.hpp"

namespace {
struct MMapAllocator {
    // allocate a heap memory of size
    void *allocate(size_t size) {
        void *map =
            mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (map == MAP_FAILED) {
            return nullptr;
        }
        return map;
    }

    void deallocate(void *map, size_t size) { munmap(map, size); }
};

// TODO: buddy allocator for page

template <typename T, size_t MAX_OBJ_NUM = (1 << 12),
          typename PageAllocator = MMapAllocator>
class Slab {
    std::vector<T *> objs_;
    PageAllocator page_allocator_;
    T *page_;

   public:
    Slab() {
        static_assert(MAX_OBJ_NUM > 1);
        objs_.reserve(MAX_OBJ_NUM);
        page_ = (T *)page_allocator_.allocate(MAX_OBJ_NUM * sizeof(T));

        for (int i = 0; i < MAX_OBJ_NUM; i++) {
            objs_.push_back(page_ + i);
        }
    }

    ~Slab() { page_allocator_.deallocate(page_, MAX_OBJ_NUM * sizeof(T)); }

    // get a ptr of unconstructed obj
    T *get_one() {
        T *ret = objs_.back();
        objs_.pop_back();
        return ret;
    }

    // return a destructed obj
    void return_one(T *obj) { objs_.push_back(obj); }

    bool is_free() const { return objs_.size() == MAX_OBJ_NUM; }
    bool is_full() const { return objs_.empty(); }
    size_t get_obj_nr() const { return objs_.size(); }
};

}  // namespace

template <typename T, size_t SLAB_SIZE = (1 << 12)>
class SlabAllocator : public Singleton<SlabAllocator<T, SLAB_SIZE>> {
    friend class Singleton<SlabAllocator<T, SLAB_SIZE>>;

   public:
    using value_type = T;

    // forbid copy
    SlabAllocator(const SlabAllocator &) = delete;
    SlabAllocator &operator=(const SlabAllocator &) = delete;

    // allocate object
    template <typename... Args>
    T *allocate(Args &&...args) {
        std::lock_guard<std::mutex> lock(mtx);

        T *obj = nullptr;
        typename slab_list::iterator it;
        if (!partial_list_.empty()) [[likely]] {
            it = partial_list_.begin();
            obj = it->get_one();
            if (it->is_full()) {
                // transfer from partial to full
                full_list_.splice(full_list_.begin(), partial_list_, it);
            }
        } else {
            if (free_list_.empty()) [[unlikely]] {
                // add a new slab
                free_list_.emplace_back();
            }
            it = free_list_.begin();
            obj = it->get_one();
            // transfer from free to partial
            partial_list_.splice(partial_list_.begin(), free_list_, it);
        }

        new (obj) T{std::forward<Args>(args)...};
        obj_2_lst_map_[obj] = it;
        return obj;
    }

    // release object
    void deallocate(T *obj) {
        std::lock_guard<std::mutex> lock(mtx);

        auto it = obj_2_lst_map_.find(obj);
        if (it == obj_2_lst_map_.end()) [[unlikely]] {
            return;
        }
        auto listIt = it->second;
        obj_2_lst_map_.erase(it);

        obj->~T();

        if (listIt->is_full()) [[unlikely]] {
            // full -> partial
            listIt->return_one(obj);
            partial_list_.splice(partial_list_.begin(), full_list_, listIt);
            return;
        }

        listIt->return_one(obj);
        if (listIt->is_free()) [[unlikely]] {
            // partial -> free
            free_list_.splice(free_list_.begin(), partial_list_, listIt);
        }
    }

    auto partial_list_size() const { return partial_list_.size(); }
    auto free_list_size() const { return free_list_.size(); }
    auto full_list_size() const { return full_list_.size(); }

   protected:
    SlabAllocator() {
        std::lock_guard<std::mutex> lock(mtx);
        free_list_.emplace_back();
    }

    ~SlabAllocator() {
        std::lock_guard<std::mutex> lock(mtx);
        for (auto &[p, _] : obj_2_lst_map_) {
            p->~T();
        }
    }

   private:
    using slab_list = std::list<Slab<T, SLAB_SIZE>>;

    slab_list partial_list_;
    slab_list free_list_;
    slab_list full_list_;  // fully used

    std::unordered_map<T *, typename slab_list::iterator> obj_2_lst_map_;
    std::mutex mtx;
};

#endif  // _SLAB_ALLOCATOR_H