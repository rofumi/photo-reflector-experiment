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
  static bool change_light_led = false;
  AtomS3.update();

  //=== ATOMS3のボタンを押した場合、点灯するLEDを変更する
  if(AtomS3.BtnA.wasPressed()){
    if(change_light_led == true)  change_light_led = false;
    else change_light_led = true;
    Serial.printf("Pressed button. LED changed.\n");
  }

  if(change_light_led){
    //可視光LEDを点灯させて、センサ値を表示する
    digitalWrite(VISIBLE_LED, HIGH);
    delay(500);
    Serial.printf("PXSTR_SENSE AD VALUE(VISIBLE LED): (LIGHTED, NOT LIGHTED): ");
    Serial.printf("(%d, \t", analogRead(PXSTR_SENSE));
    digitalWrite(VISIBLE_LED, LOW);
    delay(500);
    Serial.printf("%d)\n", analogRead(PXSTR_SENSE));
  }else{
    //フォトリフレクタについている赤外線LEDを点灯させて、センサ値を表示する
    digitalWrite(INFRARED_LED, HIGH);
    delay(500); 
    Serial.printf("PXSTR_SENSE AD VALUE(INFRARED LED): (LIGHTED, NOT LIGHTED): ");
    Serial.printf("(%d, \t", analogRead(PXSTR_SENSE));
    digitalWrite(INFRARED_LED, LOW);
    delay(500);
    Serial.printf("%d)\n", analogRead(PXSTR_SENSE));  
  }
}
