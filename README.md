# 無線式のクイズ早押し機(ESP-WROOM-02)
みんなで早押しクイズ(みんはや)で遊んでいたら早押しボタンが欲しくなったので作成してみた。


## 仕様
親機と子機（複数台）を想定。  
通信方法はesp-now。
  
- 親機の機能
  - ネットワークへの参加確認
  - 状態のリセット

- 子機の機能
  - 押下タイミングを送信

## 準備するもの
### 親機
1. [ESP-WROOM-02開発ボード](https://akizukidenshi.com/catalog/g/g112236/) 
2. [タクトスイッチ(大)10個セット](https://akizukidenshi.com/catalog/g/g102561/) 内2個を使用

### 子機
1. [ESP-WROOM-02開発ボード](https://akizukidenshi.com/catalog/g/g112236/) 
2. [抵抗内蔵5mmLED](https://akizukidenshi.com/catalog/g/g106245/) 
3. [タクトスイッチ(大)10個セット](https://akizukidenshi.com/catalog/g/g102561/) 内1個を使用
