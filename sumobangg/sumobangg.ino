#define ch1 A0
#define ch2 A1
#define ch3 A2

const int L_RPWM = 3;
const int L_LPWM = 5;

const int R_RPWM = 6;
const int R_LPWM = 11;

const int en1 = 9;
const int en2 = 7;

bool isReady;

long forwardSpeed, backwardSpeed, speed, threshold, lastFordward;
bool isTas1, isTas2, isSlowdown;
int x1, x2, t1, t2;

void setup() {
  // put your setup code here, to run once:
  isReady = false;
  Serial.begin(115200);
  threshold = 30;
  speed = 140;

  isTas1 = false;
  isTas2 = false;
  isSlowdown = false;

  x1 = 90;
  x2 = 200;
  t1 = 200;
  t2 = 50;

  pinMode(ch1, INPUT);
  pinMode(ch2, INPUT);
  pinMode(ch3, INPUT);

  pinMode(L_RPWM, OUTPUT);
  pinMode(L_LPWM, OUTPUT);
  pinMode(R_RPWM, OUTPUT);
  pinMode(R_LPWM, OUTPUT);

  pinMode(en1, OUTPUT);
  pinMode(en2, OUTPUT);

  digitalWrite(en1, HIGH);
  digitalWrite(en2, HIGH);

  direction(0, 0, 0, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  int right = pulseIn(ch1, HIGH);
  int left = pulseIn(ch2, HIGH);
  int tastas = pulseIn(ch3, HIGH);

  byte rightControl = map(right, 965, 2000, 0, 200);
  byte leftControl = map(left, 965, 2000, 0, 200);

  Serial.print("right :  ");
  Serial.println(right);
  Serial.print("left :  ");
  Serial.println(left);
  Serial.print("tas :  ");
  Serial.println(tastas);
  Serial.print("rightcontrol :  ");
  Serial.println(rightControl);
  Serial.print("leftcontrol :  ");
  Serial.println(leftControl);


  if (rightControl > 100 + threshold) {
    forwardSpeed = map(rightControl, 120, 200, 0, 255);
    backwardSpeed = 0;

  } else if (rightControl < 100 - threshold) {
    backwardSpeed = map(rightControl, 80, 0, 0, 255);
    forwardSpeed = 0;
  } else {
    forwardSpeed = 0;
    backwardSpeed = 0;
  }

  Serial.print(tastas);
  if (tastas < 1300) {
    Serial.println("tas idle");
    isTas1 = false;
    isTas2 = false;
  } else if (tastas < 1800) {
    Serial.println("tas mode 1");
    isTas1 = true;
    isTas2 = false;
  } else {
    Serial.println("tas mode 2");
    isTas1 = false;
    isTas2 = true;
  }

  if (isTas1) {
    Serial.println("tas1");
  } else if (isTas2) {
    if (leftControl > 100 + threshold) {
      tasLeft();
    } else if (leftControl < 100 - threshold) {
      tasRight();
    }
    Serial.println("tas2");
  }
  if (rightControl < 55) {
    isReady = true;
  }

  // if (isReady) {
  if (leftControl > 100 + threshold) {
    turnLeft();
    Serial.println("kiri");
  } else if (leftControl < 100 - threshold) {
    turnRight();
    Serial.println("kanan");
  } else if (backwardSpeed > 0 || forwardSpeed > 0) {
    backNforth();
    Serial.println("maju mundur");
  } else {
    slowdown();
    direction(0, 0, 0, 0);
    Serial.println("stop");
  }
  // }

  Serial.println(isReady);
}

void tasRight() {
  direction(0, x1, 0, x2);
  delay(t1);
  direction(0, x2, 0, x1);
  delay(t2);
}

void tasLeft() {
  direction(0, x2, 0, x1);
  delay(t1);
  direction(0, x1, 0, x2);
  delay(1000);
}

void turnLeft() {
  // direction(speed, 0, 0, speed);
  direction(0, 0, 255, 0);
  // direction(0, 0, speed, 0);
}
void turnRight() {
  // direction(0, speed, speed, 0);
  direction(speed, 0, 0, 0);
}

void backNforth() {
  if (forwardSpeed > threshold) {
    int forwardSpeedLeft = forwardSpeed + 50;
    lastFordward = forwardSpeed;
    if (forwardSpeedLeft < 255) {
      direction(forwardSpeed, 0, forwardSpeedLeft, 0);
    }else{
      direction(forwardSpeed, 0, 255, 0);

    }
    Serial.print("forward speed : ");
    Serial.println(forwardSpeed);
  } else if (backwardSpeed > 0) {
    direction(0, backwardSpeed, 0, backwardSpeed);

    Serial.print("backward speed : ");
    Serial.println(backwardSpeed);
  } else {
    isSlowdown = true;
  }
}

void slowdown() {
  if (isSlowdown) {
    Serial.println("slowdown");
    while (lastFordward > 0) {
      direction(lastFordward, 0, lastFordward + 20, 0);
      lastFordward -= 5;
      delay(200);
    }
    isSlowdown = false;
  }
}

void direction(int R_L, int R_R, int L_L, int L_R) {
  analogWrite(R_LPWM, R_L);
  analogWrite(R_RPWM, R_R);
  analogWrite(L_LPWM, L_L);
  analogWrite(L_RPWM, L_R);
}
