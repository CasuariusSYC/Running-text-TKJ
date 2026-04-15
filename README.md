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

## Penjelasan Library yang digunakan 

### WiFi Configuration

## EEPROMP (Electrically Erasable Programmable Read-Only Memory)

## WiFI (ESP32)

## WebServer (ESP32)
