/*
CPR feedback Test sketch
-Using LCD to display results in real time
 */
#include <DHT.h>                       // Includes the DHT sensor Library.
#include <LiquidCrystal_I2C.h>         // Includes the OLED Library.
#include  <Wire.h>
#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library.   
const int PulseWire = 1;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
const int LED = LED_BUILTIN;          // The on-board Arduino LED, close to PIN 13.
int Threshold = 550;           // Determine which Signal to "count as a beat" and which to ignore.
                               // Use the "Gettting Started Project" to fine-tune Threshold Value beyond default setting.
                               // Otherwise leave the default "550" value. 
                               
PulseSensorPlayground pulseSensor; 
int pressurePin = 0; //pressure reading pin
int pressureRead; //for reading pressure pin
const int buttonPin = 8;  // pin for CPR count
unsigned long time; //to display time for compressions
const int pressGood = 10;  // white led
const int pressBad =  9;   // red led

int BPM;                      //final output calculated beats per min
float bpmCount;               // to calculate BPM
float cprCount = 0;           // counter for the number of button presses, changed from int, changed from float
int buttonState = 0;          // current state of the button
int lastButtonState = 0;      // previous state of the button
int cprTotal = 0;             //total of the 5 stored values
int cprCycle = 0;             //keeps track of CPR cycle

int  timeSec, timeCPR, BPM_1; //calculating cpr time
float totalTime;
unsigned long timeBegin, timeEnd;
#define DHTPIN 7              // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11         // DHT 11  (AM2302), AM2321
DHT dht (DHTPIN,DHTTYPE);
#define start_breath 6        // green led
//ultrasonic sensor to measure the depth 
const int trigPin = 5;        
const int echoPin = 4;
long duration;
int distance;
//////////////
#define buzzer 3

LiquidCrystal_I2C lcd(0x27,  16, 2);

void setup() {
 
  pinMode(buttonPin, INPUT); //CPR button
  pinMode(pressGood, OUTPUT); //for good release, led on whight
  pinMode(pressBad, OUTPUT); // for bad release, led on Red
  Serial.begin(9600); // start serial communication / uncomment to debug
  Serial.println("Begin CPR");
  //initialize lcd screen
  lcd.init();
  // turn on the backlight
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Begin CPR");
  dht.begin();

  // Configure the PulseSensor object, by assigning our variables to it. 
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED);       //auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold);
  if (pulseSensor.begin()) {
    //Serial.println("We created a pulseSensor Object !");  //This prints one time at Arduino power-up,  or on Arduino reset.  
  }   
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(buzzer, OUTPUT);



}

void loop() {
 pressureRead = analogRead(pressurePin);
 time = millis(); //start time
 buttonState = digitalRead(buttonPin);
 if (buttonState != lastButtonState) {
       
    if (buttonState == HIGH) { //depth of at least 2in per AHA guidelines
      lcd.clear();
      cprCount++; //add 1 to the running total
      //Serial.println(cprCount);
      lcd.setCursor(0,0);
      lcd.print("Count: ");
      lcd.setCursor(7,0);
      lcd.print(cprCount);
      lcd.setCursor(10,0);
      lcd.print("R: ");     //rate BPM
      
      if(cprCount == 1){
         timeBegin = millis(); //beginning time
      }
      
      }
     
    }
    //pressureRead = analogRead(pressurePin);
    Serial.println(pressureRead);
    if (pressureRead < 500){
        digitalWrite(pressGood, HIGH);
        digitalWrite(pressBad, LOW);
    }
    else if (pressureRead > 501){
        digitalWrite(pressBad, HIGH);
        digitalWrite(pressGood, LOW);
       }



     if (cprCount == 30){ // 30:2 ratio for CPR, 30 total
            cprCount = 0; //restart count
            timeEnd = millis(); //capture end time for set
            //Serial.println(timeEnd);
            totalTime = (timeEnd - timeBegin) / 1000; //convert Ms to seconds
            // Serial.print("Total Time: ");
            // Serial.println(totalTime);
            bpmCount = ( 30.0 / totalTime); //bpmCount = float
            // Serial.print("BPM Count: ");
            // Serial.println(bpmCount);
            BPM = (bpmCount * 60.0);
            // Serial.print("Total time in sec: ");
            // Serial.println(totalTime);
            lcd.print("Rate (BPM): ");
            lcd.setCursor(12,0);
            lcd.print(BPM);
            // Serial.print("BPM is: ");
            // Serial.println(BPM);
            lcd.setCursor(0,1);
            lcd.print("Total Time: ");
            lcd.setCursor(12,1);
            lcd.print(totalTime);
            cprCycle++;
            // Serial.print("CPR Cycle #: ");
            // Serial.println(cprCycle);
            if (cprCycle == 5){
                cprCycle = 0;
            }
            
      }
     
     if (cprCount > 5 && cprCount <= 30) {
         float rateTime = (millis() - timeBegin) / 1000; //changed int rateTime to unsigned long
         //Serial.println(rateTime);
         int rate = (cprCount / rateTime) * 60; //changed int to unsigned int
         //Serial.print("BPM is: ");
         //Serial.println(rate);
         
         if (rate >= 100 && rate <= 120 ){ //AHA guidelines 100-120 CPM
             lcd.setCursor(12,0);
             lcd.print("Good");
         }
         else {
             lcd.setCursor(12,0);
             lcd.print("----");
         }
     }

     
    // Delay
    delay(25); //changed from 50
  lastButtonState = buttonState;
  if (pulseSensor.sawStartOfBeat()) {            // Constantly test to see if "a beat happened".
  int myBPM = pulseSensor.getBeatsPerMinute();  // Calls function on our pulseSensor object that returns BPM as an "int".
                                               // "myBPM" hold this BPM value now. 
  //  Serial.println("♥  A HeartBeat Happened ! "); // If test is "true", print a message "a heartbeat happened".
  //  Serial.print("BPM: ");                        // Print phrase "BPM: " 
  //  Serial.println(myBPM);                        // Print the value inside of myBPM. 
  }
 // see if the person start breath or not
 float h = dht.readHumidity();
 // Serial.print("Humidity: ");
 // Serial.println(h);
 if ( h > 50){
  digitalWrite(start_breath,HIGH);
 }
 digitalWrite(trigPin, LOW);
 delayMicroseconds(2);
 // Sets the trigPin on HIGH state for 10 micro seconds
 digitalWrite(trigPin, HIGH);
 delayMicroseconds(10);
 digitalWrite(trigPin, LOW);
 // Reads the echoPin, returns the sound wave travel time in microseconds
 duration = pulseIn(echoPin, HIGH);
 // Calculating the distance
 distance = duration * 0.034 / 2;
 // Prints the distance on the Serial Monitor
 // Serial.print("Distance: ");
 // Serial.println(distance);
 if (distance < 3){
    tone(buzzer,1000);
  }
  else{
    noTone(buzzer);
  }


}