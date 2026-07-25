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

// ===================== MODOS DE MANEJO =====================
enum DriveMode { MODE_MANUAL, MODE_AUTO, MODE_AGENTIC };

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
// Cableado físico real (libres de conflicto con los pines de motor: 12,13,14,25,26,27)
#define SONAR_TRIG   4
#define SONAR_ECHO   17

// ===================== VELOCIDADES =====================
#define SPEED_MIN    80    // velocidad mínima para que se mueva
#define SPEED_MAX    255   // máxima
#define SPEED_DEFAULT 180  // velocidad por defecto

// ===================== AUTÓNOMO (wall-avoider simple) =====================
#define OBSTACLE_DISTANCE_CM 25   // distancia para detectar obstáculo
#define TURN_DURATION_MS     600  // ms que dura girando al esquivar

// ===================== AGÉNTICO (control reactivo, no bloqueante) =====================
// Reutiliza OBSTACLE_DISTANCE_CM como umbral de frenado en seco / maniobra.
#define AGENTIC_INTERVAL_MS       50   // frecuencia de evaluación (más reactivo que el autónomo)
#define AGENTIC_BRAKE_DISTANCE_CM 80   // a partir de acá empieza a frenar progresivamente
#define AGENTIC_ACCEL_STEP        4    // cuánto sube la velocidad por ciclo (aceleración suave)
#define AGENTIC_BRAKE_STEP        12   // cuánto baja la velocidad por ciclo (frenado más agresivo)
#define AGENTIC_BACKUP_MS         300  // ms retrocediendo antes de girar al esquivar
#define AGENTIC_TURN_MS           500  // ms girando al esquivar

// ===================== WEB SERVER =====================
#define WEB_PORT 80

#endif
