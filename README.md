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
Atau download `libraries` dan upload pada direktori projek.

### Instalasi DHT & Adafruit Libraries

1. Buka Arduino IDE
2. Buka **Sketch** > **Include Library** > **Library Manager**
3. Cari **DHT sensor library** by Adafruit. Kemudian instal
4. Instal juga library **Adafruit unified sensor** by Adafruit

Nb. Proses instalasi dapat dilewati jika libraries telah diinstal.
Atau download `libraries` dan upload pada direktori projek.

## Project A - Mac Address

### Rangkaian & Instalasi

1. Siapkan ESP32 dan hubungkan ke Arduino IDE
2. Download dan jalankan kode dari source code sesuai project.

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

## Project B - Simplex Point to Point

### Rangkaian & Instalasi

1. Siapkan 2 ESP32 yang sudah diketahui Mac Address wifinya.

<img src="https://cdn.discordapp.com/attachments/1043462519336996894/1048951902097920090/B._Simplex_PTP.webp" width="480px">

2. Download dan jalankan kode dari source code sesuai project.

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

### Tugas B no. 8-11

![App Screenshot](https://cdn.discordapp.com/attachments/1043462519336996894/1048975577819910164/image.png)

Percobaan dilakukan di ruangan tertutup dengan beberapa perabotan (meja dan kursi) serta terdapat beberapa orang dan ESP32 lain pada radius 10m / pada ruangan yang sama. Jarak antar ESP adalah 1 meter, 2 meter, 3 meter, 4 meter, dan 5 meter pada ketinggian masing-masing yaitu ground level, 30cm, dan 1m diatas tanah secara LOS (Line of Sight) dan halangan berupa tangan dan buku.

Hasil yang didapatkan dari percobaan adalah 100% diterima dan 0% loss. Hal ini dikarenakan salah satu dari ESP yaitu ESP pengirim adalah seri ESP32U V4 yang memiliki antena eksternal dengan gain +3dB. Sehingga pada jarak 5 meter baik secara LOS maupun diberikan halangan, daya pancar antena sudah lebih dari cukup untuk mengcover keseluruhan area, sehingga penerima dengan mudah menerima sinyal tanpa kerusakan atau rugi-rugi berarti.

## Project C - Simplex Point to Multipoint

### Rangkaian & Instalasi

1. Siapkan 3 atau lebih ESP32 yang sudah diketahui Mac Address wifinya.

<img src="https://cdn.discordapp.com/attachments/1043462519336996894/1051349272299323452/C._Simplex_PTM.png" width="480px">

2. Download dan jalankan kode dari source code sesuai project.

### Penjelasan

Percobaan menggunakan 4 board ESP, satu sebagai **master** dan 3 sebagai **slave**. **Board master** akan melakukan broadcast yang pada prinsipnya (code) adalah mengirim data satu per satu ke ESP **slaves**. MAC dari **slaves** harus diidentifikasi pada **master** sebagai **address** tujuan dan membuat masing-masing peer sehingga **master** dapat mengirimkan data. Menggunakan fungsi data dan callback event yang sama, maka hasil pada serial monitor berupa data yang sama pada semua **slaves**.

### Keluaran

1. **Master**

![App Screenshot](https://cdn.discordapp.com/attachments/1043462519336996894/1051351274479681596/C._Simplex_PTM_Sender.png)

2. **Slave**

![App Screenshot](https://cdn.discordapp.com/attachments/1043462519336996894/1051351311939026994/C._Simplex_PTM_Receiver.png)

### Tugas C(a) no. 9

Percobaan menggunakan 3 board ESP, satu sebagai **master** dan 2 sebagai **slave**. Prinsip kerja yang dipakai adalah sama, akan tetapi board **slave** pada percobaan ini diputus salah satu. Sehingga hasil keluaranya menjadi sebagai berikut pada **board master**:

![App Screenshot](https://cdn.discordapp.com/attachments/1043462519336996894/1051352238523699330/C._Simplex_PTM_Tugas_1.png)

### Tugas C(a) no. 10-11

Percobaan hanya dapat menggunakan hingga 5 **board** saja karena keterbatasan ESP yang ada. Pada pengiriman 4 **slaves** tidak ada kendala sama sekali. Sedangkan menurut https://randomnerdtutorials.com/ ESP-NOW dapat berkomunikasi dengan peer terenkripsi hingga 10 ESP.
Nb. Untuk menambah jumlah ESP dalam jaringan, maka selain menambah variabel mac juga perlu menambah peer untuk masing-masing **slaves**. **Board slave** menggunakan kode yang sama seperti sebelumnya.

![App Screenshot](https://cdn.discordapp.com/attachments/1043462519336996894/1051352762329354280/C._Simplex_PTM_Tugas_2.png)

### Tugas C(b)

Percobaan menggunakan 4 board ESP, satu sebagai **master** dan 3 sebagai **slave**. Masing-masing ESP dibuatkan struktur data dan pada fungsi looping dilakukan routing pada masing-masing struktur data yang dibuat. Sehingga data yang diterima antar **slave** tidaklah sama. Sedangkan untuk **board slave** menggunakan kode yang sama seperti sebelumnya.

1. **Master**

![App Screenshot](https://cdn.discordapp.com/attachments/1043462519336996894/1051354348317327390/C._Simplex_PTM_Tugas_3b.png)

2. **Slave 1**

![App Screenshot](https://cdn.discordapp.com/attachments/1043462519336996894/1051355248234610778/C._Simplex_PTM_Tugas_3b_Slave_1.png)

3. **Slave 2**

![App Screenshot](https://cdn.discordapp.com/attachments/1043462519336996894/1051355248524021760/C._Simplex_PTM_Tugas_3b_Slave_2.jpeg)

4. **Slave 3**

![App Screenshot](https://cdn.discordapp.com/attachments/1043462519336996894/1051355248712757248/C._Simplex_PTM_Tugas_3b_Slave_3.png)

## Project D - Simplex Multipoint to Point

### Rangkaian & Instalasi

1. Siapkan 3 atau lebih ESP32 yang sudah diketahui Mac Address wifinya.

<img src="https://cdn.discordapp.com/attachments/1043462519336996894/1051375306340040804/D._Simplex_MTP.png" width="480px">

2. Download dan jalankan kode dari source code sesuai project.

### Penjelasan

Percobaan menggunakan 3 board ESP, 2 sebagai **slave** dan 1 sebagai **master**. Prinsip kerja master sama seperti pada percobaan PTP maupun PTM. Perbedaanya ada pada kode dari ESP penerima yaitu struktur dibuat dalam bentuk array yang diisi masing-masing variabel dari struktur data. Kemudian untuk mempermudah ditambahkan event callback yang mengidentifikasi **mac address** pengirim yang akan ditampilkan bersama dengan data yang diterima.

### Keluaran

1. **Master**

![App Screenshot](https://cdn.discordapp.com/attachments/1043462519336996894/1051375306625273876/D._Simplex_MTP_Master.png)

2. **Slave**

![App Screenshot](https://cdn.discordapp.com/attachments/1043462519336996894/1051375306889502822/D._Simplex_MTP_Slave.png)

## Project E - Duplex

### Rangkaian & Instalasi

1. Siapkan 2 ESP32 yang sudah diketahui Mac Address wifinya.

<img src="https://cdn.discordapp.com/attachments/1043462519336996894/1051380317451206756/E._Duplex.png" width="480px">

2. Buat rangkaian sebagai berikut.

<img src="https://cdn.discordapp.com/attachments/1043462519336996894/1051425529397518376/E._Rangkaian_DHT.png" width="480px">

3. Download dan jalankan kode dari source code sesuai project.
4. Pastikan library DHT dan adafruit sudah terinstal. Pastikan `DHTTYPE` sesuai dengan tipe sensor yang digunakan, contoh DHT11, DHT21, DHT22.

### Penjelasan

Percobaan menggunakan 1 board ESP untuk membaca sensor DHT (pada percobaan digunakan DHT11).

### Keluaran

![App Screenshot](https://cdn.discordapp.com/attachments/1043462519336996894/1051380317124042772/E._DHT11.png)

### Tugas E no. 6-9

Percobaan menggunakan 2 board ESP yang saling berkomunikasi. Hasil sensor ESP A dikirim ke ESP B dan juga sebaliknya dalam interval waktu tertentu. Sehingga program menggabungkan fungsi pengiriman dan penerimaan data serta pembacaan sensor sekaligus.

![App Screenshot](https://cdn.discordapp.com/attachments/1043462519336996894/1051380507750957096/E._Duplex_Tugas_1.png)

### Tugas E no. 10

Percobaan memerlukan minimal 4 ESP dan DHT atau lebih. Akan tetapi, dikarenakan keterbatasan alat, maka percobaan ini tidak dilakukan.
Prinsip kerjanya sama sebagaimana tugas sebelumnya dengan menambahkan struktur dan peer data baru untuk masing-masing jumlah ESP terhubung.
