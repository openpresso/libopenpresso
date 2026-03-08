#include "core.hpp"

#include <memory>

#include <libopenpresso/config.hpp>
#include <libopenpresso/interfaces/libopenpresso_core.hpp>
#include <libopenpresso/libopenpresso.hpp>

#include <utils/logger.hpp>

using namespace libopenpresso;

CorePtr libopenpresso::getCore(const DeviceConfig& config)
{
  Logger::setLogger(config.logger);
  Logger::info("Libopenpresso is starting...");

  auto core = std::make_shared<Core>(config);
  core->init();
  return core;
}