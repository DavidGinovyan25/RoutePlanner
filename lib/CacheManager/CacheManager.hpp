#include <nlohmann/json.hpp>

#include <chrono>
#include <iostream>
#include <unordered_map>
#include <list>
#include <optional>

using json = nlohmann::json;
using Clock = std::chrono::system_clock;

struct CachedRecord {
    json json_answer;
    Clock::time_point fixed_time;
};

using CacheList = std::list<std::string>;
using CacheMap = std::unordered_map<std::string, std::pair<CachedRecord, CacheList::iterator>>;

class CacheManager {
private:
    CachedRecord cache_record;
public:
    int max_size = 0;
    Clock::duration max_age = std::chrono::hours(1);

    static int hit_count;
    static int miss_count;
    std::string upload_file = "YandexScheduleCache.json";
    CacheList order;
    CacheMap storage;

    CacheManager(int max_size)
            : max_size(max_size) {}

    CacheManager(int max_size, Clock::duration max_age)
            : max_size(max_size)
            , max_age(max_age) {}

    int GetHitCount();
    int GetMissCount();

    bool HasCachedResponse(const std::string& enterd_url);
    void CacheResponse(std::string& url, json& json_answer);
    std::optional<json> GetResponce(std::string url);

    void EvictFromFull();
    void ExpireOldest();

    bool IsFull();
    bool IsEmpty();
    void Clear();

    void LoadFromFileSystem();
    void SaveInFileSystem();
};
