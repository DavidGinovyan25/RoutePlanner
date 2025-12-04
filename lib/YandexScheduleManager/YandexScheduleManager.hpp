#include <nlohmann/json.hpp>

#include <expected>

#include "RouteClasses.hpp"
#include "HttpErrorHandler.hpp"

using json = nlohmann::json;

class YandexScheduleManager {
public:
    std::expected<json, std::string> GetJsonFromUrl(std::string url);
    bool FetchStations(RoutePoint& departure, RoutePoint& arrival);
    bool FilterStationsByDate(RoutePoint& departure, std::string departure_date);
    bool SearchRoutes(RoutePoint& departure, RoutePoint& arrival, std::string departure_date);
};

