#include <WiFi.h>
#include <WebServer.h>

#define LED_MAIN      2
#define LED_INDICATOR 4
#define LIGHT_SENSOR  34
#define LIGHT_THRESHOLD 2000

const char* WIFI_SSID = "Khanh";       
const char* WIFI_PASS = "@133057k"; 

WebServer server(80);

bool manualOverride = false;
bool ledMainState   = false;
bool autoMode       = true;
int  lightValue     = 0;
unsigned long lastSensorRead = 0;

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no">
<title>Smart Light</title>
<link href="https://fonts.googleapis.com/css2?family=Syne:wght@400;700;800&family=DM+Mono:wght@400;500&display=swap" rel="stylesheet">
<style>
:root{
  --bg:#0a0a0f;--surface:#12121a;--surface2:#1a1a26;--border:#2a2a3d;
  --accent:#f5c842;--accent2:#ff6b35;--text:#e8e8f0;--muted:#6b6b8a;
  --green:#4ade80;--red:#f87171;--glow:rgba(245,200,66,0.15);
}
*{margin:0;padding:0;box-sizing:border-box;}
body{background:var(--bg);color:var(--text);font-family:'DM Mono',monospace;min-height:100vh;overflow-x:hidden;}
body::before{
  content:'';position:fixed;inset:0;
  background-image:linear-gradient(rgba(245,200,66,0.03) 1px,transparent 1px),
    linear-gradient(90deg,rgba(245,200,66,0.03) 1px,transparent 1px);
  background-size:40px 40px;pointer-events:none;z-index:0;
}
.wrap{position:relative;z-index:1;max-width:480px;margin:0 auto;padding:24px 16px 48px;}

.header{display:flex;align-items:center;gap:12px;margin-bottom:32px;}
.header-icon{width:40px;height:40px;background:var(--accent);border-radius:10px;
  display:flex;align-items:center;justify-content:center;font-size:20px;
  box-shadow:0 0 20px var(--glow);}
.header h1{font-family:'Syne',sans-serif;font-size:22px;font-weight:800;letter-spacing:-0.5px;}
.header p{font-size:11px;color:var(--muted);margin-top:2px;}

.status-bar{background:var(--surface);border:1px solid var(--border);border-radius:14px;
  padding:14px 18px;margin-bottom:20px;display:flex;justify-content:space-between;align-items:center;}
.status-dot{width:8px;height:8px;border-radius:50%;background:var(--red);flex-shrink:0;}
.status-dot.on{background:var(--green);box-shadow:0 0 8px var(--green);}
.status-label{font-size:11px;color:var(--muted);text-transform:uppercase;letter-spacing:1px;}
.status-val{font-size:13px;font-weight:500;}

.card{background:var(--surface);border:1px solid var(--border);border-radius:20px;
  padding:24px;margin-bottom:16px;}
.card-title{font-size:10px;text-transform:uppercase;letter-spacing:2px;color:var(--muted);margin-bottom:18px;}

.big-toggle{width:100%;padding:22px;border-radius:16px;border:none;cursor:pointer;
  font-family:'Syne',sans-serif;font-size:18px;font-weight:800;letter-spacing:1px;
  transition:all 0.2s;position:relative;overflow:hidden;}
.big-toggle.off{background:var(--surface2);color:var(--muted);border:2px solid var(--border);}
.big-toggle.on{background:var(--accent);color:#0a0a0f;box-shadow:0 0 30px var(--glow),0 4px 20px rgba(245,200,66,0.3);}
.big-toggle:active{transform:scale(0.97);}

.meter-wrap{margin-top:18px;}
.meter-label{display:flex;justify-content:space-between;font-size:11px;color:var(--muted);margin-bottom:8px;}
.meter-track{height:8px;background:var(--surface2);border-radius:99px;overflow:hidden;border:1px solid var(--border);}
.meter-fill{height:100%;border-radius:99px;background:linear-gradient(90deg,var(--accent2),var(--accent));
  transition:width 0.5s ease;min-width:4px;}

.row{display:flex;gap:12px;}
.mini-card{flex:1;background:var(--surface2);border:1px solid var(--border);border-radius:14px;padding:16px;}
.mini-val{font-family:'Syne',sans-serif;font-size:24px;font-weight:800;margin-bottom:4px;}
.mini-lbl{font-size:10px;color:var(--muted);text-transform:uppercase;letter-spacing:1px;}

.toggle-row{display:flex;justify-content:space-between;align-items:center;padding:12px 0;
  border-bottom:1px solid var(--border);}
.toggle-row:last-child{border-bottom:none;}
.toggle-name{font-size:13px;}
.toggle-sub{font-size:11px;color:var(--muted);margin-top:2px;}
.switch{position:relative;width:46px;height:26px;flex-shrink:0;}
.switch input{opacity:0;width:0;height:0;}
.slider{position:absolute;inset:0;background:var(--surface2);border:1px solid var(--border);
  border-radius:99px;cursor:pointer;transition:0.25s;}
.slider::before{content:'';position:absolute;width:18px;height:18px;left:3px;top:3px;
  background:var(--muted);border-radius:50%;transition:0.25s;}
input:checked+.slider{background:var(--accent);border-color:var(--accent);}
input:checked+.slider::before{transform:translateX(20px);background:#0a0a0f;}

.wifi-form input{width:100%;background:var(--surface2);border:1px solid var(--border);border-radius:10px;
  padding:12px 14px;color:var(--text);font-family:'DM Mono',monospace;font-size:13px;margin-bottom:10px;outline:none;}
.wifi-form input:focus{border-color:var(--accent);}
.wifi-form button{width:100%;padding:13px;background:var(--accent2);color:#fff;border:none;border-radius:10px;
  font-family:'Syne',sans-serif;font-weight:700;font-size:14px;cursor:pointer;transition:0.2s;}
.wifi-form button:active{transform:scale(0.98);opacity:0.9;}

.log{background:var(--surface2);border:1px solid var(--border);border-radius:10px;
  padding:12px;font-size:11px;color:var(--muted);line-height:1.8;max-height:120px;overflow-y:auto;}
.log span{color:var(--accent);}

.badge{display:inline-block;padding:3px 10px;border-radius:99px;font-size:10px;font-weight:600;
  text-transform:uppercase;letter-spacing:1px;}
.badge.auto{background:rgba(74,222,128,0.1);color:var(--green);border:1px solid rgba(74,222,128,0.3);}
.badge.manual{background:rgba(245,200,66,0.1);color:var(--accent);border:1px solid rgba(245,200,66,0.3);}
</style>
</head>
<body>
<div class="wrap">

  <div class="header">
    <div class="header-icon">💡</div>
    <div>
      <h1>Smart Light</h1>
      <p id="ip-label">ESP32</p>
    </div>
  </div>

  <div class="status-bar">
    <div>
      <div class="status-label">Trạng thái</div>
      <div style="display:flex;align-items:center;gap:8px;margin-top:4px;">
        <div class="status-dot" id="dot"></div>
        <span class="status-val" id="state-txt">--</span>
        <span class="badge" id="mode-badge">--</span>
      </div>
    </div>
    <div style="text-align:right;">
      <div class="status-label">Ánh sáng</div>
      <div class="status-val" style="margin-top:4px;" id="lux-val">-- ADC</div>
    </div>
  </div>

  <div class="card">
    <div class="card-title">Điều khiển đèn chính</div>
    <button class="big-toggle off" id="main-btn" onclick="toggleMain()">⬤ TẮT</button>
    <div class="meter-wrap">
      <div class="meter-label">
        <span>Cảm biến ánh sáng</span>
        <span id="meter-pct">0%</span>
      </div>
      <div class="meter-track"><div class="meter-fill" id="meter-fill" style="width:0%"></div></div>
    </div>
  </div>

  <div class="row">
    <div class="mini-card">
      <div class="mini-val" id="lux-big">0</div>
      <div class="mini-lbl">ADC Raw</div>
    </div>
    <div class="mini-card">
      <div class="mini-val" id="threshold-val">2000</div>
      <div class="mini-lbl">Ngưỡng tối</div>
    </div>
    <div class="mini-card">
      <div class="mini-val" id="ind-val">--</div>
      <div class="mini-lbl">Indicator</div>
    </div>
  </div>

  <div class="card" style="margin-top:16px;">
    <div class="card-title">Cài đặt</div>
    <div class="toggle-row">
      <div>
        <div class="toggle-name">Auto Mode</div>
        <div class="toggle-sub">Tự bật khi trời tối</div>
      </div>
      <label class="switch">
        <input type="checkbox" id="auto-sw" onchange="setAuto(this.checked)" checked>
        <span class="slider"></span>
      </label>
    </div>
    <div class="toggle-row">
      <div>
        <div class="toggle-name">Đèn Indicator (LED2)</div>
        <div class="toggle-sub">Đèn trạng thái phụ</div>
      </div>
      <label class="switch">
        <input type="checkbox" id="ind-sw" onchange="setIndicator(this.checked)">
        <span class="slider"></span>
      </label>
    </div>
  </div>

  <div class="card">
    <div class="card-title">Kết nối WiFi</div>
    <div class="wifi-form">
      <input type="text" id="ssid" placeholder="Tên WiFi (SSID)">
      <input type="password" id="pass" placeholder="Mật khẩu WiFi">
      <button onclick="saveWifi()">💾 LƯU & KẾT NỐI</button>
    </div>
  </div>

  <div class="card">
    <div class="card-title">Activity Log</div>
    <div class="log" id="log">
      <span>[BOOT]</span> Smart Light khởi động...<br>
    </div>
  </div>

</div>

<script>
let logs = [];
function addLog(msg){
  const now = new Date();
  const t = now.toTimeString().slice(0,8);
  logs.unshift('<span>['+t+']</span> '+msg);
  if(logs.length>20) logs.pop();
  document.getElementById('log').innerHTML = logs.join('<br>');
}

function toggleMain(){
  fetch('/toggle').then(r=>r.json()).then(d=>{
    updateUI(d);
    addLog('Đèn '+(d.led?'BẬT':'TẮT')+' (manual)');
  });
}
function setAuto(v){
  fetch('/auto?state='+(v?1:0)).then(r=>r.json()).then(d=>{
    updateUI(d);
    addLog('Auto mode: '+(v?'BẬT':'TẮT'));
  });
}
function setIndicator(v){
  fetch('/indicator?state='+(v?1:0)).then(r=>r.json()).then(d=>{
    addLog('Indicator LED: '+(v?'BẬT':'TẮT'));
  });
}
function saveWifi(){
  const s=document.getElementById('ssid').value;
  const p=document.getElementById('pass').value;
  if(!s){alert('Nhập SSID!');return;}
  fetch('/wifi?ssid='+encodeURIComponent(s)+'&pass='+encodeURIComponent(p))
    .then(r=>r.json()).then(d=>{
      addLog('Đã lưu WiFi: '+s+' — đang kết nối...');
    });
}
function updateUI(d){
  const btn = document.getElementById('main-btn');
  const dot  = document.getElementById('dot');
  const txt  = document.getElementById('state-txt');
  const badge= document.getElementById('mode-badge');
  if(d.led){
    btn.className='big-toggle on'; btn.textContent='● ĐANG BẬT';
    dot.className='status-dot on'; txt.textContent='Đèn BẬT';
  } else {
    btn.className='big-toggle off'; btn.textContent='⬤ TẮT';
    dot.className='status-dot'; txt.textContent='Đèn TẮT';
  }
  if(d.auto){
    badge.textContent='AUTO'; badge.className='badge auto';
  } else {
    badge.textContent='MANUAL'; badge.className='badge manual';
  }
  const raw  = d.light||0;
  const pct  = Math.min(100,Math.round(raw/4095*100));
  document.getElementById('lux-val').textContent  = raw+' ADC';
  document.getElementById('lux-big').textContent  = raw;
  document.getElementById('meter-pct').textContent= pct+'%';
  document.getElementById('meter-fill').style.width= pct+'%';
  document.getElementById('auto-sw').checked = !!d.auto;
  document.getElementById('ind-sw').checked  = !!d.indicator;
  document.getElementById('ind-val').textContent   = d.indicator?'ON':'OFF';
}

function poll(){
  fetch('/status').then(r=>r.json()).then(d=>{
    updateUI(d);
  }).catch(()=>{});
}
poll();
setInterval(poll,2000);
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

void handleStatus() {
  bool indState = digitalRead(LED_INDICATOR);
  String json = "{\"led\":" + String(ledMainState?1:0) +
                ",\"auto\":" + String(autoMode?1:0) +
                ",\"light\":" + String(lightValue) +
                ",\"indicator\":" + String(indState?1:0) + "}";
  server.send(200, "application/json", json);
}

void handleToggle() {
  autoMode = false;
  manualOverride = true;
  ledMainState = !ledMainState;
  digitalWrite(LED_MAIN, ledMainState ? HIGH : LOW);
  handleStatus();
}

void handleAuto() {
  if (server.hasArg("state")) {
    autoMode = server.arg("state") == "1";
    if (autoMode) manualOverride = false;
  }
  handleStatus();
}

void handleIndicator() {
  if (server.hasArg("state")) {
    bool v = server.arg("state") == "1";
    digitalWrite(LED_INDICATOR, v ? HIGH : LOW);
  }
  handleStatus();
}


void readSensor() {
  if (millis() - lastSensorRead < 1000) return;
  lastSensorRead = millis();
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(LIGHT_SENSOR);
    delay(2);
  }
  lightValue = sum / 10;
  if (autoMode && !manualOverride) {
    bool shouldOn = lightValue < LIGHT_THRESHOLD;
    if (shouldOn != ledMainState) {
      ledMainState = shouldOn;
      digitalWrite(LED_MAIN, ledMainState ? HIGH : LOW);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_MAIN, OUTPUT);
  pinMode(LED_INDICATOR, OUTPUT);
  digitalWrite(LED_MAIN, LOW);
  digitalWrite(LED_INDICATOR, HIGH);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nIP: " + WiFi.localIP().toString());

  server.on("/",          handleRoot);
  server.on("/status",    handleStatus);
  server.on("/toggle",    handleToggle);
  server.on("/auto",      handleAuto);
  server.on("/indicator", handleIndicator);
  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();
  readSensor();
}
