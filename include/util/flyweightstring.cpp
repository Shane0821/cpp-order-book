#include "flyweightstring.h"

FlyweightString::FlyweightString(const std::string& str) { value = getOrAdd(str); }

FlyweightString::FlyweightString(std::string&& str) { value = getOrAdd(std::move(str)); }

std::shared_ptr<std::string> FlyweightString::getOrAdd(const std::string& str) {
    std::lock_guard<std::mutex> lock(mutex);
    if (auto it = pool.find(str); it != pool.end()) {
        return it->second;
    }
    return pool.emplace(str, std::make_shared<std::string>(str)).first->second;
}

std::shared_ptr<std::string> FlyweightString::getOrAdd(std::string&& str) {
    std::lock_guard<std::mutex> lock(mutex);
    if (auto it = pool.find(str); it != pool.end()) {
        return it->second;
    }
    return pool.emplace(str, std::make_shared<std::string>(std::move(str))).first->second;
}