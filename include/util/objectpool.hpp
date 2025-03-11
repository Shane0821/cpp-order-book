#ifndef _OBJECTPOOL_H
#define _OBJECTPOOL_H

#include <concepts>
#include <memory>
#include <mutex>
#include <vector>

#include "singleton.hpp"

template <typename T, size_t MIN_SIZE = 32, size_t MAX_SIZE = 65536>
    requires std::default_initializable<T> && std::destructible<T>
class ObjectPool : public Singleton<ObjectPool<T, MIN_SIZE, MAX_SIZE>> {
    friend class Singleton<ObjectPool<T, MIN_SIZE, MAX_SIZE>>;

   public:
    // forbid copy
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;

    // acquire object
    std::unique_ptr<T> acquire() {
        std::lock_guard<std::mutex> lock(mtx);
        // if pool is not empty, return the last object
        if (!pool.empty()) {
            auto obj = std::move(pool.back());
            pool.pop_back();
            return obj;  // xvalue
        }
        // if pool is empty, create a new object
        return std::make_unique<T>();  // prvalue
    }

    // release object
    void release(std::unique_ptr<T> obj) {
        std::lock_guard<std::mutex> lock(mtx);
        if (pool.size() >= MAX_SIZE) {
            // obj is destroyed when out of scope
            return;
        }
        pool.push_back(std::move(obj));
    }

    size_t size() const { return pool.size(); }

   protected:
    ObjectPool(size_t initialSize = MIN_SIZE) {
        initialSize = std::min(std::max(initialSize, MIN_SIZE), MAX_SIZE);
        pool.reserve(initialSize);
        for (size_t i = 0; i < initialSize; ++i) {
            pool.push_back(std::make_unique<T>());
        }
    }

   private:
    std::vector<std::unique_ptr<T>> pool;
    std::mutex mtx;
};

#endif  // _OBJECTPOOL_H