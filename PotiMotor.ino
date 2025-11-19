const int MRED = 3;
const int MBLACK = 5;
const int POTI = A0;
int value;
void setup() {
  pinMode(MRED, OUTPUT);
  pinMode(MBLACK, OUTPUT);
  Serial.begin(9600);

}

void loop() {
  value = analogRead(POTI);
  Serial.print("Init: ");
  Serial.println(value);
  if (value <= 511) {
  int pwm = map(value, 0, 511, 255, 0);
  analogWrite(MBLACK, 0);
  analogWrite(MRED, pwm);
  Serial.print("conv: ");
  Serial.println(pwm);
} else {
  int pwm = map(value, 512, 1023, 0, 255);
  analogWrite(MRED, 0);
  analogWrite(MBLACK, pwm);
  Serial.print("conv: ");
  Serial.println(pwm);
}

  delay(10);
  
}
