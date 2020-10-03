#ifndef BOT_H
#define BOT_H

#include "weather_api.h"
#include "language_support.h"

#include "json.hpp"
#include <tgbot/tgbot.h>

#include <memory>
#include <string>
#include <cstdint>
#include <unordered_map>

namespace weather_bot
{

class Bot
{
public:
    Bot(std::string tg_token, std::string weather_token, std::unique_ptr<nlohmann::json> cities = nullptr);
    Bot(const Bot&) = delete;
    Bot& operator=(const Bot&) = delete;
    Bot(Bot&&) = delete;
    Bot& operator=(Bot&&) = delete;

private:
    enum class State
    {
        START,
        LANGUAGE_INIT,
        CITY_INIT,
        CITY_AMBIGUOUS_INIT,
        LANGUAGE_CHANGE,
        CITY_CHANGE,
        CITY_AMBIGUOUS_CHANGE,
        WEATHER_GET,
    };

    struct UserData
    {
        State state;
        Language language;
        const nlohmann::json* city;
    };

    void init_keyboards();
    void init_user(TgBot::Message::Ptr message, std::int32_t user_id);

    void poll_loop();
    static void process_message(TgBot::Message::Ptr message, Bot* self);
    void process_message_impl(TgBot::Message::Ptr message, UserData& user);

    void start_command(TgBot::Message::Ptr message, UserData& user);

    void language_change(TgBot::Message::Ptr message, UserData& user, bool init = false);
    void city_change(TgBot::Message::Ptr message, UserData& user, bool init = false);
    void city_ambiguous_change(TgBot::Message::Ptr message, UserData& user, bool init = false);

    void weather_get(TgBot::Message::Ptr message, UserData& user) const;

    TgBot::Bot m_bot;
    OpenWeatherMapApi m_weather;

    std::unique_ptr<nlohmann::json> m_cities;

    std::unordered_multimap<std::string, const nlohmann::json&> m_cities_by_name;
    std::unordered_multimap<std::string, const nlohmann::json&> m_cities_by_ru_name;
    std::unordered_map<int, const nlohmann::json&> m_cities_by_id;

    std::unordered_map<std::int32_t, UserData> m_users;
    std::unordered_map<std::int32_t, std::vector<decltype(m_cities_by_name)::const_iterator>> m_possible_cities_by_users;
    std::unordered_map<Language, std::unordered_map<State, TgBot::ReplyKeyboardMarkup::Ptr>> m_keyboards;

    friend std::string get_city_name(const UserData& user);
    friend const std::string& get_city_change_success_message(const Bot::UserData& user, const StringConstants& current_language);
};

} // namespace weather_bot

#endif // BOT_H
