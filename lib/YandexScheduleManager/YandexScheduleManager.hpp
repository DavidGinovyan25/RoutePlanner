#include <expected>

#include "CacheManager.hpp"
#include "RouteClasses.hpp"
#include "HttpErrorHandler.hpp"

class YandexScheduleManager {
public:
    CacheManager cache_manager;
    YandexScheduleManager() : cache_manager(2) {}
    std::expected<json, std::string> GetJsonFromUrl(std::string url);
    void FindPointsCode(RoutePoint& departure, RoutePoint& arrival);
    void SearchRoutes(RoutePoint& departure, RoutePoint& arrival, std::string departure_date);
};

