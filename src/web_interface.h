// ============================================================
// web_interface.h - Servidor web + interfaz de control
// ============================================================

#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

#include <WebServer.h>
#include "config.h"

extern WebServer server;

// ========== PÁGINA HTML EMBEBIDA ==========
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>KARIM Car Control</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
      background: #0f0f1a;
      color: #fff;
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      align-items: center;
      overflow: hidden;
      user-select: none;
      -webkit-user-select: none;
      touch-action: none;
    }

    .header {
      width: 100%;
      padding: 20px 16px 10px;
      text-align: center;
      background: linear-gradient(180deg, #1a1a30 0%, transparent 100%);
    }
    .header h1 { font-size: 1.3rem; font-weight: 700; letter-spacing: 2px; }
    .header .sub { font-size: 0.75rem; color: #888; margin-top: 2px; }
    .status-bar {
      display: flex;
      gap: 16px;
      justify-content: center;
      font-size: 0.75rem;
      color: #aaa;
      margin: 4px 0 10px;
    }
    .status-dot {
      display: inline-block;
      width: 8px; height: 8px;
      border-radius: 50%;
      margin-right: 4px;
    }
    .dot-online { background: #00e676; box-shadow: 0 0 6px #00e676; }
    .dot-offline { background: #ff5252; box-shadow: 0 0 6px #ff5252; }
    .dot-moving { background: #ffd740; box-shadow: 0 0 6px #ffd740; animation: pulse 0.6s infinite; }
    @keyframes pulse { 0%, 100% { opacity: 1; } 50% { opacity: 0.4; } }

    /* ===== D-PAD ===== */
    .dpad-area {
      flex: 1;
      display: flex;
      align-items: center;
      justify-content: center;
      width: 100%;
      padding: 10px;
    }
    .dpad {
      display: grid;
      grid-template-columns: repeat(3, min(84px, 24vw));
      grid-template-rows: repeat(3, min(84px, 24vw));
      gap: 10px;
    }
    .dpad-btn {
      border: 2px solid #6666cc;
      border-radius: 16px;
      background: radial-gradient(circle at 35% 35%, #5555bb, #2a2a70);
      color: #fff;
      font-size: 1.8rem;
      display: flex;
      align-items: center;
      justify-content: center;
      cursor: pointer;
      box-shadow: 0 4px 16px rgba(50,50,150,0.3), inset 0 2px 4px rgba(255,255,255,0.1);
      touch-action: none;
      user-select: none;
    }
    .dpad-btn.pressed {
      background: radial-gradient(circle at 35% 35%, #7777dd, #3a3a90);
      box-shadow: 0 0 24px rgba(100,100,220,0.7);
    }
    .dpad-fwd    { grid-column: 2; grid-row: 1; }
    .dpad-left   { grid-column: 1; grid-row: 2; }
    .dpad-center {
      grid-column: 2; grid-row: 2;
      display: flex; align-items: center; justify-content: center;
      color: #444477; font-size: 1.4rem;
    }
    .dpad-right  { grid-column: 3; grid-row: 2; }
    .dpad-bwd    { grid-column: 2; grid-row: 3; }

    /* ===== CONTROLES INFERIORES ===== */
    .controls-bottom {
      width: 100%;
      padding: 12px 20px 30px;
      display: flex;
      flex-direction: column;
      gap: 14px;
    }

    .speed-row {
      display: flex;
      align-items: center;
      gap: 12px;
    }
    .speed-row label { font-size: 0.8rem; color: #888; min-width: 60px; }
    .speed-row input[type=range] {
      flex: 1;
      -webkit-appearance: none;
      height: 4px;
      border-radius: 2px;
      background: linear-gradient(90deg, #333366, #6666cc);
      outline: none;
    }
    .speed-row input[type=range]::-webkit-slider-thumb {
      -webkit-appearance: none;
      width: 24px; height: 24px;
      border-radius: 50%;
      background: radial-gradient(circle at 35% 35%, #5555bb, #2a2a70);
      border: 2px solid #6666cc;
      cursor: pointer;
      box-shadow: 0 0 12px rgba(50,50,150,0.4);
    }
    .speed-value { font-size: 0.9rem; font-weight: 600; color: #fff; min-width: 32px; text-align: right; }

    .btn-row {
      display: flex;
      gap: 10px;
    }
    .btn {
      flex: 1;
      padding: 12px 0;
      border: none;
      border-radius: 12px;
      font-size: 0.85rem;
      font-weight: 600;
      cursor: pointer;
      transition: all 0.2s;
      letter-spacing: 0.5px;
    }
    .btn:active { transform: scale(0.96); }

    .btn-stop {
      background: #ff1744;
      color: #fff;
      box-shadow: 0 4px 16px rgba(255,23,68,0.3);
    }
    .btn-stop:active { background: #d50000; }

    .btn-auto {
      background: #2a2a70;
      color: #aaa;
      box-shadow: 0 4px 16px rgba(50,50,150,0.2);
    }
    .btn-auto.active {
      background: #00e676;
      color: #000;
      box-shadow: 0 4px 20px rgba(0,230,118,0.4);
    }

    .distance-row {
      text-align: center;
      font-size: 0.8rem;
      color: #6666aa;
    }
    .distance-row span { font-weight: 600; color: #8888cc; }
  </style>
</head>
<body>

  <div class="header">
    <h1>✦ KARIM CAR</h1>
    <div class="sub">Control inalámbrico</div>
    <div class="status-bar">
      <span><span class="status-dot dot-online" id="statusDot"></span><span id="statusText">Conectado</span></span>
      <span id="directionText">—</span>
    </div>
  </div>

  <!-- D-PAD -->
  <div class="dpad-area">
    <div class="dpad">
      <button class="dpad-btn dpad-fwd" data-dir="forward">▲</button>
      <button class="dpad-btn dpad-left" data-dir="left">◀</button>
      <div class="dpad-center">✦</div>
      <button class="dpad-btn dpad-right" data-dir="right">▶</button>
      <button class="dpad-btn dpad-bwd" data-dir="backward">▼</button>
    </div>
  </div>

  <!-- CONTROLES -->
  <div class="controls-bottom">
    <div class="speed-row">
      <label>Velocidad</label>
      <input type="range" id="speedSlider" min="80" max="255" value="180">
      <span class="speed-value" id="speedDisplay">180</span>
    </div>
    <div class="btn-row">
      <button class="btn btn-stop" id="stopBtn">⏹ DETENER</button>
    </div>
    <!-- Modo autónomo y distancia desactivados: dependen del sonar,
         desconectado por conflicto de GPIO (ver config.h) -->
    <!--
    <div class="btn-row">
      <button class="btn btn-auto" id="autoBtn">🤖 AUTÓNOMO</button>
    </div>
    <div class="distance-row">
      Distancia: <span id="distanceDisplay">—</span> cm
    </div>
    -->
  </div>

  <script>
    // ===== CONFIG =====
    const FETCH_BASE = '';
    const SEND_INTERVAL = 80;          // ms entre reenvíos mientras se mantiene presionado
    const MAX_CONSECUTIVE_FAILURES = 5; // ~400ms sin respuesta -> dejamos de insistir localmente

    let autoMode = false;
    let speed = 180;
    let activeDir = null;   // dirección que se está enviando en este momento (o null)
    let holdInterval = null;
    let consecutiveFailures = 0;

    // ===== DOM REFS =====
    const statusDot = document.getElementById('statusDot');
    const statusText = document.getElementById('statusText');
    const directionText = document.getElementById('directionText');
    const speedSlider = document.getElementById('speedSlider');
    const speedDisplay = document.getElementById('speedDisplay');
    const stopBtn = document.getElementById('stopBtn');
    // const autoBtn = document.getElementById('autoBtn');             // DESACTIVADO
    // const distanceDisplay = document.getElementById('distanceDisplay'); // DESACTIVADO
    const dpadButtons = document.querySelectorAll('.dpad-btn[data-dir]');

    // ===== FETCH HELPERS =====
    // No hay WebSocket en el firmware (solo HTTP), así que el estado de
    // conexión se deriva de si las llamadas a la API responden o no.
    function markConnected() {
      consecutiveFailures = 0;
      statusDot.className = activeDir ? 'status-dot dot-moving' : 'status-dot dot-online';
      statusText.textContent = 'Conectado';
    }

    function markDisconnected() {
      statusDot.className = 'status-dot dot-offline';
      statusText.textContent = 'Desconectado';
    }

    async function api(url) {
      try {
        await fetch(FETCH_BASE + url, { method: 'GET' });
        markConnected();
      } catch (e) {
        consecutiveFailures++;
        markDisconnected();
        // Sin red: dejamos de insistir localmente. El ESP32 se detendrá
        // solo por su propio timeout de seguridad aunque nuestro "stop"
        // nunca le llegue.
        if (consecutiveFailures >= MAX_CONSECUTIVE_FAILURES && activeDir) {
          clearInterval(holdInterval);
          activeDir = null;
          dpadButtons.forEach(b => b.classList.remove('pressed'));
          directionText.textContent = 'SIN CONEXIÓN';
        }
      }
    }

    // ===== D-PAD =====
    function startDir(dir, btn) {
      if (autoMode || activeDir === dir) return;
      activeDir = dir;
      directionText.textContent = dir.toUpperCase();
      statusDot.className = 'status-dot dot-moving';
      dpadButtons.forEach(b => b.classList.toggle('pressed', b.dataset.dir === dir));
      api(`/${dir}?speed=${speed}`);
      clearInterval(holdInterval);
      holdInterval = setInterval(() => api(`/${activeDir}?speed=${speed}`), SEND_INTERVAL);
    }

    function doStop() {
      activeDir = null;
      clearInterval(holdInterval);
      dpadButtons.forEach(b => b.classList.remove('pressed'));
      directionText.textContent = '—';
      statusDot.className = 'status-dot dot-online';
      api('/stop');
    }

    dpadButtons.forEach((btn) => {
      const dir = btn.dataset.dir;
      btn.addEventListener('pointerdown', (e) => {
        if (autoMode) return;
        e.preventDefault();
        btn.setPointerCapture(e.pointerId);
        startDir(dir, btn);
      });
      btn.addEventListener('pointerup', doStop);
      btn.addEventListener('pointercancel', doStop);
    });

    stopBtn.addEventListener('click', doStop);

    // ===== SPEED =====
    speedSlider.addEventListener('input', () => {
      speed = parseInt(speedSlider.value);
      speedDisplay.textContent = speed;
      api(`/speed?value=${speed}`);
    });

    // ===== AUTONOMOUS ===== DESACTIVADO: depende del sonar (ver config.h)
    // autoBtn.addEventListener('click', () => {
    //   autoMode = !autoMode;
    //   autoBtn.classList.toggle('active', autoMode);
    //   autoBtn.textContent = autoMode ? '🤖 AUTÓNOMO (ACTIVO)' : '🤖 AUTÓNOMO';
    //   if (autoMode) {
    //     doStop();
    //     api('/auto/on');
    //   } else {
    //     api('/auto/off');
    //     api('/stop');
    //   }
    // });

    // ===== KEYBOARD =====
    const keyMap = {
      'ArrowUp': 'forward', 'w': 'forward', 'W': 'forward',
      'ArrowDown': 'backward', 's': 'backward', 'S': 'backward',
      'ArrowLeft': 'left', 'a': 'left', 'A': 'left',
      'ArrowRight': 'right', 'd': 'right', 'D': 'right',
      ' ': 'stop'
    };

    document.addEventListener('keydown', (e) => {
      if (autoMode || e.repeat) return;
      const dir = keyMap[e.key];
      if (!dir) return;
      e.preventDefault();
      if (dir === 'stop') { doStop(); return; }
      startDir(dir);
    });

    document.addEventListener('keyup', (e) => {
      if (autoMode) return;
      if (keyMap[e.key] && keyMap[e.key] !== 'stop') doStop();
    });

    // Poll de distancia: DESACTIVADO junto con el sonar (/dist ya no existe
    // en el servidor). El estado de conexión ahora solo se actualiza cuando
    // se envía un comando de movimiento o velocidad.
    // setInterval(async () => {
    //   try {
    //     const r = await fetch('/dist');
    //     const d = await r.text();
    //     if (d) distanceDisplay.textContent = parseFloat(d).toFixed(0);
    //     markConnected();
    //   } catch (e) {
    //     markDisconnected();
    //   }
    // }, 2000);

    console.log('KARIM Car Controller loaded 🚗');
  </script>
</body>
</html>
)rawliteral";

// ========== MANEJADORES DEL SERVIDOR ==========

class WebInterface {
  private:
    MotorController* motors;
    // Sonar* sonar;       // DESACTIVADO: ver nota de cableado en car_controller.ino
    // bool* autoModePtr;  // DESACTIVADO junto con el modo autónomo
    volatile unsigned long* lastCmdMillis;

    void handleRoot() {
      server.send(200, "text/html", INDEX_HTML);
    }

    void handleForward() {
      if (server.hasArg("speed")) {
        motors->setSpeed(server.arg("speed").toInt());
      }
      motors->forward();
      *lastCmdMillis = millis();
      server.send(200, "text/plain", "forward");
    }

    void handleBackward() {
      if (server.hasArg("speed")) {
        motors->setSpeed(server.arg("speed").toInt());
      }
      motors->backward();
      *lastCmdMillis = millis();
      server.send(200, "text/plain", "backward");
    }

    void handleLeft() {
      if (server.hasArg("speed")) {
        motors->setSpeed(server.arg("speed").toInt());
      }
      motors->left();
      *lastCmdMillis = millis();
      server.send(200, "text/plain", "left");
    }

    void handleRight() {
      if (server.hasArg("speed")) {
        motors->setSpeed(server.arg("speed").toInt());
      }
      motors->right();
      *lastCmdMillis = millis();
      server.send(200, "text/plain", "right");
    }

    void handleStop() {
      motors->stop();
      *lastCmdMillis = 0;
      server.send(200, "text/plain", "stop");
    }

    void handleSpeed() {
      // Nota: la ruta usaba "/speed/{s}" pero WebServer.h sin
      // <uri/UriBraces.h> hace match literal, no wildcard — nunca
      // coincidía con una petición real. Se usa query param en su lugar.
      int s = server.arg("value").toInt();
      motors->setSpeed(s);
      server.send(200, "text/plain", String(s));
    }

    // DESACTIVADO: dependen del sonar / modo autónomo (ver car_controller.ino)
    // void handleDistance() {
    //   float d = sonar->readDistance();
    //   if (d < 0) d = 999;
    //   server.send(200, "text/plain", String(d));
    // }
    //
    // void handleAutoOn() {
    //   *autoModePtr = true;
    //   server.send(200, "text/plain", "auto_on");
    // }
    //
    // void handleAutoOff() {
    //   *autoModePtr = false;
    //   motors->stop();
    //   server.send(200, "text/plain", "auto_off");
    // }

  public:
    void begin(MotorController* m, volatile unsigned long* lastCmd) {
      motors = m;
      lastCmdMillis = lastCmd;

      server.on("/",              std::bind(&WebInterface::handleRoot,     this));
      server.on("/forward",       std::bind(&WebInterface::handleForward,  this));
      server.on("/backward",      std::bind(&WebInterface::handleBackward, this));
      server.on("/left",          std::bind(&WebInterface::handleLeft,     this));
      server.on("/right",         std::bind(&WebInterface::handleRight,    this));
      server.on("/stop",          std::bind(&WebInterface::handleStop,     this));
      // server.on("/dist",     std::bind(&WebInterface::handleDistance, this)); // DESACTIVADO
      // server.on("/auto/on",  std::bind(&WebInterface::handleAutoOn,   this)); // DESACTIVADO
      // server.on("/auto/off", std::bind(&WebInterface::handleAutoOff, this)); // DESACTIVADO
      server.on("/speed",         std::bind(&WebInterface::handleSpeed,    this));

      server.begin();
      Serial.println("  → Servidor HTTP iniciado");
    }

    void handleClient() {
      server.handleClient();
    }
};

#endif
