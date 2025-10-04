#define ch1 A0
#define ch2 A1
#define ch3 A2

const int L_RPWM = 5;
const int L_LPWM = 3;

const int R_RPWM = 10;
const int R_LPWM =11;

// const int enable = 8;


long forwardSpeed, backwardSpeed, speed, threshold;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  threshold = 50;

  pinMode(ch1, INPUT_PULLUP);
  pinMode(ch2, INPUT_PULLUP);
  pinMode(ch3, INPUT_PULLUP);

  pinMode(L_RPWM, OUTPUT);
  pinMode(L_LPWM, OUTPUT);
  pinMode(R_RPWM, OUTPUT);
  pinMode(R_LPWM, OUTPUT);
  // pinMode(enable, OUTPUT);

  // digitalWrite(enable, 255);
}

void loop() {
  // put your main code here, to run repeatedly:
  int right = pulseIn(ch1, HIGH, 25000);
  int left = pulseIn(ch2, HIGH, 25000);
  int tastas = pulseIn(ch3, HIGH, 25000);

  byte rightControl = map(right, 985, 2000, 0, 200);
  byte leftControl = map(left, 985, 2000, 0, 200);

  // Serial.print("right : ");
  // Serial.println(right);
  // Serial.print("left : ");
  // Serial.println(left);
  // Serial.print("tastas : ");
  // Serial.println(tastas);

  // Serial.print(tastas);
  // if (tastas < 1300) {
  //   Serial.println("tas idle");
  // } else if (tastas < 1800) {
  //   Serial.println("tas mode 1");
  // } else {
  //   Serial.println("tas mode 2");
  // }


  if (rightControl > 150) {
    forwardSpeed = map(rightControl, 120, 200, 0, 255);
    backwardSpeed = 0;

  } else if (rightControl < 50) {
    backwardSpeed = map(rightControl, 80, 0, 0, 255);
    forwardSpeed = 0;
  } else {
    Serial.print("chek");
    forwardSpeed = 0;
    backwardSpeed = 0;
  }


  if (backwardSpeed > 0) {
    speed = backwardSpeed;
  } else if (forwardSpeed > 0) {
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
    direction(0, 0, 0, 0);
  }
}


void turnLeft() {
  direction(speed, 0, 0, speed);

  Serial.println("turn left");
  Serial.print("forward speed : ");
  Serial.println(forwardSpeed);
  Serial.print("backward speed : ");
  Serial.println(backwardSpeed);
}
void turnRight() {
  direction(0, speed, speed, 0);

  Serial.println("turn right");
  Serial.print("forward speed : ");
  Serial.println(forwardSpeed);
  Serial.print("backward speed : ");
  Serial.println(backwardSpeed);
}

void backNforth() {
  if (forwardSpeed > threshold) {
    direction(forwardSpeed, 0, forwardSpeed, 0);

    Serial.print("forward speed : ");
    Serial.println(forwardSpeed);
  }
  if (backwardSpeed > threshold) {
    direction(0, backwardSpeed, 0, backwardSpeed);

    Serial.print("backward speed : ");
    Serial.println(backwardSpeed);
  }
}

void direction(int R_L, int R_R, int L_L, int L_R) {
  analogWrite(R_LPWM, R_L);
  analogWrite(R_RPWM, R_R);
  analogWrite(L_LPWM, L_L);
  analogWrite(L_RPWM, L_R);
}
