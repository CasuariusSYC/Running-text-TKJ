# RUNNING TEXT P10 DENGAN ESP32

**NOTE: KODE INI HANYA AKAN BEKERJA DENGAN ESP32 VERSI 2.0.1 - 2.0.4** dikarenakan library DMD32 yang digunakan tidak kompatibel dengan versi lainnya

Project ini saya buat sebagai materi pembelajaran di jurusan TKJ dan Teknik Elektronika pada SMKN 2 Manokwari.

---

## DESKRIPSI ALAT

Project ini menggunakan:
- **Panel P10** sebagai display running text
- **ESP32** sebagai controller
- **Power Supply 5V 45A** untuk menyalakan alat

---

## FITUR

User dapat mengupdate teks running text melalui website dengan cara menghubungkan ke WiFi ESP32 yang dikonfigurasi sebagai **Access Point (AP)**.

---

## SKEMATIK

![Skematik Koneksi Panel P10 ke ESP32](https://github.com/user-attachments/assets/d35f1841-e8db-4d7a-a492-f5e718bcd66c)

**Catatan:** Konfigurasi di atas adalah standar dari panel P10 ke ESP32. Pin sudah dikonfigurasi melalui library, sehingga jika ingin memindahkan pin harus berurusan langsung dengan library.

---

## KONEKSI PIN PANEL P10 KE ESP32

Berikut adalah koneksi standar yang digunakan oleh library DMD32 pada ESP32 :

| Panel P10 | Pin ESP32 | Keterangan |
|-----------|-----------|------------|
| CLK       | GPIO 18   | SPI Clock (SCK) |
| DATA (R)  | GPIO 23   | SPI MOSI (Master Out) |
| OE (nOE)  | GPIO 22   | Output Enable (aktif low) |
| A (RS)    | GPIO 19   | Row select bit 0 |
| B         | GPIO 21   | Row select bit 1 |
| SCLK (LAT)| GPIO 2    | Latch / Strobe |
| GND       | GND       | Ground |
| VCC       | 5V        | Power (dari power supply eksternal) |

**Catatan Penting:** Panel P10 membutuhkan daya yang besar (hingga 2-4A per panel). Jangan menyuplai daya dari pin 5V ESP32, gunakan power supply eksternal 5V 45A seperti yang disebutkan di atas .

---

## PENGATURAN ESP32 UNTUK DMD32

### Pemilihan Board

Karena library DMD32 hanya kompatibel dengan ESP32 versi 2.0.1 - 2.0.4, maka perlu mengatur versi board yang sesuai di Arduino IDE:

1. Buka **File > Preferences**
2. Tambahkan URL berikut di **Additional Boards Manager URLs**:
   ```
   https://dl.espressif.com/dl/package_esp32_index.json
   ```
3. Buka **Tools > Board > Boards Manager**
4. Cari "ESP32" dan install versi **2.0.4**
5. Pilih board **ESP32 Dev Module**

---

# PENJELASAN LIBRARY YANG DIGUNAKAN

## DMD32 (Dot Matrix Display Library untuk ESP32)

**Library bawaan:** `#include <DMD32.h>`  
**Penulis:** Khudhur Abdullah Alfarhan  
**Platform:** ESP32  
**Repository:** [Qudor-Engineer/DMD32](https://github.com/Qudor-Engineer/DMD32)

### Deskripsi Umum

Library DMD32 adalah library untuk ESP32 yang digunakan mengendalikan **DMD (Dot Matrix Display)** seperti panel LED P10. Library ini memanfaatkan **SPI (Serial Peripheral Interface)** untuk mengirim data ke panel dengan cepat dan efisien .

Panel P10 adalah **modul LED dot matrix 32x16 piksel** (32 kolom x 16 baris) yang biasanya digunakan untuk running text, papan skor, atau display informasi publik. Jika menggunakan lebih dari satu panel, dapat disusun secara horizontal (across) maupun vertikal (down).

### Karakteristik Panel P10

| Karakteristik | Nilai |
|---------------|-------|
| Resolusi per panel | 32 x 16 piksel |
| Jenis | Monokrom (merah) atau RGB |
| Scan mode | 1/4 scan |
| Interface | HUB12 / HUB75 |
| Tegangan | 5V DC |
| Daya maksimal | ~2-4A per panel |

---

## Fungsi dan Parameter DMD32

### Inisialisasi DMD

```cpp
#include <DMD32.h>
#include "fonts/SystemFont5x7.h"
#include "fonts/Arial_Black_16_ISO_8859_1.h"

#define DISPLAYS_ACROSS 1  // Jumlah panel horizontal
#define DISPLAYS_DOWN 1    // Jumlah panel vertikal
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);
```

### Timer untuk Scanning (Wajib untuk ESP32)

Library DMD32 membutuhkan **hardware timer** untuk melakukan refresh display secara periodik. Timer ini harus dikonfigurasi dengan benar agar panel dapat menampilkan gambar .

**Syntax:**
```cpp
hw_timer_t * timer = NULL;

void IRAM_ATTR triggerScan() {
  dmd.scanDisplayBySPI();
}

void setup() {
  uint8_t cpuClock = ESP.getCpuFreqMHz();
  timer = timerBegin(0, cpuClock, true);
  timerAttachInterrupt(timer, &triggerScan, true);
  timerAlarmWrite(timer, 300, true);  // Interval 300 mikrodetik
  timerAlarmEnable(timer);
}
```

**Parameter timerAlarmWrite:**
- `300` – Interval dalam mikrodetik (semakin kecil, semakin cepat refresh)
- Nilai yang umum digunakan: 200-500 mikrodetik

### dmd.clearScreen()

**Deskripsi:** Membersihkan layar (menonaktifkan semua LED).

**Syntax:**
```cpp
dmd.clearScreen(clearType);
```

**Parameter:**
- `true` – Mematikan semua LED (normal)
- `false` – Menyalakan semua LED (negative)

### dmd.selectFont()

**Deskripsi:** Memilih font yang akan digunakan untuk menampilkan teks.

**Syntax:**
```cpp
dmd.selectFont(&fontName);
```

**Font bawaan yang tersedia:**
- `SystemFont5x7` – Font dasar ukuran 5x7 piksel
- `Arial_Black_16_ISO_8859_1` – Font Arial ukuran 16

### dmd.drawString()

**Deskripsi:** Menampilkan string teks statis pada posisi tertentu.

**Syntax:**
```cpp
dmd.drawString(x, y, teks, panjang, graphicsMode);
```

**Parameter:**
- `x` – Posisi horizontal (0-31 per panel)
- `y` – Posisi vertikal (0-15 per panel)
- `teks` – String yang akan ditampilkan
- `panjang` – Panjang string (dalam karakter)
- `graphicsMode` – Mode grafis (`GRAPHICS_NORMAL`)

**Contoh:**
```cpp
dmd.selectFont(SystemFont5x7);
dmd.drawString(0, 0, "Hello World", 11, GRAPHICS_NORMAL);
```

### dmd.drawMarquee()

**Deskripsi:** Menampilkan teks berjalan (running text / marquee).

**Syntax:**
```cpp
dmd.drawMarquee(teks, panjang, lebar, posisiAwal);
```

**Parameter:**
- `teks` – String yang akan ditampilkan
- `panjang` – Panjang string
- `lebar` – Lebar area marquee (biasanya `32 * DISPLAYS_ACROSS`)
- `posisiAwal` – Posisi awal teks

### dmd.stepMarquee()

**Deskripsi:** Menggerakkan teks marquee satu langkah (dipanggil dalam loop).

**Syntax:**
```cpp
dmd.stepMarquee(arah, posisiY);
```

**Parameter:**
- `arah` – `-1` untuk bergerak ke kiri, `1` untuk ke kanan
- `posisiY` – Posisi vertikal teks

### dmd.drawPixel()

**Deskripsi:** Menyalakan atau mematikan satu piksel pada koordinat tertentu.

**Syntax:**
```cpp
dmd.writePixel(x, y, graphicsMode, nilai);
```

**Parameter:**
- `x` – Koordinat X (0-31 per panel)
- `y` – Koordinat Y (0-15 per panel)
- `graphicsMode` – `GRAPHICS_NORMAL`
- `nilai` – `1` untuk menyalakan, `0` untuk mematikan

---

## Contoh Program DMD32

### Contoh 1: Running Text Dasar

Program ini menampilkan teks berjalan dari kanan ke kiri .

```cpp
#include <DMD32.h>
#include "fonts/SystemFont5x7.h"
#include "fonts/Arial_Black_16_ISO_8859_1.h"

#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1

DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

hw_timer_t * timer = NULL;

void IRAM_ATTR triggerScan() {
  dmd.scanDisplayBySPI();
}

void setup() {
  uint8_t cpuClock = ESP.getCpuFreqMHz();
  timer = timerBegin(0, cpuClock, true);
  timerAttachInterrupt(timer, &triggerScan, true);
  timerAlarmWrite(timer, 300, true);
  timerAlarmEnable(timer);

  dmd.clearScreen(true);
  Serial.begin(115200);
}

void loop() {
  dmd.clearScreen(true);
  dmd.selectFont(Arial_Black_16_ISO_8859_1);
  
  const char msg[] = "SMKN 2 MANOKWARI - TEKNIK ELEKTRONIKA & TKJ ";
  dmd.drawMarquee(msg, strlen(msg), (32 * DISPLAYS_ACROSS) - 1, 0);
  
  long start = millis();
  long timerMarquee = start;
  
  while (true) {
    if ((timerMarquee + 30) < millis()) {
      dmd.stepMarquee(-1, 0);
      timerMarquee = millis();
    }
  }
}
```

### Contoh 2: Menampilkan Variabel (Suhu, Kelembaban, Skor)

Untuk menampilkan nilai variabel seperti suhu atau skor, gunakan fungsi `sprintf()` untuk menggabungkan string dengan angka .

```cpp
#include <DMD32.h>
#include "fonts/SystemFont5x7.h"

#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1

DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

hw_timer_t * timer = NULL;

void IRAM_ATTR triggerScan() {
  dmd.scanDisplayBySPI();
}

int suhu = 28;
int kelembaban = 65;

void setup() {
  uint8_t cpuClock = ESP.getCpuFreqMHz();
  timer = timerBegin(0, cpuClock, true);
  timerAttachInterrupt(timer, &triggerScan, true);
  timerAlarmWrite(timer, 300, true);
  timerAlarmEnable(timer);

  dmd.clearScreen(true);
  dmd.selectFont(SystemFont5x7);
  Serial.begin(115200);
}

void loop() {
  dmd.clearScreen(true);
  
  // Menampilkan suhu
  char suhuMsg[10];
  sprintf(suhuMsg, "%d C", suhu);
  dmd.drawString(0, 0, suhuMsg, strlen(suhuMsg), GRAPHICS_NORMAL);
  
  // Menampilkan kelembaban
  char humMsg[10];
  sprintf(humMsg, "%d %%", kelembaban);
  dmd.drawString(0, 9, humMsg, strlen(humMsg), GRAPHICS_NORMAL);
  
  delay(2000);
}
```

### Contoh 3: Multiple Panel (2x1 - Dua Panel Horizontal)

```cpp
#include <DMD32.h>
#include "fonts/Arial_Black_16_ISO_8859_1.h"

#define DISPLAYS_ACROSS 2  // Dua panel horizontal
#define DISPLAYS_DOWN 1    // Satu panel vertikal

DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

hw_timer_t * timer = NULL;

void IRAM_ATTR triggerScan() {
  dmd.scanDisplayBySPI();
}

void setup() {
  uint8_t cpuClock = ESP.getCpuFreqMHz();
  timer = timerBegin(0, cpuClock, true);
  timerAttachInterrupt(timer, &triggerScan, true);
  timerAlarmWrite(timer, 300, true);
  timerAlarmEnable(timer);

  dmd.clearScreen(true);
  
  // Total lebar = 32 x 2 = 64 piksel
  char msg[] = "WELCOME TO SMKN 2 MANOKWARI ";
  dmd.selectFont(Arial_Black_16_ISO_8859_1);
  dmd.drawMarquee(msg, strlen(msg), (32 * DISPLAYS_ACROSS) - 1, 0);
}

void loop() {
  static long timerMarquee = 0;
  if (millis() - timerMarquee >= 30) {
    dmd.stepMarquee(-1, 0);
    timerMarquee = millis();
  }
}
```

---

## EEPROM (Electrically Erasable Programmable Read-Only Memory)

**Library bawaan:** `#include <EEPROM.h>`

### Deskripsi Umum

EEPROM adalah jenis memori yang dapat ditulis dan dihapus secara elektrik, serta bersifat **non-volatile** (data tidak hilang saat daya dimatikan).

Pada ESP32, library EEPROM sebenarnya adalah **emulasi di dalam flash memory**, bukan memori EEPROM fisik yang terpisah. Hal ini penting karena mempengaruhi cara kerja dan daya tahan memori.

### Karakteristik Penting

| Karakteristik | Nilai |
|---------------|-------|
| Ukuran default ESP32 | 512 byte |
| Ukuran Arduino Uno | 1024 byte |
| Daya tahan tulis | ~100.000 - 1.000.000 siklus |
| Jenis data | 1 byte per alamat (0-255) |

**Peringatan:** Flash memory memiliki batasan jumlah siklus tulis. Setiap kali melakukan `EEPROM.commit()`, seluruh blok memori ditulis ulang, bukan hanya byte yang diubah.

---

## Fungsi dan Parameter EEPROM

### EEPROM.begin()

**Deskripsi:** Menginisialisasi EEPROM dengan ukuran tertentu. Fungsi ini **wajib** dipanggil sebelum operasi EEPROM lainnya.

**Parameter:** `size` – Jumlah byte yang akan dialokasikan (misalnya 512 atau 1024)

**Contoh:**
```cpp
#include <EEPROM.h>
#define EEPROM_SIZE 512

void setup() {
  EEPROM.begin(EEPROM_SIZE);
}
```

### EEPROM.read()

**Deskripsi:** Membaca 1 byte data dari alamat yang ditentukan.

**Parameter:** `address` – Alamat memori (dimulai dari 0)

**Returns:** `byte` – Nilai 0-255

**Contoh:**
```cpp
byte value = EEPROM.read(0);
```

### EEPROM.write()

**Deskripsi:** Menulis 1 byte data ke buffer EEPROM. **Data belum tersimpan permanen** sampai `commit()` dipanggil.

**Parameter:**
- `address` – Alamat tujuan
- `value` – Nilai yang akan ditulis (0-255)

**Contoh:**
```cpp
EEPROM.write(0, 255);
EEPROM.commit(); // JANGAN LUPA commit!
```

### EEPROM.get()

**Deskripsi:** Membaca data dengan tipe kompleks (int, float, atau struct) dari EEPROM.

**Parameter:**
- `address` – Alamat awal data
- `variable` – Variabel tujuan

**Contoh:**
```cpp
struct Config {
  float suhu;
  int kelembaban;
};
Config myConfig;
EEPROM.get(0, myConfig);
```

### EEPROM.put()

**Deskripsi:** Menyimpan data kompleks ke EEPROM. Lebih efisien dibanding `write()` karena hanya menulis jika data berubah.

**Parameter:**
- `address` – Alamat awal penyimpanan
- `data` – Data yang akan disimpan

**Contoh:**
```cpp
EEPROM.put(0, myConfig);
EEPROM.commit();
```

### EEPROM.commit()

**Deskripsi:** Menyimpan semua perubahan dari buffer ke flash memory secara permanen. **Ini adalah fungsi paling penting di ESP32**—tanpa ini, data yang ditulis tidak akan benar-benar tersimpan.

**Returns:** `bool` – `true` jika berhasil disimpan

### EEPROM.end()

**Deskripsi:** Mengakhiri penggunaan EEPROM dan membebaskan resource. Juga akan melakukan commit otomatis.

### EEPROM.length()

**Deskripsi:** Mengetahui ukuran EEPROM yang telah dialokasikan.

**Returns:** `int` – Jumlah byte yang tersedia

---

## WiFi Library untuk ESP32 (Arduino Framework)

**Library bawaan:** `#include <WiFi.h>`

### Deskripsi Umum

Library `WiFi.h` untuk ESP32 adalah implementasi dari **ESP-IDF WiFi driver** yang telah dibungkus agar kompatibel dengan sintaks Arduino. Library ini menyediakan fungsi lengkap untuk menghubungkan ESP32 ke jaringan WiFi, baik sebagai **Station (client)** maupun **Access Point (server)**.

### Mode Operasi yang Didukung

- **Station Mode (STA)** – ESP32 bertindak sebagai client yang terhubung ke jaringan WiFi (router)
- **Soft-AP Mode (AP)** – ESP32 bertindak sebagai access point, perangkat lain dapat terhubung langsung
- **AP-STA Coexistence Mode** – ESP32 bertindak sebagai client sekaligus access point secara bersamaan

### Keamanan yang Didukung

- WPA/WPA2 Personal (PSK)
- WPA3 (pada chip yang mendukung)
- WEP
- Open network (tanpa sandi)
- WPA2 Enterprise (pada beberapa varian chip)

### Perbedaan dengan Library WiFi Shield Arduino

| Aspek | WiFi Shield Arduino | ESP32 Built-in WiFi |
|-------|---------------------|---------------------|
| Library | `#include <WiFi.h>` (untuk shield) | `#include <WiFi.h>` (untuk ESP32) |
| Fitur | Terbatas | Lengkap (AP+STA, scanning, promiscuous) |
| Kecepatan | 2.4 GHz, kecepatan terbatas | 2.4 GHz, throughput hingga 30 Mbps |
| Mode | Station + AP terpisah | Station, AP, atau keduanya simultan |

**Catatan penting:** Meskipun menggunakan `#include <WiFi.h>` yang sama, library untuk ESP32 memiliki **fungsi tambahan** yang tidak tersedia pada library WiFi Shield. Pastikan Anda menggunakan board ESP32 saat mengkompilasi kode.

---

## Fungsi Dasar WiFi (Station Mode)

### WiFi.begin()

**Deskripsi:** Menghubungkan ESP32 ke jaringan WiFi sebagai Station.

**Syntax:**
```cpp
WiFi.begin(ssid, password);
```

**Parameter:**
- `ssid` – Nama jaringan WiFi
- `password` – Sandi jaringan (kosongkan untuk jaringan terbuka)

**Contoh:**
```cpp
#include <WiFi.h>

const char* ssid = "NamaJaringan";
const char* password = "SandiJaringan";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("Terhubung!");
  Serial.println(WiFi.localIP());
}
```

### WiFi.status()

**Deskripsi:** Mengembalikan status koneksi WiFi saat ini.

**Returns:** 
- `WL_CONNECTED` – Terhubung ke AP
- `WL_NO_SSID_AVAIL` – SSID tidak ditemukan
- `WL_CONNECT_FAILED` – Gagal terhubung
- `WL_DISCONNECTED` – Terputus
- `WL_IDLE_STATUS` – Sedang proses

### WiFi.disconnect()

**Deskripsi:** Memutuskan koneksi WiFi dari AP yang sedang terhubung.

**Returns:** `true` jika berhasil, `false` jika gagal

### WiFi.reconnect()

**Deskripsi:** Memaksa ESP32 untuk menyambung ulang ke AP terakhir yang dikonfigurasi.

### WiFi.localIP()

**Deskripsi:** Mendapatkan alamat IP yang diberikan oleh DHCP (atau statis) pada ESP32.

**Returns:** Objek `IPAddress`

### WiFi.macAddress()

**Deskripsi:** Mendapatkan alamat MAC dari interface WiFi ESP32.

**Returns:** `String`

### WiFi.RSSI()

**Deskripsi:** Mendapatkan kekuatan sinyal (Received Signal Strength Indicator) dari AP yang terhubung.

**Returns:** `long` – nilai RSSI dalam dBm (semakin mendekati 0, semakin bagus)

### WiFi.SSID()

**Deskripsi:** Mendapatkan nama SSID dari AP yang sedang terhubung.

**Returns:** `String`

### WiFi.BSSID()

**Deskripsi:** Mendapatkan alamat MAC (BSSID) dari AP yang terhubung.

---

## Fungsi Station Mode Lanjutan

### WiFi.config()

**Deskripsi:** Mengatur alamat IP statis (non-DHCP). Harus dipanggil **sebelum** `WiFi.begin()`

**Syntax:**
```cpp
WiFi.config(local_ip, gateway, subnet, dns1, dns2);
```

**Contoh:**
```cpp
IPAddress local_IP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WiFi.config(local_IP, gateway, subnet);
WiFi.begin(ssid, password);
```

### WiFi.setAutoConnect()

**Deskripsi:** Mengatur apakah ESP32 akan otomatis menyambung ke AP yang dikenal saat startup.

### WiFi.setAutoReconnect()

**Deskripsi:** Mengatur apakah ESP32 akan otomatis menyambung ulang jika koneksi terputus.

### Event Handler (onEvent)

**Deskripsi:** Mendaftarkan fungsi callback untuk menangani event WiFi.

**Event yang tersedia:**
- `ARDUINO_EVENT_WIFI_STA_CONNECTED` – Terhubung ke AP
- `ARDUINO_EVENT_WIFI_STA_DISCONNECTED` – Terputus dari AP
- `ARDUINO_EVENT_WIFI_STA_GOT_IP` – Berhasil mendapat IP

**Contoh:**
```cpp
void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("WiFi Connected!");
  Serial.println(WiFi.localIP());
}

void setup() {
  WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.begin(ssid, password);
}
```

---

## Fungsi Scan Jaringan

### WiFi.scanNetworks()

**Deskripsi:** Memindai semua jaringan WiFi yang tersedia di sekitar.

**Syntax:**
```cpp
WiFi.scanNetworks();
WiFi.scanNetworks(async); // async = true untuk non-blocking
```

**Returns:** `int16_t` – jumlah jaringan yang ditemukan

**Contoh:**
```cpp
int jumlahJaringan = WiFi.scanNetworks();
for (int i = 0; i < jumlahJaringan; i++) {
  Serial.print(WiFi.SSID(i));
  Serial.print(" (");
  Serial.print(WiFi.RSSI(i));
  Serial.println(" dBm)");
}
WiFi.scanDelete();
```

### WiFi.scanDelete()

**Deskripsi:** Menghapus hasil scan dari memori untuk menghemat RAM.

---

## Fungsi Access Point (Soft-AP Mode)

### WiFi.softAP()

**Deskripsi:** Mengaktifkan ESP32 sebagai Access Point.

**Syntax:**
```cpp
WiFi.softAP(ssid);
WiFi.softAP(ssid, password);
WiFi.softAP(ssid, password, channel, ssid_hidden, max_connection);
```

**Parameter:**
- `ssid` – Nama jaringan yang akan dipancarkan
- `password` – Sandi (minimal 8 karakter untuk WPA2)
- `channel` – Kanal WiFi (1-13, default 1)
- `ssid_hidden` – `true` untuk menyembunyikan SSID
- `max_connection` – Maksimal client (1-8, default 4)

**Contoh:**
```cpp
WiFi.softAP("ESP32_Network", "12345678");
Serial.print("IP AP: ");
Serial.println(WiFi.softAPIP());
```

### WiFi.softAPIP()

**Deskripsi:** Mendapatkan alamat IP dari interface Access Point (default 192.168.4.1).

### WiFi.softAPmacAddress()

**Deskripsi:** Mendapatkan alamat MAC dari interface Access Point.

### WiFi.softAPdisconnect()

**Deskripsi:** Menonaktifkan mode Access Point.

---

## Fungsi WiFi Multi (Multi SSID)

**Deskripsi:** ESP32 dapat menyimpan hingga 5 SSID berbeda dan akan terhubung ke yang terkuat.

**Contoh:**
```cpp
#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti wifiMulti;

void setup() {
  wifiMulti.addAP("SSID_1", "password1");
  wifiMulti.addAP("SSID_2", "password2");
  
  while(wifiMulti.run() != WL_CONNECTED) {
    delay(500);
  }
}
```

---

## Contoh Program Lengkap dengan Tiga Library

Berikut adalah contoh program yang menggabungkan **DMD32** (untuk P10), **EEPROM** (untuk menyimpan teks), dan **WiFi** (untuk update teks via AP):

```cpp
#include <WiFi.h>
#include <EEPROM.h>
#include <DMD32.h>
#include "fonts/SystemFont5x7.h"
#include "fonts/Arial_Black_16_ISO_8859_1.h"

// ========== KONFIGURASI DMD32 ==========
#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

hw_timer_t * timer = NULL;

void IRAM_ATTR triggerScan() {
  dmd.scanDisplayBySPI();
}

// ========== KONFIGURASI WiFi AP ==========
const char* ap_ssid = "P10_RunningText";
const char* ap_password = "12345678";

// ========== KONFIGURASI EEPROM ==========
#define EEPROM_SIZE 512
#define TEXT_ADDRESS 0
#define TEXT_MAX_LEN 256

char savedText[TEXT_MAX_LEN] = "SMKN 2 MANOKWARI";

// ========== WEBSERVER SEDERHANA ==========
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  
  // Inisialisasi EEPROM
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(TEXT_ADDRESS, savedText);
  
  // Inisialisasi DMD32 Timer
  uint8_t cpuClock = ESP.getCpuFreqMHz();
  timer = timerBegin(0, cpuClock, true);
  timerAttachInterrupt(timer, &triggerScan, true);
  timerAlarmWrite(timer, 300, true);
  timerAlarmEnable(timer);
  
  dmd.clearScreen(true);
  dmd.selectFont(Arial_Black_16_ISO_8859_1);
  
  // Inisialisasi WiFi Access Point
  WiFi.softAP(ap_ssid, ap_password);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
  
  server.begin();
}

void loop() {
  // Tampilkan running text
  dmd.clearScreen(true);
  dmd.drawMarquee(savedText, strlen(savedText), (32 * DISPLAYS_ACROSS) - 1, 0);
  
  static long lastMarquee = 0;
  if (millis() - lastMarquee >= 30) {
    dmd.stepMarquee(-1, 0);
    lastMarquee = millis();
  }
  
  // Handle web server untuk update teks
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();
    
    if (request.indexOf("POST /update") >= 0) {
      // Parse teks baru dari request
      int startIdx = request.indexOf("text=") + 5;
      int endIdx = request.indexOf(" ", startIdx);
      String newText = request.substring(startIdx, endIdx);
      newText.replace("+", " ");
      newText.toCharArray(savedText, TEXT_MAX_LEN);
      
      // Simpan ke EEPROM
      EEPROM.put(TEXT_ADDRESS, savedText);
      EEPROM.commit();
    }
    
    // Kirim response HTML
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<html><body>");
    client.println("<h2>Update Running Text</h2>");
    client.println("<form method='POST' action='/update'>");
    client.println("Teks: <input type='text' name='text'><br>");
    client.println("<input type='submit' value='Update'>");
    client.println("</form>");
    client.println("</body></html>");
    client.stop();
  }
}
```

---

## Catatan Penting

### Panel P10 & DMD32:
1. **Kompatibilitas ESP32** – Library DMD32 hanya kompatibel dengan ESP32 versi 2.0.1 - 2.0.4. Jangan gunakan versi yang lebih baru .
2. **Timer Wajib** – ESP32 membutuhkan hardware timer untuk scanning display, berbeda dengan Arduino yang menggunakan TimerOne .
3. **Power Supply** – Panel P10 membutuhkan daya besar. Gunakan power supply eksternal 5V, jangan dari pin ESP32 .
4. **SPI Conflict** – Pastikan tidak ada perangkat SPI lain yang menggunakan pin yang sama .

### EEPROM:
1. **Jangan lupa commit()** – Data tidak akan tersimpan permanen tanpa `EEPROM.commit()`
2. **Batasan ukuran** – ESP32 default 512 byte
3. **Wear leveling** – Hindari menulis terlalu sering dalam loop tanpa jeda
4. **Panggil begin() di setup** – Selalu inisialisasi EEPROM sebelum digunakan

### WiFi:
1. **Library untuk ESP32 adalah `#include <WiFi.h>`** – Berbeda dengan ESP8266
2. **Mode default** ESP32 adalah Station (client), bukan Access Point
3. **Penggunaan bersama Bluetooth** dapat mempengaruhi performa WiFi
4. **Pin yang digunakan** untuk komunikasi internal WiFi tidak perlu diatur manual

---

## Referensi

- [Dokumentasi DMD32](https://github.com/Qudor-Engineer/DMD32) 
- [Dokumentasi EEPROM Arduino](https://docs.arduino.cc/learn/programming/eeprom-guide)
- [ESP32 EEPROM Library (GitHub)](https://github.com/espressif/arduino-esp32/tree/master/libraries/EEPROM)
- [ESP-IDF WiFi Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_wifi.html)
- [Arduino-ESP32 WiFi Library](https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi)
