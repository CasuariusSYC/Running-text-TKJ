#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <DMD32.h>
#include "fonts/Arial_black_16.h"
#include "esp_task_wdt.h"

// ========== KONFIGURASI ==========
const char* ap_ssid     = "ESP32-RunningText";
const char* ap_password = "12345678";

IPAddress local_IP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

#define EEPROM_SIZE      512
#define MAX_TEXT_LENGTH  200
#define DISPLAYS_ACROSS  2
#define DISPLAYS_DOWN    1

DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

WebServer server(80);
hw_timer_t *timer = NULL;

String runningText = "";
int    xPos        = DISPLAYS_ACROSS * 32;

unsigned long lastScrollTime = 0;
unsigned long lastWebTime    = 0;

volatile bool pendingUpdate = false;
String        pendingText   = "";

// =====================================================================
// TIMER
// =====================================================================
void IRAM_ATTR triggerScan() {
  dmd.scanDisplayBySPI();
}

void timerPause() {
  timerAlarmDisable(timer);
}

void timerResume() {
  timerAlarmEnable(timer);
}

void initDMDTimer() {
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &triggerScan, true);
  timerAlarmWrite(timer, 2000, true);
  timerAlarmEnable(timer);
}

// =====================================================================
// WATCHDOG untuk versi 2.0.4 (di-nonaktifkan)
// =====================================================================
void disableWatchdog() {
  // Nonaktifkan Task WDT
  esp_task_wdt_deinit();
  
  // Untuk versi 2.0.4, tidak ada disableCore0WDT/disableCore1WDT
  // Jadi cukup deinit saja
  Serial.println("Watchdog disabled (ESP32 Core 2.0.4 compatible)");
}

// =====================================================================
// EEPROM
// =====================================================================
String readTextFromEEPROM() {
  int len = EEPROM.read(0);
  if (len <= 0 || len > MAX_TEXT_LENGTH)
    return "SELAMAT DATANG";

  String text = "";
  for (int i = 0; i < len; i++) {
    char c = EEPROM.read(i + 1);
    if (c == 0 || c == 255) break;
    text += c;
  }
  return text;
}

void writeTextToEEPROM(const String& text) {
  timerPause();
  delayMicroseconds(3000);

  for (int i = 0; i < MAX_TEXT_LENGTH + 2; i++) EEPROM.write(i, 0);
  EEPROM.write(0, text.length());
  for (int i = 0; i < (int)text.length(); i++) EEPROM.write(i + 1, text[i]);
  EEPROM.commit();

  timerResume();

  Serial.println("EEPROM saved: " + text);
}

// =====================================================================
// SCROLL
// =====================================================================
void scrollText() {
  if (millis() - lastScrollTime < 50) return;
  lastScrollTime = millis();

  dmd.clearScreen(true);
  dmd.selectFont(Arial_Black_16);

  if (runningText.length() > 0) {
    int textWidth    = runningText.length() * 9;
    int displayWidth = DISPLAYS_ACROSS * 32;

    dmd.drawString(xPos, 0, runningText.c_str(), runningText.length(), GRAPHICS_NORMAL);

    xPos--;
    if (xPos < -textWidth) xPos = displayWidth;
  }
}

// =====================================================================
// SETUP
// =====================================================================
void setup() {
  Serial.begin(115200);
  delay(500);

  disableWatchdog();  // ← Sekarang kompatibel dengan v2.0.4
  
  EEPROM.begin(EEPROM_SIZE);

  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("AP IP: " + WiFi.softAPIP().toString());

  initDMDTimer();
  dmd.clearScreen(true);
  dmd.selectFont(Arial_Black_16);

  runningText = readTextFromEEPROM();
  xPos = DISPLAYS_ACROSS * 32;
  Serial.println("Loaded: " + runningText);

  server.on("/", handleRoot);
  server.on("/update", HTTP_POST, handleUpdateText);
  server.onNotFound([]() { server.send(404, "text/plain", "Not found"); });
  server.begin();

  Serial.println("Ready: http://192.168.1.1");
  lastScrollTime = millis();
  lastWebTime    = millis();
}

// =====================================================================
// LOOP
// =====================================================================
void loop() {
  if (pendingUpdate) {
    pendingUpdate = false;

    runningText = pendingText;
    xPos        = DISPLAYS_ACROSS * 32;

    writeTextToEEPROM(runningText);

    dmd.clearScreen(true);
    dmd.selectFont(Arial_Black_16);

    Serial.println("Updated: " + runningText);
  }

  if (millis() - lastWebTime >= 10) {
    lastWebTime = millis();
    server.handleClient();
  }

  scrollText();
}

// =====================================================================
// WEB HANDLER
// =====================================================================
void handleRoot() {
  String html = F("<!DOCTYPE html><html><head>"
    "<meta charset='UTF-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>Running Text P10</title>"
    "<style>"
    "*{margin:0;padding:0;box-sizing:border-box}"
    "body{font-family:Arial,sans-serif;background:linear-gradient(135deg,#1a1a2e,#16213e);min-height:100vh;padding:20px}"
    ".card{max-width:480px;margin:0 auto;background:#fff;border-radius:20px;padding:28px;box-shadow:0 20px 40px rgba(0,0,0,.3)}"
    "h1{text-align:center;color:#16213e;margin-bottom:6px}"
    ".sub{text-align:center;color:#888;font-size:13px;margin-bottom:24px}"
    ".cur{background:#f0f8ff;border-left:4px solid #3498db;border-radius:8px;padding:14px;margin-bottom:20px}"
    ".cur b{display:block;margin-bottom:8px;color:#2c3e50}"
    "#curTxt{font-family:monospace;background:#fff;padding:8px;border-radius:6px;word-break:break-all}"
    "label{font-weight:bold;color:#2c3e50;display:block;margin-bottom:6px}"
    "textarea{width:100%;padding:10px;border:2px solid #ddd;border-radius:10px;font-size:14px;resize:vertical;min-height:90px;font-family:monospace}"
    "textarea:focus{outline:none;border-color:#3498db}"
    "button{margin-top:14px;width:100%;padding:14px;background:linear-gradient(135deg,#3498db,#2980b9);color:#fff;border:none;border-radius:10px;font-size:16px;font-weight:bold;cursor:pointer}"
    ".alert{margin-top:14px;padding:12px;border-radius:8px;display:none}"
    ".ok{background:#d4edda;color:#155724;border:1px solid #c3e6cb}"
    ".err{background:#f8d7da;color:#721c24;border:1px solid #f5c6cb}"
    "</style></head><body>"
    "<div class='card'>"
    "<h1>📟 Running Text P10</h1>"
    "<div class='sub'>ESP32 + 2x Panel P10</div>"
    "<div class='cur'><b>📝 Teks Saat Ini:</b><div id='curTxt'></div></div>"
    "<label>✏️ Teks Baru:</label>"
    "<textarea id='txt' maxlength='200'></textarea>"
    "<button id='btn' onclick='send()'>💾 Simpan & Tampilkan</button>"
    "<div id='alert' class='alert'></div>"
    "</div>"
    "<script>"
    "const cur=");

  html += "\"" + runningText + "\"";

  html += F(";"
    "document.getElementById('curTxt').textContent=cur;"
    "document.getElementById('txt').value=cur;"
    "function send(){"
      "const t=document.getElementById('txt').value.trim();"
      "if(!t){showAlert('Teks tidak boleh kosong!','err');return;}"
      "const btn=document.getElementById('btn');"
      "btn.disabled=true;btn.textContent='⏳ Menyimpan...';"
      "fetch('/update',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},"
        "body:'text='+encodeURIComponent(t)})"
      ".then(r=>{"
        "if(!r.ok)throw new Error('Gagal');"
        "return r.text();"
      "})"
      ".then(()=>{"
        "document.getElementById('curTxt').textContent=t;"
        "showAlert('✅ Berhasil disimpan!','ok');"
      "})"
      ".catch(()=>showAlert('❌ Gagal, coba lagi.','err'))"
      ".finally(()=>{btn.disabled=false;btn.textContent='💾 Simpan & Tampilkan';});"
    "}"
    "function showAlert(m,c){"
      "const a=document.getElementById('alert');"
      "a.textContent=m;a.className='alert '+c;a.style.display='block';"
      "setTimeout(()=>a.style.display='none',3000);"
    "}"
    "</script></body></html>");

  server.send(200, "text/html", html);
}

void handleUpdateText() {
  if (!server.hasArg("text") || server.arg("text").length() == 0) {
    server.send(400, "text/plain", "Teks kosong");
    return;
  }

  String newText = server.arg("text");
  if ((int)newText.length() > MAX_TEXT_LENGTH)
    newText = newText.substring(0, MAX_TEXT_LENGTH);

  server.send(200, "text/plain", "OK");

  pendingText   = newText;
  pendingUpdate = true;
}
