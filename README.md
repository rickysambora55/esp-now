## 🚀 About Me

Nama : Ricky Sambora<br>
NIM : 4.31.20.1.21<br>
Kelas : TE-3B

# Jobsheet 1.1 ESP-NOW

Pada jobsheet 1.1 terdapat 3 project yaitu simulasi GPIO, PWM, dan ADC menggunakan ESP32.

### Alat dan Bahan

-   ESP32 >4
-   Arduino IDE (Terinstal ESP32)
-   Library DHT dan Adafruit unified sensor
-   Breadboard
-   Kabel Jumper
-   Resistor < 220Ω

### Instalasi ESP-32

1. Buka Arduino IDE
    - Masuk ke **Preferences**
    - Isikan board url dengan link : https://dl.espressif.com/dl/package_esp32_index.json dan simpan
    - Buka **Tools** > **Board** > **Boards Manager**
    - Cari ESP32, by Espressif. Kemudian instal
    - Pilih flash mode DIO/QIU menyesuaikan
2. Jalankan program .ino
3. Jika terdapat error saat uploading, tekan dan tahan tombol _Boot_ pada ESP32 saat upload, hingga _Connecting_ selesai

Nb. Proses instalasi dapat dilewati jika Arduino IDE sudah terintegrasi dengan ESP32.

### Instalasi DHT & Adafruit Libraries

1. Buka Arduino IDE
2. Buka **Sketch** > **Include Library** > **Library Manager**
3. Cari **DHT sensor library** by Adafruit. Kemudian instal
4. Instal juga library **Adafruit unified sensor** by Adafruit

Nb. Proses instalasi dapat dilewati jika libraries telah diinstal.

## Project A - Mac Address

### Rangkaian & Instalasi

1. Siapkan ESP32 dan hubungkan ke Arduino IDE
2. Download dan jalankan kode dari source code sesuai project

### Penjelasan

Import library wifi

```c
#include "WiFi.h"
```

Set mode dan print mac address di serial monitor

```c
    WiFi.mode(WIFI_MODE_STA);
    Serial.println(WiFi.macAddress());
```

### Keluaran

Mac akan keluar di serial monitor. Pastikan baud rate sama dengan kode. Jika tidak keluar, maka tekan tombol reset.

![App Screenshot](https://cdn.discordapp.com/attachments/1043462519336996894/1048950668855091301/A._Mac.png)

## Project B - One-Way Point to Point

### Rangkaian & Instalasi

1. Siapkan 2 ESP32 yang sudah diketahui Mac Address wifinya.

<img src="https://cdn.discordapp.com/attachments/1043462519336996894/1048951902097920090/B._Simplex_PTP.webp" width="480px">

2. Download dan jalankan kode dari source code sesuai project

### Penjelasan

Import library yang akan digunakan

```c
#include <esp_now.h>
#include <WiFi.h>
```

**Sender**
Kemudian ketikan perintah ini untuk memasukkan mac address tujuan (receiver). Ganti nilai FF untuk masing oktet pada mac address FF:FF:FF:FF:FF:FF.

```c
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
```

`typedef struct` digunakan untuk mendeklarasi format atau struktur dari pesan yang akan dikirimkan. Maksimal pesan yang dapat dikirim ESP32 adalah 250 byte data. Pada contoh struktur dinamai `struct_message` yang berisi 4 jenis variabel. char bernilai 32 byte, int 4 byte, float 4 byte, dan bool 2 byte.

```c
typedef struct struct_message {
    char a[32];
    int b;
    float c;
    bool d;
} struct_message;
```

Buat variabel untuk menyimpan data dari struktur dengan nama `myData`.

```c
struct_message myData;
```

Buat variabel untuk menyimpan informasi peer dengan nama `peerInfo`.

```c
esp_now_peer_info_t peerInfo;
```

Fungsi `OnDataSent()` akan dipanggil ketika data terkirim. Pada contoh, isi dari fungsi adalah print pada serial monitor yang menunjukan data terkirim atau tidak.

```c
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nStatus Paket Terakhir :\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sukses Terkirim" : "Gagal Terkirim");
}
```

Inisiasi dilakukan pada fungsi `setup()` sama seperti pada arduino. Inisialisasikan serial baud rate dan mode wifi sebagai station.

```c
Serial.begin(115200);
WiFi.mode(WIFI_STA);
```

Cek inisialisasi ESP-NOW.

```c
if (esp_now_init() != ESP_OK) {
    Serial.println("Gagal menginisialisasi ESP-NOW");
    return;
}
```

Buat callback yang akan memanggil fungsi `OnDataSent()`.

```c
esp_now_register_send_cb(OnDataSent);
```

Lakukan pair dengan ESP lain.

```c
// Register peer
memcpy(peerInfo.peer_addr, broadcastAddress, 6);
peerInfo.channel = 0;
peerInfo.encrypt = false;

// Add peer
if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Gagal menambahkan peer");
return;
}
```

Pada fungsi `loop()` lakukan pengiriman data. Pertama panggil variabel yang akan dikirimkan datanya.

```c
strcpy(myData.a, "INI ADALAH CHAR");
myData.b = random(1,20);
myData.c = 1.2;
myData.d = false;
```

Kirimkan pesan dengan perintah berikut.

```c
esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
```

Cek apakah pesan berhasil dikirim atau tidak.

```c
if (result == ESP_OK) {
    Serial.println("Data berhasil terkirim");
}
else {
    Serial.println("Gagal mengirim data");
}
```

Tambah jeda 2 detik (2000 ms) pada fungsi `loop()`. Sehingga data akan dikirimkan dengan interval 2 detik.

```c
delay(2000);
```

**Receiver**
Import library dan buat struktur data sama persis dengan struktur data dari pengirim.

```c
#include <esp_now.h>
#include <WiFi.h>
typedef struct struct_message {
    char a[32];
    int b;
    float c;
    bool d;
} struct_message;
struct_message myData;
```

Buat callback fungsi yang akan menerima data.

```c
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len)
```

Simpan data dari fungsi kedalam variabel `myData`. Perintah berada di bawah fungsi `OnDataRecv`.

```c
memcpy(&myData, incomingData, sizeof(myData))
```

Dilanjutkan dengan print data pada serial monitor.

```c
Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Char: ");
    Serial.println(myData.a);
    Serial.print("Int: ");
    Serial.println(myData.b);
    Serial.print("Float: ");
    Serial.println(myData.c);
    Serial.print("Bool: ");
    Serial.println(myData.d);
    Serial.println();
```

Pada fungsi `setup()` insialisasi baud rate dan mode wifi menjadi station serta cek apakah ESP-NOW aktif.

```c
Serial.begin(115200);
WiFi.mode(WIFI_STA);
if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
}
```

Set `OnDataRecv()` sebagai fungsi yang akan dipanggil saat data diterima.

```c
esp_now_register_recv_cb(OnDataRecv);
```

### Keluaran

1. Sender

![App Screenshot](https://cdn.discordapp.com/attachments/1043462519336996894/1048964922966540309/B._Simplex_PTP_Sender.png)

2. Receiver

![App Screenshot](https://cdn.discordapp.com/attachments/1043462519336996894/1048964955749232710/B._Simplex_PTP_Receiver.png)
