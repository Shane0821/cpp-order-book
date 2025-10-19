#ifndef _OBJECTPOOL_H
#define _OBJECTPOOL_H

#include <sys/mman.h>

#include <concepts>
#include <memory>
#include <mutex>
#include <unordered_set>
#include <vector>

#include "singleton.hpp"

template <typename T, size_t BLOCK_SIZE = (1 << 12)>
class ObjectPool : public Singleton<ObjectPool<T, BLOCK_SIZE>> {
    friend class Singleton<ObjectPool<T, BLOCK_SIZE>>;

   public:
    using value_type = T;

    // forbid copy
    ObjectPool(const ObjectPool &) = delete;
    ObjectPool &operator=(const ObjectPool &) = delete;

    // allocate object
    template <typename... Args>
    T *allocate(Args &&...args) {
        std::lock_guard<std::mutex> lock(mtx);

        T *obj = nullptr;
        // if pool is not empty, return the last object
        if (!objList_.empty()) [[likely]] {
            obj = objList_.back();
            objList_.pop_back();
            new (obj) T{std::forward<Args>(args)...};
        } else {
            // if pool is empty, create a new object
            obj = new T{std::forward<Args>(args)...};
        }
        unreturned_.insert(obj);
        return obj;
    }

    // release object
    void deallocate(T *obj) {
        std::lock_guard<std::mutex> lock(mtx);

        auto it = unreturned_.find(obj);
        if (it == unreturned_.end()) [[unlikely]] {
            return;
        }
        unreturned_.erase(it);

        if (isMmapAllocated(obj)) [[likely]] {
            obj->~T();
            objList_.push_back(obj);
            return;
        }
        // allocated by new
        delete obj;
    }

    size_t size() {
        std::lock_guard<std::mutex> lock(mtx);
        return objList_.size();
    }

   protected:
    ObjectPool() {
        std::lock_guard<std::mutex> lock(mtx);
        void *map = mmap(0, sizeof(T) * BLOCK_SIZE, PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (map == MAP_FAILED) {
            throw std::runtime_error("fail to alloc pool buffer");
        }

        buffer_ = reinterpret_cast<T *>(map);
        objList_.reserve(BLOCK_SIZE);
        T *tmp = buffer_;
        for (int i = 0; i < BLOCK_SIZE; i++) {
            objList_.push_back(tmp);
            tmp++;
        }
    }

    ~ObjectPool() {
        std::lock_guard<std::mutex> lock(mtx);
        // handle objs not returned
        for (auto obj : unreturned_) {
            if (isMmapAllocated(obj)) {
                obj->~T();
            } else {
                delete obj;
            }
        }

        // every element in freeeList is not constructed
        // so we can just do unmap
        munmap(buffer_, sizeof(T) * BLOCK_SIZE);
    }

   private:
    bool isMmapAllocated(T *obj) const {
        return (obj >= buffer_ && obj < buffer_ + BLOCK_SIZE);
    }

    std::vector<T *> objList_;
    std::unordered_set<T *> unreturned_;
    T *buffer_;
    std::mutex mtx;
};

#endif  // _OBJECTPOOL_H