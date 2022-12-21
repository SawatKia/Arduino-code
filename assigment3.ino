#define switch1 13
#define switch2 12
#define A 2
#define B 3
#define C 4
#define D 5
#define E 6
#define F 7
#define G 8
#define DP 9

bool isbuttonpress = false;
boolean num[12][8] = {1, 1, 1, 1, 1, 1, 0, 0, //0
                      0, 1, 1, 0, 0, 0, 0, 0, //1
                      1, 1, 0, 1, 1, 0, 1, 0, //2
                      1, 1, 1, 1, 0, 0, 1, 0, //3
                      0, 1, 1, 0, 0, 1, 1, 0, //4
                      1, 0, 1, 1, 0, 1, 1, 0, //5
                      1, 0, 1, 1, 1, 1, 1, 0, //6
                      1, 1, 1, 0, 0, 1, 0, 0, //7
                      1, 1, 1, 1, 1, 1, 1, 0, //8
                      1, 1, 1, 1, 0, 1, 1, 0, //9
                      0, 0, 0, 1, 1, 1, 0, 0, //L
                      0, 1, 1, 0, 1, 1, 1,0};//H
int counter ;
long randnum = 0;
long pushtime = 0;
long ringtime = 0;

int Incbuttonstate = 0;
int lastIncbuttonstate = 0;

int guessbuttonstate = 0;
int lastguessbuttonstate = 0;

const byte numPins = 8;
//              A B C D E F G DP
const int segmentPins[8] = {2, 3, 4, 5, 6, 7, 8, 9};

void setup()
{
  Serial.begin(9600);
  pinMode(switch1, INPUT);
  pinMode(switch2, INPUT);
  for ( int i = 0; i < numPins; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }
  counter=0;
  randomSeed(analogRead(A0));
  randnum = random(1, 10);  
}
void loop() {
 
  Incbuttonstate = digitalRead(switch1);
  checkIncbuttonpress();

  if (isbuttonpress) {
    turnoff();
    showDigit(counter);
    guessnumber(counter, randnum);
  }
  Serial.print("   Rd : ");
  Serial.println(randnum);
  Serial.print("counter : ");
  Serial.print(counter);
}

int guessnumber(int counter, int randnum) {
  guessbuttonstate = digitalRead(switch2);
  if (guessbuttonstate == 1 && lastguessbuttonstate == 0) {
    delay(40);
    turnoff();
    ringtime = millis();
    while (millis() - ringtime < 4000) {
      showLEDring();
    }
    if (counter == randnum) {
      pushtime = millis();
      while (millis() - pushtime < 2000) {
        showDigit(0);
        delay(100);
        turnoff();
        delay(100);
      }
      for (int i = counter; i >= 1; i--) {
        showDigit(i);
        delay(250);
        turnoff();
      }      
      setup();
    }
    else if (counter < randnum) {
      pushtime = millis();
      while (millis() - pushtime < 2000) {
        showL();
        delay(100);
        turnoff();
        delay(100);
      }
    }
    else {
      pushtime = millis();
      while (millis() - pushtime < 2000) {
        showH();
        delay(100);
        turnoff();
        delay(100);
      }
    }
  }
  lastguessbuttonstate = guessbuttonstate;
  
  /*return randnum;
  return counter;*/
}

void showLEDring() {
  for (int i = 2; i < 8; i++) {
    digitalWrite(i, HIGH);
    delay(100);
    turnoff();
  }
}
void showL() {
  for (int i = 2; i <= 8; i++) {
    digitalWrite(i, num[10][i - 2]);
  }
}
void showH() {
  for (int i = 2; i <= 8; i++)
    digitalWrite(i, num[11][i - 2]);
}


int checkIncbuttonpress() {
  if (Incbuttonstate != lastIncbuttonstate) {
    if (Incbuttonstate == LOW) {
      isbuttonpress = true;

      counter++;
      if (counter > 9) {
        counter = 0;
      }
    }
    delay(50);
  }
  lastIncbuttonstate = Incbuttonstate;
}

void showDigit(int digit) {
  if (digit == 0) {
    for (int i = 2; i <= 8; i++)
      digitalWrite(i, num[0][i - 2]);
  }
  else if (digit == 1) {
    for (int i = 2; i <= 8; i++)
      digitalWrite(i, num[1][i - 2]);
  }
  else if (digit == 2) {
    for (int i = 2; i <= 8; i++)
      digitalWrite(i, num[2][i - 2]);
  }
  else if (digit == 3) {
    for (int i = 2; i <= 8; i++)
      digitalWrite(i, num[3][i - 2]);
  }
  else if (digit == 4) {
    for (int i = 2; i <= 8; i++)
      digitalWrite(i, num[4][i - 2]);
  }
  else if (digit == 5) {
    for (int i = 2; i <= 8; i++)
      digitalWrite(i, num[5][i - 2]);
  }
  else if (digit == 6) {
    for (int i = 2; i <= 8; i++)
      digitalWrite(i, num[6][i - 2]);
  }
  else if (digit == 7) {
    for (int i = 2; i <= 8; i++)
      digitalWrite(i, num[7][i - 2]);
  }
  else if (digit == 8) {
    for (int i = 2; i <= 8; i++)
      digitalWrite(i, num[8][i - 2]);
  }
  else if (digit == 9) {
    for (int i = 2; i <= 8; i++)
      digitalWrite(i, num[9][i - 2]);
  }
}

void turnoff() {
  for (int i = 2; i <= 9; i++)
    digitalWrite(i, LOW);
}
