#define ch1 A0
#define ch2 A1
#define ch3 A2

const int L_RPWM = 3;
const int L_LPWM = 5;

const int R_RPWM = 10;
const int R_LPWM =11;

const int enable = 8;


long forwardSpeed, backwardSpeed, speed, threshold;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  threshold = 10;

  pinMode(ch1, INPUT_PULLUP);
  pinMode(ch2, INPUT_PULLUP);
  pinMode(ch3, INPUT_PULLUP);

  pinMode(L_RPWM, OUTPUT);
  pinMode(L_LPWM, OUTPUT);
  pinMode(R_RPWM, OUTPUT);
  pinMode(R_LPWM, OUTPUT);
  pinMode(enable, OUTPUT);

  digitalWrite(enable, 255);
}

void loop() {
  // put your main code here, to run repeatedly:
  int right = pulseIn(ch1, HIGH, 25000);
  int left = pulseIn(ch2, HIGH, 25000);
  int tastas = pulseIn(ch3, HIGH, 25000);

  byte rightControl = map(right, 985, 2000, 0, 200);
  byte leftControl = map(left, 985, 2000, 0, 200);

  Serial.print(tastas);
  if (tastas < 1300) {
    Serial.println("tas idle");
  } else if (tastas < 1800) {
    Serial.println("tas mode 1");
  } else {
    Serial.println("tas mode 2");
  }


  if (rightControl > 110) {
    forwardSpeed = map(rightControl, 120, 200, 0, 255);
    backwardSpeed = 0;

  } else if (rightControl < 90) {
    backwardSpeed = map(rightControl, 80, 0, 0, 255);
    forwardSpeed = 0;
  } else {
    forwardSpeed = 0;
    backwardSpeed = 0;
  }

  if (backwardSpeed > threshold) {
    speed = backwardSpeed;
  } else if (forwardSpeed > threshold) {
    speed = forwardSpeed;
  } else {
    speed = 0;
  }


  if (leftControl > 120) {
    turnLeft();
  } else if (leftControl < 80) {
    turnRight();
  } else if (backwardSpeed != threshold || forwardSpeed != threshold) {
    backNforth();
  } else {
    motorStop();
  }
}

void motorStop() {
  digitalWrite(R_LPWM, 0);
  digitalWrite(R_RPWM, 0);
  digitalWrite(L_LPWM, 0);
  digitalWrite(L_RPWM, 0);
}

void turnLeft() {
  digitalWrite(R_LPWM, speed);
  digitalWrite(R_RPWM, 0);
  digitalWrite(L_LPWM, 0);
  digitalWrite(L_RPWM, speed);
  Serial.println("turn left");
  Serial.print("forward speed : ");
  Serial.println(forwardSpeed);
  Serial.print("backward speed : ");
  Serial.println(backwardSpeed);
}
void turnRight() {
  digitalWrite(R_LPWM, 0);
  digitalWrite(R_RPWM, speed);
  digitalWrite(L_LPWM, speed);
  digitalWrite(L_RPWM, 0);
  Serial.println("turn right");
  Serial.print("forward speed : ");
  Serial.println(forwardSpeed);
  Serial.print("backward speed : ");
  Serial.println(backwardSpeed);
}

void backNforth() {
  if (forwardSpeed != 0) {
    digitalWrite(R_LPWM, forwardSpeed);
    digitalWrite(R_RPWM, 0);
    digitalWrite(L_LPWM, forwardSpeed);
    digitalWrite(L_RPWM, 0);
    Serial.print("forward speed : ");
    Serial.println(forwardSpeed);
  }
  if (backwardSpeed != 0) {
    digitalWrite(R_LPWM, 0);
    digitalWrite(R_RPWM, backwardSpeed);
    digitalWrite(L_LPWM, 0);
    digitalWrite(L_RPWM, backwardSpeed);
    Serial.print("backward speed : ");
    Serial.println(backwardSpeed);
  }
}
