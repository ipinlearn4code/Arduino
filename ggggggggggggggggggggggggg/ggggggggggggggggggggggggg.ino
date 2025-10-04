const int switchPin []= {6,7};

void setup() {
  //pinMode(switchPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  for (int i=0; i<2; i++) {
    Serial.println(digitalRead(switchPin[i]));
  }
    
  
  delay(1000);
}
