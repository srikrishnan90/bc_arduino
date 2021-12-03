
#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"
#include <Wire.h>

#define TX_PIN 6                   // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 5                   // Arduino receive   GREEN WIRE   labeled TX on printer

SoftwareSerial mySerial(RX_PIN, TX_PIN);            // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&mySerial);                // Pass addr to printer constructor

int Incu_ThermistorPin = A1;
int Read_ThermistorPin = A0;
int Vo;
float R1 = 10000;
float logR2, R2, IT, RT, I_Tc, R_Tc, I_Tf, R_Tf;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

String de;
String te;

// --------------------------------------------------------------------------------
void setup()
{

  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  mySerial.begin(9600);                // Initialize SoftwareSerial
  Serial.begin(9600);
  printer.begin();                     // Initialize printer (same regardless of serial type)
  Wire.begin(0x07); //Set Arduino up as an I2C slave at address 0x07
  Wire.onRequest(requestEvent); //Prepare to send data
  Wire.onReceive(receiveEvent); //Prepare to recieve data

}

void loop()
{
  if (te.substring(0, 4) == "done")
  {
    printfunc();
    te = "none";
  }
  else
  {
    Vo = analogRead(Incu_ThermistorPin);
    R2 = R1 * (1023.0 / (float)Vo - 1.0);
    logR2 = log(R2);
    IT = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
    I_Tc = IT - 273.15;
    I_Tf = (I_Tc * 9.0) / 5.0 + 32.0;

    Serial.print("Incu Temperature: ");
    Serial.print(I_Tf);
    Serial.print(" F; ");
    Serial.print(I_Tc);
    Serial.print(" C    ");
    if (I_Tc < 37)
    {
      analogWrite(6, 250);
    }
    else
    {
      analogWrite(6, 0);
    }

    delay(100);
    Vo = analogRead(Read_ThermistorPin);
    R2 = R1 * (1023.0 / (float)Vo - 1.0);
    logR2 = log(R2);
    RT = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
    R_Tc = RT - 273.15;
    R_Tf = (R_Tc * 9.0) / 5.0 + 32.0;

    Serial.print("Read Temperature: ");
    Serial.print(R_Tf);
    Serial.print(" F; ");
    Serial.print(R_Tc);
    Serial.println(" C");
    if (R_Tc < 37)
    {
      analogWrite(5, 250);
    }
    else
    {
      analogWrite(5, 0);
    }
    delay(100);
  }
}

void printfunc()
{
  printer.boldOn();
  printer.justify('C');
  mySerial.println(de);
  /*printer.boldOn();
    printer.justify('C');
    mySerial.println("BIO CHEMISTRY REPORT");
    printer.boldOff();
    mySerial.println("-----------------------------");
    printer.justify('L');
    mySerial.println("Running Number:             ");
    mySerial.println("Report Time:                ");
    mySerial.println("-----------------------------");

    //Patient details
    printer.justify('L');
    mySerial.println("Patient Name:"              );
    mySerial.println("Patient ID:"                );
    mySerial.println("Test Name:                 ");
    mySerial.println("Main WaveLength:           ");
    mySerial.println("Temperature:               ");
    mySerial.println("Reference Range:           ");
    mySerial.println("Test Result:               ");
    mySerial.println("-----------------------------");
    mySerial.println("Disclaimer: This result only\nresponsible for this specimen!");
    mySerial.println("Print Time:          \n\n\n");
  */
}

void requestEvent()
{
  String s1 = String(R_Tc, 1);
  String s2 = String(I_Tc, 1);  
  String all = s1 + " " + s2;
  char buf[30];
  all.toCharArray(buf, 30);
  Wire.write(buf, 30);
}


void receiveEvent(int numBytes)
{
  char rc[30] = "";
  int count = 0;
  while (Wire.available())
  {
    char c = Wire.read();
    rc[count] = c;
    count++;
    delay(10);
  }
  de = rc;
  te = "done";
  Serial.println(de);
}
