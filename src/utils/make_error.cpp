#include <utils/make_error.hpp>

#include <string_view>

#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>

#include <docs/api/api.hpp>

namespace uservice_dynconf::utils {

userver::formats::json::Value MakeError(std::string_view code,
                                        std::string_view message) {
  handlers::Error result{code.data(), message.data()};
  return userver::formats::json::ValueBuilder{result}.ExtractValue();
}

} // namespace uservice_dynconf::utils
