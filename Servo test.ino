#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define PCA_ADDR_0 0x40
#define PCA_ADDR_1 0x41
#define SERVO_FREQ 50

Adafruit_PWMServoDriver pwm[2] = {
  Adafruit_PWMServoDriver(PCA_ADDR_0),
  Adafruit_PWMServoDriver(PCA_ADDR_1)
};

uint8_t  selBoard = 0;
uint8_t  selChan  = 0;
uint16_t pulseUs  = 1500;

const uint16_t US_MIN = 500;
const uint16_t US_MAX = 2500;

void apply() {
  pwm[selBoard].writeMicroseconds(selChan, pulseUs);
  Serial.print(F("board ")); Serial.print(selBoard);
  Serial.print(F(" ch "));   Serial.print(selChan);
  Serial.print(F(" -> "));   Serial.print(pulseUs);
  Serial.println(F(" us"));
}

void sweep() {
  for (uint16_t u = US_MIN; u <= US_MAX; u += 10) {
    pwm[selBoard].writeMicroseconds(selChan, u);
    delay(10);
  }
  for (int32_t u = US_MAX; u >= (int32_t)US_MIN; u -= 10) {
    pwm[selBoard].writeMicroseconds(selChan, (uint16_t)u);
    delay(10);
  }
  pulseUs = 1500;
  apply();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);

  for (uint8_t b = 0; b < 2; b++) {
    pwm[b].begin();
    pwm[b].setOscillatorFrequency(27000000);
    pwm[b].setPWMFreq(SERVO_FREQ);
  }

  Serial.println(F("PCA9685 servo test ready."));
  Serial.println(F("b0/b1 board, 0-9 a-f channel, +/- nudge 25us, n centre, w sweep."));
  apply();
}

void loop() {
  if (!Serial.available()) return;
  char k = Serial.read();

  if (k == 'b') {           
    while (!Serial.available()) {}
    char n = Serial.read();
    selBoard = (n == '1') ? 1 : 0;
    apply();
  } else if (k >= '0' && k <= '9') {
    selChan = k - '0';  apply();
  } else if (k >= 'a' && k <= 'f') {
    selChan = 10 + (k - 'a'); apply();
  } else if (k == '+' || k == '=') {
    if (pulseUs + 25 <= US_MAX) pulseUs += 25;
    apply();
  } else if (k == '-' || k == '_') {
    if (pulseUs >= US_MIN + 25) pulseUs -= 25;
    apply();
  } else if (k == 'n') {
    pulseUs = 1500; apply();
  } else if (k == 'w') {
    sweep();
  }
}
