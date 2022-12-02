void setup(){
  pinMode(A0, INPUT);
  pinMode(D2, OUTPUT);
  Serial.begin(9600);
}
void loop(){
  int value = analogRead(A0);
  if (value < 10)
    value = 0;
  if (value > 1015)
    value = 1023;
  analogWrite(D2, value);
  Serial.println(value);
}
