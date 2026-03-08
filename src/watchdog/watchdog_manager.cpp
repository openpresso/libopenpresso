#include "watchdog_manager.hpp"

#include <memory>
#include <optional>

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/pin_info.hpp>

#include <gpio/pins_manager.hpp>
#include <watchdog/ac_sensor_validator.hpp>
#include <watchdog/pin_output_validator.hpp>
#include <watchdog/watchdog_thread.hpp>

using namespace libopenpresso::watchdog;

WatchdogManager::WatchdogManager(const std::optional<WatchdogConfig>& config)
: m_thread{config ? std::make_shared<WatchdogThread>(config->watchdogDev, config->timeout)
                  : std::shared_ptr<WatchdogThread>{}}
{
}

std::shared_ptr<libopenpresso::gpio::PinMonitor> WatchdogManager::getAcMonitor(
  const pin_addr_t& addr, const gpio::PinsManager& pins)
{
  if (!m_thread) {
    return pins.findPinMonitor(addr);
  }

  auto acSensorId = pins.getPinId(addr);
  if (auto it = m_watchdogProtectedMonitors.find(acSensorId); it != m_watchdogProtectedMonitors.end()) {
    return it->second;
  }
  auto monitor = pins.findPinMonitor(addr);
  auto validator = std::make_shared<AcSensorValidator>(m_thread, monitor);
  monitor = std::shared_ptr<gpio::PinMonitor>{validator, monitor.get()};
  return m_watchdogProtectedMonitors.insert({acSensorId, monitor}).first->second;
}

std::shared_ptr<libopenpresso::gpio::PinOutput> WatchdogManager::getPinOutput(const pin_addr_t& addr,
                                                                              const gpio::PinsManager& pins)
{
  if (!m_thread) {
    return pins.findPinOutput(addr);
  }

  auto pinId = pins.getPinId(addr);
  if (auto it = m_watchdogProtectedOutputs.find(pinId); it != m_watchdogProtectedOutputs.end()) {
    return it->second;
  }
  auto output = pins.findPinOutput(addr);
  output = std::make_shared<PinOutputValidator>(m_thread, output);
  return m_watchdogProtectedOutputs.insert({pinId, output}).first->second;
}
