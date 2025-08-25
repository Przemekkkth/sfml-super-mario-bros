#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

class PIDController
{
public:
    PIDController() = default;
    PIDController(double kP, double kI, double kD);
    PIDController(double kP, double kI, double kD, double period);

    double calculate(double measurement);
    double calculate(double measurement, double setpoint);
    void setSetpoint(double setpoint);

private:
    double clamp(double value, double low, double high);
    double m_kP = 0.0;
    double m_kI = 0.0;
    double m_kD = 0.0;

    double m_period = 1.0;

    double m_minIntegral = -1.0;
    double m_maxIntegral = 1.0;

    double m_setpoint = 0.0;
    double m_totalError = 0.0;
    double m_velocityError = 0.0;
    double m_positionError = 0.0;
};

#endif // PIDCONTROLLER_H
