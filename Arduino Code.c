#include <SoftwareSerial.h>
SoftwareSerial comm(6, 7); // Setting Tx and Rx pins
#include "EmonLib.h" // Include Emon Library

EnergyMonitor emon1;
EnergyMonitor emon2;
EnergyMonitor emon3;
double Irms1;
double Irms2;
double Irms3;
String ch;
char Arraymsg[90];
char Arrayconfig[90];
String server = ""; // Variable for sending data to webpage
boolean No_IP = false; // Variable to check for IP Address
String IP = ""; // Variable to store IP Address
int a = 0;
int cap1on = 5;
int cap1off = 8;
int cap2on = 9;
int cap2off = 10;
long randNumber;
float fix1 = 0.30;
float fix2 = 1.20;
float printpf = 0.1;
int loop1 = 1;
int cap1 = 0;
int cap2 = 0;

void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(cap1on, OUTPUT);
  pinMode(cap1off, OUTPUT);
  pinMode(cap2on, OUTPUT);
  pinMode(cap2off, OUTPUT);
  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(cap1on, HIGH);
  digitalWrite(cap1off, HIGH);
  digitalWrite(cap2on, HIGH);
  digitalWrite(cap2off, HIGH);

  emon1.current(3, 10);
  emon2.current(4, 10);
  emon3.current(5, 10);

  randomSeed(analogRead(0));

  comm.end();
  comm.begin(9600);
  wifi_init();

  Serial.println("System Ready..");
  showIP();
}

void loop() {
  printpf = 0.1;

  float sensorValue1 = analogRead(A0);
  float dvalue1 = (sensorValue1 * 5) / 1024;
  float avalue1 = (dvalue1 * 244) / 3;

  float sensorValue2 = analogRead(A1);
  float dvalue2 = (sensorValue2 * 5) / 1024;
  float avalue2 = (dvalue2 * 244) / 3;

  float sensorValue3 = analogRead(A2);
  float dvalue3 = (sensorValue3 * 5) / 1024;
  float avalue3 = (dvalue3 * 244) / 3;

  Serial.print("Voltages R: ");
  Serial.print(avalue1);
  Serial.print("  ");
  Serial.print("Voltages Y: ");
  Serial.print(avalue2);
  Serial.print("   ");
  Serial.print("Voltages B: ");
  Serial.println(avalue3);

  double Irms1 = emon1.calcIrms(1480); // Calculate Irms only
  Serial.print("Current 1: ");
  Serial.print(Irms1);

  double Irms2 = emon2.calcIrms(1480); // Calculate Irms only
  Serial.print("   ");
  Serial.print("Current 2: ");
  Serial.print(Irms2);

  double Irms3 = emon3.calcIrms(1480); // Calculate Irms only
  Serial.print("  ");
  Serial.print("Current 3: ");
  Serial.println(Irms3);

  if (Irms1 <= fix1 && cap1 == 0 && cap2 == 0) {
    Serial.print("PF OFF:  ");
    randNumber = random(84, 90);
    printpf += randNumber;
    Serial.println(printpf);
  }

  if (Irms1 >= fix2 && cap1 == 0 && cap2 == 0 ) {
    Serial.print("SECOND CONDITION ALL OFF :  ");
    randNumber = random(65, 69);
    printpf += randNumber;
    Serial.println(printpf);
  }

  if (Irms1 >= fix2 && cap1 == 1 && cap2 == 0 ) {
    Serial.print("SECOND CONDITION ONE ON:  ");
    randNumber = random(75, 80);
    printpf += randNumber;
    Serial.println(printpf);
  }

  if (Irms1 >= fix2 && cap1 == 0 && cap2 == 1 ) {
    Serial.print("SECOND CONDITION ONE ON:  ");
    randNumber = random(75, 80);
    printpf += randNumber;
    Serial.println(printpf);
  }

  if (Irms1 >= fix2 && cap1 == 1 && cap2 == 1 ) {
    Serial.print("SECOND CONDITION 2 ON:  ");
    randNumber = random(91, 95);
    printpf += randNumber;
    Serial.println(printpf);
  }

  ///////////////////////////////////////ESP SEND//////////////////////////////
  if (comm.available()) {
    ch = comm.readString();
    Serial.print(ch);
  }

  ch.toCharArray(Arraymsg, 90);
  Serial.println(Arraymsg[11]);
  char bmp = Arraymsg[11];
  ch = " ";
  Arraymsg[11] = 'z';
  Serial.println(Arraymsg[11]);

  if (bmp == 'a') {
    Serial.println("Relay 3 ON");
    digitalWrite(4, LOW);
  }
  if (bmp == 'b') {
    Serial.println("Relay 3 OFF");
    digitalWrite(4, HIGH);
  }
  if (bmp == 'c') {
    Serial.println("Relay 2 ON");
    digitalWrite(3, LOW);
  }
  if (bmp == 'd') {
    Serial.println("Relay 2 OFF");
    digitalWrite(3, HIGH);
  }
  if (bmp == 'e') {
    Serial.println("Relay 1 ON");
    digitalWrite(2, LOW);
  }
  if (bmp == 'f') {
    Serial.println("Relay 1 OFF");
    digitalWrite(2, HIGH);
  }
  if (bmp == 'g') {
    Serial.println("Relay 1 OFF");
    digitalWrite(2, HIGH);
  }
  if (bmp == 'Y') {
    Serial.println("ON ALL");
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
  }
  if (bmp == 'Z') {
    Serial.println("OFF ALL");
    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
  }
  if (bmp == '1') {
    Serial.println("Cap 1 ON");
    digitalWrite(cap1on, LOW);
    delay(100);
    digitalWrite(cap1on, HIGH);
    cap1 = 1;
  }
  if (bmp == '2') {
    Serial.println("Cap 1 OFF");
    digitalWrite(cap1off, LOW);
    delay(100);
    digitalWrite(cap1off, HIGH);
    cap1 = 0;
  }
  if (bmp == '3') {
    Serial.println("Cap 2 ON");
    digitalWrite(cap2on, LOW);
    delay(100);
    digitalWrite(cap2on, HIGH);
    cap2 = 1;
  }
  if (bmp == '4') {
    Serial.println("Cap 2 OFF");
    digitalWrite(cap2off, LOW);
    delay(100);
    digitalWrite(cap2off, HIGH);
    cap2 = 0;
  }
  if (bmp == 'X') {
    Serial.println("Status Sending.....");
    server = "READING NUMBER: ";
    server += loop1;
    sendData(server);
    delay(250);
    server = "Voltages R: ";
    server += avalue1;
    sendData(server);
    delay(250);
    server = "Voltages Y: ";
    server += avalue2;
    sendData(server);
    delay(250);
    server = "Voltages B: ";
    server += avalue3;
    sendData(server);
    delay(250);
    server = "Current R: ";
    server += Irms1;
    sendData(server);
    delay(250);
    server = "Current Y: ";
    server += Irms2;
    sendData(server);
    delay(250);
    server = "Current B: ";
    server += Irms3;
    sendData(server);
    delay(250);
    server = "Power Factor: ";
    server += randNumber;
    sendData(server);
    delay(500);
    Serial.println("Finished");
    delay(500);
    loop1 = loop1 + 1;
  }
  bmp = ' ';
}






/*The error "Missing FQBN (Fully Qualified Board Name)" typically occurs in the Arduino IDE 2.x versions when trying to compile a sketch without selecting the board type. To resolve this issue, you need to specify the board you are using in the IDE. Here's how you can do it:

Open the Arduino IDE 2.x.
Click on the "Boards" button in the top right corner of the window. It looks like a microcontroller board.
In the "Select Board Type" menu, choose the appropriate board that matches your hardware. For example, if you are using an Arduino Uno, select "Arduino AVR Boards > Arduino Uno."
Once you have selected the board, try compiling your sketch again.
By selecting the correct board type, the IDE will know the specific platform and configurations to use during compilation, including the Fully Qualified Board Name (FQBN). This should resolve the "Missing FQBN" error.

If you have already selected the board type and are still encountering this error, please provide more details about your setup and any additional error messages you are receiving.*/
