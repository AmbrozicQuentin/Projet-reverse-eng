#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <secret.h>


#define RST_PIN        D0        // Configurable, see typical pin layout above
#define SS_PIN         D4      // Configurable, see typical pin layout above
#define GREEN_LED      D1
#define RED_LED        D2


#define API_CHECK      "https://guardia-api.iadjedj.ovh/check_badge?badge_id="


unsigned long stockagetimecarte = 0;


String token = "" ;
String level = "" ;


int compteur = 0;


MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance




void setup() {
  Serial.begin(115200);


  WiFi.begin(ID_WIFI, PASSWD_WIFI);


  while(WiFi.status() != WL_CONNECTED){
    delay(100);
  }
  Serial.println("Wifi connecter");
    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  delay(4);       // Optional delay. Some board do need more time after init to be ready, see Readme
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));


  configTime(3600,0,"pool.ntp.org");


  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

}



void POST() {
  #define URL_T  "https://guardia-api.iadjedj.ovh/token"


  HTTPClient http;
  http.begin(URL_T);
  http.addHeader( "Accept", "application/json");
  http.addHeader("Content-type", "application/x-www-form-urlencoded");
  String bodyPoste = String("grant_type=password&username=") + String(USER_API) + "&password=" + String(PASSWD_API);
  int httpRep = http.POST(bodyPoste); // envoie reponse 
  String response = http.getString();
  if (httpRep > 0){
    Serial.println("Serveur response: ");
    Serial.println(httpRep);
    Serial.println(response);
    if (httpRep == 200 | httpRep == 201){
      Serial.println("Requête réussite");
    }
    else if(httpRep == 204){
      Serial.println("Pas de réponse du serveur");
    }
    else{
      Serial.println("Serveur injoignable");
   }
  }
  http.end();




  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, response);
  if (error){
    Serial.print("deserializedJson faild");
  }
  Serial.println("json deserialize ok");
  String tokenconvert = doc["access_token"];
  token = tokenconvert;
  Serial.print("le token est "); Serial.println(tokenconvert);
  delay(100);


}


void check_api(){
  String UID = "";


  for (byte i = 0; i < mfrc522.uid.size; i++){
    UID.concat(int(mfrc522.uid.uidByte[i]));
  }


  String urlfull = API_CHECK + UID;


  Serial.println(urlfull);


  Serial.print("Il s'agie de l'UID: ");  Serial.println(UID);


  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));




  HTTPClient http;
  String authHeader = "Bearer " + token;
  http.begin(urlfull);
  http.addHeader("Accept",  "application/json");
  http.addHeader("Authorization", authHeader);


  Serial.println(authHeader);


  int httpRepc = http.GET();
 
  if(httpRepc != HTTP_CODE_OK){
    Serial.print("Error Http"); Serial.println(httpRepc);
    http.end();
  }
  Serial.println("Send Request");


  String response = http.getString() ;


  Serial.println(response);


  http.end();


  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, response);
  if (error){
    Serial.print("deserializedJson faild");
  }
  Serial.println("json deserialize ok");
  String levelconvert = doc["level"];
  level = levelconvert;
  Serial.print("the level badge is : "); Serial.println(level);
  delay(100);
}

void autorizade_user(){
  delay(1000);
  digitalWrite(GREEN_LED, 1);
  digitalWrite(RED_LED, 0);
  delay(2000);
  digitalWrite(GREEN_LED, 0);
}

void autorizade_admin(){
delay(1000);
digitalWrite(GREEN_LED, 1);
digitalWrite(RED_LED, 1);
delay(2000);
digitalWrite(GREEN_LED, 0);
}

void not_autorizade(){
  delay(1000);
  digitalWrite(RED_LED, 1 );
  delay (500);
  digitalWrite(RED_LED, 0 );
  delay(500);
  digitalWrite(RED_LED, 1 );
  delay(500);
  digitalWrite(RED_LED, 0 );
  Serial.print("the acces for bedent beceause is close");
  delay(1000);

}

void loop() {
  digitalWrite(RED_LED,1);
  
  unsigned long timecarte = millis();
  struct  tm timeinfo;


  // timee internal
  if( !getLocalTime(&timeinfo) ){
    Serial.println("Failed to obein time");
  }


  if (timecarte - stockagetimecarte >= 3600000 ){
  Serial.println(&timeinfo, "%H :%M :%S");
  stockagetimecarte= timecarte;
  }


  int hour_check = timeinfo.tm_hour;
 


  if(timecarte < stockagetimecarte){
    Serial.print("fonction millis revenue à zero");
    compteur = 0;
  }



 //  

  if(timecarte - stockagetimecarte >= 3600000  || compteur == 0 ){
    Serial.println("Token Demande");
    POST();
    stockagetimecarte = timecarte;
    Serial.println("--------");
    Serial.print("token in the loop:"); Serial.println(token);
    compteur += 1;
    Serial.println(compteur);
  }

 
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }


    // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

    check_api();

  if(level == ADMIN){
    autorizade_admin();
  }
  if (hour_check > 8 && hour_check < 18 && level == USER){
    autorizade_user();
  }
  else{
    not_autorizade();
  }
}




  // Dump debug info about the card; PICC_HaltA() is automatically called


