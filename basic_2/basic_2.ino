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
  AtomS3.update();

  digitalWrite(INFRARED_LED, HIGH);
  delay(500); 
  Serial.printf("PXSTR_SENSE AD VALUE: (LIGHTED, NOT LIGHTED): ");
  Serial.printf("(%d, \t", analogRead(PXSTR_SENSE));
  digitalWrite(INFRARED_LED, LOW);
  delay(500);
  Serial.printf("%d)\n", analogRead(PXSTR_SENSE));

}
