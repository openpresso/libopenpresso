#include "steam_controller.hpp"

#include <atomic>
#include <cstdlib>
#include <exception>
#include <future>
#include <utility>

#include <libopenpresso/exception.hpp>
#include <libopenpresso/interfaces/flow_rate_controller.hpp>
#include <libopenpresso/interfaces/pressure_sensor.hpp>
#include <libopenpresso/interfaces/temperature_controller.hpp>
#include <libopenpresso/interfaces/temperature_sensor.hpp>
#include <libopenpresso/types.hpp>

#include <utils/logger.hpp>

using namespace libopenpresso;

SteamController::SteamController(TemperatureSensorPtr temperatureSensor,
                                 TemperatureControllerPtr preheatController,
                                 TemperatureControllerPtr steamingTempertureController,
                                 millidegrees_t temperatureThreshold,
                                 PressureSensorPtr presureSensor,
                                 millibars_t pressureThreshold,
                                 FlowRateControllerPtr flowController,
                                 milligrams_p_second_t refillFlow,
                                 time_delta_t updatePeriod)
: m_temperatureSensor{std::move(temperatureSensor)}
, m_preheatController{std::move(preheatController)}
, m_steamingTemperatureController{std::move(steamingTempertureController)}
, m_presureSensor{std::move(presureSensor)}
, m_flowController{std::move(flowController)}
, m_pressureThreshold{pressureThreshold}
, m_temperatureThreshold{temperatureThreshold}
, m_refillFlow{refillFlow}
, m_updatePeriod{updatePeriod}
{
}

SteamController::~SteamController()
{
  if (!isActive()) {
    return;
  }

  m_exit.set_value();
  m_workerThread.join();
}

void SteamController::activate()
{
  if (isActive()) {
    throw libopenpresso::Exception{"Steam controller is already active"};
  }

  if (m_preheatController->isActive() || m_steamingTemperatureController->isActive()) {
    throw libopenpresso::Exception{"Temperature controller is busy"};
  }

  if (m_flowController->isActive()) {
    throw libopenpresso::Exception{"Flow rate controller is busy"};
  }

  m_preheatController->setTargetTemperature(getTargetTemperature());
  m_steamingTemperatureController->setTargetTemperature(getTargetTemperature());
  m_flowController->setTargetRate(m_refillFlow);
  m_preheatController->activate();

  m_exit = {};
  m_workerThread = std::thread{&SteamController::worker, this, m_exit.get_future()};
  m_isActive.store(true, std::memory_order_relaxed);
}

void SteamController::deactivate()
{
  if (!isActive()) {
    return;
  }

  m_exit.set_value();
  m_workerThread.join();

  m_flowController->deactivate();
  m_preheatController->deactivate();
  m_steamingTemperatureController->deactivate();
  m_isActive.store(false, std::memory_order_relaxed);
}

bool SteamController::isActive() const noexcept
{
  return m_isActive.load(std::memory_order_relaxed);
}

millidegrees_t SteamController::getTargetTemperature() const
{
  return m_steamTemperature.load(std::memory_order_relaxed);
}

void SteamController::setTargetTemperature(millidegrees_t millidegrees)
{
  m_steamTemperature.store(millidegrees, std::memory_order_relaxed);
  if(isActive()) {
    m_preheatController->setTargetTemperature(millidegrees);
    m_steamingTemperatureController->setTargetTemperature(millidegrees);
  }
}

void SteamController::worker(const std::future<void>& exit)
{
  try {
    controlLoop(exit);
  }
  catch (const libopenpresso::Exception& e) {
    Logger::critical("Steam controller loop raised an exception, reason: {}, thrown from "
                     "file: {}, function: {}, line: {}",
                     e.what(),
                     e.throwLocation().file_name(),
                     e.throwLocation().function_name(),
                     e.throwLocation().line());
    std::abort();
  }
  catch (const std::exception& e) {
    Logger::critical("Steam controller loop raised an exception, reason: {}", e.what());
    std::abort();
  }
  catch (...) {
    Logger::critical("Steam controller loop raised an unknow exception");
    std::abort();
  }
}

void SteamController::controlLoop(const std::future<void>& exit)
{
  while (exit.wait_for(m_updatePeriod) == std::future_status::timeout) {
    if (isSteamValveOpen()) {
      if (!m_flowController->isActive()) {
        m_flowController->activate();
      }
      m_preheatController->deactivate();
      if (!m_steamingTemperatureController->isActive()) {
        m_steamingTemperatureController->activate();
      }
    }
    else {
      m_flowController->deactivate();
      m_steamingTemperatureController->deactivate();
      if (!m_preheatController->isActive()) {
        m_preheatController->activate();
      }
    }
  }
}

bool SteamController::isSteamValveOpen() const
{
  return m_temperatureSensor->getTemperature() + m_temperatureThreshold >=
           m_steamTemperature.load(std::memory_order_relaxed) &&
         m_presureSensor->getPressure() <= m_pressureThreshold;
}
