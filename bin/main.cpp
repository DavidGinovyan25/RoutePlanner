#include "YandexScheduleManager.hpp"

class Runner {
public:
    void Run(int argc, char* argv[]);
};

void Runner::Run(int argc, char* argv[]) {
    RoutePoint departure(argv[1]);
    RoutePoint arrival(argv[2]);
    std::string departure_date = argv[3];

    YandexScheduleManager api_requester;

    api_requester.cache_manager.LoadFromFileSystem();
    api_requester.FindPointsCode(departure, arrival);
    api_requester.SearchRoutes(departure, arrival, departure_date);
    api_requester.cache_manager.SaveInFileSystem();
}

int main(int argc, char *argv[]) {
    Runner runner;
    runner.Run(argc, argv);
    return 0;
}


    // std::string url1 =
    //     "https://api.rasp.yandex-net.ru/v3.0/stations_list/"
    //     "?apikey=dbfeef6a-2419-43b8-83fd-66d8e630dbbd"
    //     "&lang=ru_RU"
    //     "&format=json";
    // // std::cout << api_requester.cache_manager.storage[url1].first.fixed_time << std::endl;
    // std::string url =
    //     "https://api.rasp.yandex-net.ru/v3.0/search/"
    //     "?apikey=dbfeef6a-2419-43b8-83fd-66d8e630dbbd"
    //     "&from=" + departure.code +
    //     "&to=" + arrival.code +
    //     "&page=1"
    //     "&date=" + departure_date +
    //     "&transfers=true";

    // std::cout << api_requester.cache_manager.order.size() << std::endl;
    // std::cout << api_requester.cache_manager.storage.size() << std::endl;
