#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include "DHT.h"
//======================================== 

//======================================== 
#define WIFI_SSID "Dave1"
#define WIFI_PASSWORD "dattadev"

#define API_KEY "AIzaSyCpKbfcAshYHxhqATYowO3U6C7DA1cS60Y"
#define DATABASE_URL "https://freemason2-3aaea-default-rtdb.firebaseio.com/" 
//======================================== 

//======================================== 



 
#include "addons/RTDBHelper.h"
#include "addons/TokenHelper.h"



// Define Firebase Data object.
const int led = 18;
const int led2 = 13;
const int led3 = 21;
#define DHTPIN 35
#define DHTTYPE DHT11
#define gasanalog 32
#define ldranalog 25
#define potanalog 34
const int buttonPin = 4;
int potval = 0;
int ldrval = 0;
int gasval = 0;
int irsensor=34;
int ledval = 0;
int led2val = 0;
int buttonval=0;
const int FADE_DELAY = 10;

DHT dht11(DHTPIN, DHTTYPE);
//======================================== 

String fireStatus = "";                                                     // led status received from firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

//======================================== Millis variable to send/store data to firebase database.
unsigned long sendDataPrevMillis = 0;
const long sendDataIntervalMillis = 100; //--> Sends/stores data to firebase database every 10 seconds.
//======================================== 
bool signupOK = false;
float Temp_Val;
int Humd_Val;
int LED_01_State;

//________________________________________________________________________________ Get temperature and humidity values from the DHT11 sensor.
void read_DHT11() {
  Temp_Val = dht11.readTemperature();
  Humd_Val = dht11.readHumidity();
                
//---------------------------------------- Check if any reads failed.
  if (isnan(Temp_Val) || isnan(Humd_Val)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    Temp_Val = 0.00;
    Humd_Val = 0;
  }
  //---------------------------------------- 

  Serial.println("---------------Read_DHT11");
  Serial.print(F("Humidity   : "));
  Serial.print(Humd_Val);
  Serial.println("%");
  Serial.print(F("Temperature: "));
  Serial.print(Temp_Val);
  Serial.println("°C");
  Serial.println("---------------");
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Store data to firebase database.
void store_data_to_firebase_database() {
  Serial.println();
  Serial.println("---------------Store Data");  
  // Write an Int number on the database path DHT11_Data/Temperature.
  if (Firebase.RTDB.setFloat(&fbdo, "Temperature/val", Temp_Val)) {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
  
  // Write an Float number on the database path DHT11_Data/Humidity.
  if (Firebase.RTDB.setInt(&fbdo, "Humidity/val", Humd_Val)) {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
    Serial.println("---------------");
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Read data from firebase database.

//________________________________________________________________________________ 

//________________________________________________________________________________ VOID SETUP
void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  Serial.println();

  pinMode(buttonPin, INPUT);
  pinMode(led2, OUTPUT);
  pinMode(led, OUTPUT); 
  pinMode(led3, OUTPUT); 

  pinMode(irsensor,INPUT);
  //---------------------------------------- The process of connecting the WiFi on the ESP32 to the WiFi Router/Hotspot.
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("---------------Connection");
  Serial.print("Connecting to : ");
  Serial.println(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    digitalWrite(led, HIGH);
    delay(1000);
    digitalWrite(led, LOW);
    delay(250);
  }
  digitalWrite(led, LOW);
  Serial.println();
  Serial.print("Successfully connected to : ");
  Serial.println(WIFI_SSID);
  //Serial.print("IP : ");
  //Serial.println(WiFi.localIP());
  Serial.println("---------------");
  //---------------------------------------- 

  // Assign the api key (required).
  config.api_key = API_KEY;

  // Assign the RTDB URL (required).
  config.database_url = DATABASE_URL;

  // Sign up.
  Serial.println();
  Serial.println("---------------Sign up");
  Serial.print("Sign up new user... ");
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  Serial.println("---------------");
  
  // Assign the callback function for the long running token generation task.
  config.token_status_callback = tokenStatusCallback; //--> see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  dht11.begin();
  delay(100);
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID LOOP
void loop() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > sendDataIntervalMillis || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    read_DHT11();

    //ir sensor loop................................................................
    int h = digitalRead(irsensor);  
    Serial.print(F("ir sensor val   : "));
    Serial.println(h);
    if (h==0){
          if (Firebase.RTDB.setInt(&fbdo, "LED_Data/LED_01", 0)) {
                Serial.println("ir pushed led on");
           }
          else {
                Serial.println("FAILED to push ir");
            }
      }

   else{
          if (Firebase.RTDB.setInt(&fbdo, "LED_Data/LED_01", 1))
          {
                Serial.println("ir pushed led off");
            }
        else {
                Serial.println("FAILED to push ir");
              }
    }
   delay(20);
       //gas sensor loop................................................................

   gasval = analogRead(gasanalog);
   Serial.print(F("gas val : "));
   Serial.println(gasval);      
      if (Firebase.RTDB.setInt(&fbdo, "gas/val", gasval))
      {
           Serial.println("gas val pushed");
      }
        else {
                Serial.println("FAILED to push gas");
     }
         //pot sensor loop................................................................

   potval = analogRead(potanalog);
   Serial.print(F("pot val : "));
   Serial.println(potval);      
      if (Firebase.RTDB.setInt(&fbdo, "ldr/val", potval))
      {
           Serial.println("pot val pushed");
      }
        else {
                Serial.println("FAILED to push pot");
     }
         //ldr sensor loop................................................................

   ldrval = analogRead(ldranalog);
   Serial.print(F("ldr val : "));
   Serial.println(ldrval);      
      if (Firebase.RTDB.setInt(&fbdo, "pot/val", ldrval))
      {
           Serial.println("ldr val pushed");
      }
        else {
                Serial.println("FAILED to push ldr");
     }
    store_data_to_firebase_database();
      Serial.println("-----------------------------------------------------------Get Data");
//  digitalWrite(led, HIGH);
  
  if (Firebase.RTDB.getInt(&fbdo, "/LED1/val")) {
         Serial.println("read");
      
String ledval = fbdo.stringData();

  int x = ledval.toInt();
  // Use x here

      digitalWrite(led, x);
      Serial.print("LED_01_State : ");
      Serial.println(ledval);
 //   }
  }
  if (Firebase.RTDB.getInt(&fbdo, "/LED2/val")) {
         Serial.println("read");
               //relay sensor loop................................................................

String ledval = fbdo.stringData();

  int y = ledval.toInt();
  // Use x here

      digitalWrite(led2, y);
      Serial.print("LED_02_State : ");
      Serial.println(led2val);
 //   }
  }
  int ledBrightness = map(ldrval, 0, 1023, 0, 255);
  analogWrite(led3, ledBrightness);
  delay(FADE_DELAY);
  }

}



  
  
