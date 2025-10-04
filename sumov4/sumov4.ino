#define CH1_PIN A0
#define CH2_PIN A1
#define CH3_PIN A2

const int leftForward = 9;
const int leftBackward = 10;
const int rightForward = 11;
const int rightBackward = 12;

int treshold;
//9101112

const int speedPin1 = 3;
// const int speedPin2 = 13;


unsigned int ch1, ch2, ch3;

void direction(uint8_t lfd, uint8_t lbd, uint8_t rfd, uint8_t rbd) {
  digitalWrite(leftForward, lfd);
  digitalWrite(leftBackward, lbd);
  digitalWrite(rightForward, rfd);
  digitalWrite(rightBackward, rbd);
}

void motorStop() {
  Serial.println("motorStop");
  direction(HIGH, HIGH, HIGH, HIGH);
}
void forward() {
  Serial.println("forward");
  direction(HIGH, LOW, HIGH, LOW);
}

void backward() {
  Serial.println("backward");
  direction(LOW, HIGH, LOW, HIGH);
}

void pivotBackLeft(){
  Serial.println("pivotBackLeft");
  direction(LOW, LOW, LOW, HIGH);
}

void pivotBackRight(){
  Serial.println("pivotBackRight");
  direction(LOW, HIGH, LOW, LOW);
}

void turnForLeft() {
  Serial.println("turnForLeft");
  direction(LOW, HIGH, HIGH, LOW);
}

void turnForRight() {
  Serial.println("turnForRight");
  direction(HIGH, LOW, LOW, HIGH);
}

void setup() {
  Serial.begin(115200);

  pinMode(CH1_PIN, INPUT_PULLUP);
  pinMode(CH2_PIN, INPUT_PULLUP);
  pinMode(CH3_PIN, INPUT_PULLUP);

  pinMode(leftForward, OUTPUT);
  pinMode(leftBackward, OUTPUT);
  pinMode(rightForward, OUTPUT);
  pinMode(rightBackward, OUTPUT);

  pinMode(speedPin1, OUTPUT);

  treshold = 200;
}

void loop() {

  ch1 = pulseIn(CH1_PIN, HIGH);
  ch2 = pulseIn(CH2_PIN, HIGH);
  ch3 = pulseIn(CH3_PIN, HIGH);

  Serial.print("ch 1 ");
  Serial.println(ch1);
  Serial.print("ch 2 ");
  Serial.println(ch2);
  Serial.print("ch 3 ");
  Serial.println(ch3);

  //ch1 for forward n backward
  //ch2 for right n left
  digitalWrite(4, HIGH);

  if (ch1 > 1500+treshold) {
    forward();
  } else if (ch1 < 1500-treshold) {
    if (ch2 > 1500+treshold) {
      pivotBackRight();
    } else if (ch2 < 1500-treshold) {
      pivotBackLeft();
    } else {
    backward();
    }
  } else {
    if (ch2 > 1500+treshold) {
      turnForRight();
    } else if (ch2 < 1500-treshold) {
      turnForLeft();
    } else {
      motorStop();
    }
  }

  //ch3 for speed

  if (ch3 < 1200) {
    Serial.println("opsi 1");
    digitalWrite(speedPin1, LOW);
    // digitalWrite(speedPin2, LOW);
  } else if (ch3 > 1600) {
    Serial.println("opsi 2");
    digitalWrite(speedPin1, HIGH);
    // digitalWrite(speedPin2, LOW);
  } 
}
