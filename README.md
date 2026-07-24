# KARIM Car Controller 🚗

Control remoto + autónomo para carrito con ESP32, L298N y sensor ultrasónico.

## Hardware necesario

| Componente       | Cantidad |
|-----------------|----------|
| ESP32           | 1        |
| L298N (Puente H) | 1       |
| Motor DC (x2)   | 2        |
| HC-SR04         | 1        |
| Batería (7-12V) | 1        |
| Cables dupont   | varios   |

## Conexiones

```
ESP32 → L298N
GPIO 16 → IN1
GPIO 17 → IN2
GPIO 21 → ENA (PWM)
GPIO 18 → IN3
GPIO 19 → IN4
GPIO 22 → ENB (PWM)

ESP32 → HC-SR04
GPIO 26 → Trig
GPIO 27 → Echo
```

## Primer despliegue (USB)

1. Copia `src/secrets.h.example` a `src/secrets.h` y pon tu WiFi y contraseña OTA (ese archivo no se sube a git)
2. Conecta el ESP32 por USB
3. Compila y sube:

```bash
# Con PlatformIO:
pio run --target upload --upload-port /dev/ttyUSB0

# Con Arduino IDE:
# Abre car_controller.ino → Tools → Board: ESP32 Dev Module → Port: /dev/ttyUSB0 → Upload
```

4. Abre monitor serial para ver la IP asignada

## Actualizaciones OTA (inalámbricas)

Después del primer flash por USB, usa OTA:

```bash
# Opción 1: Script Python
python3 ota_deploy.py

# Opción 2: PlatformIO OTA
pio run --target upload --upload-port 192.168.1.100

# Opción 3: Arduino IDE
# Tools → Port → Network Ports → karim-car @ 192.168.1.100
```

## Uso

1. Abre `http://<IP_DEL_ESP32>` en tu navegador
2. **Joystick táctil**: arrastra para mover el carro
3. **Teclado**: flechas direccionales o WASD + espacio para detener
4. **Velocidad**: slider para ajustar velocidad
5. **Modo Autónomo**: el carro esquiva obstáculos solo
6. **Botón DETENER**: emergencia

## Archivos del proyecto

```
car_controller/
├── car_controller.ino      # Main
├── config.h                # Configuración (pines, velocidades, incluye secrets.h)
├── secrets.h                # WiFi/OTA reales (gitignored, no existe hasta que lo copies)
├── secrets.h.example       # Plantilla de secrets.h
├── motor_control.h         # Control de motores (L298N)
├── sonar.h                 # Sensor ultrasónico (HC-SR04)
├── web_interface.h         # Servidor web + interfaz HTML
├── platformio.ini          # Configuración PlatformIO
├── ota_deploy.py           # Script de despliegue OTA
└── README.md               # Este archivo
```
