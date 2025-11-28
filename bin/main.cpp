#include <RouteCache.hpp>

#include <iostream>
#include <fstream>

using json = nlohmann::json;

auto print_string = [](std::string s){std::cout << s << std::endl;};

template <typename T>
auto print = [](T s){std::cout << s << std::endl;};



struct Station {
    double latitude;
    double longitude;
    std::vector<std::string> codes;
};

std::ostream& operator<<(std::ostream& os, const Station& s) {
    return os << "Station " << s.latitude << " " << s.longitude;
}


json GetJsonFromUrl(std::string url) {
    cpr::Response r = cpr::Get(cpr::Url{url});
    if (r.status_code != 200) {
        print_string("UNSUCCESS!");
        return json();
    }
    print_string("SUCCESS!");
    return json::parse(r.text);
}

struct SettlementInfo {
    std::string name;
    std::string code;
    std::vector<Station> stations;
    SettlementInfo(std::string name) : name(name) {}
};

int main(int argc, char *argv[]) {
    SettlementInfo departure(argv[1]);
    SettlementInfo arrival(argv[2]);
    std::string departure_date = argv[3];

    std::ofstream json_file("data.json"); 
    json all_stations = GetJsonFromUrl(
        "https://api.rasp.yandex-net.ru/v3.0/stations_list/?apikey=dbfeef6a-2419-43b8-83fd-66d8e630dbbd&lang=ru_RU&format=json"
    );


    for (int i = 0; i < all_stations["countries"].size(); ++i) {
        for (int j = 0; j < all_stations["countries"][i]["regions"].size(); ++j) {
            for (int k = 0; k < all_stations["countries"][i]["regions"][j]["settlements"].size(); ++k) {
                auto current_settlement = all_stations["countries"][i]["regions"][j]["settlements"][k];
                for (int l = 0; l < current_settlement["stations"].size(); ++l) { 
                    auto current_station = all_stations["countries"][i]["regions"][j]["settlements"][k]["stations"][l];

                    if (!current_settlement.contains("title") || !current_settlement["title"].is_string())
                        continue;
                    if (!current_station.contains("latitude") || !current_station["latitude"].is_number())
                        continue;
                    if (!current_station.contains("longitude") || !current_station["longitude"].is_number())
                        continue;
                    
                    auto push_station = [&](const std::string& name, std::vector<Station>& stations){
                        if (name == current_settlement["title"])
                            stations.push_back(Station{
                            .latitude = current_station["latitude"], 
                            .longitude = current_station["longitude"]
                        });
                    };
                    push_station(departure.name, departure.stations);
                    push_station(arrival.name, arrival.stations);
                }
            }
        }
    }

    json_file << all_stations.dump(4);
    return 0;
}



