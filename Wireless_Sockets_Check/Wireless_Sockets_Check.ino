/*
OpenAutomation.center
Wireless sockets check routine for DFRobot Xboard V.2

*/

//Controlled outputs                               
int L4OnnA0 = A0;                                     
int L4OffA1 = A1;                                     
int L3OnnA2 = A2;                                     
int L3OffA3 = A3;                                     
int L2OnnD6 = 6;                                     
int L2OffD7 = 7;                                     
int L1OnnD9 = 9;
int L1OffD8 = 8;



// Starting Setup
void setup()
{
  //Initialising Arduino outputs 
   
    pinMode(L1OnnD9, OUTPUT); 
    pinMode(L1OffD8, OUTPUT);
    pinMode(L2OnnD6, OUTPUT);
    pinMode(L2OffD7, OUTPUT);
    pinMode(L3OnnA2, OUTPUT);
    pinMode(L3OffA3, OUTPUT);
    pinMode(L4OnnA0, OUTPUT);
    pinMode(L4OffA1, OUTPUT);    
  

  //Starting serial port
  Serial.begin(9600);
  delay(1000);
  //Serial.println("Open Automation Coordinator Wireless sockets check");
  //Serial.println("==================================================");

}// Setup end

//Starting Main program 
  void loop(){ 
           
      Serial.println("");  
      AllOff();

            
       //L1 ==================================================================
            
            Serial.println("");
            Serial.println("L1 ON Check...");
                delay(1000);
                digitalWrite(L1OnnD9, HIGH);
                Serial.println("  L1OnnD9 - 1");
                delay(3000);
                AllOff();

          
            Serial.println("");
            Serial.println("L1 OFF Check...");
                delay(1000);           
                digitalWrite(L1OffD8, HIGH);                   
                Serial.println("  L1OffD8 - 1");
                delay(3000);
                AllOff();
                
                
       //L2 ==================================================================
            
            Serial.println("");
            Serial.println("L2 ON Check...");
                delay(1000);
                digitalWrite(L2OnnD6, HIGH);
                Serial.println("  L2OnnD6 - 1");
                delay(3000);
                AllOff();

          
            Serial.println("");
            Serial.println("L1 OFF Check...");
                delay(1000);           
                digitalWrite(L2OffD7, HIGH);                   
                Serial.println("  L2OffD7 - 1");
                delay(3000);
                AllOff();                
            
       //L3 ==================================================================                   
            
            Serial.println("");
            Serial.println("L3 ON Check...");
                delay(1000);           
                digitalWrite(L3OnnA2, HIGH);                   
                Serial.println("  L3OnnA2 - 1");
                delay(3000);
                AllOff();
 
          
            Serial.println("");
            Serial.println("L3 OFF Check...");
                delay(1000);           
                digitalWrite(L3OffA3, HIGH);                   
                Serial.println("  L3OffA3 - 1");
                delay(3000);
                AllOff();

          
       //L4 ==================================================================                   
            
            Serial.println("");
            Serial.println("L4 ON Check...");
                delay(1000);           
                digitalWrite(L4OnnA0, HIGH);                   
                Serial.println("  L4OnnA0 - 1");
                delay(3000);
                AllOff();
  
          
            Serial.println("");
            Serial.println("L4 OFF Check...");
                delay(1000);           
                digitalWrite(L4OffA1, HIGH);                   
                Serial.println("  L4OffA1 - 1");
                delay(3000);
                AllOff();




       //FINISH ==================================================================
            Serial.println("");
            Serial.println("All Tests Done");
            delay(1000);
            
            

}//Main program end

void AllOff(){

      digitalWrite(L1OnnD9, LOW);
      digitalWrite(L1OffD8, LOW);
      digitalWrite(L2OnnD6, LOW);
      digitalWrite(L2OffD7, LOW);
      digitalWrite(L3OnnA2, LOW);
      digitalWrite(L3OffA3, LOW);
      digitalWrite(L4OnnA0, LOW);
      digitalWrite(L4OffA1, LOW);
      Serial.println("  All OFF");
      delay(1000);

}



