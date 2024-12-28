#include <ESP8266WiFi.h>
#include <espnow.h>

// グローバル変数
bool isFirstSignalReceived = false; // 最初の信号を受信したか
uint8_t firstResponderMac[6];       // 最初に応答した子機のMACアドレス

// ボタンのピン番号
#define BUTTON1_PIN 12 // 点灯
#define BUTTON2_PIN 13 // 消灯+reset
int delaytime = 250;

// 初期化
void setup() {
  Serial.begin(115200);
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);

  WiFi.mode(WIFI_STA); // ステーションモードに設定

  // ESP-NOWの初期化とエラーチェック
  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW Initialization Failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_recv_cb(onReceive); // 受信コールバックを登録

  // 自身のMACアドレスを表示
  uint8_t macAddr[6];
  WiFi.macAddress(macAddr);
  Serial.printf("Device\n");
  Serial.printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
}

// メッセージを送信
void sendMessage(const uint8_t* macAddr, const char* message) {
  esp_now_send((uint8_t*)macAddr, (uint8_t*)message, strlen(message)); // macAddr を非 const にキャスト
}

// ブロードキャストでメッセージを送信
void sendMessageBroadcast(const char* message) {
  uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // ブロードキャストアドレスを指定
  esp_now_send(broadcastAddress, (uint8_t*)message, strlen(message));
}

// ボタン1: LED点灯
void turnOnLEDsBroadcast() {
  sendMessageBroadcast("LED_ON");
  Serial.println("Broadcast Sent: LED_ON");
}

// ボタン2: LED消灯
void resetSequenceAndTurnOffLEDs() {
  //sendMessage(firstResponderMac, "LED_OFF");
  sendMessageBroadcast("LED_OFF");
  Serial.println("Sent: LED_OFF");
  isFirstSignalReceived = false; // 順番をリセット
  Serial.println("Sequence reset.");
}

// 子機からの信号を受信
void onReceive(uint8_t* macAddr, uint8_t* data, uint8_t len) {
  if (!isFirstSignalReceived) {
    isFirstSignalReceived = true;
    memcpy(firstResponderMac, macAddr, 6); // MACアドレスを記録
    sendMessage(firstResponderMac, "LED_ON"); // 初回応答にLED点灯指示を送信
    Serial.println("First responder signal received.");
  }
}

// メインループ
void loop() {
  if (digitalRead(BUTTON1_PIN) == LOW) { // LOWでボタンが押されたと判定
    Serial.println("Button 1 Pressed");
    turnOnLEDsBroadcast();
    delay(delaytime);
  } else if (digitalRead(BUTTON2_PIN) == LOW) {
    Serial.println("Button 2 Pressed");
    resetSequenceAndTurnOffLEDs();
    delay(delaytime);
  }
}
