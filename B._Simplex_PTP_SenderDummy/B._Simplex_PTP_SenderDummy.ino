#include <esp_now.h>
#include <WiFi.h>

uint8_t broadcastAddress[] = {0x24, 0x0A, 0xC4, 0xC6, 0x09, 0x94};
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
  char e[192];
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nStatus Paket Terakhir :\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sukses Terkirim" : "Gagal Terkirim");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Gagal menginisialisasi ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Gagal menambahkan peer");
    return;
  }
}
void loop() {
  // Set values to send
  strcpy(myData.a, "INI ADALAH CHAR");
  myData.b = random(100, 2000);
  myData.c = 3.6;
  myData.d = true;
  strcpy(myData.e, "Kelompok Embedded Davin M. dan Ricky S.");

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Data berhasil terkirim");
  }
  else {
    Serial.println("Gagal mengirim data");
  }
  delay(2000);
}
