#!/usr/bin/env python3
"""
ota_deploy.py - Despliegue OTA para KARIM Car Controller

Uso:
  python3 ota_deploy.py                    # busca el ESP32 por hostname y sube
  python3 ota_deploy.py --ip 192.168.1.100 # IP directa
  python3 ota_deploy.py --port 3232        # puerto OTA (default 3232)

Requisitos:
  pip install esptool
  O tener el binario compilado en build/car_controller.ino.bin

Primero compila el sketch desde Arduino IDE o PlatformIO:
  Arduino IDE:  Sketch → Export Compiled Binary
  PlatformIO:   pio run (genera .pio/build/esp32dev/firmware.bin)
"""

import os, re, sys, subprocess, socket, time, argparse

# === CONFIG ===
HOSTNAME = "karim-car"
OTA_PORT = 3232
OTA_PASSWORD_DEFAULT = "karim123"

SECRETS_PATH = os.path.join(os.path.dirname(__file__), "src", "secrets.h")


def load_ota_password():
    """Lee OTA_PASSWORD desde src/secrets.h para no duplicar el valor
    a mano en dos archivos. Si no existe, usa el valor por defecto."""
    try:
        with open(SECRETS_PATH, "r", encoding="utf-8") as f:
            content = f.read()
        m = re.search(r'OTA_PASSWORD\s*=\s*"([^"]*)"', content)
        if m:
            return m.group(1)
    except FileNotFoundError:
        pass
    return OTA_PASSWORD_DEFAULT


OTA_PASSWORD = load_ota_password()

# Posibles rutas del binario compilado
BIN_PATHS = [
    # PlatformIO
    ".pio/build/esp32dev/firmware.bin",
    "build/esp32dev/firmware.bin",
    # Arduino IDE (Linux)
    f"/tmp/arduino_build*/car_controller.ino.bin",
    # Arduino IDE (junto al sketch)
    "car_controller.ino.esp32.bin",
    "car_controller.ino.bin",
]


def find_binary():
    """Busca el binario compilado en rutas comunes."""
    # Primero exactas
    for p in BIN_PATHS:
        if "*" in p:
            import glob
            matches = glob.glob(p)
            for m in matches:
                if os.path.isfile(m):
                    print(f"  ✓ Binario encontrado: {m}")
                    return m
        elif os.path.isfile(p):
            print(f"  ✓ Binario encontrado: {p}")
            return p

    # Buscar recursivamente en directorios de build
    for root, dirs, files in os.walk("."):
        for f in files:
            if f.endswith(".bin") and ("firmware" in f or "car_controller" in f):
                path = os.path.join(root, f)
                print(f"  ✓ Binario encontrado: {path}")
                return path

    # Buscar en /tmp (Arduino IDE builds)
    for root, dirs, files in os.walk("/tmp"):
        for f in files:
            if "car_controller" in f and f.endswith(".bin"):
                path = os.path.join(root, f)
                print(f"  ✓ Binario encontrado: {path}")
                return path

    return None


def resolve_ip(hostname):
    """Resuelve hostname a IP vía mDNS o DNS."""
    try:
        ip = socket.gethostbyname(f"{hostname}.local")
        print(f"  ✓ Resuelto {hostname}.local → {ip}")
        return ip
    except:
        pass
    try:
        ip = socket.gethostbyname(hostname)
        print(f"  ✓ Resuelto {hostname} → {ip}")
        return ip
    except:
        pass
    return None


def ota_upload(ip, bin_path, port=OTA_PORT, password=OTA_PASSWORD):
    """Sube el binario vía OTA usando esptool."""
    print(f"\n{'='*50}")
    print(f"  Subiendo a {ip}:{port} ...")
    print(f"{'='*50}\n")

    # Método 1: esptool write_flash via OTA
    cmd = [
        sys.executable, "-m", "esptool",
        "--chip", "esp32",
        "--port", f"ota://{ip}:{port}",
        "--baud", "921600",
        "write_flash",
        "0x10000", bin_path
    ]

    # Método 2: espota.py (viene con Arduino ESP32 core)
    espota_candidates = [
        os.path.expanduser("~/.arduino15/packages/esp32/tools/espota-ng/*/espota-ng.py"),
        os.path.expanduser("~/.arduino15/packages/esp32/tools/espota-ng/*/espota.py"),
        "/usr/local/bin/espota.py",
    ]

    espota = None
    for pattern in espota_candidates:
        import glob
        matches = glob.glob(pattern)
        if matches:
            espota = matches[0]
            break

    if espota:
        print(f"  Usando espota: {espota}")
        cmd = [
            sys.executable, espota,
            "-i", ip,
            "-p", str(port),
            "-f", bin_path,
            "-s", password,
            "-d"  # debug
        ]
    else:
        # Si esptool no soporta OTA, probar con python-esp32-ota
        try:
            from esp32_ota import OTAUpdater
            print("  Usando python-esp32-ota...")
            updater = OTAUpdater(ip, port, password)
            with open(bin_path, "rb") as f:
                data = f.read()
            updater.update(data)
            print("\n✅ ¡Actualización OTA completada!")
            return True
        except ImportError:
            print("  ✗ No se encontró espota ni python-esp32-ota")
            print("  Alternativa: usa Arduino IDE → Tools → Port → Network Ports")
            print(f"    y selecciona {hostname}")
            return False

    try:
        result = subprocess.run(cmd, check=True, capture_output=True, text=True)
        print(result.stdout)
        print("\n✅ ¡Actualización OTA completada!")
        return True
    except subprocess.CalledProcessError as e:
        print(f"\n✗ Error OTA: {e}")
        if e.stderr:
            print(e.stderr)
        return False
    except FileNotFoundError:
        print("\n✗ No se encuentra esptool. Instálalo:")
        print("  pip install esptool")
        return False


def main():
    parser = argparse.ArgumentParser(description="KARIM Car - Despliegue OTA")
    parser.add_argument("--ip", help="Dirección IP del ESP32")
    parser.add_argument("--port", type=int, default=OTA_PORT, help="Puerto OTA")
    parser.add_argument("--bin", help="Ruta al binario compilado")
    args = parser.parse_args()

    print(f"\n🔧 KARIM Car OTA Deploy")
    print(f"{'='*50}")

    # 1. Encontrar binario
    bin_path = args.bin or find_binary()
    if not bin_path:
        print("\n✗ No se encontró binario compilado.")
        print("  Compila primero:\n")
        print("  PlatformIO:  pio run")
        print("  Arduino IDE: Sketch → Export Compiled Binary\n")
        sys.exit(1)

    # 2. Resolver IP
    ip = args.ip
    if not ip:
        print(f"\nResolviendo {HOSTNAME}...")
        ip = resolve_ip(HOSTNAME)
        if not ip:
            print("\n✗ No se pudo resolver la IP.")
            print("  Asegúrate de que el ESP32 esté encendido y conectado a la red.")
            print("  O pásala con --ip 192.168.1.X")
            sys.exit(1)

    # 3. Subir
    success = ota_upload(ip, bin_path, args.port)

    if not success:
        sys.exit(1)

    print(f"\n  El ESP32 se reiniciará automáticamente.")
    print(f"  Abre http://{ip} en tu navegador para controlar el carro 🚗\n")


if __name__ == "__main__":
    main()
