// master

#include <Wire.h>
#define AD2 2
#define AD3 3

int n;

void setup()
{
  n=1;
  Wire.begin();          // join i2c bus (address optional for master)
  Serial.begin(9600);    // start serial for output
}

void loop()
{
  Wire.beginTransmission(AD2); // transmit to device #2
  Wire.write("HELLO ");        // sends five bytes
  Wire.write(n);              // sends one byte  
  Wire.endTransmission();    // stop transmitting
  
  Wire.requestFrom(AD2,7);    //request 6byte from AD2
  
  while(1 < Wire.available()){
    char c = Wire.read();    // receive a byte as character
    Serial.print(c);         // print the character
  }
  n = Wire.read();           // receive byte as an integer
  Serial.println(n);           // print the integer
  
  Wire.beginTransmission(AD3); // transmit to device #3
  Wire.write("HELLO ");        // sends five bytes
  Wire.write(n);              // sends one byte  
  Wire.endTransmission();    // stop transmitting
  
  Wire.requestFrom(AD3,7);    //request 6byte from AD3
  
  while(1 < Wire.available()){
    char c = Wire.read();    // receive a byte as character
    Serial.print(c);         // print the character
  }
  n = Wire.read();           // receive byte as an integer
  Serial.println(n);           // print the integer
  
  if(n >= 9){
    n = 1;
  }
  delay(1000);
}
