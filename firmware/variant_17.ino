// Вариант 17. Кухня ресторана. Контроль загазованности
// Плата: Arduino Uno (ATmega328P), симулятор Wokwi
// Подключение:
//   Датчик 1 (MQ2, газ 0-100 %)        -> AOUT на A0
//   Датчик 2 (NTC, температура, °C)    -> OUT на A1
//   Датчик 3 (DHT22, влажность, %)     -> SDA на D2
//   Реле K1 (вытяжка)                  -> IN на D7
//   Светодиод HL1 (жёлтый) через R1 = 200 Ом -> D8

#include <DHT.h>

const int PIN_S1    = A0;   // датчик 1: MQ2 (газ)
const int PIN_S2    = A1;   // датчик 2: NTC (температура)
const int PIN_S3    = 2;    // датчик 3: DHT22 (влажность)
const int PIN_RELAY = 7;    // вытяжка
const int PIN_LED   = 8;    // светодиод-индикатор

const unsigned long T_MS = 1000;  // период опроса T = 1 с
const float S1_MIN = 0.0;         // диапазон датчика 1: 0...100 %
const float S1_MAX = 100.0;
const float THRESHOLD = 40.0;     // порог: газ > 40 %
const float BETA = 3950.0;        // коэффициент NTC в Wokwi

DHT dht(PIN_S3, DHT22);

unsigned long n = 0;   // номер измерения
bool alarm = false;    // порог сейчас превышен?

// Датчик 1: MQ2 (газ, %)
float readS1() {
  return S1_MIN + (S1_MAX - S1_MIN) * analogRead(PIN_S1) / 1023.0;
}

// Датчик 2: NTC (температура, °C)
float readS2() {
  int raw = analogRead(PIN_S2);
  if (raw < 1) raw = 1;  // защита от деления на ноль
  return 1.0 / (log(1.0 / (1023.0 / raw - 1.0)) / BETA + 1.0 / 298.15) - 273.15;
}

// Датчик 3: DHT22 (влажность, %)
float readS3() {
  return dht.readHumidity();
}

void actuatorOn()  { digitalWrite(PIN_RELAY, HIGH); }
void actuatorOff() { digitalWrite(PIN_RELAY, LOW); }

bool conditionMet(float s1, float s2, float s3) {
  return s1 > THRESHOLD;
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  actuatorOff();
  digitalWrite(PIN_LED, LOW);
}

void loop() {
  n++;
  float s1 = readS1();
  float s2 = readS2();
  float s3 = readS3();

  Serial.print("DATA n="); Serial.print(n);
  Serial.print(" s1="); Serial.print(s1, 2);
  Serial.print(" s2="); Serial.print(s2, 1);
  Serial.print(" s3=");
  if (isnan(s3)) Serial.println("nan"); else Serial.println(s3, 1);

  bool cond = conditionMet(s1, s2, s3);
  if (cond && !alarm) {
    alarm = true;
    actuatorOn();
    digitalWrite(PIN_LED, HIGH);
    Serial.print("EVENT n="); Serial.print(n);
    Serial.println(" type=THRESHOLD");
  } else if (!cond && alarm) {
    alarm = false;
    actuatorOff();
    digitalWrite(PIN_LED, LOW);
    Serial.print("EVENT n="); Serial.print(n);
    Serial.println(" type=NORMAL");
  }

  delay(T_MS);
}
