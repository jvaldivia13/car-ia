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
      --agentic: #22d3ee;
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
    .dot-online { background: var(--good); box-shadow: 0 0 6px var(--good); animation: pulse 1.6s infinite; }
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

    .mode-field label { display: block; margin-bottom: 8px; }
    .mode-switch {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      gap: 6px;
      background: var(--bg-1);
      border: 1px solid var(--panel-border);
      border-radius: 14px;
      padding: 4px;
    }
    .mode-btn {
      display: flex; flex-direction: column; align-items: center; justify-content: center; gap: 2px;
      padding: 10px 4px;
      border: none;
      border-radius: 11px;
      background: transparent;
      color: var(--text-dim);
      font-size: 0.68rem;
      font-weight: 700;
      letter-spacing: 0.02em;
      cursor: pointer;
      transition: transform 0.08s ease, background 0.15s ease, color 0.15s ease;
    }
    .mode-btn:active { transform: scale(0.96); }
    .mode-btn .mode-icon { font-size: 1rem; }
    .mode-btn[data-mode="manual"].active {
      background: linear-gradient(155deg, var(--accent-bright), var(--accent));
      color: #10101f;
      box-shadow: 0 4px 14px rgba(111,111,232,0.4);
    }
    .mode-btn[data-mode="auto"].active {
      background: linear-gradient(155deg, #4dffc0, var(--good));
      color: #0a1a14;
      box-shadow: 0 4px 14px rgba(47,230,160,0.35);
    }
    .mode-btn[data-mode="agentic"].active {
      background: linear-gradient(155deg, #7fe9fb, var(--agentic));
      color: #052a30;
      box-shadow: 0 4px 14px rgba(34,211,238,0.4);
    }
    .mode-hint {
      font-size: 0.66rem;
      color: var(--text-dim);
      text-align: center;
      margin-top: 6px;
      min-height: 1.4em;
    }

    .agent-panel { display: flex; flex-direction: column; gap: 8px; }
    .agent-speed {
      font-family: var(--font-mono);
      font-variant-numeric: tabular-nums;
      font-size: 0.68rem;
      font-weight: 700;
      color: var(--agentic);
    }
    .agent-decision {
      display: flex; align-items: center; gap: 10px;
      padding: 11px 13px;
      border-radius: 12px;
      background: var(--bg-1);
      border: 1px solid var(--panel-border);
      transition: background 0.2s ease, border-color 0.2s ease;
    }
    .agent-decision.maneuvering { border-color: var(--warn); background: rgba(255,207,77,0.08); }
    .agent-decision-icon {
      font-size: 1.05rem;
      color: var(--agentic);
      flex: none;
      width: 20px;
      text-align: center;
      transition: color 0.2s ease;
    }
    .agent-decision.maneuvering .agent-decision-icon { color: var(--warn); }
    .agent-decision-text { font-size: 0.78rem; font-weight: 600; }

    .agent-log {
      display: flex; flex-direction: column;
      max-height: 100px;
      overflow-y: auto;
      font-family: var(--font-mono);
      font-size: 0.6rem;
      color: var(--text-dim);
    }
    .agent-log-line { padding: 3px 2px; border-bottom: 1px solid var(--panel-border); }
    .agent-log-line:first-child { color: var(--agentic); }
    .agent-log-line:last-child { border-bottom: none; }

    .sensor-panel { display: flex; flex-direction: column; gap: 8px; }
    .sensor-badge {
      font-family: var(--font-mono);
      font-size: 0.62rem;
      font-weight: 700;
      letter-spacing: 0.05em;
      padding: 3px 9px;
      border-radius: 999px;
      background: var(--bg-1);
      color: var(--text-dim);
      transition: background 0.2s ease, color 0.2s ease;
    }
    .sensor-badge.clear { background: var(--good); color: #0a1a14; }
    .sensor-badge.near  { background: var(--danger); color: #250a0e; }

    .distance-readout { display: flex; align-items: baseline; gap: 5px; }
    .distance-readout .value {
      font-family: var(--font-mono);
      font-variant-numeric: tabular-nums;
      font-size: 2rem;
      font-weight: 800;
      line-height: 1;
      color: var(--accent-bright);
    }
    .distance-readout .unit { font-size: 0.75rem; color: var(--text-dim); }

    .gauge-track {
      position: relative;
      height: 8px;
      border-radius: 4px;
      background: var(--bg-1);
      overflow: hidden;
    }
    .gauge-fill {
      position: absolute; left: 0; top: 0; bottom: 0;
      width: 0%;
      background: var(--good);
      border-radius: 4px;
      transition: width 0.25s ease, background 0.2s ease;
    }
    .gauge-threshold {
      position: absolute; top: -2px; bottom: -2px;
      width: 2px;
      background: var(--text);
      opacity: 0.4;
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

          <div class="field mode-field">
            <label>Modo de manejo</label>
            <div class="mode-switch" id="modeSwitch">
              <button class="mode-btn active" data-mode="manual">
                <span class="mode-icon">🕹️</span><span>MANUAL</span>
              </button>
              <button class="mode-btn" data-mode="auto">
                <span class="mode-icon">🧭</span><span>AUTÓNOMO</span>
              </button>
              <button class="mode-btn" data-mode="agentic">
                <span class="mode-icon">⚡</span><span>AGÉNTICO</span>
              </button>
            </div>
            <p class="mode-hint" id="modeHint">D-pad y teclado activos</p>
          </div>

          <div class="agent-panel" id="agentPanel" hidden>
            <div class="field-head">
              <label>Decisión del agente</label>
              <span class="agent-speed" id="agentSpeed">— / —</span>
            </div>
            <div class="agent-decision" id="agentDecision">
              <span class="agent-decision-icon" id="agentIcon">●</span>
              <span class="agent-decision-text" id="agentDecisionText">Esperando datos…</span>
            </div>
            <div class="agent-log" id="agentLog"></div>
          </div>

          <div class="sensor-panel">
            <div class="field-head">
              <label>Sensor de distancia</label>
              <span class="sensor-badge" id="sensorBadge">SIN DATOS</span>
            </div>
            <div class="distance-readout">
              <span class="value" id="distanceDisplay">—</span><span class="unit">cm</span>
            </div>
            <div class="gauge-track">
              <div class="gauge-fill" id="gaugeFill"></div>
              <div class="gauge-threshold" id="gaugeThreshold"></div>
            </div>
          </div>
        </section>
      </div>
    </main>
  </div>

  <script>
    // ===== CONFIG =====
    const FETCH_BASE = '';
    const SEND_INTERVAL = 80;          // ms entre reenvíos mientras se mantiene presionado
    const MAX_CONSECUTIVE_FAILURES = 5; // ~400ms sin respuesta -> dejamos de insistir localmente

    let driveMode = 'manual'; // 'manual' | 'auto' | 'agentic'
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
    const modeButtons = document.querySelectorAll('.mode-btn[data-mode]');
    const modeHint = document.getElementById('modeHint');
    const agentPanel = document.getElementById('agentPanel');
    const agentSpeedEl = document.getElementById('agentSpeed');
    const agentDecisionEl = document.getElementById('agentDecision');
    const agentIconEl = document.getElementById('agentIcon');
    const agentDecisionTextEl = document.getElementById('agentDecisionText');
    const agentLog = document.getElementById('agentLog');
    const distanceDisplay = document.getElementById('distanceDisplay');
    const sensorBadge = document.getElementById('sensorBadge');
    const gaugeFill = document.getElementById('gaugeFill');
    const gaugeThreshold = document.getElementById('gaugeThreshold');
    const dpadButtons = document.querySelectorAll('.dpad-btn[data-dir]');

    // Debe coincidir con OBSTACLE_DISTANCE_CM en config.h
    const OBSTACLE_THRESHOLD_CM = 25;
    const GAUGE_MAX_CM = 100; // rango visible del gauge (el sensor puede leer más lejos)
    gaugeThreshold.style.left = Math.min(100, (OBSTACLE_THRESHOLD_CM / GAUGE_MAX_CM) * 100) + '%';

    function updateSensorUI(distance) {
      distanceDisplay.textContent = distance.toFixed(0);
      const pct = Math.max(0, Math.min(100, (distance / GAUGE_MAX_CM) * 100));
      gaugeFill.style.width = pct + '%';
      if (distance < OBSTACLE_THRESHOLD_CM) {
        gaugeFill.style.background = 'var(--danger)';
        sensorBadge.textContent = 'OBSTÁCULO';
        sensorBadge.className = 'sensor-badge near';
      } else if (distance < OBSTACLE_THRESHOLD_CM * 2) {
        gaugeFill.style.background = 'var(--warn)';
        sensorBadge.textContent = 'CERCA';
        sensorBadge.className = 'sensor-badge';
      } else {
        gaugeFill.style.background = 'var(--good)';
        sensorBadge.textContent = 'LIBRE';
        sensorBadge.className = 'sensor-badge clear';
      }
    }

    // ===== PANEL DE DECISIONES DEL AGENTE (tiempo real) =====
    let lastAgentDecision = null;

    function logAgentDecision(text) {
      const time = new Date().toLocaleTimeString('es-ES', { hour12: false });
      const line = document.createElement('div');
      line.className = 'agent-log-line';
      line.textContent = time + '  ' + text;
      agentLog.prepend(line);
      while (agentLog.children.length > 6) agentLog.removeChild(agentLog.lastChild);
    }

    function resetAgentPanel() {
      lastAgentDecision = null;
      agentLog.innerHTML = '';
      agentDecisionTextEl.textContent = 'Esperando datos…';
      agentIconEl.textContent = '●';
      agentSpeedEl.textContent = '— / —';
      agentDecisionEl.classList.remove('maneuvering');
    }

    function updateAgentUI(agentic) {
      agentSpeedEl.textContent = agentic.speed + ' / ' + agentic.target;

      const maneuvering = agentic.state === 'backup' || agentic.state === 'turn';
      agentDecisionEl.classList.toggle('maneuvering', maneuvering);

      let icon = '●';
      if (agentic.state === 'backup') icon = '⏪';
      else if (agentic.state === 'turn') icon = agentic.decision.indexOf('izquierda') !== -1 ? '◀' : '▶';
      else if (agentic.target > agentic.speed + 3) icon = '▲';
      else if (agentic.target < agentic.speed - 3) icon = '▼';
      agentIconEl.textContent = icon;

      agentDecisionTextEl.textContent = agentic.decision;

      if (agentic.decision !== lastAgentDecision) {
        logAgentDecision(agentic.decision);
        lastAgentDecision = agentic.decision;
      }
    }

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
      if (driveMode !== 'manual' || activeDir === dir) return;
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
        if (driveMode !== 'manual') return;
        e.preventDefault();
        btn.setPointerCapture(e.pointerId);
        if (navigator.vibrate) navigator.vibrate(12);
        startDir(dir, btn);
      });
      btn.addEventListener('pointerup', doStop);
      btn.addEventListener('pointercancel', doStop);
    });

    // DETENER es el botón de emergencia: si hay un modo automático activo,
    // primero lo saca de ahí (vuelve a manual) y de paso corta motores;
    // si ya está en manual, solo corta motores.
    stopBtn.addEventListener('click', () => {
      if (driveMode !== 'manual') {
        setDriveMode('manual');
      } else {
        doStop();
      }
    });

    // ===== SPEED =====
    speedSlider.addEventListener('input', () => {
      speed = parseInt(speedSlider.value);
      speedDisplay.textContent = speed;
      api(`/speed?value=${speed}`);
    });

    // ===== MODO DE MANEJO (manual / autónomo / agéntico) =====
    const MODE_HINTS = {
      manual: 'D-pad y teclado activos',
      auto: 'El carro avanza solo y esquiva obstáculos girando a la izquierda',
      agentic: 'Acelera y frena según la distancia, decide la maniobra en tiempo real'
    };

    function setDriveMode(mode) {
      if (mode === driveMode) return;
      driveMode = mode;
      modeButtons.forEach(b => b.classList.toggle('active', b.dataset.mode === mode));
      modeHint.textContent = MODE_HINTS[mode] || '';
      agentPanel.hidden = mode !== 'agentic';
      if (mode === 'agentic') resetAgentPanel();
      if (mode !== 'manual') doStop(); // suelta el D-pad, el firmware toma el control
      api(`/mode/${mode}`);
    }

    modeButtons.forEach((btn) => {
      btn.addEventListener('click', () => setDriveMode(btn.dataset.mode));
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
      if (driveMode !== 'manual' || e.repeat) return;
      const dir = keyMap[e.key];
      if (!dir) return;
      e.preventDefault();
      if (dir === 'stop') { doStop(); return; }
      startDir(dir);
    });

    document.addEventListener('keyup', (e) => {
      if (driveMode !== 'manual') return;
      if (keyMap[e.key] && keyMap[e.key] !== 'stop') doStop();
    });

    // Poll de estado (distancia + decisión del agente si aplica; también
    // sirve de heartbeat de conexión). 200ms para que las maniobras del
    // modo agéntico (retroceso 300ms, giro 500ms) se alcancen a ver.
    setInterval(async () => {
      try {
        const r = await fetch('/status');
        const data = await r.json();
        if (typeof data.distance === 'number') updateSensorUI(data.distance);
        if (data.agentic) updateAgentUI(data.agentic);
        markConnected();
      } catch (e) {
        markDisconnected();
      }
    }, 200);

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
    DriveMode* modePtr;
    AgenticStatus* agenticPtr;
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

    // Estado completo para el dashboard: modo activo, distancia, y si el
    // modo agéntico está corriendo, su decisión en curso (para mostrarla
    // en tiempo real, ver panel "Decisión del agente" en el HTML).
    void handleStatus() {
      float d = sonar->readDistance();
      if (d < 0) d = 999;

      const char* modeStr = "manual";
      if (*modePtr == MODE_AUTO) modeStr = "auto";
      else if (*modePtr == MODE_AGENTIC) modeStr = "agentic";

      String json = "{\"mode\":\"" + String(modeStr) + "\",\"distance\":" + String(d, 1);

      if (*modePtr == MODE_AGENTIC) {
        const char* stateStr = "cruise";
        String decision;

        if (agenticPtr->state == AGENTIC_BACKUP) {
          stateStr = "backup";
          decision = "Retrocediendo, obstaculo detectado";
        } else if (agenticPtr->state == AGENTIC_TURN) {
          stateStr = "turn";
          decision = agenticPtr->turnLeft ? "Girando a la izquierda" : "Girando a la derecha";
        } else if (agenticPtr->targetSpeed > agenticPtr->speed + 3) {
          decision = "Acelerando";
        } else if (agenticPtr->targetSpeed < agenticPtr->speed - 3) {
          decision = "Frenando";
        } else {
          decision = "Crucero estable";
        }

        json += ",\"agentic\":{\"state\":\"" + String(stateStr) + "\"";
        json += ",\"speed\":" + String(motors->getSpeed());
        json += ",\"target\":" + String(agenticPtr->targetSpeed);
        json += ",\"decision\":\"" + decision + "\"}";
      }

      json += "}";
      server.send(200, "application/json", json);
    }

    void handleModeManual() {
      *modePtr = MODE_MANUAL;
      motors->stop();
      server.send(200, "text/plain", "manual");
    }

    void handleModeAuto() {
      *modePtr = MODE_AUTO;
      server.send(200, "text/plain", "auto");
    }

    void handleModeAgentic() {
      *modePtr = MODE_AGENTIC;
      server.send(200, "text/plain", "agentic");
    }

  public:
    void begin(MotorController* m, Sonar* s, DriveMode* mode, volatile unsigned long* lastCmd, AgenticStatus* agStatus) {
      motors = m;
      sonar = s;
      modePtr = mode;
      lastCmdMillis = lastCmd;
      agenticPtr = agStatus;

      server.on("/",              std::bind(&WebInterface::handleRoot,     this));
      server.on("/forward",       std::bind(&WebInterface::handleForward,  this));
      server.on("/backward",      std::bind(&WebInterface::handleBackward, this));
      server.on("/left",          std::bind(&WebInterface::handleLeft,     this));
      server.on("/right",         std::bind(&WebInterface::handleRight,    this));
      server.on("/stop",          std::bind(&WebInterface::handleStop,     this));
      server.on("/dist",          std::bind(&WebInterface::handleDistance, this));
      server.on("/status",        std::bind(&WebInterface::handleStatus,   this));
      server.on("/mode/manual",   std::bind(&WebInterface::handleModeManual,  this));
      server.on("/mode/auto",     std::bind(&WebInterface::handleModeAuto,    this));
      server.on("/mode/agentic",  std::bind(&WebInterface::handleModeAgentic, this));
      server.on("/speed",         std::bind(&WebInterface::handleSpeed,    this));

      server.begin();
      Serial.println("  → Servidor HTTP iniciado");
    }

    void handleClient() {
      server.handleClient();
    }
};

#endif
