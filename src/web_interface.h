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
    :root {
      --bg-0: #08080f;
      --bg-1: #12121f;
      --bg-2: #191933;
      --panel-border: #2c2c54;
      --text: #f4f4fb;
      --text-dim: #8d8db3;
      --accent: #6f6fe8;
      --accent-bright: #a3a3f7;
      --danger: #ff3b5c;
      --good: #2fe6a0;
      --warn: #ffcf4d;
      --font-sans: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
      --font-mono: ui-monospace, 'SFMono-Regular', Menlo, Consolas, monospace;
    }
    * { margin: 0; padding: 0; box-sizing: border-box; }
    html, body { height: 100%; }
    body {
      font-family: var(--font-sans);
      color: var(--text);
      background:
        radial-gradient(ellipse 900px 500px at 50% -10%, rgba(111,111,232,0.20), transparent 60%),
        radial-gradient(ellipse 700px 500px at 100% 100%, rgba(47,230,160,0.08), transparent 55%),
        var(--bg-0);
      display: flex;
      justify-content: center;
      align-items: center;
      overflow: hidden;
      overscroll-behavior: none;
      user-select: none;
      -webkit-user-select: none;
      touch-action: none;
      padding: env(safe-area-inset-top) env(safe-area-inset-right) env(safe-area-inset-bottom) env(safe-area-inset-left);
    }

    @media (prefers-reduced-motion: no-preference) {
      .console { animation: rise 0.5s ease-out; }
    }
    @keyframes rise { from { opacity: 0; transform: translateY(10px); } to { opacity: 1; transform: translateY(0); } }

    .stage { width: 100%; height: 100%; display: flex; justify-content: center; align-items: center; padding: 14px; }

    .console {
      width: 100%;
      max-width: 460px;
      max-height: 100%;
      overflow-y: auto;
      background: linear-gradient(180deg, var(--bg-1), var(--bg-0));
      border: 1px solid var(--panel-border);
      border-radius: 22px;
      box-shadow: 0 20px 60px rgba(0,0,0,0.5), inset 0 1px 0 rgba(255,255,255,0.04);
      padding: 18px;
      display: flex;
      flex-direction: column;
      gap: 16px;
      touch-action: pan-y;
    }

    /* ===== TOPBAR ===== */
    .topbar { display: flex; align-items: center; justify-content: space-between; gap: 10px; }
    .brand { display: flex; align-items: center; gap: 10px; }
    .brand-mark {
      width: 34px; height: 34px; border-radius: 10px; flex: none;
      display: flex; align-items: center; justify-content: center;
      background: linear-gradient(135deg, var(--accent), #3a3a9c);
      box-shadow: 0 4px 14px rgba(111,111,232,0.4);
      font-size: 1.1rem;
    }
    .brand-text h1 { font-size: 0.95rem; font-weight: 800; letter-spacing: 0.06em; }
    .brand-text .brand-sub { font-size: 0.68rem; color: var(--text-dim); }

    .status-pill {
      display: flex; align-items: center; gap: 6px;
      background: var(--bg-2);
      border: 1px solid var(--panel-border);
      border-radius: 999px;
      padding: 5px 10px 5px 8px;
      font-size: 0.7rem;
      color: var(--text-dim);
      white-space: nowrap;
    }
    .status-dot { display: inline-block; width: 7px; height: 7px; border-radius: 50%; flex: none; }
    .dot-online { background: var(--good); box-shadow: 0 0 6px var(--good); }
    .dot-offline { background: var(--danger); box-shadow: 0 0 6px var(--danger); }
    .dot-moving { background: var(--warn); box-shadow: 0 0 6px var(--warn); animation: pulse 0.6s infinite; }
    @keyframes pulse { 0%, 100% { opacity: 1; } 50% { opacity: 0.35; } }

    /* ===== DECK (reflows: column on phones, row on wide screens) ===== */
    .deck { display: flex; flex-direction: column; gap: 16px; }

    .panel {
      background: var(--bg-2);
      border: 1px solid var(--panel-border);
      border-radius: 18px;
      padding: 18px;
    }

    /* ===== D-PAD ===== */
    .dpad-panel { display: flex; flex-direction: column; align-items: center; gap: 12px; }
    .dpad {
      position: relative;
      display: grid;
      grid-template-columns: repeat(3, minmax(58px, 78px));
      grid-template-rows: repeat(3, minmax(58px, 78px));
      gap: 8px;
    }
    .dpad::before {
      content: '';
      position: absolute; inset: -20px;
      background: radial-gradient(circle, rgba(111,111,232,0.16), transparent 70%);
      z-index: -1;
    }
    .dpad-btn {
      border: 1px solid var(--panel-border);
      border-radius: 14px;
      background: linear-gradient(155deg, #23234a, #1a1a38);
      color: var(--text);
      font-size: 1.5rem;
      display: flex;
      align-items: center;
      justify-content: center;
      cursor: pointer;
      box-shadow: 0 3px 10px rgba(0,0,0,0.35), inset 0 1px 0 rgba(255,255,255,0.05);
      transition: transform 0.08s ease, box-shadow 0.15s ease, background 0.15s ease;
      touch-action: none;
      user-select: none;
    }
    .dpad-btn.pressed {
      background: linear-gradient(155deg, var(--accent-bright), var(--accent));
      border-color: var(--accent-bright);
      color: #10101f;
      transform: scale(0.94);
      box-shadow: 0 0 22px rgba(163,163,247,0.6);
    }
    .dpad-fwd    { grid-column: 2; grid-row: 1; }
    .dpad-left   { grid-column: 1; grid-row: 2; }
    .dpad-center {
      grid-column: 2; grid-row: 2;
      display: flex; align-items: center; justify-content: center;
      font-family: var(--font-mono);
      font-size: 0.62rem;
      font-weight: 700;
      letter-spacing: 0.04em;
      color: var(--text-dim);
      text-align: center;
      padding: 4px;
    }
    .dpad-right  { grid-column: 3; grid-row: 2; }
    .dpad-bwd    { grid-column: 2; grid-row: 3; }

    .hint {
      font-size: 0.68rem;
      color: var(--text-dim);
      text-align: center;
      line-height: 1.5;
      max-width: 30ch;
    }

    /* ===== CONTROLS PANEL ===== */
    .controls-panel { display: flex; flex-direction: column; gap: 18px; justify-content: center; }

    .field-head { display: flex; align-items: baseline; justify-content: space-between; margin-bottom: 8px; }
    .field-head label { font-size: 0.72rem; font-weight: 700; letter-spacing: 0.06em; text-transform: uppercase; color: var(--text-dim); }
    .speed-value {
      font-family: var(--font-mono);
      font-variant-numeric: tabular-nums;
      font-size: 1.05rem;
      font-weight: 700;
      color: var(--accent-bright);
    }

    input[type=range] {
      -webkit-appearance: none;
      width: 100%;
      height: 6px;
      border-radius: 3px;
      background: linear-gradient(90deg, var(--bg-1), var(--accent));
      outline: none;
    }
    input[type=range]::-webkit-slider-thumb {
      -webkit-appearance: none;
      width: 26px; height: 26px;
      border-radius: 50%;
      background: linear-gradient(155deg, var(--accent-bright), var(--accent));
      border: 3px solid var(--bg-1);
      cursor: pointer;
      box-shadow: 0 2px 10px rgba(111,111,232,0.6);
    }
    input[type=range]::-moz-range-thumb {
      width: 26px; height: 26px;
      border-radius: 50%;
      background: linear-gradient(155deg, var(--accent-bright), var(--accent));
      border: 3px solid var(--bg-1);
      cursor: pointer;
    }
    .speed-ticks { display: flex; justify-content: space-between; margin-top: 6px; font-size: 0.62rem; letter-spacing: 0.06em; color: var(--text-dim); }

    .btn-stop {
      width: 100%;
      display: flex; align-items: center; justify-content: center; gap: 8px;
      padding: 15px 0;
      border: none;
      border-radius: 14px;
      font-size: 0.9rem;
      font-weight: 800;
      letter-spacing: 0.06em;
      color: #fff;
      cursor: pointer;
      background: linear-gradient(155deg, #ff5473, var(--danger));
      box-shadow: 0 6px 20px rgba(255,59,92,0.35);
      transition: transform 0.08s ease;
    }
    .btn-stop:active { transform: scale(0.97); }
    .btn-stop-icon { font-size: 1rem; }

    .btn-auto {
      width: 100%;
      display: flex; align-items: center; justify-content: center; gap: 8px;
      padding: 13px 0;
      border: 1px solid var(--panel-border);
      border-radius: 14px;
      font-size: 0.82rem;
      font-weight: 700;
      letter-spacing: 0.04em;
      color: var(--text-dim);
      cursor: pointer;
      background: var(--bg-1);
      transition: transform 0.08s ease, background 0.15s ease, color 0.15s ease, border-color 0.15s ease;
    }
    .btn-auto:active { transform: scale(0.97); }
    .btn-auto.active {
      color: #0a1a14;
      border-color: var(--good);
      background: linear-gradient(155deg, #4dffc0, var(--good));
      box-shadow: 0 4px 16px rgba(47,230,160,0.35);
    }

    .distance-row {
      text-align: center;
      font-size: 0.72rem;
      color: var(--text-dim);
    }
    .distance-row span {
      font-family: var(--font-mono);
      font-variant-numeric: tabular-nums;
      font-weight: 700;
      color: var(--accent-bright);
    }

    /* ===== RESPONSIVE: landscape / tablet / desktop ===== */
    @media (min-width: 640px) {
      .console { max-width: 720px; padding: 24px; }
      .deck { flex-direction: row; align-items: stretch; }
      .dpad-panel { flex: 0 0 auto; justify-content: center; }
      .controls-panel { flex: 1; }
      .dpad { grid-template-columns: repeat(3, 80px); grid-template-rows: repeat(3, 80px); }
    }
  </style>
</head>
<body>

  <div class="stage">
    <main class="console">
      <header class="topbar">
        <div class="brand">
          <span class="brand-mark">✦</span>
          <div class="brand-text">
            <h1>KARIM CAR</h1>
            <span class="brand-sub">Control remoto</span>
          </div>
        </div>
        <div class="status-pill">
          <span class="status-dot dot-online" id="statusDot"></span>
          <span id="statusText">Conectado</span>
        </div>
      </header>

      <div class="deck">
        <!-- D-PAD -->
        <section class="panel dpad-panel">
          <div class="dpad">
            <button class="dpad-btn dpad-fwd" data-dir="forward" aria-label="Adelante">▲</button>
            <button class="dpad-btn dpad-left" data-dir="left" aria-label="Izquierda">◀</button>
            <div class="dpad-center" id="directionText">—</div>
            <button class="dpad-btn dpad-right" data-dir="right" aria-label="Derecha">▶</button>
            <button class="dpad-btn dpad-bwd" data-dir="backward" aria-label="Atrás">▼</button>
          </div>
          <p class="hint">Mantén presionado para moverte · también funciona con flechas / WASD</p>
        </section>

        <!-- CONTROLES -->
        <section class="panel controls-panel">
          <div class="field speed-field">
            <div class="field-head">
              <label for="speedSlider">Velocidad</label>
              <span class="speed-value" id="speedDisplay">180</span>
            </div>
            <input type="range" id="speedSlider" min="80" max="255" value="180">
            <div class="speed-ticks"><span>LENTO</span><span>RÁPIDO</span></div>
          </div>

          <button class="btn-stop" id="stopBtn">
            <span class="btn-stop-icon">⏹</span><span>DETENER</span>
          </button>

          <button class="btn-auto" id="autoBtn">
            <span>🤖</span><span>AUTÓNOMO</span>
          </button>

          <div class="distance-row">Distancia: <span id="distanceDisplay">—</span> cm</div>
        </section>
      </div>
    </main>
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
    const autoBtn = document.getElementById('autoBtn');
    const distanceDisplay = document.getElementById('distanceDisplay');
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
        if (navigator.vibrate) navigator.vibrate(12);
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

    // ===== AUTONOMOUS =====
    autoBtn.addEventListener('click', () => {
      autoMode = !autoMode;
      autoBtn.classList.toggle('active', autoMode);
      autoBtn.querySelector('span:last-child').textContent = autoMode ? 'AUTÓNOMO (ACTIVO)' : 'AUTÓNOMO';
      if (autoMode) {
        doStop();
        api('/auto/on');
      } else {
        api('/auto/off');
        api('/stop');
      }
    });

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
