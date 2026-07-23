#include <Arduino.h>
#include "LedControl.h"
 
const int x_pin = 27;
const int y_pin = 26;
const int buton_pin = 22;
 
LedControl lc = LedControl(11, 13, 12, 1);
 
const int olu_bolge = 75;
 
void bar_ciz(int sutun, int hiz) {
  for (int i = 0; i < 8; i++) {
    lc.setLed(0, sutun, i, false);
  }
 
  if (abs(hiz) < olu_bolge) {
    lc.setLed(0, sutun, 3, true);
    lc.setLed(0, sutun, 4, true);
    return;
  }
 
  int seviye = map(abs(hiz), olu_bolge, 235, 1, 4);
  seviye = constrain(seviye, 1, 4);
 
  if (hiz > 0) {
    for (int i = 0; i < seviye; i++) {
      lc.setLed(0, sutun, 4 + i, true);
    }
  } else {
    for (int i = 0; i < seviye; i++) {
      lc.setLed(0, sutun, 3 - i, true);
    }
  }
}
 
void setup() {
  Serial.begin(115200);
  Serial1.begin(2400);
 
  pinMode(buton_pin, INPUT_PULLUP);
  analogReadResolution(12);
 
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
}
 
void loop() {
  int x = analogRead(x_pin) - 2048;          
  int y = analogRead(y_pin) - 2048;               
  int buton = digitalRead(buton_pin);   
 
  Serial1.print('<');
  Serial1.print(x);
  Serial1.print(',');
  Serial1.print(y);
  Serial1.print(',');
  Serial1.print(buton);
  Serial1.print('>');
 
  int hiz = map(y, -2048, 2047, -255, 255);
  int donus = map(x, -2048, 2047, -255, 255);
 
  if (abs(hiz) < olu_bolge) hiz = 0;
  if (abs(donus) < olu_bolge) donus = 0;
 
  int sol_motor = constrain(hiz + donus, -255, 255);
  int sag_motor = constrain(hiz - donus, -255, 255);
 
  bar_ciz(0, sol_motor);
  bar_ciz(1, sol_motor);
 
  bar_ciz(6, sag_motor);
  bar_ciz(7, sag_motor);
 
  delay(80);
}