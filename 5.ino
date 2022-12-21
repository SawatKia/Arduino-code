//button
#define B1 1
#define B2 2
#define B3 3
//LED
#define Red_N 4
#define Yellow_N 5
#define Green_N 6
#define Red_E 7
#define Yellow_E 8
#define Green_E 9
#define Red_walk 11
#define Green_walk 12
//status
#define Nocar 0
#define N_car 8

int lastB1 = LOW;
int lastB2 = LOW;
int lastB3 = LOW;

void setup()
{
  //LEd setup
  for (int i = 4; i <= 12; i++) {
    pinMode(i, OUTPUT);
  }
  //button setup
  for (int i = 1; i <= 3; i++) {
    pinMode(i, INPUT);
  }
  //at first no light all pass free 
  digitalWrite(Green_N, HIGH);
  digitalWrite(Green_E, HIGH);
  digitalWrite(Green_walk, HIGH);
}

void loop() {
  if (digitalRead(B1) == HIGH && lastB1 == LOW) {
    delay(50);
    Walk_Red();
    North_Green();
  }
  else if (digitalRead(B2) == HIGH && lastB2 == LOW) {
    delay(50);
    Walk_Red();
    East_Green();
  }
  else if (digitalRead(B3) == HIGH && lastB3 == LOW) {
    delay(50);
    Walk_Green();
  }
}

void North_Green() {
  East_Red();
  /*if(digitalRead(Green_walk)==HIGH){
     Walk_Red();
    }*/
  digitalWrite(Red_N, LOW);
  digitalWrite(Green_N, HIGH);
}
void North_Red() {
  if (digitalRead(Green_N) == HIGH) {
    digitalWrite(Green_N, LOW);
    //Walk_Red();
    long Ytime = millis();
    while (millis() - Ytime <= 1500) { //3s too long
      digitalWrite(Yellow_N, HIGH);
    }
    digitalWrite(Yellow_N, LOW);
    digitalWrite(Red_N, HIGH);
  }
}
void East_Red() {
  if(digitalRead(Green_E)==HIGH){
  digitalWrite(Green_E, LOW);
  //Walk_Red();
  long Ytime = millis();
  while (millis() - Ytime <= 1500) { //3s too long
    digitalWrite(Yellow_E, HIGH);
  }
  digitalWrite(Yellow_E, LOW);
  digitalWrite(Red_E, HIGH);
  }
}
void East_Green() {
  North_Red();
  /*if(digitalRead(Green_walk)==HIGH){
     Walk_Red();
    }*/
  digitalWrite(Red_E, LOW);
  digitalWrite(Green_E, HIGH);
}
void Walk_Red() {
  if (digitalRead(Red_walk) != HIGH) { //if it red don't do
    long Gtime = millis();
    while (millis() - Gtime < 2000) {
      digitalWrite(Green_walk, HIGH);
      delay(400);
      digitalWrite(Green_walk, LOW);
      delay(400);
    }
    digitalWrite(Red_walk, HIGH);
  }
}
void Walk_Green() {
  North_Red();
  East_Red();
  if (digitalRead(Red_walk) == HIGH) {
    digitalWrite(Red_walk, LOW);
    digitalWrite(Green_walk, HIGH);
  }
}
