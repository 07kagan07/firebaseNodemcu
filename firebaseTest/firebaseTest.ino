#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <OneWire.h>

#define FIREBASE_HOST "*****.firebaseio.com"
#define FIREBASE_AUTH "SecretCode"
#define WIFI_SSID "wifiName"
#define WIFI_PASSWORD "wifiPW"

int DS18S20_Pin = D3;
OneWire ds(DS18S20_Pin);

void setup() 
{
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}



void loop() {
  
  float temp = getTemp();
  Serial.print("Sicaklik: ");
  Serial.println(temp);
  delay(1000);

  Firebase.setFloat("temp", temp);
  if (Firebase.failed()) 
    {
        Serial.print("setting /Temp failed:");
        Serial.println(Firebase.error());
    }

}





float getTemp()
{
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) 
  {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) 
  {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28)
  {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad

  for (int i = 0; i < 9; i++) 
  { // we need 9 bytes
    data[i] = ds.read();
  }

  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;
  
  
}
