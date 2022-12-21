#include <Arduino_FreeRTOS.h>
#include "queue.h"

#define RED     8
#define YELLOW  7
#define GREEN   6

#define SW1     12
#define SW2     11
#define SW3     10

#define debounce 50

QueueHandle_t Blinkled;

void setup()
{
  Serial.begin(9600);
  Blinkled =  xQueueCreate(10, sizeof(int32_t));

  xTaskCreate(vSenderTask, "Sender Task", 100, SW1, 1, NULL);
  xTaskCreate(vSenderTask, "Sender Task", 100, SW2, 1, NULL);
  xTaskCreate(vSenderTask, "Sender Task", 100, SW3, 1, NULL);

  xTaskCreate(vReceiverTask, "Receiver Task ", 100, RED, 1, NULL);
  xTaskCreate(vReceiverTask, "Receiver Task", 100, YELLOW, 1, NULL);
  xTaskCreate(vReceiverTask, "Receiver Task", 100, GREEN, 1, NULL);
}



void vSenderTask(int *pvParameters) {
  long previousPress = 0;
  BaseType_t qStatus;
  int32_t valueToSend = 0;
  int SW = pvParameters;
  const TickType_t xTicksToWait = pdMS_TO_TICKS(100);

  pinMode(SW, INPUT);

  while (1) {
    if (digitalRead(SW) == HIGH && millis - previousPress >= debounce) {
      previousPress = millis();
      valueToSend = SW;//passes switch pin to valueToSend
      Serial.print("send Value  : ");
      Serial.println(valueToSend);
    }
    else
      valueToSend = 0;
    if (valueToSend != 0) {
      qStatus = xQueueSend(Blinkled, &valueToSend, xTicksToWait); //added SW to queue if cant,will wait 100ms
      vTaskDelay(50);
    }
  }
}
void vReceiverTask(void *pvParameters) {
  BaseType_t qStatus;
  int32_t valueReceived;
  int LED = (int32_t)pvParameters;
  const TickType_t xTicksToWait = pdMS_TO_TICKS(100);
  int Cred = 0;
  long RTime = 0;

  bool R_state = false;
  bool Y_state = false;
  bool G_state = false;

  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);

  while (1) {
    qStatus = xQueueReceive(Blinkled, &valueReceived, xTicksToWait);
    Serial.print("Received value  : ");
    Serial.println(valueReceived);
    Serial.print("Receive LED : ");
    if (LED == 8) {
      Serial.println("RED");
    }
    else if (LED == 7) {
      Serial.println("YELLOW");
    }
    else {
      Serial.println("GREEN");
    }
    
    if (qStatus == pdPASS) { //received data
      if (valueReceived == 12 && LED == RED) { //if press sw1
        R_state = true;
        RTime = millis();
        Serial.println("RED is set to turnON");
        Cred++;
        digitalWrite(RED, HIGH);
      }
      else if (valueReceived == 11 && LED == YELLOW) {
        Serial.println("bt2 PRESSED");
        Y_state = !(Y_state);
        Serial.print("Y status : ");
        Serial.println(Y_state);
      }
      else if (valueReceived == 10 && LED == GREEN) {
        G_state = true;
        Serial.println("bt3 PRESSED");
        /*Serial.print("Green status : ");
          if (G_state == true)
          Serial.println("on");
          else
          Serial.println("off");*/
        for (int i = 1; i <= 3; i++) {
          digitalWrite(GREEN, HIGH);
          vTaskDelay(50);
          digitalWrite(GREEN, LOW);
          vTaskDelay(50);
        }
      }
    }
    if (R_state != true && LED == YELLOW && G_state != true) {
      if (Y_state == true) {
        digitalWrite(YELLOW, HIGH);
        vTaskDelay(25);
        digitalWrite(YELLOW, LOW);
        vTaskDelay(25);
      }
    }
    if (Cred > 0) {
      Serial.print("press          ");
      Serial.print(Cred);
      Serial.println("           times");
      while (millis - RTime >= Cred * 3000) {
        digitalWrite(RED, LOW);
        Cred = 0;
      }
    }

  }
}
void loop() {
  // put your main code here, to run repeatedly:

}
