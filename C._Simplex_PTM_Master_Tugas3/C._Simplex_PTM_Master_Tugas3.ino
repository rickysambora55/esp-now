#include <esp_now.h>
#include <WiFi.h>

uint8_t broadcastAddress1[] = {0x24, 0x0A, 0xC4, 0xC6, 0x09, 0x94};
uint8_t broadcastAddress2[] = {0x24, 0x6F, 0x28, 0x2B, 0x6D, 0xD8};
uint8_t broadcastAddress3[] = {0x24, 0x0A, 0xC4, 0xC6, 0x0E, 0x7C};

typedef struct test_struct1 {
  int x;
  int y;
} test_struct1;
test_struct1 test1;

typedef struct test_struct2 {
  int x;
  int y;
} test_struct2;
test_struct2 test2;

typedef struct test_struct3 {
  int x;
  int y;
} test_struct3;
test_struct3 test3;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  memcpy(peerInfo.peer_addr, broadcastAddress3, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  test1.x = random(0, 50);
  test1.y = random(0, 50);
  test2.x = random(100, 500);
  test2.y = random(100, 500);
  test3.x = random(1000, 5000);
  test3.y = random(1000, 5000);
  esp_err_t result1 = esp_now_send(broadcastAddress1, (uint8_t *) &test1, sizeof(test_struct1));
  esp_err_t result2 = esp_now_send(broadcastAddress2, (uint8_t *) &test2, sizeof(test_struct2));
  esp_err_t result3 = esp_now_send(broadcastAddress3, (uint8_t *) &test3, sizeof(test_struct3));

  if (result1 == ESP_OK) {
    Serial.println("Sent id 1 success");
  }
  else {
    Serial.println("Error sending id 1 the data");
  }
  
  if (result2 == ESP_OK) {
    Serial.println("Sent id 2 success");
  }
  else {
    Serial.println("Error sending id 2 the data");
  }
  
  if (result3 == ESP_OK) {
    Serial.println("Sent id 3 success");
  }
  else {
    Serial.println("Error sending id 3 the data");
  }
  delay(2000);
}
