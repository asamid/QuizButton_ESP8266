#include <ESP8266WiFi.h>
#include <espnow.h>

#define BUTTON_PIN 12 // ボタン
#define LED_PIN 16    // LED

uint8_t masterMac[] = {0x30, 0x83, 0x98, 0xA4, 0xD8, 0x27};
bool isLEDOn = false;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.println("restart");

  WiFi.mode(WIFI_STA); // ステーションモードで初期化

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW Initialization Failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_add_peer(masterMac, ESP_NOW_ROLE_CONTROLLER, 1, NULL, 0);
  esp_now_register_recv_cb(onReceive);
}

// ボタンが押されたとき
void onButtonPress() {
  Serial.println("Button Pressed!");
  esp_now_send(masterMac, (uint8_t*)"PRESSED", strlen("PRESSED"));
  Serial.println("SEND!");
}

// 親機からの信号を受信
void onReceive(uint8_t* macAddr, uint8_t* data, uint8_t len) {
  String message = "";
  for (int i = 0; i < len; i++) {
    message += (char)data[i];
  }

  if (message == "LED_ON") {
    digitalWrite(LED_PIN, HIGH);
    isLEDOn = true;
  } else if (message == "LED_OFF") {
    digitalWrite(LED_PIN, LOW);
    isLEDOn = false;
  }

  Serial.println("Message Received: " + message);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == HIGH) {
    onButtonPress();
    delay(100); // デバウンス
  }
}
