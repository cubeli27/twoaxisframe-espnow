/* Include Libraries */
#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
/* Pins */
//const int x_value = 36;
//const int y_value = 32;
const int sw_value = 16 ;
/* Readings */
float x_read;
float y_read;

/* Variables for test data */
int x;
int y;
bool sw = false;

/* MAC Address of responder - ESP32-Cam */
uint8_t broadcastAddress[] = {0x0C, 0xB8, 0x15, 0x77, 0x0F, 0x28};

/* Define a data structure */
typedef struct struct_message {
  char a[32];
  int x;
  int y;
  bool sw;
} struct_message;

/* Create a structured object */
struct_message myData;

/* Peer info */
esp_now_peer_info_t peerInfo;

/* Callback function called when data is sent */
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  
  // Set up Serial Monitor
  Serial.begin(115200);

  /* Accelerometer */
    /* Initialise the ADXL345 */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }
  accel.setRange(ADXL345_RANGE_8_G);

  /* ESP-Now */
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Inputs/Outputs
  //pinMode(x_value,INPUT);
  //pinMode(y_value,INPUT);
  pinMode(sw_value,INPUT);

  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the send callback
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  delay(20);
  /* Get a new sensor event */ 
  sensors_event_t event; 
  accel.getEvent(&event);

  // Read x
  x_read = event.acceleration.x;
  delay(10);
  x = map(x_read,-10,10,0,180);
  //Read y
  y_read = event.acceleration.y;
  delay(10);
  y = map(y_read,-10,10,0,180);
  //Read SW
  if(digitalRead(sw_value) == HIGH){
    sw = true;
  } else {
    sw = false;
  }
  

  // Format structured data
  strcpy(myData.a, "Hello Jure");
  myData.x = x;
  myData.y = y;
  myData.sw = sw;
  //Serial.println(x_read);
  //Serial.println(x);
  //Serial.println(y_read);
  //Serial.println(y);
  //Serial.println(sw);
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sending confirmed");
  }
  else {
    Serial.println("Sending error");
  }
  delay(10);
}