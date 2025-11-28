add_executable(main
    main.cpp
)

find_package(cpr REQUIRED)

target_link_libraries(main PRIVATE cpr::cpr
    s
)