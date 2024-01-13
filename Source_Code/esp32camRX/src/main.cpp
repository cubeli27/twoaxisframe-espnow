//BROWNOUT
//#include "soc/soc.h"
//#include "soc/rtc_cntl_reg.h"
// Include Libraries
#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h>

//For Servo
static const int servoPin = 13;
static const int servo2Pin = 12;
static const int led = 4;
Servo servo1;
Servo servo2;

// Define a data structure
typedef struct struct_message {
  char a[32];
  int x;
  int y;
  bool sw;
} struct_message;

// Create a structured object
struct_message myData;


// Callback function executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  /*Serial.print("Data received: ");
  Serial.println(len);
  Serial.print("Message: ");
  Serial.println(myData.a);
  Serial.print("x: ");
  Serial.println(myData.x);
  Serial.print("y: ");
  Serial.println(myData.y);
  Serial.print("Boolean Value: ");
  Serial.println(myData.sw);
  Serial.println();*/
}

void setup() {
  //BROWNOUT
  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable detector
  // Set up Serial Monitor
  Serial.begin(115200);
  // Servo 
  servo1.setPeriodHertz(50);    // standard 50 hz servo
  servo1.attach(servoPin);
  servo2.setPeriodHertz(50);    // standard 50 hz servo
  servo2.attach(servo2Pin);

  pinMode(led,OUTPUT);
  digitalWrite(led,HIGH);
  
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register callback function
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  delay(100);
  if(myData.x<0){
    myData.x = 0;
  }
  if(myData.x>180){
    myData.x = 180;
  }
  if(myData.y<0){
    myData.y = 0;
  }
  if(myData.y>180){
    myData.y = 180;
  }
  servo1.write(myData.x);
  servo2.write(myData.y);
  digitalWrite(led,myData.sw);
}