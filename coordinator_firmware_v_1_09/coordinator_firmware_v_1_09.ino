/*
 Wireless Coordinator code 
 Designed by V.Gudaitis oxyo@smartsensors.lt 2013 09 19
 
 VERSION: 1.09
 * Reset function added
 * Command length check added
 
 APPLICATION: FanCoils 
 SENSORS COUNT: 4
 T0; T1; T2; T3; 

 */
 
#include <avr/wdt.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// API key 
const char key[36+1] = "000000000000000000000000000000000000"; //Replace with your Key

// Coordinator ID: 
const char cid[18+1] = "000000000000000000";                     //Replace with your Controller ID
const char cGET[38+1] = "GET /?cid=000000000000000000 HTTP/1.0"; //put the same value after cid=

// Controller Header 
const char cName[12+1] = "SMART 1.09"; //Replace with your Controller ID
 
const int numberOfSensors = 6; //Number of All sensors;
const int numberOfRooms = 6; //Number of Controled rooms;
const int commandLength = 29; //Command length set

int resetInterval = 5;
int resetCounter = 0;
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 

// Reandom response delay value
const int rDelay = 300;

int sensorCount = 0;  // Wireless sensors counter initialisation
#define DATABUFFERSIZE 80 
char tempData[DATABUFFERSIZE+1]; //char array for Temperature Post data
byte tempDataIndex = 0; 
boolean result = false; 
boolean debug = false;  // Program debugging switch

String incomingString = "";   // Incoming String from Serial interface built up one byte at a time.
String tempValue;   // Variable to store Radio Sensor Temperature Value
String tempSensor;  // Variable to store Temperature Sensor name

//Temporary variables for fancoil temperature level set 
char tmpStr[5+1];
char tmp1Str[2+1];
char tmp2Str[2+1];
int tmp1Value;
int tmp2Value;

char humStr[5+1];
char hum1Str[2+1];
char hum2Str[2+1];
int hum1Value;
int hum2Value;

//Temporary variables for fancoil non working hours window set 
int nonWorkStartHour;
int nonWorkEndHour;


//Ethernet shield initialisation
byte mac[] = {0x90, 0xB2, 0xDA, 0xD3, 0xA2, 0x97 }; // physical mac address

IPAddress ip(192, 168, 0, 101);          // ip address
IPAddress gateway(192, 168, 0, 254);     // internet access via router
IPAddress dnsSrv(8, 8, 8, 8);            // DNS server IP address
IPAddress subnet( 255, 255, 255, 0 );    // subnet mask
IPAddress server(5, 35, 247, 113);       // WEB server IP address
int webPort = 8888;                      // WEB server Port


// Initialize the Ethernet client library
EthernetClient client;

//Controlled outputs PINs                                  
int L4OnnA0 = A0;                                     
int L4OffA1 = A1;                                     
int L3OnnA2 = A2;                                     
int L3OffA3 = A3;                                     
int L2OnnD6 = 6;                                     
int L2OffD7 = 7;                                     
int L1OnnD9 = 9;
int L1OffD8 = 8;
int failbackLED = 13;

void setup() {

  //Initialising Arduino outputs
  pinMode(L1OnnD9, OUTPUT); 
  pinMode(L1OffD8, OUTPUT);
  pinMode(L2OnnD6, OUTPUT);
  pinMode(L2OffD7, OUTPUT);
  pinMode(L3OnnA2, OUTPUT);
  pinMode(L3OffA3, OUTPUT);
  pinMode(L4OnnA0, OUTPUT);
  pinMode(L4OffA1, OUTPUT); 
  pinMode(failbackLED,OUTPUT); 
  digitalWrite(failbackLED, LOW);  
  

  //Starting serial port
  Serial.begin(9600);
  delay(500);
  
  //Starting Ethernet
  Ethernet.begin(mac, ip, dnsSrv, gateway, subnet);

  //Printing Controller Header
  Serial.println("* "); 
  Serial.print(cName);
  Serial.print(" ");

  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    if (thisByte < 3) Serial.print(".");
  }
  Serial.println();  

  wdt_enable(WDTO_8S);  //Set up the watchdog timer. If it isn't reset every 8 seconds, the arduino will reset 
  wdt_reset();  //Good practice to reset the watchdog in setup.
}

void(* resetFunc) (void) = 0; //declare reset function @ address 0

//Starting Main program ============================================================
void loop() {
  
  wdt_reset(); //Reset watchdog timer
  
  String data; //Dynamic string array to store HTTP POST data 
     
  // Check if there's incoming serial data.
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();
    incomingString += incomingByte;

    // Checks for termination of the string.
    if (incomingByte == 'a') {

      if (incomingString.substring(2,6) == "TMPA" || incomingString.substring(2,6) == "CRHP") {

        tempSensor = incomingString.substring(0,2); // String T1 || H1 || D1
        tempValue = incomingString.substring(6,11); // String 00.00

        tempData[tempDataIndex++] = tempSensor[0];
        tempData[tempDataIndex++] = tempSensor[1];
        tempData[tempDataIndex++] = '=';
        tempData[tempDataIndex++] = tempValue[0];
        tempData[tempDataIndex++] = tempValue[1];
        tempData[tempDataIndex++] = tempValue[2];
        tempData[tempDataIndex++] = tempValue[3];
        tempData[tempDataIndex++] = tempValue[4];

        if(sensorCount + 1 == numberOfSensors) { // if last sensor data arrived it's time for post
          result = true;
          sensorCount = sensorCount + 1; 
        } 
        else {
          tempData[tempDataIndex++] = '&';
          sensorCount = sensorCount + 1;
        }

        //Serial.print(" * ");
        //Serial.println(sensorCount);
                
        //if (debug) { 
        //Serial.print("-- GET:");
        //Serial.write(tempData);
        //Serial.println("");
        //}

        if (result) {

          //Serial.write(tempData);
          //Serial.println("");

          //Posting data to server ===========================================================================

          if (client.connect(server, webPort)) {
            int len = (9 * numberOfSensors) - 1 + 18 + 5; //
            data = "";        
            data += tempData;
            data += "&cid=";
            data += cid;
            data = data.substring(0,len); //removing last char from string
            data += "&key=";
            len = (9 * numberOfSensors) - 1 + 18 + 5 + 5 + 36; 
            data += key;
            data = data.substring(0,len);  //remowing last char from string

            client.println("POST /data/ HTTP/1.1");
            client.println("Host: smartsensors.io");
            client.println("Content-Type: application/x-www-form-urlencoded; charset=utf-8");
            client.println("Accept: application/xhtml+xml");
            client.println("Connection: close");
            client.print("Content-Length: ");
            client.println(data.length());
            client.println();
            client.println(data);           

            //After data POST we should stop Ethernet Client to close connection. 
            client.stop();
            Serial.print("* SENT ");
            Serial.print(resetCounter + 1);
          } 
          else {
            Serial.println("* POST FAILED!");
            Serial.print(cName);             
          }

          //End Post operation ===============================================================================
          //Clearing temp data
          tempDataIndex = 0;
          sensorCount = 0;
          result = false;  

          //Getting command from server ======================================================================
          if (client.connect(server, webPort)) {

            String readCommand = "";  
            //Sending "GET HTTP/1.0" request to HTTP server
            client.println(cGET);
            client.println();

            if (client) {
              while (client.connected()) {
                if (client.available()) {
                  char c = client.read(); //Reading char by char WEB server answer. Storing characters to string: 
                  readCommand += c; 
                  //Serial.write(c);             

                  //if HTTP request has ended (# is last character in command line)
                  if (c == '#') {                    
                    String commandFX = "";                    
                    for(int i=0; i < numberOfRooms; i++){
                         commandFX = getValue(readCommand, '&', i+1);

                         Serial.println();
                         Serial.print('a');
                         Serial.print(commandFX);
                         Serial.print('#');
                         
                         if (commandFX.length() < commandLength) {
                           Serial.println();
                           Serial.print(commandFX.length());
                           Serial.println();
                           delay(1000);
                           resetFunc();
                         }
                         
                         if (i == numberOfRooms - 1) {
                            resetCounter += 1;                     
                            if (resetCounter > resetInterval) {                           
                               delay(1000);
                               resetFunc();
                            }
                         }
                                               
                    }                  
                    Serial.println();
                  }
                }
              }
            }	 

            // Clearing readstring and stopping client to close connection
            readCommand = "";
            data = "";
            client.stop(); 

          } // if client.connect for GET command                
        } //if result   
      }

      incomingString = "";
    }
  }
}

//FUNCTIONS ==========================================================

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

