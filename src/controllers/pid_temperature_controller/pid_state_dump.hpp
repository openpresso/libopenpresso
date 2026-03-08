#ifndef CONTROLLERS_PID_TEMPERATURE_CONTROLLER_PID_STATE_DUMP_HPP
#define CONTROLLERS_PID_TEMPERATURE_CONTROLLER_PID_STATE_DUMP_HPP

#include <atomic>

#include <libopenpresso/interfaces/pid_controller_state.hpp>
#include <libopenpresso/types.hpp>

namespace libopenpresso
{

class PidStateDump : public interfaces::PidControllerState {
public:
  void store(pid_calc_t p, pid_calc_t i, pid_calc_t d, pid_calc_t f, pid_calc_t sum);
  void reset();

  pid_calc_t pTerm() const override;
  pid_calc_t iTerm() const override;
  pid_calc_t dTerm() const override;
  pid_calc_t fTerm() const override;
  pid_calc_t pidSum() const override;

private:
  std::atomic<pid_calc_t> m_pTerm = pid_calc_t{0};
  std::atomic<pid_calc_t> m_dTerm = pid_calc_t{0};
  std::atomic<pid_calc_t> m_iTerm = pid_calc_t{0};
  std::atomic<pid_calc_t> m_fTerm = pid_calc_t{0};
  std::atomic<pid_calc_t> m_pidSum = pid_calc_t{0};
};

} // namespace libopenpresso

#endif // CONTROLLERS_PID_TEMPERATURE_CONTROLLER_PID_STATE_DUMP_HPP