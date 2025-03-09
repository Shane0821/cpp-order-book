#ifndef _FLYWEIGHTSTRING_HPP
#define _FLYWEIGHTSTRING_HPP

#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

class FlyweightString {
   public:
    struct StringHash {
        std::size_t operator()(const std::string& str) const {
            return std::hash<std::string>()(str);
        }
    };

    explicit FlyweightString(const std::string& str) {
        std::lock_guard<std::mutex> lock(mutex);
        value = getOrAdd(str);
    }

    explicit FlyweightString(std::string&& str) {
        std::lock_guard<std::mutex> lock(mutex);
        value = getOrAdd(std::move(str));
    }

    const std::string& get() const { return *value; }

    // implicit conversion to std::string
    operator const std::string&() const { return *value; }

    bool operator==(const FlyweightString& other) const { return value == other.value; }
    bool operator!=(const FlyweightString& other) const { return value != other.value; }
    bool operator<(const FlyweightString& other) const { return value < other.value; }

    std::ostream& operator<<(std::ostream& os) const { return os << *value; }

   private:
    // get or add a string to the pool
    static std::shared_ptr<std::string> getOrAdd(const std::string& str) {
        if (auto it = pool.find(str); it != pool.end()) {
            return it->second;
        }
        std::lock_guard<std::mutex> lock(mutex);
        return pool.emplace(str, std::make_shared<std::string>(str)).first->second;
    }

    static std::shared_ptr<std::string> getOrAdd(std::string&& str) {
        if (auto it = pool.find(str); it != pool.end()) {
            return it->second;
        }
        std::lock_guard<std::mutex> lock(mutex);
        return pool.emplace(str, std::make_shared<std::string>(std::move(str)))
            .first->second;
    }

    // string pool
    static inline std::unordered_map<std::string, std::shared_ptr<std::string>,
                                     StringHash>
        pool;
    // mutex for pool
    static inline std::mutex mutex;

    // shared pointer to the string in the pool
    std::shared_ptr<std::string> value;
};

// initialize static members
std::unordered_map<std::string, std::shared_ptr<std::string>, FlyweightString::StringHash>
    FlyweightString::pool;
std::mutex FlyweightString::mutex;

#endif  // _FLYWEIGHTSTRING_HPP