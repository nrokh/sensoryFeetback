/* 
NROKH 2/14/18 
nrokh.github.io

This is an Arduino Uno with four force-sensitive resistors triggering four ERM motor vibrations through i2c communication. 
Script includes sending the bootstate array (8 parameters) over ROS.
See attached file for wiring layout. 

Thank you, namaste, and good luck.
*/

#include <Wire.h>                          // i2c library
#include "Adafruit_DRV2605.h"              // driver library
#include <ros.h>                           // ros library
#include <std_msgs/Int16MultiArray.h>      // array library


ros::NodeHandle nh;                       // create node called nh

std_msgs::Int16MultiArray intArray;       // create an array called intArray
ros::Publisher bootState("bootState", &intArray);     // create a ros topic called bootState to the pointer intArray


Adafruit_DRV2605 drv0;                    // label drivers, 0-3
Adafruit_DRV2605 drv1; 
Adafruit_DRV2605 drv2;
Adafruit_DRV2605 drv3; 

#define TCAADDR 0x70                      // indexing variable for i2c communication

int fsrPin0 = 0;                          // label FSR pin, 0-3
int fsrPin1 = 1;
int fsrPin2 = 2;
int fsrPin3 = 3;

int vibeState0;                           // initialize vibration states
int vibeState1;
int vibeState2;
int vibeState3;

int fsrReading0;                          // initialize fsr states
int fsrReading1;
int fsrReading2;
int fsrReading3; 


//***********************************
void tcaselect(uint8_t i) {               // the function to select the particular driver you're using
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);        // begin from 0x70
  Wire.write(1 << i);                     // this is bit indexing the channel, the 1 is because the indexing starts from 1-8 not 0-7
  Wire.endTransmission(); 
}
//************************************

void setup() {
  
  Serial.begin(57600);                    // baud rate 57600
  Wire.begin();                           // begin i2c comm -- it will work sporadically if you forget to include this line >:-( 
  
  intArray.data_length = 8;               // tell ros what length of the array to expect
    
  nh.initNode();                          // initialize the ros node
  nh.advertise(bootState);                // advertise the topic to ros
  
//**************************************
  tcaselect(0);                           // initialize first driver, connected to SD0/SC0
  if(!drv0.begin())
  {
    Serial.println("\nNo DRV2605 drv0 detected");
    while(1);
  }
  
  tcaselect(1);                           // initialize second driver, connected to SD1/SC1
  if(!drv1.begin())
  {
    Serial.println("\nNo DRV2605 drv1 detected");
    while(1);
  }
  
    tcaselect(2);                         // initialize third driver, connected to SD2/SC2
  if(!drv2.begin())
  {
    Serial.println("\nNo DRV2605 drv2 detected");
    while(1);
  }

  tcaselect(3);                           //initialize fourth driver, connected to SD3/SC3
    if(!drv3.begin())
  {
    Serial.println("\nNo DRV2605 drv3 detected");
    while(1);
  }
  
//*******************************************
tcaselect(0);                             // select the first driver
  drv0.begin();                           // begin i2c commm
  drv0.setMode(DRV2605_MODE_INTTRIG);     // set the vibration mode to internal trigger (see datasheet 7.3.5.6: Input Trigger Option)
  drv0.selectLibrary(1);                  // select the first vibration library
  drv0.setWaveform(0,84);                 // setting waveforms:(index of all waveforms, index of particular waveform)
  drv0.setWaveform(1,0);                  // end waveform setting
 
  tcaselect(1);                           // repeat for all four drivers
  drv1.begin();
  drv1.setMode(DRV2605_MODE_INTTRIG);
  drv1.selectLibrary(1);
  drv1.setWaveform(0,84);
  drv1.setWaveform(1,0);
  
  tcaselect(2);
  drv2.begin();
  drv2.setMode(DRV2605_MODE_INTTRIG);
  drv2.selectLibrary(1);
  drv2.setWaveform(0,84);
  drv2.setWaveform(1,0);
  
  tcaselect(3);
  drv3.begin();
  drv3.setMode(DRV2605_MODE_INTTRIG);
  drv3.selectLibrary(1);
  drv3.setWaveform(0,84);
  drv3.setWaveform(1,0);

  
}

void loop() {

  fsrReading0 = analogRead(fsrPin0);        // read the value from the labeled fsr pin
  vibeState0 = 0;                           // initialize the vibe state when not buzzing to be zero
  fsrReading1 = analogRead(fsrPin1);
  vibeState1 = 0;
  fsrReading2 = analogRead(fsrPin2);
  vibeState2 = 0;
  fsrReading3 = analogRead(fsrPin3);
  vibeState3 = 0;
  
  if (fsrReading0 > 15) {                   // if the fsr is pressed hard enough past a certain threshold (adjust value as necessary)
    vibeState0 = 1;                         // set the variable vibeState to be 1, for on
    tcaselect(0);                           // select the appropriate driver
    drv0.go();                              // activate the vibration waveform
  }
  if (fsrReading1 > 15) {                    // repeat for all fsrs and vibes
    vibeState1 = 1;
    tcaselect(1);
    drv1.go();
  }
  if (fsrReading2 > 15) {
    vibeState2 = 1;
    tcaselect(2);
    drv2.go();
  }
  if (fsrReading3 > 15) {
    vibeState3 = 1;
    tcaselect(3);
    drv3.go();
  }
  //**********************************************

  intArray.data[0] = fsrReading0;           // input the values read from the fsrs into the array 
  intArray.data[1] = fsrReading1;
  intArray.data[2] = fsrReading2;
  intArray.data[3] = fsrReading3;

  intArray.data[4] = vibeState0;            // input the states of the motors into the array
  intArray.data[5] = vibeState1;
  intArray.data[6] = vibeState2;
  intArray.data[7] = vibeState3;

  bootState.publish(&intArray);             // publish the bootState to ros
  
  nh.spinOnce();                            // spin to send information 
  
  delay(100);                               // 10Hz frequency -- can use millis() instead if you have enough memory on your Arduino
}
