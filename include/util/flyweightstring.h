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

    explicit FlyweightString(const std::string& str);

    explicit FlyweightString(std::string&& str);

    const std::string& get() const { return *value; }

    // implicit conversion to std::string
    operator const std::string&() const { return *value; }

    bool operator==(const FlyweightString& other) const { return value == other.value; }
    bool operator!=(const FlyweightString& other) const { return value != other.value; }
    bool operator<(const FlyweightString& other) const { return value < other.value; }

    std::ostream& operator<<(std::ostream& os) const { return os << *value; }

   private:
    // get or add a string to the pool
    static std::shared_ptr<std::string> getOrAdd(const std::string& str);

    static std::shared_ptr<std::string> getOrAdd(std::string&& str);

    // string pool
    inline static std::unordered_map<std::string, 
                                     std::shared_ptr<std::string>, StringHash> pool;
    // mutex for pool
    inline static std::mutex mutex;

    // shared pointer to the string in the pool
    std::shared_ptr<std::string> value;
};

#endif  // _FLYWEIGHTSTRING_HPP