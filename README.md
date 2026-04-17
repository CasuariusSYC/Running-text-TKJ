# Running Text P10 with ESP32 
# NOTE: KODE INI HANYA AKAN BEKERJA DENGAN ESP32 VERSI 2.0.1 - 2.0.4 dikarenakan library DMD32 yang di gunakan tidak kompetible dengan versi lainnya
Project ini saya buat sebagai materi pembelajaran di jurusan TKJ dan Teknik Elektronika pada SMKN 2 Manokwari

## Deskripsi Alat

Project ini menggunakan:
- Panel P10 sebagai display running text
- ESP32 sebagai controller
- Power Supply 5V 45A untuk menyalakan alat

## Fitur

User dapat mengupdate teks running text melalui website dengan cara menghubungkan ke WiFi ESP32 yang dikonfigurasi sebagai Access Point (AP).

## Skematik

![Skematik Koneksi Panel P10 ke ESP32](https://github.com/user-attachments/assets/d35f1841-e8db-4d7a-a492-f5e718bcd66c)

Catatan: Konfigurasi di atas adalah standar dari panel P10 ke ESP32. Pin sudah dikonfigurasi melalui library, sehingga jika ingin memindahkan pin harus berurusan langsung dengan library.

# Penjelasan Library yang digunakan 



## EEPROMP (Electrically Erasable Programmable Read-Only Memory)
Pada Arduino khususnya di ESP32, EEPROM digunakan untuk menyimpan data yang tetap tersimpan walaupun perangkat dimatikan (non-volatile). Namun penting dipahami, pada ESP32 EEPROM bukan memori fisik terpisah, melainkan emulasi di dalam flash memory. Karena itu, cara kerjanya sedikit berbeda dibanding EEPROM asli, terutama harus menggunakan proses commit agar data benar-benar tersimpan.

## Fungsi dan Parameter EEPROM
### EEPROM.begin(size)
Fungsi ini digunakan untuk menginisialisasi EEPROM sebelum digunakan. Parameter size menentukan jumlah memori (dalam byte) yang ingin dialokasikan, misalnya 512 atau 1024. Fungsi ini wajib dipanggil di awal (biasanya di setup()), karena tanpa ini semua fungsi EEPROM tidak akan bekerja.

### EEPROM.read(address)
Fungsi ini digunakan untuk membaca 1 byte data dari EEPROM. Parameter address adalah alamat memori (dimulai dari 0). Nilai yang dikembalikan berupa angka 0–255 (tipe byte). Cocok untuk data sederhana seperti karakter atau nilai kecil.

### EEPROM.write(address, value)
Fungsi ini digunakan untuk menulis 1 byte data ke EEPROM. Parameter address adalah lokasi penyimpanan, dan value adalah data yang ingin disimpan (0–255). Fungsi ini hanya menulis ke buffer, belum menyimpan permanen.

### EEPROM.get(address, variable)
Fungsi ini digunakan untuk membaca data dengan tipe lebih kompleks (misalnya int, float, atau struct). Parameter address adalah lokasi awal data, dan variable adalah variabel tujuan. Fungsi ini akan langsung mengisi variabel tersebut sesuai tipe datanya.

### EEPROM.put(address, data)
Fungsi ini digunakan untuk menyimpan data kompleks ke EEPROM. Parameter address adalah lokasi awal, dan data adalah variabel yang ingin disimpan. Kelebihannya dibanding write adalah lebih efisien karena hanya menulis jika data berubah.

### EEPROM.commit()
Fungsi ini digunakan untuk menyimpan semua perubahan ke flash memory secara permanen. Tidak memiliki parameter. Ini adalah fungsi paling penting di ESP32—tanpa ini, data yang ditulis tidak akan benar-benar tersimpan.

### EEPROM.end()
Fungsi ini digunakan untuk mengakhiri penggunaan EEPROM dan membebaskan resource. Tidak memiliki parameter. Biasanya digunakan pada sistem yang lebih kompleks, tapi sering tidak wajib di project sederhana.

### EEPROM.length()
Fungsi ini digunakan untuk mengetahui ukuran EEPROM yang telah dialokasikan. Tidak memiliki parameter dan mengembalikan jumlah byte sesuai dengan nilai saat begin() dipanggil.



# WiFi Library untuk ESP32 (Arduino Framework)
**Source:
**Library bawaan:** #include <WiFi.h>  
**Platform:** ESP32 (ESP-WROOM-32, ESP32-S2, ESP32-S3, ESP32-C3, dll.)  
**Dasar:** Mengacu pada standar API WiFi dari ESP-IDF 

---

## Deskripsi Umum

Library `WiFi.h` untuk ESP32 pada Arduino Framework adalah implementasi dari **ESP-IDF WiFi driver** yang telah dibungkus agar kompatibel dengan sintaks Arduino . Library ini menyediakan fungsi lengkap untuk menghubungkan ESP32 ke jaringan WiFi, baik sebagai **Station (client)** maupun **Access Point (server)**.

## Mode Operasi yang Didukung

ESP32 mendukung beberapa mode operasi WiFi yang dapat diatur sesuai kebutuhan:

- **Station Mode (STA)** – ESP32 bertindak sebagai client yang terhubung ke jaringan WiFi (router) 
- **Soft-AP Mode (AP)** – ESP32 bertindak sebagai access point, perangkat lain dapat terhubung langsung 
- **AP-STA Coexistence Mode** – ESP32 bertindak sebagai client sekaligus access point secara bersamaan 

## Keamanan yang Didukung

- WPA/WPA2 Personal (PSK)
- WPA3 (pada chip yang mendukung)
- WEP
- Open network (tanpa sandi)
- WPA2 Enterprise (pada beberapa varian chip) 

## Perbedaan dengan Library WiFi Shield Arduino

| Aspek | WiFi Shield Arduino | ESP32 Built-in WiFi |
|-------|---------------------|---------------------|
| **Library** | `#include <WiFi.h>` (untuk shield) | `#include <WiFi.h>` (untuk ESP32) |
| **Fitur** | Terbatas | Lengkap (AP+STA, scanning, promiscuous) |
| **Kecepatan** | 2.4 GHz, kecepatan terbatas | 2.4 GHz, throughput hingga 30 Mbps  |
| **Mode** | Station + AP terpisah | Station, AP, atau keduanya simultan  |

**Catatan penting:** Meskipun menggunakan `#include <WiFi.h>` yang sama, library untuk ESP32 memiliki **fungsi tambahan** yang tidak tersedia pada library WiFi Shield . Pastikan Anda menggunakan board ESP32 saat mengkompilasi kode.

---

## Fungsi Dasar WiFi (Station Mode)

### 1. WiFi.begin()

**Deskripsi:** Menghubungkan ESP32 ke jaringan WiFi sebagai Station.

**Syntax:**
```cpp
WiFi.begin(ssid, password);
```

**Parameter:**
- `ssid` – Nama jaringan WiFi (const char*)
- `password` – Sandi jaringan (const char*, kosongkan untuk jaringan terbuka)

**Returns:** `WL_CONNECTED` atau status koneksi (sebenarnya prosesnya async)

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

void loop() {}
```

### 2. WiFi.status()

**Deskripsi:** Mengembalikan status koneksi WiFi saat ini.

**Syntax:**
```cpp
WiFi.status();
```

**Returns:** Nilai enum status:
- `WL_CONNECTED` – Terhubung ke AP
- `WL_NO_SSID_AVAIL` – SSID tidak ditemukan
- `WL_CONNECT_FAILED` – Gagal terhubung
- `WL_DISCONNECTED` – Terputus
- `WL_IDLE_STATUS` – Sedang proses

**Contoh:**
```cpp
if (WiFi.status() == WL_CONNECTED) {
  Serial.println("Terhubung ke WiFi");
} else {
  Serial.println("Tidak terhubung");
}
```

### 3. WiFi.disconnect()

**Deskripsi:** Memutuskan koneksi WiFi dari AP yang sedang terhubung.

**Syntax:**
```cpp
WiFi.disconnect();
```

**Parameter:** Tidak ada

**Returns:** `true` jika berhasil, `false` jika gagal

### 4. WiFi.reconnect()

**Deskripsi:** Memaksa ESP32 untuk menyambung ulang ke AP terakhir yang dikonfigurasi.

**Syntax:**
```cpp
WiFi.reconnect();
```

### 5. WiFi.localIP()

**Deskripsi:** Mendapatkan alamat IP yang diberikan oleh DHCP (atau statis) pada ESP32.

**Syntax:**
```cpp
WiFi.localIP();
```

**Returns:** Objek `IPAddress`

**Contoh:**
```cpp
Serial.print("IP Address: ");
Serial.println(WiFi.localIP());
```

### 6. WiFi.macAddress()

**Deskripsi:** Mendapatkan alamat MAC dari interface WiFi ESP32.

**Syntax:**
```cpp
WiFi.macAddress();
WiFi.macAddress(mac_buffer);
```

**Parameter (opsional):** `mac_buffer` – array byte berukuran 6 untuk menyimpan MAC

**Returns:** `String` jika tanpa parameter, atau `void` jika dengan buffer

**Contoh:**
```cpp
Serial.print("MAC Address: ");
Serial.println(WiFi.macAddress());
```

### 7. WiFi.RSSI()

**Deskripsi:** Mendapatkan kekuatan sinyal (Received Signal Strength Indicator) dari AP yang terhubung.

**Syntax:**
```cpp
WiFi.RSSI();
```

**Returns:** `long` – nilai RSSI dalam dBm (semakin mendekati 0, semakin bagus)

**Contoh:**
```cpp
long rssi = WiFi.RSSI();
Serial.print("Signal strength: ");
Serial.print(rssi);
Serial.println(" dBm");
```

### 8. WiFi.SSID()

**Deskripsi:** Mendapatkan nama SSID dari AP yang sedang terhubung.

**Syntax:**
```cpp
WiFi.SSID();
```

**Returns:** `String` – nama SSID

### 9. WiFi.BSSID()

**Deskripsi:** Mendapatkan alamat MAC (BSSID) dari AP yang terhubung.

**Syntax:**
```cpp
WiFi.BSSID(bssid);
```

**Parameter:** `bssid` – array byte berukuran 6

**Returns:** `void` (hasil disimpan ke parameter)

---

## Fungsi Station Mode Lanjutan

### 10. WiFi.config()

**Deskripsi:** Mengatur alamat IP statis (non-DHCP).

**Syntax:**
```cpp
WiFi.config(local_ip, gateway, subnet, dns1, dns2);
```

**Parameter:**
- `local_ip` – IP statis untuk ESP32
- `gateway` – IP gateway jaringan
- `subnet` – Subnet mask
- `dns1` – DNS server utama (opsional)
- `dns2` – DNS server sekunder (opsional)

**Catatan:** Harus dipanggil **sebelum** `WiFi.begin()` 

**Contoh:**
```cpp
IPAddress local_IP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);

WiFi.config(local_IP, gateway, subnet, primaryDNS);
WiFi.begin(ssid, password);
```

### 11. WiFi.setAutoConnect()

**Deskripsi:** Mengatur apakah ESP32 akan otomatis menyambung ke AP yang dikenal saat startup.

**Syntax:**
```cpp
WiFi.setAutoConnect(true);  // default
WiFi.setAutoConnect(false);
```

### 12. WiFi.setAutoReconnect()

**Deskripsi:** Mengatur apakah ESP32 akan otomatis menyambung ulang jika koneksi terputus.

**Syntax:**
```cpp
WiFi.setAutoReconnect(true);
WiFi.setAutoReconnect(false);
```

### 13. WiFi.persistent()

**Deskripsi:** Mengatur apakah konfigurasi WiFi disimpan ke flash memory (NVS) secara permanen.

**Syntax:**
```cpp
WiFi.persistent(true);   // simpan ke flash
WiFi.persistent(false);  // hanya di RAM
```

### 14. Event Handler (onEvent)

**Deskripsi:** Mendaftarkan fungsi callback untuk menangani event WiFi .

**Syntax:**
```cpp
WiFi.onEvent(callback_function, event_type);
```

**Event yang tersedia:**
- `ARDUINO_EVENT_WIFI_STA_CONNECTED` – Terhubung ke AP
- `ARDUINO_EVENT_WIFI_STA_DISCONNECTED` – Terputus dari AP
- `ARDUINO_EVENT_WIFI_STA_GOT_IP` – Berhasil mendapat IP
- `ARDUINO_EVENT_WIFI_STA_LOST_IP` – Kehilangan IP

**Contoh:**
```cpp
#include <WiFi.h>

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("WiFi Connected!");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.begin(ssid, password);
}
```

---

## Fungsi Scan Jaringan

### 15. WiFi.scanNetworks()

**Deskripsi:** Memindai semua jaringan WiFi yang tersedia di sekitar .

**Syntax:**
```cpp
WiFi.scanNetworks();
WiFi.scanNetworks(async);
```

**Parameter:**
- `async` – `true` untuk scan asynchronous (tidak blocking), `false` untuk blocking

**Returns:** `int16_t` – jumlah jaringan yang ditemukan, atau negatif jika error

**Contoh:**
```cpp
int jumlahJaringan = WiFi.scanNetworks();
Serial.print("Ditemukan ");
Serial.print(jumlahJaringan);
Serial.println(" jaringan");

for (int i = 0; i < jumlahJaringan; i++) {
  Serial.print(i + 1);
  Serial.print(": ");
  Serial.print(WiFi.SSID(i));
  Serial.print(" (");
  Serial.print(WiFi.RSSI(i));
  Serial.println(" dBm)");
}

WiFi.scanDelete(); // membersihkan hasil scan dari memori
```

### 16. WiFi.scanDelete()

**Deskripsi:** Menghapus hasil scan dari memori untuk menghemat RAM.

**Syntax:**
```cpp
WiFi.scanDelete();
```

---

## Fungsi Access Point (Soft-AP Mode)

### 17. WiFi.softAP()

**Deskripsi:** Mengaktifkan ESP32 sebagai Access Point.

**Syntax:**
```cpp
WiFi.softAP(ssid);
WiFi.softAP(ssid, password);
WiFi.softAP(ssid, password, channel, ssid_hidden, max_connection);
```

**Parameter:**
- `ssid` – Nama jaringan yang akan dipancarkan
- `password` – Sandi (minimal 8 karakter untuk WPA2, kosongkan untuk open)
- `channel` – Kanal WiFi (1-13, default 1)
- `ssid_hidden` – `true` untuk menyembunyikan SSID, `false` untuk publik
- `max_connection` – Maksimal client terhubung (1-8, default 4)

**Returns:** `true` jika berhasil, `false` jika gagal

**Contoh:**
```cpp
WiFi.softAP("ESP32_Network", "12345678");
Serial.println("Access Point aktif!");
Serial.print("IP AP: ");
Serial.println(WiFi.softAPIP());
```

### 18. WiFi.softAPIP()

**Deskripsi:** Mendapatkan alamat IP dari interface Access Point (default 192.168.4.1).

**Syntax:**
```cpp
WiFi.softAPIP();
```

**Returns:** Objek `IPAddress`

### 19. WiFi.softAPmacAddress()

**Deskripsi:** Mendapatkan alamat MAC dari interface Access Point.

**Syntax:**
```cpp
WiFi.softAPmacAddress();
```

**Returns:** `String`

### 20. WiFi.softAPdisconnect()

**Deskripsi:** Menonaktifkan mode Access Point.

**Syntax:**
```cpp
WiFi.softAPdisconnect(wifioff);
```

**Parameter:** `wifioff` – `true` untuk mematikan radio WiFi sepenuhnya

**Returns:** `true` jika berhasil

---

## Fungsi WiFi Multi (Multi SSID)

### 21. WiFi.begin() dengan Multi SSID

**Deskripsi:** ESP32 dapat menyimpan hingga 5 SSID berbeda dan akan terhubung ke yang terkuat.

**Contoh:**
```cpp
#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti wifiMulti;

void setup() {
  wifiMulti.addAP("SSID_1", "password1");
  wifiMulti.addAP("SSID_2", "password2");
  wifiMulti.addAP("SSID_3", "password3");
  
  while(wifiMulti.run() != WL_CONNECTED) {
    delay(500);
  }
}
```

---

## Contoh Program Lengkap

### Contoh 1: Station Mode dengan Event Handler
```cpp
#include <WiFi.h>

const char* ssid = "YourNetwork";
const char* password = "YourPassword";

void onConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("Connected to AP!");
}

void onGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("Got IP: " + WiFi.localIP().toString());
}

void onDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("Disconnected from AP");
  WiFi.reconnect();
}

void setup() {
  Serial.begin(115200);
  
  WiFi.onEvent(onConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(onGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(onDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  
  WiFi.begin(ssid, password);
}

void loop() {
  delay(1000);
}
```

### Contoh 2: Scan Jaringan WiFi
```cpp
#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  Serial.println("Scanning WiFi...");
  int n = WiFi.scanNetworks();
  
  if (n == 0) {
    Serial.println("No networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; i++) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(" dBm) ");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
    }
  }
  WiFi.scanDelete();
}

void loop() {
  delay(10000);
}
```

### Contoh 3: AP + STA Mode Bersamaan
```cpp
#include <WiFi.h>

const char* sta_ssid = "RouterNetwork";
const char* sta_password = "RouterPassword";
const char* ap_ssid = "ESP32_AP";
const char* ap_password = "12345678";

void setup() {
  Serial.begin(115200);
  
  // Mode AP + STA
  WiFi.mode(WIFI_AP_STA);
  
  // Konfigurasi sebagai Access Point
  WiFi.softAP(ap_ssid, ap_password);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
  
  // Konfigurasi sebagai Station
  WiFi.begin(sta_ssid, sta_password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nStation connected!");
  Serial.print("Station IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  delay(1000);
}
```

---

## Catatan Penting

1. **Library untuk ESP32 adalah `#include <WiFi.h>`**, berbeda dengan ESP8266 yang menggunakan `#include <ESP8266WiFi.h>` 

2. **Mode default** ESP32 adalah Station (client), bukan Access Point 

3. **Penggunaan bersama Bluetooth** dapat mempengaruhi performa WiFi karena keduanya berbagi antena

4. **Power saving** dapat diaktifkan dengan `WiFi.setSleep(true)` untuk menghemat daya 

5. **Koneksi otomatis** ke AP yang dikenal adalah fitur bawaan ESP32, disimpan di memori NVS

6. **Pin yang digunakan** untuk komunikasi internal WiFi tidak perlu diatur manual, semua sudah ditangani library

---

## Referensi

- [ESP-IDF WiFi Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_wifi.html) 
- [Arduino-ESP32 WiFi Library](https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi)
- [WiFi Examples untuk ESP32](https://tigoe.github.io/Wifi_Examples/) 
