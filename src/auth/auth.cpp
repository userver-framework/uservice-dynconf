#include "auth.hpp"

std::string
checkSessionToken(const userver::components::ComponentContext &context,
                  std::string token) {
  std::string host = "locahost:8080";

  userver::clients::http::Headers headers = {{"Ticket", token}};

  userver::clients::http::Client &http_client_(
      context.FindComponent<userver::components::HttpClient>().GetHttpClient());

  auto response = http_client_.CreateRequest()
                      ->post(host + "/check")
                      ->headers(headers)
                      ->retry(5)
                      ->timeout(std::chrono::seconds(1))
                      ->perform();

  if (response->IsOk()) {
    auto request_body = userver::formats::json::FromString(response->body());
    return request_body["id"].As<std::optional<std::string>>().value();
  } else {
    return "";
  }
}