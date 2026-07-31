// ============================================================
// motor_control.h - Control de motores DC (L298N)
// ============================================================

#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "config.h"

class MotorController {
  private:
    int currentSpeed;

  public:
    MotorController() : currentSpeed(SPEED_DEFAULT) {}

    void begin() {
      pinMode(MOTOR_A_IN1, OUTPUT);
      pinMode(MOTOR_A_IN2, OUTPUT);
      pinMode(MOTOR_A_ENA, OUTPUT);
      pinMode(MOTOR_B_IN3, OUTPUT);
      pinMode(MOTOR_B_IN4, OUTPUT);
      pinMode(MOTOR_B_ENB, OUTPUT);
      stop();
    }

    void setSpeed(int speed) {
      currentSpeed = constrain(speed, SPEED_MIN, SPEED_MAX);
    }

    int getSpeed() { return currentSpeed; }

    void forward() {
      digitalWrite(MOTOR_A_IN1, HIGH);
      digitalWrite(MOTOR_A_IN2, LOW);
      digitalWrite(MOTOR_B_IN3, HIGH);
      digitalWrite(MOTOR_B_IN4, LOW);
      analogWrite(MOTOR_A_ENA, currentSpeed);
      analogWrite(MOTOR_B_ENB, currentSpeed);
    }

    void backward() {
      digitalWrite(MOTOR_A_IN1, LOW);
      digitalWrite(MOTOR_A_IN2, HIGH);
      digitalWrite(MOTOR_B_IN3, LOW);
      digitalWrite(MOTOR_B_IN4, HIGH);
      analogWrite(MOTOR_A_ENA, currentSpeed);
      analogWrite(MOTOR_B_ENB, currentSpeed);
    }

    void left() {
      // Gira en el lugar: izquierda reversa, derecha forward
      digitalWrite(MOTOR_A_IN1, LOW);
      digitalWrite(MOTOR_A_IN2, HIGH);
      digitalWrite(MOTOR_B_IN3, HIGH);
      digitalWrite(MOTOR_B_IN4, LOW);
      analogWrite(MOTOR_A_ENA, currentSpeed);
      analogWrite(MOTOR_B_ENB, currentSpeed);
    }

    void right() {
      // Gira en el lugar: izquierda forward, derecha reversa
      digitalWrite(MOTOR_A_IN1, HIGH);
      digitalWrite(MOTOR_A_IN2, LOW);
      digitalWrite(MOTOR_B_IN3, LOW);
      digitalWrite(MOTOR_B_IN4, HIGH);
      analogWrite(MOTOR_A_ENA, currentSpeed);
      analogWrite(MOTOR_B_ENB, currentSpeed);
    }

    void stop() {
      digitalWrite(MOTOR_A_IN1, LOW);
      digitalWrite(MOTOR_A_IN2, LOW);
      digitalWrite(MOTOR_B_IN3, LOW);
      digitalWrite(MOTOR_B_IN4, LOW);
      analogWrite(MOTOR_A_ENA, 0);
      analogWrite(MOTOR_B_ENB, 0);
    }
};

#endif
