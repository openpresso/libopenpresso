#ifndef UTILS_DERIVATIVE_FILTER_HPP
#define UTILS_DERIVATIVE_FILTER_HPP

#include <libopenpresso/exception.hpp>

template <typename T, bool fixedSamplingRate>
class FilteredDerivative;

template <typename T>
class FilteredDerivative<T, true> {
public:
  FilteredDerivative(T filterTime, T samplingTime, T initState = T{})
  : m_filterTime(filterTime)
  , m_alpha(samplingTime / filterTime)
  , m_state(initState)
  {
    if (filterTime < samplingTime) {
      throw libopenpresso::Exception{
        "Filter time {} cannot be less than sampling rate {}", filterTime, samplingTime};
    }
  }

  void reset(T state = T{})
  {
    m_state = state;
  }

  T process(T input)
  {
    const T u_minus_x = input - m_state;
    m_state = m_state + m_alpha * u_minus_x;
    return u_minus_x / m_filterTime;
  }

private:
  T m_filterTime;
  T m_alpha;
  T m_state;
};

template <typename T>
class FilteredDerivative<T, false> {
public:
  FilteredDerivative(T filterTime, T initState = T{})
  : m_filterTime(filterTime)
  , m_state(initState)
  {
  }

  void reset(T state = T{})
  {
    m_state = state;
  }

  T process(T input, T dt)
  {
    const T u_minus_x = input - m_state;
    if (dt >= m_filterTime) {
      m_state = input;
      return u_minus_x / dt;
    }
    const T alpha = dt / m_filterTime;
    m_state = m_state + alpha * u_minus_x;
    return u_minus_x / m_filterTime;
  }

private:
  T m_filterTime;
  T m_state;
};

#endif // UTILS_DERIVATIVE_FILTER_HPP