#include "weather_api.h"
#include "bot.h"

#include "json.hpp"
#include <tgbot/tgbot.h>

#include <string>
#include <fstream>
#include <sstream>

nlohmann::json read_settings()
{
    std::ifstream settings_file("settings.json");
    nlohmann::json settings;
    settings_file >> settings;
    return settings;
}

int main()
{
    nlohmann::json settings = read_settings();
    const std::string tg_token = settings["tg_token"];
    const std::string owm_token = settings["owm_token"];

    weather_bot::Bot bot(tg_token, owm_token);
}
