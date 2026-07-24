// ============================================================
// sonar.h - Sensor ultrasónico HC-SR04
// ============================================================

#ifndef SONAR_H
#define SONAR_H

#include "config.h"

class Sonar {
  public:
    void begin() {
      pinMode(SONAR_TRIG, OUTPUT);
      pinMode(SONAR_ECHO, INPUT);
      digitalWrite(SONAR_TRIG, LOW);
    }

    // Mide distancia en cm. Retorna -1 si hay timeout.
    float readDistance() {
      digitalWrite(SONAR_TRIG, LOW);
      delayMicroseconds(2);
      digitalWrite(SONAR_TRIG, HIGH);
      delayMicroseconds(10);
      digitalWrite(SONAR_TRIG, LOW);

      long duration = pulseIn(SONAR_ECHO, HIGH, 30000); // timeout 30ms (~5m)
      if (duration == 0) return -1; // sin eco

      float cm = duration * 0.034 / 2;
      return cm;
    }
};

#endif
