/*
 This sketch is intended for an ESP8266-12E/F.

 The purpose is to allow the ESP to switch a transistor, generating a current
 that when passed through a transformer will generate a high voltage shock

 BLYNK PIN CONFIGURATION:
 V1 = button INPUT, 1 on down 0 on up
 V2 = power value
 V3 = battery level
 V4 = WiFi RSSI

*/

#include "main.h" 


#define OTA_LOG_TAG "OTA"

HTTPClient http;

const char auth[] = "450cf8ff747d480f9c7d40c5f8f1346c";
WidgetTerminal terminal(V4);

void updateMain();

// Called on shock button press
BLYNK_WRITE(V1) {
  Serial.println("Starting callback");
  int STATE = param.asInt(); //
  if (STATE) {
    Serial.println("Starting shock");
    shock.beginShock();
    
    // espLed.stop();
    // espLed.on();
  } else {
    Serial.println("Ending shock");
    shock.endShock();
    // espLed.start();
  }
}

// Updates power level on change in app
BLYNK_WRITE(V0) { 
  //shock.setPower(param.asInt());
  Serial.println("Set power to " + String(param.asInt()));
  terminal.println("Set power to " + String(param.asInt())); 
}

// Called on menu selection
BLYNK_WRITE(V3) {
  switch (param.asInt()) {

  case 0:
    ESP.deepSleep(0);
    break;

  case 1:
    Serial.println("Update");
    updateMain();
    break;

  case 2:
    WiFi.disconnect();

  case 3:
    //ESP.reset();
    break;

  // Factory reset
  case 4:
    break;

  default:
    break;
  }
}

// uint8_t measureBattery() {
//   return (VCC_MEAS) ? map(ESP.getVcc(), 0, PWMRANGE, 0, 100)
//                     : map(analogRead(A0), VOLT_MIN, VOLT_MAX, 0, 100);
// }

uint8_t dbmToQuality(int dBm) { return constrain(2 * (dBm + 100), 0, 100); }

void pushData() {
  Blynk.virtualWrite(V4, 0);
  Blynk.virtualWrite(V5, dbmToQuality(WiFi.RSSI()));
}

void shock_init(){
  // shock.setPower(10);
  // shock.setChannel(CH1);
  // shock.setMode(SHOCK);
}

void wifi_init(){
  WiFi.mode(WIFI_STA);
  if( !WiFi.begin() ) {
    Serial.print("Beginning smart config");
    WiFi.beginSmartConfig();
    while( !WiFi.smartConfigDone() ){
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
  }
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void updateMain(){
  http.begin(UPDATE_URL, caCert);

  // Use WiFi_clientSecure class to create TLS connection
  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  if(httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
  }

  // file found at server
  if(httpCode != HTTP_CODE_OK) {
    return;
  }

  // get lenght of document (is -1 when Server sends no Content-Length header)
  int len = http.getSize();
  Serial.printf("Content length: %i\n", len);

  
  
  if(!Update.begin(len)){
    Serial.println("Did not begin!");
    return;
  }

  ESP_LOGI(OTA_LOG_TAG, "Beginning OTA update\n");
  xTaskCreate(
    update,
    "update",
    12000,
    NULL,
    1,
    NULL
  );
  
  while(Update.isRunning()){
    delay(50);
  }
  Serial.println("\nFinished");

  
  
    
  if(!Update.hasError() && Update.isFinished()){

    http.end();
    ESP.restart();
  }
  else{
    Serial.println(Update.getError());
  }
}


void setup() {
  

  esp_log_level_set("*", ESP_LOG_VERBOSE);
  TransmitRF::logLevel(ESP_LOG_VERBOSE);


  

  shock_init();
  wifi_init();
  
  
  delay(1000);

  // configure time
  configTime(3 * 3600, 0, "pool.ntp.org");
  delay(3000);

  Blynk.config(auth);

}


void update( void * ptr ){
  const uint16_t SIZE = 64;

  WiFiClient * stream = http.getStreamPtr();
  stream->setNoDelay(true);
  size_t written = 0;
  Serial.println("loop start");
  while( stream->available() || stream->connected() ){
    const size_t incoming = stream->available();
    if(incoming <= 0) continue;

    uint8_t *buf = new uint8_t[incoming];
    if(!buf) buf = new uint8_t[SIZE];

    
    stream->read(buf, incoming);
    written += Update.write(buf, incoming);
    Serial.println(written);
    delete [] buf;
    Serial.printf( "Updating... %i%\r", Update.progress() / Update.size() );

  }
  Serial.println("Done");
  ESP_LOGI(OTA_LOG_TAG, "End of available data\n");
  if(written != http.getSize()) 
    ESP_LOGE(OTA_LOG_TAG, "Incomplete write, wrote %i/%i\n", written, http.getSize());

  ESP_LOGI(OTA_LOG_TAG, "Update ended with code (%i)\n", Update.end());
 
  stream->flush();
  stream->stop();
  Update.end();
  vTaskDelete(NULL);
}


void loop() {
  Blynk.run();
}