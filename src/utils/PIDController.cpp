#include "PIDController.h"
#include <algorithm>
#include <iostream>

PIDController::PIDController(double kP, double kI, double kD)
    : m_kP{kP}
    , m_kI{kI}
    , m_kD{kD}
{}

PIDController::PIDController(double kP, double kI, double kD, double period)
    : m_kP{kP}
    , m_kI{kI}
    , m_kD{kD}
    , m_period{period}
{}

double PIDController::calculate(double measurement)
{
    m_positionError = m_setpoint - measurement;
    m_velocityError = (m_setpoint - measurement) / m_period;

    if (m_kI != 0) {
        m_totalError = clamp(m_totalError + m_velocityError * m_period,
                             m_minIntegral / m_kI,
                             m_maxIntegral / m_kI);
    }

    return m_positionError * m_kP + m_totalError * m_kI + m_velocityError * m_kD;
}

double PIDController::calculate(double measurement, double setpoint)
{
    m_positionError = setpoint - measurement;
    m_velocityError = (setpoint - measurement) / m_period;

    if (m_kI != 0) {
        m_totalError = clamp(m_totalError + m_velocityError * m_period,
                             m_minIntegral / m_kI,
                             m_maxIntegral / m_kI);
    }
    return (m_positionError * m_kP) + (m_totalError * m_kI) + (m_velocityError * m_kD);
}

void PIDController::setSetpoint(double setpoint)
{
    m_setpoint = setpoint;
}

double PIDController::clamp(double value, double low, double high)
{
    return std::max(low, std::min(value, high));
}
