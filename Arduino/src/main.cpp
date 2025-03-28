#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define RST_PIN         D0        // Configurable, see typical pin layout above
#define SS_PIN          D4      // Configurable, see typical pin layout above
#define GREEN_LED       D1
#define RED_LED         D2
#define URL_T           "https://guardia-api.iadjedj.ovh/token"   
#define admin          "E4 B1 E0 62"
#define user           "D4 EF EC 62"
#define ID_WIFI        "Kirito"
#define PASSWD_WIFI    "kirito400."
#define USER_API        "admin_123"
#define PASSWD_API      "password_456"

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void setup() {
  Serial.begin(115200);	

  WiFi.begin(ID_WIFI, PASSWD_WIFI);

  while(WiFi.status() != WL_CONNECTED){
    delay(100);
  }
  Serial.println("Wifi connecter");
		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(RED_LED, 1);
}


void loop() {
  
  HTTPClient http;
  http.begin(URL_T);
  http.addHeader( "Accept", "application/json");
  http.addHeader("Content-type", "application/x-www-form-urlencoded");
  String bodyPoste = "grant_type=password&username=" USER_API  "&password=" PASSWD_API;
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
  delay(10000);











	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}
  String UID = ""; 
  for (byte i = 0; i < mfrc522.uid.size; i++){
    UID.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    UID.concat(String(mfrc522.uid.uidByte[i], HEX ));
  }
  UID.toUpperCase();

  if(UID.substring(1) == admin){
    delay(1000);
    digitalWrite(GREEN_LED, 1);
    digitalWrite(RED_LED, 1);
    delay(3000);
    digitalWrite(GREEN_LED, 0);
    digitalWrite(RED_LED, 0) ;
  }
  else if (UID.substring(1) == user){
    delay(1000);
    digitalWrite(GREEN_LED, 1);
    digitalWrite(RED_LED, 0);
    delay(4000);
    digitalWrite(GREEN_LED, 0);
  }
  else{
    delay(1000);
    digitalWrite(GREEN_LED,0);
    digitalWrite(RED_LED, 1 );
  }

	// Dump debug info about the card; PICC_HaltA() is automatically called
	mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}