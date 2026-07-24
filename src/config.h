// ============================================================
// KARIM CAR CONTROLLER - Configuración
// ============================================================

#ifndef CONFIG_H
#define CONFIG_H

// WIFI_SSID, WIFI_PASSWORD y OTA_PASSWORD viven en secrets.h (gitignored).
// Si no existe, copia secrets.h.example → secrets.h y completa tus datos.
#include "secrets.h"

// ===================== OTA =====================
const char* OTA_HOSTNAME  = "karim-car";

// ===================== PINES MOTORES (L298N) =====================
// Motor A (izquierdo)
#define MOTOR_A_IN1  16
#define MOTOR_A_IN2  17
#define MOTOR_A_ENA  21   // PWM - velocidad

// Motor B (derecho)
#define MOTOR_B_IN3  18
#define MOTOR_B_IN4  19
#define MOTOR_B_ENB  22   // PWM - velocidad

// ===================== SENSOR ULTRASONIDO (HC-SR04) =====================
#define SONAR_TRIG   26
#define SONAR_ECHO   27

// ===================== VELOCIDADES =====================
#define SPEED_MIN    80    // velocidad mínima para que se mueva
#define SPEED_MAX    255   // máxima
#define SPEED_DEFAULT 180  // velocidad por defecto

// ===================== AUTÓNOMO =====================
#define OBSTACLE_DISTANCE_CM 25   // distancia para detectar obstáculo
#define TURN_DURATION_MS     600  // ms que dura girando al esquivar

// ===================== WEB SERVER =====================
#define WEB_PORT 80

#endif
