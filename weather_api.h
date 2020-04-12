#ifndef WEATHER_API_H
#define WEATHER_API_H

#include "json.hpp"

#include <string>

namespace weather_bot
{

class OpenWeatherMapApi
{
public:
    OpenWeatherMapApi(std::string token);
    OpenWeatherMapApi(const OpenWeatherMapApi&) = delete;
    OpenWeatherMapApi& operator=(const OpenWeatherMapApi&) = delete;
    OpenWeatherMapApi(OpenWeatherMapApi&&) = delete;
    OpenWeatherMapApi& operator=(OpenWeatherMapApi&&) = delete;
    ~OpenWeatherMapApi() = default;

    nlohmann::json make_request_by_city_id(unsigned int city_id, const std::string& language) const;
private:
    nlohmann::json make_request(const std::string& target) const;

    const std::string m_host = "api.openweathermap.org";
    const std::string m_token;
    const std::string m_target_common_part;
};

} // namespace weather_bot

#endif // WEATHER_API_H
