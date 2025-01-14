#define M5STACK_MPU6886 
#include <M5Stack.h>
#include <SoftwareSerial.h>
/*#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

String MACadd = "C4:5B:BE:33:BE:6A";
uint8_t address[6] ={0xC4,0x5B,0xBE,0x33,0xBE,0x6A};
bool connected;*/

//PID係数
#define TARGET            90.0f
#define KP                0.5f
#define KI                0.0f
#define KD                0.0f

//Motor
/*#define MOTOR_PIN_F        21   // to DC Motor Driver FIN
#define MOTOR_PIN_R        22   // to DC Motor Driver RIN
#define MOTOR_PWM_F        0   // PWM CHANNEL
#define MOTOR_PWM_R        1   // PWM CHANNEL*/

#define MOTOR_POWER_MIN    50
#define MOTOR_POWER_MAX    255
//PID
float P = 0, I = 0, D = 0, preP=0;
float preTime;
float power = 0;
//uint8_t power = 0;

float pitch0, roll0, yaw0;

#define rxPin 3
#define txPin 1

//EspSoftwareSerial::UART mySerial;

void setup() {
  M5.begin();
  M5.Lcd.begin();

  //pinMode(rxPin, INPUT);
  //pinMode(txPin, OUTPUT);
  //mySerial.begin(115200); 
  Serial.begin(115200); 
  /*SerialBT.begin("ESP32test", true); 
  Serial.println("The device started in master mode, make sure remote BT device is on!");

  connected = SerialBT.connect(address);

  if(connected) {
    Serial.println("Connected Succesfully!");
  } 
  else {
    while(!SerialBT.connected(10000)) {
      Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app."); 
    }
  }
  // disconnect() may take upto 10 secs max
  if (SerialBT.disconnect()) {
    Serial.println("Disconnected Succesfully!");
  }
  // this would reconnect to the name(will use address, if resolved) or address used with connect(name/address).
  SerialBT.connect();*/

  M5.MPU6886.Init(); //MPU設定
  M5.MPU6886.getAhrsData(&pitch0,&roll0,&yaw0);
  preTime = micros(); //PID初期化
}

void loop() {
  float pitch,roll,yaw;
  float now,dt,Time;
  uint8_t Duty;
  //ロール角取得
  M5.MPU6886.getAhrsData(&pitch,&roll,&yaw);
  
  //PID計算
  now     = TARGET - roll                 ; // 目標角度から現在の角度を引いて偏差を求める
  if (-20 < now && now < 20) { 
    Time    = micros()                    ;
    dt      = (Time - preTime) / 1000000  ; // 処理時間を求める
    preTime = Time                        ; // 処理時間を記録
    P       = now / 90                    ; // -90~90→-1.0~1.0
    I      += P * dt                      ; // 偏差を積分する
    D       = (P - preP) / dt             ; // 偏差微分するを
    preP    = P                           ; // 偏差を記録する
    power  += KP * P + KI * I + KD * D    ; // 出力を計算する
    if (power < -1){ power = -1;}           // →-1.0~1.0
    if (1 < power) { power =  1;}             
    
    Duty = (int)((MOTOR_POWER_MAX - MOTOR_POWER_MIN)* abs(power) + MOTOR_POWER_MIN); 

    // SerialBT.write(Duty);
    Serial.println(Duty);

  }  
  else {  // +-20度を越えたら倒れたとみなす
    power = 0;
    I = 0;
    // SerialBT.write(Duty);
    Serial.println(Duty);
  }
  M5.Lcd.clear(BLACK);  
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.printf("now:%6.1f",now);
  M5.Lcd.setCursor(150, 00);
  M5.Lcd.printf("power:%6.1f",power  );
  M5.Lcd.setCursor(0, 40);
  M5.Lcd.printf("Duty:%6.1f",Duty);
  M5.Lcd.setCursor(150,40);
  M5.Lcd.printf("roll:%6.1f",roll);
  M5.Lcd.setCursor(0, 120);
  M5.Lcd.printf("P:%6.4f",P);
  M5.Lcd.setCursor(90, 120);
  M5.Lcd.printf("I:%6.4f",I);
  M5.Lcd.setCursor(180, 120);
  M5.Lcd.printf("D:%6.4f",D);
  /*Serial.println(roll);
  Serial.println(now);
  Serial.println(power);
  Serial.println(Duty);
  delay(10);*/
}
