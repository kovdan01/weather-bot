cmake_minimum_required(VERSION 3.13)

project(weather-smart-bot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
    src/weather_api.cpp
    src/bot.cpp)
set(HEADERS
    src/weather_api.h
    src/bot.h
    src/language_support.h)

add_executable(weather-smart-bot ${SOURCES} ${HEADERS})

set(THIRD_PARTY_DIR ${CMAKE_CURRENT_LIST_DIR}/3rd_party)

set(TgbotCpp_ROOT_DIR ${THIRD_PARTY_DIR}/tgbot-cpp)
set(TgbotCpp_INCLUDE_DIR ${TgbotCpp_ROOT_DIR}/include)
add_subdirectory(${TgbotCpp_ROOT_DIR})

find_package(Boost COMPONENTS COMPONENTS system REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE ${TgbotCpp_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE TgBot)

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::system)

file(COPY config/settings.json DESTINATION ${PROJECT_BINARY_DIR})
file(COPY config/city.list.json DESTINATION ${PROJECT_BINARY_DIR})
