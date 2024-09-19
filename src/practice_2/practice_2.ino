#include "M5AtomS3.h"

#define VISIBLE_LED               G6
#define INFRARED_LED              G5
#define PXSTR_SENSE               G8

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

  Serial.printf("Done: setup func.\n");
}

void loop() {
  unsigned int env_light_value;
  unsigned int led_and_env_light_value;
  unsigned int led_light_value;

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
  led_light_value = led_and_env_light_value - env_light_value;

  //センサ値の表示
  Serial.printf("PXSTR_SENSE AD VALUE: LED+ENV %d\tENV %d\tLED %d\n", led_and_env_light_value, env_light_value, led_light_value);
  delay(500);
}
