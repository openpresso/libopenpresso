#include "pid_state_dump.hpp"

#include <atomic>

#include <libopenpresso/types.hpp>

using namespace libopenpresso;

void PidStateDump::store(pid_calc_t p, pid_calc_t i, pid_calc_t d, pid_calc_t f, pid_calc_t sum)
{
  m_pTerm.store(p, std::memory_order_relaxed);
  m_dTerm.store(d, std::memory_order_relaxed);
  m_iTerm.store(i, std::memory_order_relaxed);
  m_fTerm.store(f, std::memory_order_relaxed);
  m_pidSum.store(sum, std::memory_order_relaxed);
}

void PidStateDump::reset()
{
  m_pTerm.store(pid_calc_t{0}, std::memory_order_relaxed);
  m_dTerm.store(pid_calc_t{0}, std::memory_order_relaxed);
  m_iTerm.store(pid_calc_t{0}, std::memory_order_relaxed);
  m_fTerm.store(pid_calc_t{0}, std::memory_order_relaxed);
  m_pidSum.store(pid_calc_t{0}, std::memory_order_relaxed);
}

pid_calc_t PidStateDump::pTerm() const
{
  return m_pTerm.load(std::memory_order_relaxed);
}

pid_calc_t PidStateDump::iTerm() const
{
  return m_iTerm.load(std::memory_order_relaxed);
}

pid_calc_t PidStateDump::dTerm() const
{
  return m_dTerm.load(std::memory_order_relaxed);
}

pid_calc_t PidStateDump::fTerm() const
{
  return m_fTerm.load(std::memory_order_relaxed);
}

pid_calc_t PidStateDump::pidSum() const
{
  return m_pidSum.load(std::memory_order_relaxed);
}