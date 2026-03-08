/**
 * @file controller_base.hpp
 * @brief Base interface for all controller components.
 *
 * Defines the common interface for all devices that actively control
 * physical outputs (heaters, pumps, solenoids, etc.).
 */

#ifndef LIBOPENPRESSO_INTERFACES_CONTROLLER_BASE_HPP
#define LIBOPENPRESSO_INTERFACES_CONTROLLER_BASE_HPP

#include <memory>

namespace libopenpresso::interfaces
{

/**
 * @class ControllerBase
 * @brief Abstract base interface for all controllable devices.
 *
 * All controllers (pressure, temperature, flow rate, etc.) inherit from this
 * interface to provide consistent activation/deactivation semantics.
 */

/**
 * @fn virtual void ControllerBase::activate()
 * @brief Activate the controller to start regulation.
 *
 * After activation, the controller will assume unique control over
 * underlying device driver class. May spawn internal threads,
 * start periodic control loops or subscribe to sensor updates as needed.
 */

/**
 * @fn virtual void ControllerBase::deactivate()
 * @brief Deactivate the controller, stopping regulation.
 *
 * After deactivation, the controller should stop all control actions
 * and will power down associated hardware (heaters, pumps, etc.).
 */

/**
 * @fn virtual bool ControllerBase::isActive() const noexcept
 * @brief Query whether the controller is currently active.
 *
 * @return true if controller is actively regulating, false otherwise.
 * @note noexcept - this operation should never throw.
 */

/**
 * @fn virtual ControllerBase::~ControllerBase()
 * @brief Virtual destructor for polymorphic cleanup.
 */

class ControllerBase {
public:
  virtual void activate() = 0;
  virtual void deactivate() = 0;
  virtual bool isActive() const noexcept = 0;
  virtual ~ControllerBase() = default;
};

} // namespace libopenpresso::interfaces

namespace libopenpresso
{
using ControllerBasePtr = std::shared_ptr<interfaces::ControllerBase>; ///< Alias to ControllerBase
                                                                       ///< shared pointer
} // namespace libopenpresso

#endif // LIBOPENPRESSO_INTERFACES_CONTROLLER_BASE_HPP