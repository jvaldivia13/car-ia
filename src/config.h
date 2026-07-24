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
// Cableado físico real (no el original de la placa de referencia)
// Motor A (izquierdo)
#define MOTOR_A_IN1  14
#define MOTOR_A_IN2  27
#define MOTOR_A_ENA  12   // PWM - velocidad

// Motor B (derecho)
#define MOTOR_B_IN3  26
#define MOTOR_B_IN4  25
#define MOTOR_B_ENB  13   // PWM - velocidad

// ===================== SENSOR ULTRASONIDO (HC-SR04) =====================
// DESACTIVADO por ahora: GPIO 26/27 (Trig/Echo originales) quedaron
// reasignados a IN3/IN2 del motor. Reactivar con pines libres (p.ej.
// GPIO32/33) el día que se conecte el sonar físicamente.
// #define SONAR_TRIG   26
// #define SONAR_ECHO   27

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
