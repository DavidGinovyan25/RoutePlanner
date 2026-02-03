#include <iostream>

#include "YandexScheduleManager.hpp"

std::expected<json, std::string> YandexScheduleManager::GetJsonFromUrl(std::string url) {
    cpr::Response r = cpr::Get(cpr::Url{url});
    HttpErrorHandler http_error;
    if (r.error.code != cpr::ErrorCode::OK)
        return std::unexpected(http_error.HandleNetworkErrors(r));
    if (r.status_code != static_cast<int>(HttpErrorHandler::StatusCode::OK))
        return std::unexpected(http_error.HandleStatusCodeErrors(r));
    try {
        return json::parse(r.text);
    } catch (const json::parse_error& e) {
        return std::unexpected(std::string("JSON parse error: ") + e.what());
    }
}

void YandexScheduleManager::FindPointsCode(RoutePoint& departure, RoutePoint& arrival) {
    std::string url =
        "https://api.rasp.yandex-net.ru/v3.0/stations_list/"
        "?apikey=dbfeef6a-2419-43b8-83fd-66d8e630dbbd"
        "&lang=ru_RU"
        "&format=json";

    json all_stations;
    bool is_empty_cache = cache_manager.IsEmpty();
    cache_manager.EvictFromFull();
    if (cache_manager.HasCachedResponse(url)) {
        all_stations = cache_manager.GetResponce(url).value();
    } else {
        auto result = GetJsonFromUrl(url);
        if (!result) {
            std::cerr << "FetchStations: " + result.error() << std::endl;
            return;
        }
        all_stations = *result;
        cache_manager.CacheResponse(url, all_stations);
    }
    cache_manager.ExpireOldest();

    bool departure_flag = false;
    bool arrival_flag = false;

    auto all = (is_empty_cache) ? all_stations["countries"] : all_stations["json_answer"]["countries"];

    for (auto& country : all) {
        for (auto& region : country["regions"]) {
            for (auto& settlement : region["settlements"]) {

                if (!settlement.contains("title") || !settlement["title"].is_string())
                    continue;
                if (!settlement.contains("codes") || settlement["codes"].is_null())
                    continue;
                if (!settlement["codes"].contains("yandex_code") || settlement["codes"]["yandex_code"].is_null())
                    continue;

                auto settlement_title = settlement["title"].get<std::string>();
                auto settlement_code = settlement["codes"]["yandex_code"].get<std::string>();
                auto compare_title = [&](bool& flag, const std::string& name, std::string& code){
                    if (!flag && name == settlement_title) {
                        code = settlement_code;
                        flag = true;}};
                compare_title(departure_flag, departure.name, departure.code);
                compare_title(arrival_flag, arrival.name, arrival.code);
                if (departure_flag && arrival_flag)
                    break;
            }
            if (departure_flag && arrival_flag)
                break;
        }
        if (departure_flag && arrival_flag)
            break;
    }
    if (!departure_flag)
        std::cerr << "Error: Unknown settlement " + departure.name  << std::endl;
    if (!arrival_flag)
        std::cerr << "Error: Unknown settlement " + arrival.name  << std::endl;
}

void YandexScheduleManager::SearchRoutes(
            RoutePoint& departure,
            RoutePoint& arrival,
            std::string departure_date) {

    std::string url =
        "https://api.rasp.yandex-net.ru/v3.0/search/"
        "?apikey=dbfeef6a-2419-43b8-83fd-66d8e630dbbd"
        "&from=" + departure.code +
        "&to=" + arrival.code +
        "&page=1"
        "&date=" + departure_date +
        "&transfers=true";

    json searched_routes;
    cache_manager.EvictFromFull();
    if (cache_manager.HasCachedResponse(url)) {
        searched_routes = cache_manager.GetResponce(url).value();
    } else {
        auto result = GetJsonFromUrl(url);
        if (!result) {
            std::cerr << "SearchRoutes: " + result.error() << std::endl;
            return;
        }
        searched_routes = *result;
        cache_manager.CacheResponse(url, searched_routes);
    }
    cache_manager.ExpireOldest();

    int kMaxCountTransfers = 1;
    int kIndexNormalization = 2;
    for (auto& segments : searched_routes["segments"]) {
        bool has_transfers = segments["has_transfers"];
        if (!has_transfers) {
            std::cout << "================================================================" << std::endl;
            std::cout << segments["from"]["title"] << ": " << segments["departure"] << std::endl;
            std::cout << segments["to"]["title"] << ": " << segments["arrival"] << std::endl;
            std::cout << "================================================================" << std::endl;
            continue;
        }
        if (segments["details"].size() / kIndexNormalization > kMaxCountTransfers)
            continue;
        std::cout << "================================================================" << std::endl;
        std::cout << segments["details"][0]["from"]["title"] << ": " << segments["details"][0]["departure"] << std::endl;
        std::cout << segments["details"][0]["to"]["title"] << ": " << segments["details"][0]["arrival"] << std::endl ;
        std::cout << "----------------------------------------------------------------" << std::endl;
        std::cout << segments["details"][1 * kIndexNormalization]["from"]["title"] << ": "
            << segments["details"][1 * kIndexNormalization]["departure"] << std::endl;
        std::cout << segments["details"][1 * kIndexNormalization]["to"]["title"] << ": "
            << segments["details"][1 * kIndexNormalization]["arrival"] << std::endl ;
        std::cout << "================================================================" << std::endl;
    }
}
