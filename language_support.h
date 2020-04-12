#ifndef LANGUAGE_SUPPORT_H
#define LANGUAGE_SUPPORT_H

#include <string>
#include <unordered_map>

namespace weather_bot
{

enum class Language
{
    ENGLISH,
    RUSSIAN,
};

struct StringConstants
{
    struct Language
    {
        std::string choose;
        std::string choose_success;
        std::string change;
        std::string change_success;
    } language;

    struct City
    {
        std::string choose;
        std::string choose_success;
        std::string change;
        std::string change_success;
        std::string no_cities;
        std::string multiple_cities;
        std::string country;
        std::string state;
    } city;

    std::string weather_get;
    std::string back_button;

    std::string internal_error;
    std::string unknown_command;
    std::string out_of_range;
    std::string cancel_changes;

    struct Weather
    {
        std::string weather;
        std::string temperature;
        std::string from;
        std::string to;
        std::string average;
        std::string feels_like;
        std::string pressure;
        std::string mm_hg;
        std::string humidity;
        std::string cloudiness;
        struct Wind
        {
            std::string wind;
            std::string north;
            std::string northeast;
            std::string east;
            std::string southeast;
            std::string south;
            std::string southwest;
            std::string west;
            std::string northwest;
            std::string meters_per_second;
        } wind;
    } weather;
};

inline const std::unordered_map<Language, std::string> language_to_string =
{
    { Language::ENGLISH, "English" },
    { Language::RUSSIAN, "Русский" },
};

inline const std::unordered_map<Language, std::string> language_aliases =
{
    { Language::ENGLISH, "en" },
    { Language::RUSSIAN, "ru" },
};

inline const std::unordered_map<std::string, Language> string_to_language =
{
    { "English", Language::ENGLISH },
    { "Русский", Language::RUSSIAN },
};

inline const std::unordered_map<Language, StringConstants> language_pack =
{
    {
        Language::ENGLISH,
        {
            .language.choose = "Please choose your language:",
            .language.choose_success = "English language chosen successfully.",
            .language.change = "Change language",
            .language.change_success = "Language changed to English successfully.",

            .city.choose = "Please choose your city from the list below or type its name manually:",
            .city.choose_success = "City chosen: ",
            .city.change = "Change city",
            .city.change_success = "City changed to: ",
            .city.no_cities = "No such cities found.",
            .city.multiple_cities = "Multiple cities with this name found. Please specify, which one you mean (send the corresponding number).",
            .city.country = "country",
            .city.state = "state",

            .weather_get = "Get weather!",
            .back_button = "Back",

            .internal_error = "An internal error occurred, please try again later.",
            .unknown_command = "Unknown command, please try again.",
            .out_of_range = "The given number is ot of valid range. Please try again.",
            .cancel_changes = "Changes were cancelled.",

            .weather.weather = "Weather",
            .weather.temperature = "Temperature",
            .weather.from = "from",
            .weather.to = "to",
            .weather.average = "average",
            .weather.feels_like = "feels like",
            .weather.pressure = "Atmospheric pressure",
            .weather.mm_hg = "mm Hg",
            .weather.humidity = "Humidity",
            .weather.cloudiness = "Cloudiness",
            .weather.wind.wind = "Wind",
            .weather.wind.north = "N",
            .weather.wind.northeast = "NE",
            .weather.wind.east = "E",
            .weather.wind.southeast = "SE",
            .weather.wind.south = "S",
            .weather.wind.southwest = "SW",
            .weather.wind.west = "W",
            .weather.wind.northwest = "NW",
            .weather.wind.meters_per_second = "m/s",
        }
    },
    {
        Language::RUSSIAN,
        {
            .language.choose = "Пожалуйста, выберете ваш язык:",
            .language.choose_success = "Русский язык успешно выбран.",
            .language.change = "Сменить язык",
            .language.change_success = "Язык успешно изменен на русский.",


            .city.choose = "Пожалуйста, выберите ваш город из списка или введите его вручную:",
            .city.choose_success = "Выбран город: ",
            .city.change = "Сменить город",
            .city.change_success = "Город изменен на: ",
            .city.no_cities = "Такого города не найдено.",
            .city.multiple_cities = "Найдено несколько городов с таким именем. Пожалуйста, укажите, какой вы имели в виду (отправьте нужное число).",
            .city.country = "страна",
            .city.state = "штат",

            .weather_get = "Узнать погоду!",
            .back_button = "Назад",

            .internal_error = "Произошла внутренняя ошибка, пожалуйста, попробуйте позднее.",
            .unknown_command = "Неизвестная команда, пожалуйста, попробуйте еще раз.",
            .out_of_range = "Введенное число выходит за границы нужного диапазона. Пожалуйста, попробуйте еще раз.",
            .cancel_changes = "Изменения отменены.",

            .weather.weather = "Погода",
            .weather.temperature = "Температура",
            .weather.from = "от",
            .weather.to = "до",
            .weather.average = "в среднем",
            .weather.feels_like = "ощущается как",
            .weather.pressure = "Атмосферное давление",
            .weather.mm_hg = "мм рт. ст.",
            .weather.humidity = "Влажность",
            .weather.cloudiness = "Облачность",
            .weather.wind.wind = "Ветер",
            .weather.wind.north = "С",
            .weather.wind.northeast = "СВ",
            .weather.wind.east = "В",
            .weather.wind.southeast = "ЮВ",
            .weather.wind.south = "Ю",
            .weather.wind.southwest = "ЮЗ",
            .weather.wind.west = "З",
            .weather.wind.northwest = "СЗ",
            .weather.wind.meters_per_second = "м/с",
        }
    },
};

} // namespace weather_bot

#endif // LANGUAGE_SUPPORT_H
