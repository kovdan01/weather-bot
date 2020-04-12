#include "bot.h"

#include <utility>
#include <fstream>
#include <cassert>
#include <sstream>

namespace weather_bot
{

Bot::Bot(std::string tg_token, std::string weather_token, std::unique_ptr<nlohmann::json> cities)
    : m_bot(std::move(tg_token))
    , m_weather(std::move(weather_token))
    , m_cities(std::move(cities))
{
    if (m_cities == nullptr)
    {
        m_cities = std::make_unique<nlohmann::json>();
        std::ifstream cities_file("city.list.json");
        cities_file >> *m_cities;
    }

    for (const nlohmann::json& city : *m_cities)
    {
        m_cities_by_id.emplace(city["id"], city);
        m_cities_by_name.emplace(city["name"], city);
        if (city.contains("name_ru"))
            m_cities_by_ru_name.emplace(city["name_ru"], city);
    }

    init_keyboards();

    m_bot.getEvents().onCommand("start", [this](TgBot::Message::Ptr message)
    {
        auto it = m_users.find(message->from->id);
        if (it == m_users.end())
        {
            init_user(message, message->from->id);
        }
        else
        {
            m_bot.getApi().sendMessage(message->chat->id, language_pack.at(it->second.language).unknown_command, false, 0, m_keyboards[it->second.language][it->second.state]);
        }
    });

    m_bot.getEvents().onNonCommandMessage(std::bind(process_message, std::placeholders::_1, this));

    poll_loop();
}

void Bot::init_keyboards()
{
    for (Language language : std::vector<Language>{ Language::ENGLISH, Language::RUSSIAN })
    {
        std::vector<TgBot::KeyboardButton::Ptr> back_row;
        auto back_button = std::make_shared<TgBot::KeyboardButton>();
        back_button->text = language_pack.at(language).back_button;
        back_row.emplace_back(std::move(back_button));
        // LANGUAGE_INIT, LANGUAGE_CHANGE
        {
            auto keyboard_init = std::make_shared<TgBot::ReplyKeyboardMarkup>();
            auto keyboard_change = std::make_shared<TgBot::ReplyKeyboardMarkup>();
            keyboard_init->resizeKeyboard = true;
            keyboard_change->resizeKeyboard = true;

            std::vector<TgBot::KeyboardButton::Ptr> row0;
            auto english_button = std::make_shared<TgBot::KeyboardButton>();
            english_button->text = language_to_string.at(Language::ENGLISH);
            row0.emplace_back(std::move(english_button));
            auto russian_button = std::make_shared<TgBot::KeyboardButton>();
            russian_button->text = language_to_string.at(Language::RUSSIAN);
            row0.emplace_back(std::move(russian_button));
            keyboard_init->keyboard.emplace_back(row0);
            keyboard_change->keyboard.emplace_back(std::move(row0));

            keyboard_change->keyboard.emplace_back(back_row);

            m_keyboards[language][State::LANGUAGE_INIT] = std::move(keyboard_init);
            m_keyboards[language][State::LANGUAGE_CHANGE] = std::move(keyboard_change);
        }
        // CITY_INIT, CITY_CHANGE
        {
            auto keyboard_init = std::make_shared<TgBot::ReplyKeyboardMarkup>();
            auto keyboard_change = std::make_shared<TgBot::ReplyKeyboardMarkup>();
            keyboard_init->resizeKeyboard = true;
            keyboard_change->resizeKeyboard = true;

            std::vector<TgBot::KeyboardButton::Ptr> row0, row1;
            auto first_button = std::make_shared<TgBot::KeyboardButton>();
            auto second_button = std::make_shared<TgBot::KeyboardButton>();
            switch (language)
            {
            case Language::ENGLISH:
                first_button->text = "New York City";
                second_button->text = "Zürich";
                break;
            case Language::RUSSIAN:
                first_button->text = "Москва";
                second_button->text = "Санкт-Петербург";
                break;
            }
            row0.emplace_back(std::move(first_button));
            row1.emplace_back(std::move(second_button));
            keyboard_init->keyboard.emplace_back(row0);
            keyboard_init->keyboard.emplace_back(row1);
            keyboard_change->keyboard.emplace_back(std::move(row0));
            keyboard_change->keyboard.emplace_back(std::move(row1));

            keyboard_change->keyboard.emplace_back(back_row);

            m_keyboards[language][State::CITY_INIT] = std::move(keyboard_init);
            m_keyboards[language][State::CITY_CHANGE] = std::move(keyboard_change);
        }
        // CITY_AMBIDUOUS_INIT, CITY_AMBIDUOUS_CHANGE
        {
            auto keyboard_init = std::make_shared<TgBot::ReplyKeyboardMarkup>();
            auto keyboard_change = std::make_shared<TgBot::ReplyKeyboardMarkup>();
            keyboard_init->resizeKeyboard = true;
            keyboard_change->resizeKeyboard = true;

            keyboard_init->keyboard.emplace_back(back_row);
            keyboard_change->keyboard.emplace_back(back_row);

            m_keyboards[language][State::CITY_AMBIGUOUS_INIT] = std::move(keyboard_init);
            m_keyboards[language][State::CITY_AMBIGUOUS_CHANGE] = std::move(keyboard_change);
        }
        // WEATHER_GET
        {
            auto keyboard = std::make_shared<TgBot::ReplyKeyboardMarkup>();
            keyboard->resizeKeyboard = true;

            std::vector<TgBot::KeyboardButton::Ptr> row0;
            auto weather_get_button = std::make_shared<TgBot::KeyboardButton>();
            weather_get_button->text = language_pack.at(language).weather_get;
            row0.emplace_back(std::move(weather_get_button));
            keyboard->keyboard.emplace_back(std::move(row0));

            std::vector<TgBot::KeyboardButton::Ptr> row1;
            auto city_change_button = std::make_shared<TgBot::KeyboardButton>();
            city_change_button->text = language_pack.at(language).city.change;
            row1.emplace_back(std::move(city_change_button));
            keyboard->keyboard.emplace_back(std::move(row1));

            std::vector<TgBot::KeyboardButton::Ptr> row2;
            auto language_change_button = std::make_shared<TgBot::KeyboardButton>();
            language_change_button->text = language_pack.at(language).language.change;
            row2.emplace_back(std::move(language_change_button));
            keyboard->keyboard.emplace_back(std::move(row2));

            m_keyboards[language][State::WEATHER_GET] = std::move(keyboard);
        }
    }
}

void Bot::init_user(TgBot::Message::Ptr message, std::int32_t user_id)
{
    auto emplace_result = m_users.emplace(user_id, UserData
    {
        .state = State::START,
        .language = Language::ENGLISH,
        .city = nullptr,
    });
    start_command(message, emplace_result.first->second);
}

void Bot::process_message(TgBot::Message::Ptr message, Bot* self)
{
    auto print_error = [self, &message]()
    {
        std::cerr << " while processing message \"" << message->text << "\" from user with id " << message->from->id << ": " << std::endl;
        self->m_bot.getApi().sendMessage(message->chat->id, "An internal error occurred, please try again later", false, 0);
    };
    try
    {
        try
        {
            auto it = self->m_users.find(message->from->id);
            if (it == self->m_users.end())
            {
                self->init_user(message, message->from->id);
                return;
            }
            UserData& user = it->second;
            self->process_message_impl(message, user);
        }
        catch (const TgBot::TgException& e)
        {
            std::cerr << "TgExcept error: \"" << e.what() << "\"";
            throw e;
        }
        catch (const std::exception& e)
        {
            std::cerr << "error: \"" << e.what() << "\"";
            throw e;
        }
        catch (...)
        {
            std::cerr << "unknown error";
            print_error();
        }
    }
    catch (...)
    {
        print_error();
    }
}

void Bot::process_message_impl(TgBot::Message::Ptr message, UserData& user)
{
    switch (user.state)
    {
    case State::START:
        assert(false);
        break;
    case State::LANGUAGE_INIT:
        language_change(message, user, true);
        break;
    case State::CITY_INIT:
        city_change(message, user, true);
        break;
    case State::CITY_AMBIGUOUS_INIT:
        city_ambiguous_change(message, user, true);
        break;
    case State::LANGUAGE_CHANGE:
        language_change(message, user);
        break;
    case State::CITY_CHANGE:
        city_change(message, user);
        break;
    case State::CITY_AMBIGUOUS_CHANGE:
        city_ambiguous_change(message, user);
        break;
    case State::WEATHER_GET:
        weather_get(message, user);
        break;
    }
}

void Bot::poll_loop()
{
    try
    {
        std::cout << "Started poll loop on bot: " << m_bot.getApi().getMe()->username << std::endl;
        TgBot::TgLongPoll longPoll(m_bot);
        for (;;)
        {
            longPoll.start();
        }
    }
    catch (const TgBot::TgException& e)
    {
        std::cerr << "TgExcept error: " << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "error: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "unknown error" << std::endl;
    }
}

void Bot::start_command(TgBot::Message::Ptr message, UserData& user)
{
    assert(user.state == State::START);
    user.state = State::LANGUAGE_INIT;
    m_bot.getApi().sendMessage(message->chat->id, language_pack.at(user.language).language.choose, false, 0, m_keyboards[user.language][State::LANGUAGE_INIT]);
}

void Bot::language_change(TgBot::Message::Ptr message, UserData& user, bool init)
{
    const StringConstants& current_language = language_pack.at(user.language);
    std::string text;
    if (!init && message->text == current_language.back_button)
    {
        text = current_language.cancel_changes;
        user.state = State::WEATHER_GET;
    }
    else
    {
        auto it = string_to_language.find(message->text);
        if (it == string_to_language.end())
        {
            text = current_language.unknown_command;
        }
        else
        {
            user.language = it->second;
            if (init)
            {
                text = language_pack.at(user.language).language.choose_success + "\n" + language_pack.at(user.language).city.choose;
                user.state = State::CITY_INIT;
            }
            else
            {
                text = language_pack.at(user.language).language.change_success;
                user.state = State::WEATHER_GET;
            }
        }
    }
    m_bot.getApi().sendMessage(message->chat->id, text, false, 0, m_keyboards[user.language][user.state]);
}

void Bot::city_change(TgBot::Message::Ptr message, UserData& user, bool init)
{
    const StringConstants& current_language = language_pack.at(user.language);
    std::string text;

    if (!init && message->text == current_language.back_button)
    {
        user.state = State::WEATHER_GET;
        text = current_language.cancel_changes;
    }
    else
    {
        std::vector<decltype(m_cities_by_name)::const_iterator> possible_cities;
        std::vector<const decltype(m_cities_by_name)*> cities_lists = [&user, this]() -> std::vector<const decltype(m_cities_by_name)*>
        {
            switch (user.language)
            {
            case Language::ENGLISH:
                return { &m_cities_by_name, &m_cities_by_ru_name };
            case Language::RUSSIAN:
                return { &m_cities_by_ru_name, &m_cities_by_name };
            }
        }();

        for (const auto& cities_list : cities_lists)
            for (auto [it1, it2] = cities_list->equal_range(message->text); it1 != it2; ++it1)
                possible_cities.emplace_back(it1);

        if (possible_cities.empty())
        {
            text = current_language.city.no_cities + "\n" + current_language.city.choose;
        }
        else if (possible_cities.size() == 1)
        {
            user.city = &possible_cities.front()->second;
            const std::string& city_name = get_city_name(user);
            text = current_language.city.choose_success + city_name;
            user.state = State::WEATHER_GET;
        }
        else
        {
            text = current_language.city.multiple_cities + "\n";
            int number = 1;
            for (const auto& city : possible_cities)
            {
                text += "\n" + std::to_string(number) + ". " + std::string{city->second["name"]}
                        + " (" + current_language.city.country + ": " + std::string{city->second["country"]};
                if (city->second["state"] == "")
                    text += ")";
                else
                    text += ", " + current_language.city.state + ": " + std::string{city->second["state"]} + ")";
                ++number;
            }
            m_possible_cities_by_users[message->from->id] = std::move(possible_cities);
            user.state = State::CITY_AMBIGUOUS_INIT;
        }
    }
    m_bot.getApi().sendMessage(message->chat->id, text, false, 0, m_keyboards[user.language][user.state]);
}

void Bot::city_ambiguous_change(TgBot::Message::Ptr message, UserData& user, bool init)
{
    assert(user.state == State::CITY_AMBIGUOUS_INIT || user.state == State::CITY_AMBIGUOUS_CHANGE);
    auto it = m_possible_cities_by_users.find(message->from->id);
    if (it == m_possible_cities_by_users.end())
    {
        assert(false);
    }
    else
    {
        const StringConstants& current_language = language_pack.at(user.language);
        std::string text;
        if (message->text == current_language.back_button)
        {
            user.state = (init ? State::CITY_INIT : State::CITY_CHANGE);
            text = current_language.city.choose;
        }
        else
        {
            const auto& possible_cities = it->second;
            unsigned long index;
            bool exc_caught = false;
            try
            {
                index = std::stoul(message->text);
            }
            catch (const std::exception&)
            {
                text = "\"" + message->text + "\" is not a valid number. Please try again.";
                exc_caught = true;
            }
            if (!exc_caught)
            {
                if (index < 1 || index > possible_cities.size())
                {
                    text = current_language.out_of_range;
                }
                else
                {
                    user.city = &possible_cities[index - 1]->second;
                    const std::string& city_name = get_city_name(user);
                    text = get_city_change_success_message(user, current_language) + city_name;
                    user.state = State::WEATHER_GET;
                    m_possible_cities_by_users.erase(it);
                }
            }
        }
        m_bot.getApi().sendMessage(message->chat->id, text, false, 0, m_keyboards[user.language][user.state]);
    }
}

void Bot::weather_get(TgBot::Message::Ptr message, UserData& user) const
{
    const StringConstants& current_language = language_pack.at(user.language);
    std::string text;
    if (message->text == current_language.weather_get)
    {
        nlohmann::json json = m_weather.make_request_by_city_id(user.city->at("id"), language_aliases.at(user.language));

        double temperature, temp_min, temp_max, feels_like,
               pressure, wind_speed, wind_degree;
        int humidity, clouds;

        bool have_temp = false, have_temp_range = false, have_feels_like = false,
             have_pressure = false, have_humidity = false, have_wind_speed = false,
             have_wind_deg = false, have_cloudiness = false;

        if (json.contains("main"))
        {
            const auto& json_main = json["main"];
            if (json_main.contains("temp"))
            {
                temperature = json_main["temp"];
                have_temp = true;
            }
            if (json_main.contains("temp_min") && json_main.contains("temp_max"))
            {
                temp_min = json_main["temp_min"];
                temp_max = json_main["temp_max"];
                have_temp_range = true;
            }
            if (json_main.contains("feels_like"))
            {
                feels_like = json_main["feels_like"];
                have_feels_like = true;
            }
            if (json_main.contains("pressure"))
            {
                pressure = json_main["pressure"];
                pressure *= 3.0 / 4.0;
                have_pressure = true;
            }
            if (json_main.contains("humidity"))
            {
                humidity = json_main["humidity"];
                have_humidity = true;
            }
        }

        if (json.contains("wind"))
        {
            const auto& json_wind = json["wind"];
            if (json_wind.contains("speed"))
            {
                wind_speed = json_wind["speed"];
                have_wind_speed = true;
            }
            if (json_wind.contains("deg"))
            {
                wind_degree = json_wind["deg"];
                have_wind_deg = true;
            }
        }

        if (json.contains("clouds") && json["clouds"].contains("all"))
        {
            clouds = json["clouds"]["all"];
            have_cloudiness = true;
        }

        std::ostringstream text_stream;
        const std::string city_name = get_city_name(user);

        text_stream << current_language.weather.weather << " (" << city_name << "): ";
        std::string description;
        if (!json["weather"].empty() && json["weather"][0].contains("description"))
            description = std::string{json["weather"][0]["description"]} +";\n";
        else
            description = "\n";
        text_stream << description;

        if (have_temp_range && temp_max - temp_min >= 1)
        {
            text_stream << current_language.weather.temperature << ": "
                 << current_language.weather.from << " " << temp_min << "°C "
                 << current_language.weather.to << " " << temp_max <<  "°C ";
            if (have_temp)
                text_stream << "(" << current_language.weather.average << " " << temperature << "°C)";
        }
        else if (have_temp)
        {
            text_stream << current_language.weather.temperature << ": " << temperature << "°C";
        }

        if (have_feels_like)
            text_stream << ", " << current_language.weather.feels_like << " " << feels_like << "°C;\n";
        else
            text_stream << ";\n";
        if (have_pressure)
            text_stream << current_language.weather.pressure << ": " << pressure << " " << current_language.weather.mm_hg << ";\n";
        if (have_humidity)
            text_stream << current_language.weather.humidity << ": " << humidity << "%;\n";
        if (have_cloudiness)
            text_stream << current_language.weather.cloudiness << ": " << clouds << "%;\n";


        std::string wind_degree_text;
        if (have_wind_deg)
        {
            if (wind_degree >= 337.5 || wind_degree < 22.5)
                wind_degree_text = current_language.weather.wind.north;
            else if (wind_degree >= 22.5 && wind_degree < 67.5)
                wind_degree_text = current_language.weather.wind.northeast;
            else if (wind_degree >= 67.5 && wind_degree < 112.5)
                wind_degree_text = current_language.weather.wind.east;
            else if (wind_degree >= 112.5 && wind_degree < 157.5)
                wind_degree_text = current_language.weather.wind.southeast;
            else if (wind_degree >= 157.5 && wind_degree < 202.5)
                wind_degree_text = current_language.weather.wind.south;
            else if (wind_degree >= 202.5 && wind_degree < 247.5)
                wind_degree_text = current_language.weather.wind.southwest;
            else if (wind_degree >= 247.5 && wind_degree < 292.5)
                wind_degree_text = current_language.weather.wind.west;
            else if (wind_degree >= 292.5 && wind_degree < 337.5)
                wind_degree_text = current_language.weather.wind.northwest;
            else
                assert(false);
        }

        if (have_wind_speed)
        {
            text_stream << current_language.weather.wind.wind << ": " << wind_speed << " " << current_language.weather.wind.meters_per_second;
            if (have_wind_deg)
                text_stream << ", " << wind_degree_text << ".";
        }
        else if (have_wind_deg)
            text_stream << current_language.weather.wind.wind << ": " << wind_degree_text;

        text = text_stream.str();
    }
    else if (message->text == current_language.language.change)
    {
        user.state = State::LANGUAGE_CHANGE;
        text = current_language.language.choose;
    }
    else if (message->text == current_language.city.change)
    {
        user.state = State::CITY_CHANGE;
        text = current_language.city.choose;
    }
    else
    {
        text = current_language.unknown_command;
    }
    m_bot.getApi().sendMessage(message->chat->id, text, false, 0, m_keyboards.at(user.language).at(user.state));
}

std::string get_city_name(const Bot::UserData& user)
{
    switch (user.language)
    {
    case Language::ENGLISH:
        return user.city->at("name");
    case Language::RUSSIAN:
        if (user.city->contains("name_ru"))
            return user.city->at("name_ru");
        return user.city->at("name");
    }
}

const std::string& get_city_change_success_message(const Bot::UserData& user, const StringConstants& current_language)
{
    switch (user.state)
    {
    case Bot::State::CITY_INIT:
    case Bot::State::CITY_AMBIGUOUS_INIT:
        return current_language.city.choose_success;
    case Bot::State::CITY_CHANGE:
    case Bot::State::CITY_AMBIGUOUS_CHANGE:
        return current_language.city.change_success;
    }
    assert(false);
}

} // namespace weather_bot
