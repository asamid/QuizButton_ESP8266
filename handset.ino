#include <ESP8266WiFi.h>
#include <espnow.h>

// ピンの設定
#define BUTTON_PIN 12 // ボタン
#define LED_PIN 16    // LED

// グローバル変数
bool isLEDOn = false; // LED状態
int delaytime = 250;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // 初期状態でLEDを消灯

  WiFi.mode(WIFI_STA); // ステーションモードに設定

  // ESP-NOWの初期化とエラーチェック
  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW Initialization Failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(onReceive); // 受信コールバックを登録
}

// ボタンが押されたときにメッセージをブロードキャスト送信
void onButtonPress() {
  uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  esp_now_send(broadcastAddress, (uint8_t*)"PRESSED", strlen("PRESSED"));
  Serial.println("Sent: PRESSED");
}

// 親機からの信号を受信
void onReceive(uint8_t* macAddr, uint8_t* data, uint8_t len) {
  String message = "";
  for (int i = 0; i < len; i++) {
    message += (char)data[i];
  }

  // 受信メッセージに応じた処理
  if (message == "LED_ON") {
    digitalWrite(LED_PIN, HIGH);
    isLEDOn = true;
    Serial.println("LED_ON received. LED turned on.");
  } else if (message == "LED_OFF") {
    digitalWrite(LED_PIN, LOW);
    isLEDOn = false;
    Serial.println("LED_OFF received. LED turned off.");
  }
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    onButtonPress();
    delay(delaytime);
  }
}
