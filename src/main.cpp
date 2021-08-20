/**************************************************************************************************
 * Desinfeccion
 *
 * Created: 13/08/2021
 * Author: Lars Lindner
 *
 * Control de un Arduino Pro Mini para una Estacion de Desinfeccion
 * (Nueva Implementacion usando switch - case)
 * 
**************************************************************************************************/

#include <Arduino.h>
#include <HCSR04.h>

#define echopin 2
#define trigpin 3
#define relaypin 4
#define ledPin 13

enum machineStates
{
  DETECT,
  RUN,
  STOP
};
machineStates states = DETECT;
HCSR04 hc(trigpin, echopin);

int count1 = 0;
int count2 = 0;
float val_min = 10.0;
float val_max = 50.0;
float act_val = 0.0;

void setup()
{
  // pinMode(ledPin, OUTPUT);
  pinMode(relaypin, OUTPUT);
  Serial.println("Enter DETECT");
  Serial.begin(9600);
}

void state_machine()
{
  switch (states)
  {
  case DETECT:
    act_val = hc.dist();
    if (act_val < val_min)
    {
      delay(100);
      count1++;
      if (count1 >= 5)
      {
        count1 = 0;
        Serial.println("Enter RUN");
        states = RUN;
      }
    }
    else
    {
      count1 = 0;
    }
    break;

  case RUN:
    Serial.print("[cm]: ");
    Serial.println(act_val);
    digitalWrite(relaypin, HIGH);
    delay(5000);
    digitalWrite(relaypin, LOW);
    Serial.println("Enter STOP");
    states = STOP;
    break;

  case STOP:
    act_val = hc.dist();
    if (act_val > val_max)
    {
      delay(100);
      count2++;
      if (count2 >= 10)
      {
        count2 = 0;
        digitalWrite(relaypin, LOW); // Para asegurar que realmente se apaga el relay
        // delay(1000);                 // Para evitar que por un movimiento leve del pie, no entre rapido al estado DETECT!
        Serial.println("Enter DETECT");
        states = DETECT;
      }
    }
    else
    {
      count2 = 0;
    }

    // if (hc.dist() > val_max)
    // {
    //   digitalWrite(relaypin, LOW); // Para asegurar que realmente se apaga el relay
    //   delay(1000);                 // Para evitar que por un movimiento leve del pie, no entre rapido al estado DETECT!
    //   Serial.println("Enter DETECT");
    //   states = DETECT;
    // }

    break;
  }
}

void loop()
{
  state_machine();
  delay(10);
}
