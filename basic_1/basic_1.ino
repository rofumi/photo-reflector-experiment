#include "M5AtomS3.h"

#define VISIBLE_LED G6

void setup() {
  auto cfg = M5.config();
  AtomS3.begin(cfg);

  //=== ATOMS3の設定
  pinMode(VISIBLE_LED, OUTPUT);         //VISIBLE_LEDに繋がるピンを出力に設定
  digitalWrite(VISIBLE_LED, LOW);       //電圧をLOW（0V）に設定
  Serial.println("init pinmode.");

  Serial.printf("Done: setup func.\n");
}

void loop() {
  AtomS3.update();

  digitalWrite(VISIBLE_LED, HIGH);
  delay(500);
  digitalWrite(VISIBLE_LED, LOW);
  delay(500); 

}
