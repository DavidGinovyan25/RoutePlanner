#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include <cstdint>
#include <list>
#include <string>
#include <unordered_map>

using json = nlohmann::json;
using CachePair = std::pair<std::string, json>;
using CacheList = std::list<CachePair>; 
using CacheMap = std::unordered_map<std::string, CacheList::iterator>;

class ParseJson {
public:
    std::string getKey();
};

class RouteCache {
public:
    int32_t capacity;
    CacheList list;
    CacheMap map;
    std::ofstream file_cach;
    //file_cache.open("RouteCache.json");
    RouteCache(int32_t capacity) : capacity(capacity) {};
    int size() const;

    void AddRequest();  //LRU
    void DeleteRequest();   //LRU

    //cache miss
    //cache hit
    // https://habr.com/ru/articles/874546/
    //read cache
    //write cache
};

void RouteCache::AddRequest() {

}

void RouteCache::DeleteRequest() {

}

int RouteCache::size() const {
    return list.size();
}


