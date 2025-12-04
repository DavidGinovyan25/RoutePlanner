#include "YandexScheduleManager.hpp"

std::expected<json, std::string> YandexScheduleManager::GetJsonFromUrl(std::string url) {
    cpr::Response r = cpr::Get(cpr::Url{url});
    HttpErrorHandler http_error;
    if (r.error.code != cpr::ErrorCode::OK) 
        return std::unexpected(http_error.HandleNetworkErrors(r));
    if (r.status_code != static_cast<int>(http_error.StatusCode::OK))
        return std::unexpected(http_error.HandleStatusCodeErrors(r));
    try {
        return json::parse(r.text);
    } catch (const json::parse_error& http_error) {
        return std::unexpected(std::string("JSON parse error: ") + http_error.what());
    } catch (...) {
        return std::unexpected("Unknown JSON error");
    }
}

bool YandexScheduleManager::FetchStations(RoutePoint& departure, RoutePoint& arrival) {
    auto result = GetJsonFromUrl(
        "https://api.rasp.yandex-net.ru/v3.0/stations_list/"
        "?apikey=dbfeef6a-2419-43b8-83fd-66d8e630dbbd"
        "&lang=ru_RU"
        "&format=json"
    );
    if (!result) {
        std::cerr << result.error() << std::endl;
        return false;
    }
    json all_stations = *result;
    for (auto& country : all_stations["countries"]) {
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

                if (departure.name == settlement_title) 
                    departure.code = settlement_code;
                if (arrival.name == settlement_title) 
                    arrival.code = settlement_code;

                for (auto& station : settlement["stations"]) { 
                    if (!station.contains("latitude") || !station["latitude"].is_number())
                        continue;
                    if (!station.contains("longitude") || !station["longitude"].is_number())
                        continue;
                    if (!station.contains("codes") || station["codes"].is_string())
                        continue;
                    if (!station["codes"].contains("yandex_code") || station["codes"]["yandex_code"].is_null())
                        continue;

                    auto push_station = [&](RoutePoint& point) {
                        if (point.name == settlement_title)
                            point.stations.push_back(Station{
                                .latitude = station["latitude"], 
                                .longitude = station["longitude"],
                                .code = station["codes"]["yandex_code"]});
                            };    
                    push_station(departure);
                    push_station(arrival);
                }
            }
        }
    }
    return true;
}

bool YandexScheduleManager::FilterStationsByDate(RoutePoint& departure, std::string departure_date) {
    for (int i = departure.stations.size() - 1; i >= 0; --i) {
        auto result = GetJsonFromUrl(
            "https://api.rasp.yandex-net.ru/v3.0/schedule/"
            "?apikey=dbfeef6a-2419-43b8-83fd-66d8e630dbbd"
            "&station=" + departure.stations[i].code +
            "&date=" + departure_date +
            "&transfers=true"
        );
        if (!result) {
            std::cerr << result.error() << std::endl;
            return false;
        }
        json route_schedule_request = *result;
        if (route_schedule_request.is_null())
            departure.stations.erase(departure.stations.begin() + i);
    }
    return true;
}


bool YandexScheduleManager::SearchRoutes(
            RoutePoint& departure, 
            RoutePoint& arrival,
            std::string departure_date) {
                
    for (Station& d_s : departure.stations) {
        for (Station& a_s : arrival.stations) {
            auto result  = GetJsonFromUrl(
                "https://api.rasp.yandex-net.ru/v3.0/search/"
                "?apikey=dbfeef6a-2419-43b8-83fd-66d8e630dbbd"
                "&from=" + d_s.code + 
                "&to=" + a_s.code + 
                "&page=1"
                "&date=" + departure_date
            );
            if (!result) {
                std::cerr << result.error() << std::endl;
                return false;
            }
            json route_schedule_request = *result; 
            if (route_schedule_request.is_null())
                continue;
            for (auto& segments : route_schedule_request["segments"]) {
                if (segments["has_transfers"] == true && segments["thread"].size() > 2) //FIX THIS 
                    continue; 
                std::cout << segments["from"]["title"] << " " << segments["to"]["title"];
            }
        }
    }
    return true;
}