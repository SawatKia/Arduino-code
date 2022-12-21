//Arduino3
#include <Wire.h>
#define AD3 3
int n;

void setup()
{
  Wire.begin(AD3);                // join i2c bus with address 
  Wire.onReceive(receiveEvent);    // Function to run when data received from master
  Wire.onRequest(requestEvent);    // Function to run when master request
  Serial.begin(9600);              // start serial for output
}

void loop()
{
  delay(1000);
}

void receiveEvent(int num)
{
  while(1<Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  n = Wire.read();    // receive byte as an integer
  Serial.println(n);         // print the integer
  
}

void requestEvent(){
    n += 1;
  Wire.write("HELLO ");
    Wire.write(n);
  
}
