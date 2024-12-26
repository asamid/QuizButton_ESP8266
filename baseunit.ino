#include <ESP8266WiFi.h>
#include <espnow.h>

// 子機のMACアドレス
uint8_t macs[][6] = {
  {0x30, 0x83, 0x98, 0xA3, 0x54, 0x16},
  {0x30, 0x83, 0x98, 0xA3, 0xC3, 0xAA}
};

bool isFirstSignalReceived = false; // 最初の信号を受信したか
uint8_t firstResponderMac[6];       // 最初に応答した子機のMACアドレス

#define BUTTON1_PIN 12 // 点灯ボタン
#define BUTTON2_PIN 13 // 消灯ボタン (順番リセットも含む)

// 初期化
void setup() {
  Serial.begin(115200);
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);

  WiFi.mode(WIFI_STA); // ステーションモードで初期化

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW Initialization Failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_recv_cb(onReceive);

  // 子機をすべて登録
  for (auto &mac : macs) {
    esp_now_add_peer(mac, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  }
}

// メッセージを全子機に送信
void sendMessageToAll(const char* message) {
  for (auto &mac : macs) {
    esp_now_send(mac, (uint8_t*)message, strlen(message));
  }
}

// ボタン1: LED点灯
void turnOnLEDs() {
  sendMessageToAll("LED_ON");
}

// ボタン2: LED消灯 + 順番リセット
void resetSequenceAndTurnOffLEDs() {
  sendMessageToAll("LED_OFF");
  isFirstSignalReceived = false; // 順番をリセット
  Serial.println("Sequence reset and LEDs turned off.");
}

// 子機からの信号を受信
void onReceive(uint8_t* macAddr, uint8_t* data, uint8_t len) {
  if (!isFirstSignalReceived) {
    isFirstSignalReceived = true;
    memcpy(firstResponderMac, macAddr, 6);
    esp_now_send(firstResponderMac, (uint8_t*)"LED_ON", strlen("LED_ON"));
    Serial.println("First responder signal received.");
  }
}

// メインループ
void loop() {
  if (digitalRead(BUTTON1_PIN) == HIGH) {
    Serial.println("BUTTON1");
    turnOnLEDs();
    delay(200); // デバウンス
  } else if (digitalRead(BUTTON2_PIN) == HIGH) {
    Serial.println("BUTTON2");
    resetSequenceAndTurnOffLEDs();
    delay(200); // デバウンス
  }
}
