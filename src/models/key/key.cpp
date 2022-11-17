#include "key.hpp"

namespace uservice_dynconf::models {

bool Key::operator==(const Key &other) const {
    return std::tie(uuid) ==
           std::tie(uuid);
  }
} // namespace uservice_dynconf::models

namespace std {
template <>
size_t hash<uservice_dynconf::models::Key>::operator()(const uservice_dynconf::models::Key &param) const {
    size_t seed = 0;
    boost::hash_combine(seed, param.uuid);
    return seed;
}
} // namespace std