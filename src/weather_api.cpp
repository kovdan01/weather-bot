#include "weather_api.h"

#include <boost/beast.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <utility>
#include <sstream>
#include <iostream>
#include <mutex>

namespace weather_bot
{

OpenWeatherMapApi::OpenWeatherMapApi(std::string token)
    : m_token(std::move(token))
    , m_target_common_part("/data/2.5/weather?units=metric&appid=" + m_token)
{
}

nlohmann::json OpenWeatherMapApi::make_request(const std::string& target) const
{
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::asio::ip::tcp::socket socket(io_context);
    boost::asio::connect(socket, resolver.resolve(m_host, "80"));

    namespace http = boost::beast::http;
    http::request<http::string_body> request(http::verb::get, m_target_common_part + "&" + target, 11);
    request.set(http::field::host, m_host);
    request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    http::write(socket, request);

    boost::beast::flat_buffer buffer;
    http::response<http::string_body> response;
    http::read(socket, buffer, response);
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);

    nlohmann::json json;
    std::stringstream sstream;
    sstream << response.body();
    sstream >> json;

    return json;
}

nlohmann::json OpenWeatherMapApi::make_request_by_city_id(unsigned int city_id, const std::string& language) const
{
    return make_request("lang=" + language + "&id=" + std::to_string(city_id));
}

} // namespace weather_bot
