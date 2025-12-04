# Объявляем библиотеку YandexScheduleManager
add_library(YandexScheduleManager
    YandexScheduleManager.hpp
    YandexScheduleManager.cpp
)

target_include_directories(YandexScheduleManager PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}  
)

target_link_libraries(YandexScheduleManager PRIVATE
    cpr::cpr              
    RouteClasses          
    HttpErrorHandler      
)
