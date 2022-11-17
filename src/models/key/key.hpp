#pragma once

#include "userver/formats/json/value.hpp"
#include "userver/storages/postgres/io/chrono.hpp"
#include "userver/utils/strong_typedef.hpp"
#include <boost/functional/hash.hpp>
#include <chrono>
#include <iterator>
#include <string>

namespace uservice_dynconf::models {
struct Key {
  std::string uuid;

  bool operator==(const Key &other) const;
};
} // namespace uservice_dynconf::models

namespace std {
template <> struct hash<uservice_dynconf::models::Key> {
  size_t operator()(const uservice_dynconf::models::Key &param) const;
};
} // namespace std
