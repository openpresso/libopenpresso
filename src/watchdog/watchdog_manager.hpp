#ifndef WATCHDOG_WATCHDOG_MANAGER_HPP
#define WATCHDOG_WATCHDOG_MANAGER_HPP

#include <memory>
#include <optional>
#include <unordered_map>

#include <libopenpresso/component_config.hpp>
#include <libopenpresso/pin_info.hpp>
#include <libopenpresso/types.hpp>

#include <gpio/pin_data.hpp>
#include <watchdog/sensor_value_validator.hpp>

namespace libopenpresso::gpio
{
class PinMonitor;
class PinOutput;
class PinsManager;
} // namespace libopenpresso::gpio

namespace libopenpresso::watchdog
{
class WatchdogThread;

class WatchdogManager {
public:
  WatchdogManager(const std::optional<WatchdogConfig>& config);
  std::shared_ptr<gpio::PinMonitor> getAcMonitor(const pin_addr_t& addr, const gpio::PinsManager& pins);
  std::shared_ptr<gpio::PinOutput> getPinOutput(const pin_addr_t& addr, const gpio::PinsManager& pins);

  template <typename SensorT, typename ValueT>
  std::shared_ptr<SensorT> wrapSensorWithValidator(const std::shared_ptr<SensorT>& sensor,
                                                   ValueT minValue,
                                                   ValueT maxValue,
                                                   time_delta_t updateInterval) const
  {
    if (!m_thread) {
      return sensor;
    }

    auto ptr = sensor.get();
    auto validator =
      std::make_shared<SensorValueValidator>(m_thread, sensor, minValue, maxValue, updateInterval);
    return std::shared_ptr<SensorT>{std::move(validator), ptr};
  }

private:
  std::shared_ptr<WatchdogThread> m_thread;
  std::unordered_map<gpio::pin_id_t, std::shared_ptr<gpio::PinMonitor>> m_watchdogProtectedMonitors;
  std::unordered_map<gpio::pin_id_t, std::shared_ptr<gpio::PinOutput>> m_watchdogProtectedOutputs;
};

} // namespace libopenpresso::watchdog

#endif // WATCHDOG_WATCHDOG_MANAGER_HPP