/*

    Name: ROBI LED valgustuse lisapakett
    Version: v1.1
    Author: Aleksandr
    Company: Oomipood
    Description: Control of 2WD-1 robot platform using Arduino UNO.
    with obstacle avoidance using HC-SR04 ultra sonic sensor.

*/


//SOFTWARE SERIAL FOR BLUETOOTH VARIABLES
#include <SoftwareSerial.h>
SoftwareSerial bluetoothSerial(12, 4); // RX, TX
char bluetoothCommand;

//DISTANCE VARIABLES
const int trigPin = 3;
const int echoPin = 2;
int dist_check1, dist_check2, dist_check3;
long duration, distance, distance_all;
int dist_result;

//MOTORS VARIABLES
const int motor1_forward = 6;
const int motor1_back = 5;
const int motor2_forward = 11;
const int motor2_back = 10;
int ROBI_SPEED = 100; //motors speed
int k = 0; //BRAKE

//LOGICS VARIABLES
const int dist_stop = 25;
//const int dist_slow = 40;
const int max_range = 800;
const int min_range = 0;
int errorLED = 13;
byte ROBI_MODE = 1;

const int LED1 = 7;
const int LED2 = 8;
byte LEDS_MODE = 0; // leds on/off
byte active_led = 1;
long LED_on_count=0;
byte MOVING = 0;

//INITIALIZATION
void setup() {
  Serial.begin(9600);
  bluetoothSerial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(errorLED, OUTPUT);
  pinMode(motor1_forward, OUTPUT);
  pinMode(motor1_back, OUTPUT);
  pinMode(motor2_forward, OUTPUT);
  pinMode(motor1_back, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  //LEDS TESE AT STURTUP
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  delay(500);
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);


}


void loop() { //MAIN PROGRAM CYCLE
  
  //*******************WAITING FOR BLUETOOTH COMMANDS****************
  if (ROBI_MODE == 1){

        if (bluetoothSerial.available() > 0) {

          bluetoothCommand = bluetoothSerial.read();

          if (bluetoothCommand == 'X') {ROBI_MODE = 2;}              //AUTOPILOT ON       
          if (bluetoothCommand == 'F') {ROBI_FORWARD();}             //FORWARD
          if (bluetoothCommand == 'B') {ROBI_BACK();}                //BACK
          if (bluetoothCommand == 'S') {ROBI_STOP();}                //STOP
          if (bluetoothCommand == 'L') {ROBI_LEFT();}                //LEFT
          if (bluetoothCommand == 'R') {ROBI_RIGHT();}               //RIGHT
          if (bluetoothCommand == 'G') {ROBI_FORW_LEFT();}           //FORWARD LEFT
          if (bluetoothCommand == 'I') {ROBI_FORW_RIGHT();}          //FORWARD RIGHT
          if (bluetoothCommand == 'H') {ROBI_BACK_LEFT();}           //BACK LEFT
          if (bluetoothCommand == 'J') {ROBI_BACK_RIGHT();}          //BACK RIGHT
          if (bluetoothCommand == '0') {ROBI_SPEED = 0;}             //SET MOTORS SPEED TO 0
          if (bluetoothCommand == '1') {ROBI_SPEED = 100;}           //SET MOTORS SPEED TO 1
          if (bluetoothCommand == '2') {ROBI_SPEED = 125;}           //SET MOTORS SPEED TO 2
          if (bluetoothCommand == '3') {ROBI_SPEED = 150;}           //SET MOTORS SPEED TO 3
          if (bluetoothCommand == '4') {ROBI_SPEED = 165;}           //SET MOTORS SPEED TO 4
          if (bluetoothCommand == '5') {ROBI_SPEED = 180;}           //SET MOTORS SPEED TO 5
          if (bluetoothCommand == '6') {ROBI_SPEED = 195;}           //SET MOTORS SPEED TO 6
          if (bluetoothCommand == '7') {ROBI_SPEED = 210;}           //SET MOTORS SPEED TO 7
          if (bluetoothCommand == '8') {ROBI_SPEED = 225;}           //SET MOTORS SPEED TO 8
          if (bluetoothCommand == '9') {ROBI_SPEED = 240;}           //SET MOTORS SPEED TO 9
          if (bluetoothCommand == 'q') {ROBI_SPEED = 255;}           //SET MOTORS SPEED TO MAX
          if (bluetoothCommand == 'W') {LEDS_MODE = 1;}              //LEDS ON
          if (bluetoothCommand == 'w') {LEDS_MODE = 0;}              //LEDS OFF
            
        }

        //LEDS
        if(1 == LEDS_MODE){

          if (active_led == 1){


            digitalWrite(LED1, LOW);

              if(LED_on_count > 10000){

                  digitalWrite(LED1, HIGH);
                  LED_on_count = 0;
                  active_led = 2;

              }             
          }   

          if (active_led == 2){

              digitalWrite(LED2, LOW);

                if(LED_on_count > 10000){

                    digitalWrite(LED2, HIGH);
                    LED_on_count = 0;
                    active_led = 1;

                }                
          }
        }  

        if(0 == LEDS_MODE){

          digitalWrite(LED1, HIGH);
          digitalWrite(LED2, HIGH);

        } 

        LED_on_count++;      

  }

  if (ROBI_MODE == 2){ //AUTOPILOT MODE ON

          digitalWrite(LED1, HIGH); //LED1 OFF
          digitalWrite(LED2, HIGH); //LED2 OFF

        int result = ping();    //Check distance

        if (result <= min_range){                  //Check min range
          digitalWrite(errorLED, 1);
          delay(500); 
        }

        if (result == max_range || result > max_range){  //Check max range
          digitalWrite(errorLED, 1);
          delay(500); 
        }

        if (result == dist_stop || result < dist_stop){  //Check stop range
          digitalWrite(errorLED, 0);
          ROBI_BACK();
          delay(1000);
          ROBI_STOP();
          delay(200);
          ROBI_LEFT();
          delay(300);
          ROBI_STOP();
          delay(200);  
        } 

        if (result > dist_stop){          //If all is OK, go forward
          ROBI_FORWARD();
          delay(100);
        } 

        bluetoothCommand = bluetoothSerial.read();  //Check autopilot OFF/ON
        if (bluetoothCommand == 'x'){
          ROBI_STOP();
          ROBI_MODE = 1; //AUTOPILOT IS OFF
        }
  }



}   

//***********************FUNCTIONS*******************************

int ping(){                      //CHECK DISTANCE FUNCTION (3x)
  digitalWrite(trigPin, 0);
  delayMicroseconds(2);
  digitalWrite(trigPin, 1);
  delayMicroseconds(10);
  digitalWrite(trigPin, 0);
  duration = pulseIn(echoPin, 1);
  distance = duration/58;
  
  dist_check1 = distance;
  
  digitalWrite(trigPin, 0);
  delayMicroseconds(2);
  digitalWrite(trigPin, 1);
  delayMicroseconds(10);
  digitalWrite(trigPin, 0);
  duration = pulseIn(echoPin, 1);
  distance = duration/58;
  
  dist_check2 = distance;
  
  digitalWrite(trigPin, 0);
  delayMicroseconds(2);
  digitalWrite(trigPin, 1);
  delayMicroseconds(10);
  digitalWrite(trigPin, 0);
  duration = pulseIn(echoPin, 1);
  distance = duration/58;
  
  dist_check3 = distance;
  
  int dist_check_sum;
  dist_check_sum = dist_check1 + dist_check2 + dist_check3;
  dist_result = dist_check_sum/3;
  return dist_result;  
}

void ROBI_FORWARD(){                    //MOTORS FORWARD FUNCTION
  analogWrite(motor1_forward, ROBI_SPEED);
  analogWrite(motor2_forward, ROBI_SPEED);
  digitalWrite(motor1_back, 0);
  digitalWrite(motor2_back, 0);
}
void ROBI_BACK(){                      //MOTORS BACK FUNCTION
  digitalWrite(motor1_forward, 0);
  digitalWrite(motor2_forward, 0);
  analogWrite(motor1_back, ROBI_SPEED);
  analogWrite(motor2_back, ROBI_SPEED);
}
void ROBI_STOP() {                      //MOTORS STOP FUNCTION

  digitalWrite(motor1_forward, 1);
  digitalWrite(motor2_forward, 1);
  digitalWrite(motor1_back, 1);
  digitalWrite(motor2_back, 1);
}
void ROBI_LEFT() {                     //MOTORS LEFT FUNCTION
  analogWrite(motor1_forward, ROBI_SPEED);
  digitalWrite(motor2_forward, 0);
  digitalWrite(motor1_back, 0);
  analogWrite(motor2_back, ROBI_SPEED);
}
void ROBI_RIGHT() {                    //MOTORS RIGHT FUNCTION
  digitalWrite(motor1_forward, 0);
  analogWrite(motor2_forward, ROBI_SPEED);
  analogWrite(motor1_back, ROBI_SPEED);
  digitalWrite(motor2_back, 0);
}
void ROBI_FORW_LEFT() {             //FORWARD LEFT FUNCTION
k = ROBI_SPEED*0.8;
  analogWrite(motor1_forward, ROBI_SPEED);
  analogWrite(motor2_forward, k);
  digitalWrite(motor1_back, 0);
  digitalWrite(motor2_back, 0);
}
void ROBI_FORW_RIGHT() {             //FORWARD RIGHT FUNCTION
k = ROBI_SPEED*0.8;
  analogWrite(motor1_forward, k);
  analogWrite(motor2_forward, ROBI_SPEED);
  analogWrite(motor1_back, 0);
  analogWrite(motor2_back, 0);
}
void ROBI_BACK_LEFT() {               //BACK LEFT FUNCTION
k = ROBI_SPEED*0.8;
  digitalWrite(motor1_forward, 0);
  digitalWrite(motor2_forward, 0);
  analogWrite(motor1_back, k);
  analogWrite(motor2_back, ROBI_SPEED);
}
void ROBI_BACK_RIGHT() {               //BACK RIGHT FUNCTION
k = ROBI_SPEED*0.8;
  digitalWrite(motor1_forward, 0);
  digitalWrite(motor2_forward, 0);
  analogWrite(motor1_back, ROBI_SPEED);
  analogWrite(motor2_back, k);
}
