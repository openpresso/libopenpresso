#ifndef CONTROLLERS_BREW_PROFILER_SENSOR_CONDITION_CHECKER_HPP
#define CONTROLLERS_BREW_PROFILER_SENSOR_CONDITION_CHECKER_HPP

#include <functional>
#include <memory>

namespace libopenpresso
{

template <typename Sensor, typename Value, Value (Sensor::*getter)() const, auto comparator>
class SensorConditionChecker {
public:
  SensorConditionChecker(const std::shared_ptr<Sensor>& sensor, Value val)
  : m_sensor{sensor}
  , m_val{val}
  {
  }
  void prepare() const noexcept
  {
  }
  bool isSatisfied() const
  {
    return comparator(m_val, std::invoke(getter, m_sensor.get()));
  }

private:
  std::shared_ptr<Sensor> m_sensor;
  Value m_val;
};

} // namespace libopenpresso

#endif // CONTROLLERS_BREW_PROFILER_SENSOR_CONDITION_CHECKER_HPP