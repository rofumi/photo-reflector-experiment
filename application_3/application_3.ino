#include "M5AtomS3.h"

#define VISIBLE_LED               G6
#define INFRARED_LED              G5
#define PXSTR_SENSE               G8
#define SERVO                     G7

#define SERVO_PULSE_MS_MIN      0.5
#define SERVO_PULSE_MS_MAX      2.4
#define SERVO_ANGLE_MIN         0.0
#define SERVO_ANGLE_MAX         180.0
#define SERVO_FREQ              50               //Hz
#define SERVO_PWM_PERIOD_MS     20.0
#define SERVO_CH                0
#define SERVO_RESOLUTION        16383
#define SERVO_DUTY(w)           (unsigned int)((float)SERVO_RESOLUTION * (w)/SERVO_PWM_PERIOD_MS)

#define LIGHT_VALUE_MAX         4095

#define LED_DATA_PIN 35                         //ATOMS3に搭載されているRGB LEDのDATA PINを指定
#define NUM_LEDS 1                              //RGB LEDの数を指定
CRGB leds[NUM_LEDS];

unsigned int angle2duty(float angle) {
    float pulse;

    if(angle < SERVO_ANGLE_MIN){
        pulse = SERVO_PULSE_MS_MIN;
    }else if(angle > SERVO_ANGLE_MAX){
        pulse = SERVO_PULSE_MS_MAX;
    }else{
        pulse = angle*(SERVO_PULSE_MS_MAX - SERVO_PULSE_MS_MIN)/SERVO_ANGLE_MAX + SERVO_PULSE_MS_MIN;
    }

    return SERVO_DUTY(pulse);
}

void setup() {
  auto cfg = M5.config();
  AtomS3.begin(cfg);

  //=== ATOMS3の設定
  pinMode(VISIBLE_LED, OUTPUT);                         //VISIBLE_LEDに繋がるピンを出力に設定
  digitalWrite(VISIBLE_LED, LOW);                       //電圧をLOW（0V）に設定
  pinMode(INFRARED_LED, OUTPUT);
  digitalWrite(INFRARED_LED, LOW);
  pinMode(PXSTR_SENSE, ANALOG);                         //フォトリフレクタ（PXSTR）繋がるピンをADCに設定

  pinMode(SERVO, OUTPUT);                               //SERVOに繋がるピンの設定
  ledcSetup(SERVO_CH, SERVO_FREQ, 14);                  //CH Hz 分解能
  ledcAttachPin(SERVO, SERVO_CH);                       //pinとchをセット
  ledcWrite(SERVO_CH, angle2duty(0.0));                 //chにdutyを設定

  Serial.println("init pinmode.");

  //=== ATOMS3にソフトを書き込み出来ているかの確認用。 ATOMS3内部のLEDを青に光らせる。
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
  static float tar_angle = 0.0;
  static bool moving_dir_is_plus;
  static float control_value;
  static unsigned char mode = 0;

  AtomS3.update();

  if(AtomS3.BtnA.wasPressed()){
    mode++;
    if(mode > 1)  mode = 0;
  }

  //=== フォトリフレクタのセンサ値取得
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
  control_value = 0.85 * control_value + 0.15 * led_light_value;

  //=== サーボモータの制御
  if(moving_dir_is_plus == true){
    tar_angle += 2.0 * (1.0 + 3.0 * control_value / LIGHT_VALUE_MAX);
    if(tar_angle > 160.0){
      tar_angle = 160.0;
      moving_dir_is_plus = false;
    }   
  }else{
    tar_angle -= 2.0 * (1.0 + 3.0 * control_value / LIGHT_VALUE_MAX);
    if(tar_angle < 20.0){
      tar_angle = 20.0;
      moving_dir_is_plus = true;
    }   
  }

  if(mode == 1){
    tar_angle = 0.0;
    leds[0] = CRGB::Blue;
  }else{
    leds[0] = CRGB::Red; 
  }

  ledcWrite(SERVO_CH, angle2duty(tar_angle));
  FastLED.show();  

  Serial.printf("led_light_value %d, control_value %f, tar_angle %f\n", led_light_value, control_value, tar_angle);
}
