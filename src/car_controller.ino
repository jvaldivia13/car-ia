// ============================================================
// KARIM CAR CONTROLLER - Main
// Control remoto + autónomo para carrito con ESP32
// ============================================================
//
// HARDWARE:
//   - ESP32 + L298N (puente H) + 2 motores DC
//   - HC-SR04 (ultrasonido) - DESACTIVADO por ahora, ver nota abajo
//
// CONEXIONES (cableado físico real):
//   ESP32        → L298N
//   GPIO 14      → IN1
//   GPIO 27      → IN2
//   GPIO 12 (PWM) → ENA
//   GPIO 26      → IN3
//   GPIO 25      → IN4
//   GPIO 13 (PWM) → ENB
//
//   HC-SR04: desactivado. GPIO 26/27, que originalmente iban a Trig/Echo,
//   quedaron ocupados por IN3/IN2 del motor. Todo el código del sonar y
//   el modo autónomo (que depende de él) está comentado más abajo hasta
//   que se recableé el sensor a pines libres.
//
// PRIMER FLASH (vía USB):
//   1. Conectar ESP32 por USB
//   2. Abrir este .ino en Arduino IDE / VS Code (PlatformIO)
//   3. Configurar credenciales WiFi en config.h
//   4. Subir (Upload)
//   5. Abrir monitor serial para ver la IP
//
// SIGUIENTES FLASH (vía OTA inalámbrica):
//   $ python3 ota_deploy.py
//   ... o desde Arduino IDE: Tools → Port → Network Ports → karim-car
//
// ============================================================

#include <WiFi.h>
#include <ArduinoOTA.h>
#include <WebServer.h>
#include "config.h"
#include "motor_control.h"
// #include "sonar.h" // DESACTIVADO: ver nota de cableado arriba
#include "web_interface.h"

// ===== GLOBALES =====
MotorController motors;
// Sonar sonar; // DESACTIVADO
WebServer server(WEB_PORT);
WebInterface webUI;

// ===== MODO AUTÓNOMO (DESACTIVADO, depende del sonar) =====
// bool autonomousMode = false;
// unsigned long lastAutoUpdate = 0;
// const unsigned long AUTO_INTERVAL = 100; // ms entre decisiones autónomas

// unsigned long lastDistancePoll = 0;
// float lastDistance = 0;

// Dead man's switch: si no llega un comando de movimiento nuevo dentro de
// este plazo (cliente desconectado, pestaña cerrada, WiFi caído, etc.) se
// detiene el carro solo. 0 = sin movimiento activo.
// El cliente reenvía cada 80ms mientras mantiene presionada una dirección,
// así que 1500ms tolera varios paquetes perdidos / una micro-caída de WiFi
// sin frenar el carro de golpe en medio de un movimiento normal.
volatile unsigned long lastCommandMillis = 0;
const unsigned long COMMAND_TIMEOUT_MS = 1500;

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\n==================================");
  Serial.println("  KARIM CAR CONTROLLER v1.0");
  Serial.println("==================================\n");

  // Inicializar hardware
  motors.begin();
  // sonar.begin(); // DESACTIVADO

  // Conectar WiFi
  connectWiFi();

  // Iniciar OTA
  setupOTA();

  // Iniciar servidor web
  webUI.begin(&motors, &lastCommandMillis);

  Serial.println("\n✅ Sistema listo!");
  Serial.print("  → Abre http://");
  Serial.print(WiFi.localIP());
  Serial.println(" en tu navegador\n");
}

// ===== LOOP =====
void loop() {
  // OTA
  ArduinoOTA.handle();

  // Web server
  webUI.handleClient();

  // Modo autónomo: DESACTIVADO (depende del sonar, ver nota de cableado arriba)
  // if (autonomousMode) {
  //   if (millis() - lastAutoUpdate > AUTO_INTERVAL) {
  //     lastAutoUpdate = millis();
  //     runAutonomous();
  //   }
  // } else
  if (lastCommandMillis != 0 && millis() - lastCommandMillis > COMMAND_TIMEOUT_MS) {
    // Dead man's switch: sin comandos nuevos del cliente, detener por seguridad.
    motors.stop();
    lastCommandMillis = 0;
  }

  // Poll de distancia: DESACTIVADO (depende del sonar)
  // if (millis() - lastDistancePoll > 500) {
  //   lastDistancePoll = millis();
  //   lastDistance = sonar.readDistance();
  //   if (lastDistance < 0) lastDistance = 999;
  // }
}

// ===== WIFI =====
void connectWiFi() {
  Serial.print("Conectando a WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n  ✓ WiFi conectado!");
    Serial.print("  → IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n  ✗ Falló conexión WiFi. Verifica credenciales en config.h");
    // Modo AP de respaldo
    WiFi.mode(WIFI_AP);
    WiFi.softAP("KARIM-Car", "karim123");
    Serial.print("  → Modo AP: ");
    Serial.println(WiFi.softAPIP());
  }
}

// ===== OTA =====
void setupOTA() {
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);

  ArduinoOTA.onStart([]() {
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
    Serial.println("\n📦 OTA: Iniciando actualización (" + type + ")...");
    motors.stop();
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\n✅ OTA: Actualización completa. Reiniciando...");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    int pct = progress / (total / 100);
    Serial.printf("  Progreso: %u%%\r", pct);
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("\n✗ Error OTA [%u]: ", error);
    if (error == OTA_AUTH_ERROR)    Serial.println("Autenticación fallida");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Error al iniciar");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Error de conexión");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Error de recepción");
    else if (error == OTA_END_ERROR)    Serial.println("Error al finalizar");
  });

  ArduinoOTA.begin();
  Serial.println("  → OTA listo (hostname: " + String(OTA_HOSTNAME) + ")");
}

// ===== MODO AUTÓNOMO (simple wall-avoider) - DESACTIVADO =====
// Depende del sonar, que está desconectado por el conflicto de GPIO 26/27.
// void runAutonomous() {
//   float dist = sonar.readDistance();
//
//   // Si no hay eco, asumir libre
//   if (dist < 0) dist = 999;
//
//   if (dist < OBSTACLE_DISTANCE_CM) {
//     // Obstáculo detectado - esquivar
//     motors.stop();
//     delay(150);
//
//     // Gira siempre a la izquierda: solo hay un sonar fijo al frente,
//     // no hay forma de medir qué lado está más despejado.
//     motors.left();
//     motors.setSpeed(SPEED_DEFAULT);
//     delay(TURN_DURATION_MS);
//     motors.stop();
//
//     // Avanzar
//     motors.forward();
//   } else {
//     // Camino libre
//     motors.forward();
//     motors.setSpeed(SPEED_DEFAULT);
//   }
// }
