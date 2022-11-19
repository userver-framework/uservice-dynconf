#pragma once

#include <userver/components/loggable_component_base.hpp>
#include <userver/dynamic_config/source.hpp>

using namespace userver;

namespace myservice::smth {

class Component final : public components::LoggableComponentBase {
 public:
  // name of your component to refer in static config
  static constexpr std::string_view kName = "middleware-me";

  Component(const components::ComponentConfig& config,
            const components::ComponentContext& context);

  int DoSomething() const;

  ~Component() final = default;

  static yaml_config::Schema GetStaticConfigSchema();

 private:
  dynamic_config::Source config_;
};

}  // namespace myservice::smth
