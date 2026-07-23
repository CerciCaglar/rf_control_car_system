#include <Arduino.h>
#include <SoftwareSerial.h>
 
SoftwareSerial rfMerkezi(2, 12);
 
unsigned long son_radar = 0;
 
const int ENA = 5;
const int IN1 = 8;
const int IN2 = 7;
const int IN3 = 10;
const int IN4 = 9;
const int ENB = 6;
 
const int buzzer_pin = 11;
const int olu_bolge = 75;
 
const int trig_pin = 4;
const int echo_pin = 3;
 
const int ledR = A5;
const int ledG = A4;
 
bool led_aktif = false;
int eski_buton = 1;
 
int mesafeOku() {
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
 
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
 
  long sure = pulseIn(echo_pin, HIGH, 15000);
 
  int mesafe = sure * 0.034 / 2;
 
  if (mesafe == 0) {
    return 999;
  }
 
  return mesafe;
}
 
void setup() {
  Serial.begin(115200);
  rfMerkezi.begin(2400);
 
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
 
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
 
  pinMode(buzzer_pin, OUTPUT);
  digitalWrite(buzzer_pin, LOW);
 
  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
}
 
void loop() {
  while (rfMerkezi.available() > 20) {
    char gelen_karakter = rfMerkezi.read();
 
    if (gelen_karakter == '<') {
      String paket = rfMerkezi.readStringUntil('>');
 
      int virgul1 = paket.indexOf(',');
      int virgul2 = paket.indexOf(',', virgul1 + 1);
 
      if (virgul1 != -1 && virgul2 != -1) {
 
        String x_str = paket.substring(0, virgul1);
        String y_str = paket.substring(virgul1 + 1, virgul2);
        String buton_str = paket.substring(virgul2 + 1);
 
        int x_ekseni = x_str.toInt();
        int y_ekseni = y_str.toInt();
        int buton = buton_str.toInt();
 
        int hiz = map(y_ekseni, -2048, 2047, -255, 255);
        int donus = map(x_ekseni, -2048, 2047, -255, 255);
 
        // Ölü bölge kontrolü
        if (abs(hiz) < olu_bolge) hiz = 0;
        if (abs(donus) < olu_bolge) donus = 0;
 
        int sol_motor = hiz - donus;
        int sag_motor = hiz + donus;
 
        //-255 ile 255 sınırlandır
        sol_motor = constrain(sol_motor, -255, 255);
        sag_motor = constrain(sag_motor, -255, 255);
 
        //SOL MOTOR
        if (sol_motor > 0) {
          digitalWrite(IN1, HIGH);
          digitalWrite(IN2, LOW);
          analogWrite(ENA, sol_motor);
        } else if (sol_motor < 0) {
          digitalWrite(IN1, LOW);
          digitalWrite(IN2, HIGH);
          analogWrite(ENA, abs(sol_motor));
        } else {
          analogWrite(ENA, 0);
        }
 
        //SAĞ MOTOR
        if (sag_motor > 0) {
          digitalWrite(IN3, HIGH);
          digitalWrite(IN4, LOW);
          analogWrite(ENB, sag_motor);
        } else if (sag_motor < 0) {
          digitalWrite(IN3, LOW);
          digitalWrite(IN4, HIGH);
          analogWrite(ENB, abs(sag_motor));
        } else {
          analogWrite(ENB, 0);
        }
 
        if (buton == 0 && eski_buton == 1) {
          led_aktif = !led_aktif;
        }
 
        eski_buton = buton;
      }
    }
  }
 
  if (millis() - son_radar > 50) {

    rfMerkezi.stopListening();

    int mesafe = mesafeOku();
    
    rfMerkezi.listen();
 
    son_radar = millis();
 
    if (led_aktif == true) {
 
      if (mesafe > 40 || mesafe == 999) {
        digitalWrite(ledR, LOW);
        digitalWrite(ledG, HIGH);
        digitalWrite(buzzer_pin, LOW);
      }
      else if (mesafe <= 40 && mesafe > 15) {
        digitalWrite(ledR, HIGH);
        digitalWrite(ledG, HIGH);
        digitalWrite(buzzer_pin, LOW);
      }
      else {
        digitalWrite(ledR, HIGH);
        digitalWrite(ledG, LOW);
        digitalWrite(buzzer_pin, HIGH); // led kırmızıyken buzzer öter
      }
 
    } else { // ledi ve kornayı kapat
      digitalWrite(ledR, LOW);
      digitalWrite(ledG, LOW);
      digitalWrite(buzzer_pin, LOW);
    }
  }
  delay(30);
}