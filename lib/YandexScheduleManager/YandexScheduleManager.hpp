#include <nlohmann/json.hpp>

#include <expected>

#include "RouteClasses.hpp"
#include "HttpErrorHandler.hpp"

struct RoutePoint {
    std::string name;
    std::string code;
    RoutePoint(std::string name) : name(name) {}
};

using json = nlohmann::json;

class YandexScheduleManager {
public:
    std::expected<json, std::string> GetJsonFromUrl(std::string url);
    void FindPointsCode(RoutePoint& departure, RoutePoint& arrival);
    void SearchRoutes(RoutePoint& departure, RoutePoint& arrival, std::string departure_date);
};

