#include "M5AtomS3.h"

#define VISIBLE_LED               G6
#define INFRARED_LED              G5
#define PXSTR_SENSE               G8

#define LED_DATA_PIN 35                 //ATOMS3に搭載されているRGB LEDのDATA PINを指定
#define NUM_LEDS 1                      //RGB LEDの数を指定
CRGB leds[NUM_LEDS];

void setup() {
  auto cfg = M5.config();
  AtomS3.begin(cfg);

  //=== ATOMS3の設定
  pinMode(VISIBLE_LED, OUTPUT);         //VISIBLE_LEDに繋がるピンを出力に設定
  digitalWrite(VISIBLE_LED, LOW);       //電圧をLOW（0V）に設定
  pinMode(INFRARED_LED, OUTPUT);
  digitalWrite(INFRARED_LED, LOW);
  pinMode(PXSTR_SENSE, ANALOG);         //フォトリフレクタ（PXSTR）繋がるピンをADCに設定
  Serial.println("init pinmode.");

  //=== ATOMS3 Lite内蔵のLED設定
  FastLED.addLeds<WS2811, LED_DATA_PIN, GRB>(leds, NUM_LEDS);   // RGB LEDを初期設定
  FastLED.setBrightness(20);                                    // 明るさを設定（20以上は熱で壊れる可能性あり。）
  leds[0] = CRGB::Black;
  Serial.println("init FastLED.");

  Serial.printf("Done: setup func.\n");
}

void loop() {
  unsigned int env_light_value;
  unsigned int led_and_env_light_value;
  unsigned int led_light_value;
  static bool led_status = false;

  AtomS3.update();

  //LEDを点灯させていないときのセンサ値（環境光のみ）
  env_light_value = analogRead(PXSTR_SENSE);

  //LEDを点灯させたときのセンサ値（環境光 + LED光）
  digitalWrite(INFRARED_LED, HIGH);
  delay(10);
  led_and_env_light_value = analogRead(PXSTR_SENSE);
  digitalWrite(INFRARED_LED, LOW);
  delay(10);

  //LEDの点灯有無センサ値の差分（LED光のみ）
  if(led_and_env_light_value >= env_light_value)  led_light_value = led_and_env_light_value - env_light_value;
  else led_light_value = 0;

  //センサ値が400以上の場合、可視光LEDを点灯させる。
  if(led_light_value > 800){
    digitalWrite(VISIBLE_LED, HIGH);    //可視光LEDの点灯

    leds[0] = CRGB::Blue;               //ATOMS3に付属のLEDの色設定と点灯
    FastLED.show();

    if(led_status == false){         //センサ値を超えたらSwitch onと表示する
      led_status = true;
      Serial.printf("LED on\n");
    }    
  }else{
    digitalWrite(VISIBLE_LED, LOW);

    leds[0] = CRGB::Black;
    FastLED.show();

    if(led_status == true){
      led_status = false;
      Serial.printf("LED off\n");
    }          
  }

  delay(100);
  Serial.printf("PXSTR_SENSE AD VALUE: LED+ENV %d\tENV %d\tLED %d\n", led_and_env_light_value, env_light_value, led_light_value);
}
