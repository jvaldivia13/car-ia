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

    /* ===== JOYSTICK ===== */
    .joystick-area {
      flex: 1;
      display: flex;
      align-items: center;
      justify-content: center;
      width: 100%;
      padding: 10px;
    }
    .joystick-container {
      position: relative;
      width: min(280px, 75vw);
      height: min(280px, 75vw);
      border-radius: 50%;
      background: radial-gradient(circle, #1e1e3a 0%, #14142a 100%);
      border: 2px solid #333366;
      box-shadow: 0 0 40px rgba(50,50,150,0.3), inset 0 0 30px rgba(0,0,0,0.5);
    }
    .joystick-knob {
      position: absolute;
      top: 50%; left: 50%;
      width: 80px; height: 80px;
      border-radius: 50%;
      background: radial-gradient(circle at 35% 35%, #5555bb, #2a2a70);
      border: 2px solid #6666cc;
      box-shadow: 0 4px 20px rgba(0,0,0,0.5), inset 0 2px 4px rgba(255,255,255,0.1);
      transform: translate(-50%, -50%);
      pointer-events: none;
      transition: none;
    }
    .direction-label {
      position: absolute;
      font-size: 0.65rem;
      color: #5555aa;
      font-weight: 600;
      letter-spacing: 1px;
    }
    .dl-fwd { top: 12px; left: 50%; transform: translateX(-50%); }
    .dl-bwd { bottom: 12px; left: 50%; transform: translateX(-50%); }
    .dl-left { left: 12px; top: 50%; transform: translateY(-50%); }
    .dl-right { right: 12px; top: 50%; transform: translateY(-50%); }

    .direction-arrows {
      position: absolute;
      top: 50%; left: 50%;
      transform: translate(-50%, -50%);
      width: 90%; height: 90%;
      pointer-events: none;
      opacity: 0.15;
    }
    .dir-arrow {
      position: absolute;
      font-size: 1.4rem;
      color: #6666cc;
    }
    .da-up { top: 20%; left: 50%; transform: translateX(-50%); }
    .da-down { bottom: 20%; left: 50%; transform: translateX(-50%); }
    .da-left { left: 15%; top: 50%; transform: translateY(-50%); }
    .da-right { right: 15%; top: 50%; transform: translateY(-50%); }

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

    /* Hidden stop button overlay on joystick */
    .stop-overlay {
      display: none;
      position: fixed;
      top: 0; left: 0; right: 0; bottom: 0;
      background: rgba(255,23,68,0.15);
      z-index: 10;
      align-items: center;
      justify-content: center;
    }
    .stop-overlay.show { display: flex; }
    .stop-overlay .big-stop {
      width: 100px; height: 100px;
      border-radius: 50%;
      background: #ff1744;
      border: none;
      color: #fff;
      font-size: 1.2rem;
      font-weight: 700;
      box-shadow: 0 0 60px rgba(255,23,68,0.5);
      cursor: pointer;
    }

    .connection-status {
      position: fixed;
      top: 10px; right: 10px;
      font-size: 0.65rem;
      color: #555;
      z-index: 20;
    }
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

  <!-- JOYSTICK -->
  <div class="joystick-area">
    <div class="joystick-container" id="joystickArea">
      <div class="joystick-knob" id="joystickKnob"></div>
      <div class="direction-label dl-fwd">FWD</div>
      <div class="direction-label dl-bwd">REV</div>
      <div class="direction-label dl-left">LEFT</div>
      <div class="direction-label dl-right">RIGHT</div>
      <div class="direction-arrows">
        <div class="dir-arrow da-up">▲</div>
        <div class="dir-arrow da-down">▼</div>
        <div class="dir-arrow da-left">◀</div>
        <div class="dir-arrow da-right">▶</div>
      </div>
    </div>
  </div>

  <!-- STOP OVERLAY -->
  <div class="stop-overlay" id="stopOverlay">
    <button class="big-stop" id="bigStopBtn">STOP</button>
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
      <button class="btn btn-auto" id="autoBtn">🤖 AUTÓNOMO</button>
    </div>
    <div class="distance-row">
      Distancia: <span id="distanceDisplay">—</span> cm
    </div>
  </div>

  <div class="connection-status" id="connStatus">●</div>

  <script>
    // ===== CONFIG =====
    const FETCH_BASE = '';

    let currentDir = 'stop';
    let autoMode = false;
    let speed = 180;
    let isTouching = false;
    let lastSend = 0;
    const SEND_INTERVAL = 80; // ms entre envíos

    // ===== DOM REFS =====
    const knob = document.getElementById('joystickKnob');
    const area = document.getElementById('joystickArea');
    const statusDot = document.getElementById('statusDot');
    const statusText = document.getElementById('statusText');
    const directionText = document.getElementById('directionText');
    const speedSlider = document.getElementById('speedSlider');
    const speedDisplay = document.getElementById('speedDisplay');
    const stopBtn = document.getElementById('stopBtn');
    const autoBtn = document.getElementById('autoBtn');
    const bigStopBtn = document.getElementById('bigStopBtn');
    const stopOverlay = document.getElementById('stopOverlay');
    const distanceDisplay = document.getElementById('distanceDisplay');

    // ===== FETCH HELPERS =====
    // No hay WebSocket en el firmware (solo HTTP), así que el estado de
    // conexión se deriva de si las llamadas a la API responden o no.
    function markConnected() {
      statusDot.className = currentDir === 'stop' ? 'status-dot dot-online' : 'status-dot dot-moving';
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
        markDisconnected();
      }
    }

    // ===== JOYSTICK TOUCH =====
    function getJoystickDir(clientX, clientY) {
      const rect = area.getBoundingClientRect();
      const cx = rect.left + rect.width / 2;
      const cy = rect.top + rect.height / 2;
      const dx = clientX - cx;
      const dy = clientY - cy;
      const dist = Math.sqrt(dx*dx + dy*dy);
      const maxDist = rect.width / 2 - 10;

      // Limit knob position
      const clamped = Math.min(dist, maxDist);
      const angle = Math.atan2(dy, dx);
      const knobX = (clamped / maxDist) * 55 * Math.cos(angle);
      const knobY = (clamped / maxDist) * 55 * Math.sin(angle);
      knob.style.transform = `translate(calc(-50% + ${knobX}px), calc(-50% + ${knobY}px))`;

      if (dist < 20) return 'stop';

      // Deadzone angles: 8-direction.
      // atan2 usa coordenadas de pantalla (Y crece hacia abajo), así que
      // "arriba" (FWD) cae en 270°, "derecha" en 0°, "abajo" (REV) en 90°
      // e "izquierda" en 180°.
      const degrees = (angle * 180 / Math.PI + 360) % 360;

      if (degrees >= 225 && degrees < 315) return 'forward';
      if (degrees >= 315 || degrees < 45) return 'right';
      if (degrees >= 45 && degrees < 135) return 'backward';
      return 'left';
    }

    function resetKnob() {
      knob.style.transform = 'translate(-50%, -50%)';
      currentDir = 'stop';
      directionText.textContent = '—';
      statusDot.className = 'status-dot dot-online';
      if (!autoMode) api('/stop');
    }

    // Touch events
    area.addEventListener('touchstart', (e) => {
      if (autoMode) return;
      e.preventDefault();
      isTouching = true;
      stopOverlay.classList.add('show');
      const t = e.changedTouches[0];
      currentDir = getJoystickDir(t.clientX, t.clientY);
      if (currentDir !== 'stop') directionText.textContent = currentDir.toUpperCase();
      sendCommand();
    });

    area.addEventListener('touchmove', (e) => {
      if (autoMode) return;
      e.preventDefault();
      const t = e.changedTouches[0];
      const newDir = getJoystickDir(t.clientX, t.clientY);
      if (newDir !== currentDir) {
        currentDir = newDir;
        if (currentDir !== 'stop') directionText.textContent = currentDir.toUpperCase();
      }
      sendCommand();
    });

    area.addEventListener('touchend', (e) => {
      e.preventDefault();
      isTouching = false;
      stopOverlay.classList.remove('show');
      resetKnob();
    });

    // Mouse events (for desktop testing)
    let mouseDown = false;
    area.addEventListener('mousedown', (e) => {
      if (autoMode) return;
      mouseDown = true;
      currentDir = getJoystickDir(e.clientX, e.clientY);
      if (currentDir !== 'stop') directionText.textContent = currentDir.toUpperCase();
      sendCommand();
    });
    document.addEventListener('mousemove', (e) => {
      if (!mouseDown || autoMode) return;
      const newDir = getJoystickDir(e.clientX, e.clientY);
      if (newDir !== currentDir) {
        currentDir = newDir;
        if (currentDir !== 'stop') directionText.textContent = currentDir.toUpperCase();
      }
      sendCommand();
    });
    document.addEventListener('mouseup', () => {
      if (!mouseDown) return;
      mouseDown = false;
      resetKnob();
    });

    function sendCommand() {
      const now = Date.now();
      if (now - lastSend < SEND_INTERVAL) return;
      lastSend = now;
      if (currentDir !== 'stop') {
        api(`/${currentDir}?speed=${speed}`);
      } else {
        api('/stop');
      }
    }

    // ===== STOP BUTTON =====
    function doStop() {
      resetKnob();
      stopOverlay.classList.remove('show');
      isTouching = false;
      mouseDown = false;
      api('/stop');
    }
    stopBtn.addEventListener('click', doStop);
    bigStopBtn.addEventListener('click', doStop);

    // ===== SPEED =====
    speedSlider.addEventListener('input', () => {
      speed = parseInt(speedSlider.value);
      speedDisplay.textContent = speed;
      api(`/speed?value=${speed}`);
    });

    // ===== AUTONOMOUS =====
    autoBtn.addEventListener('click', () => {
      autoMode = !autoMode;
      autoBtn.classList.toggle('active', autoMode);
      autoBtn.textContent = autoMode ? '🤖 AUTÓNOMO (ACTIVO)' : '🤖 AUTÓNOMO';
      if (autoMode) {
        resetKnob();
        api('/auto/on');
      } else {
        api('/auto/off');
        api('/stop');
      }
    });

    // Keyboard controls
    document.addEventListener('keydown', (e) => {
      if (autoMode) return;
      const keyMap = {
        'ArrowUp': 'forward', 'w': 'forward', 'W': 'forward',
        'ArrowDown': 'backward', 's': 'backward', 'S': 'backward',
        'ArrowLeft': 'left', 'a': 'left', 'A': 'left',
        'ArrowRight': 'right', 'd': 'right', 'D': 'right',
        ' ': 'stop'
      };
      const dir = keyMap[e.key];
      if (!dir) return;
      e.preventDefault();
      if (dir === 'stop') { doStop(); return; }
      currentDir = dir;
      directionText.textContent = dir.toUpperCase();
      statusDot.className = 'status-dot dot-moving';
      api(`/${dir}?speed=${speed}`);
    });

    document.addEventListener('keyup', (e) => {
      if (autoMode) return;
      if (['ArrowUp','ArrowDown','ArrowLeft','ArrowRight','w','W','s','S','a','A','d','D'].includes(e.key)) {
        doStop();
      }
    });

    // Poll de distancia (también sirve de heartbeat de conexión)
    setInterval(async () => {
      try {
        const r = await fetch('/dist');
        const d = await r.text();
        if (d) distanceDisplay.textContent = parseFloat(d).toFixed(0);
        markConnected();
      } catch (e) {
        markDisconnected();
      }
    }, 2000);

    console.log('KARIM Car Controller loaded 🚗');
  </script>
</body>
</html>
)rawliteral";

// ========== MANEJADORES DEL SERVIDOR ==========

class WebInterface {
  private:
    MotorController* motors;
    Sonar* sonar;
    bool* autoModePtr;
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

    void handleDistance() {
      float d = sonar->readDistance();
      if (d < 0) d = 999;
      server.send(200, "text/plain", String(d));
    }

    void handleAutoOn() {
      *autoModePtr = true;
      server.send(200, "text/plain", "auto_on");
    }

    void handleAutoOff() {
      *autoModePtr = false;
      motors->stop();
      server.send(200, "text/plain", "auto_off");
    }

  public:
    void begin(MotorController* m, Sonar* s, bool* autoFlag, volatile unsigned long* lastCmd) {
      motors = m;
      sonar = s;
      autoModePtr = autoFlag;
      lastCmdMillis = lastCmd;

      server.on("/",              std::bind(&WebInterface::handleRoot,     this));
      server.on("/forward",       std::bind(&WebInterface::handleForward,  this));
      server.on("/backward",      std::bind(&WebInterface::handleBackward, this));
      server.on("/left",          std::bind(&WebInterface::handleLeft,     this));
      server.on("/right",         std::bind(&WebInterface::handleRight,    this));
      server.on("/stop",          std::bind(&WebInterface::handleStop,     this));
      server.on("/dist",          std::bind(&WebInterface::handleDistance, this));
      server.on("/auto/on",       std::bind(&WebInterface::handleAutoOn,   this));
      server.on("/auto/off",      std::bind(&WebInterface::handleAutoOff,  this));
      server.on("/speed",         std::bind(&WebInterface::handleSpeed,    this));

      server.begin();
      Serial.println("  → Servidor HTTP iniciado");
    }

    void handleClient() {
      server.handleClient();
    }
};

#endif
