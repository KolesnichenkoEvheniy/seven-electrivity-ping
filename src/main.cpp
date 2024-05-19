#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32httpUpdate.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>

String FirmwareVer = "1.0"; // current firmware version.
WiFiClientSecure client;
const char* string1 = FirmwareVer.c_str();

// Comment it if you use your private url.
#define URL_fw_Version "https://raw.githubusercontent.com/KolesnichenkoEvheniy/seven-electrivity-ping/main/ota/version.txt"
#define URL_fw_Bin "https://raw.githubusercontent.com/KolesnichenkoEvheniy/seven-electrivity-ping/main/ota/latest.bin"
#define LED_BUILTIN 2
#define SITE_FINGERPRINT "97:D8:C5:70:0F:12:24:6C:88:BC:FA:06:7E:8C:A7:4D:A8:62:67:28"

HTTPClient http;

// const char* ssid = "Your_SSID"; // Put your wifi name here
// const char* password = "your_password"; // put your wifi password here, if it's open then only declare = ""

void FirmwareUpdate()
{
  Serial.print("\n Current Version is: ");
  Serial.print(string1);
  
  // check version URL, Thumbprint(Fingerprint) of Website and thumbprint is change in some months.
  // you can find website thumbprint using this website : https://www.grc.com/fingerprints.htm
  client.setInsecure();
  http.begin(URL_fw_Version, SITE_FINGERPRINT);

  delay(100);
  int httpCode = http.GET();            // get data from version file
  delay(100);
  String newVersion;
  const char* string2 = newVersion.c_str();


  if (httpCode == HTTP_CODE_OK)         // if version received
  {
    newVersion = http.getString();  // save received version
    Serial.print("\n New Version is : ");
    Serial.print(newVersion);
  }
  else
  {
    Serial.print("\n error in downloading version file:");
    Serial.println(httpCode);

  }

  http.end();
  if (httpCode == HTTP_CODE_OK)         // if version received
  {
    int ver = strcmp(string1, string2); // compare both version
    int dif = ver; // assign compare value in dif variable 
    Serial.printf("\n differrence is : %d", dif);
    if (ver == -10)
    {
      Serial.println("\n Device already on latest firmware version");
    }
    else if(ver == dif)
    {
      Serial.println("\n New firmware detected");
      WiFiClient client;

      t_httpUpdate_return ret = ESPhttpUpdate.update(URL_fw_Bin, "", SITE_FINGERPRINT);

      switch (ret) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("\n HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          break;

        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("\n HTTP_UPDATE_NO_UPDATES");
          break;

        case HTTP_UPDATE_OK:
          Serial.println("\n HTTP_UPDATE_OK");
          break;
      }
    }
  }
}

unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 1*20*1000; // 1 Minute interval (60*1000= 60 Second)

void repeatedCall() {
  unsigned long currentMillis = millis();
  if ((currentMillis - previousMillis) >= interval)
  {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    FirmwareUpdate();
  }
}


void setup()
{
  Serial.begin(115200);
  WiFiManager wm;
  std::vector<const char *> menu = {"wifi","info", "exit"};
  wm.setMenu(menu);
  Serial.println("");
  Serial.println("Start");

  bool res;
    // res = wm.autoConnect(); // auto generated AP name from chipid
    // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("AutoConnectAP","12345678"); // password protected ap

  if(!res) {
      Serial.println("Failed to connect");
      // ESP.restart();
  } 
  else {
      //if you get here you have connected to the WiFi    
      Serial.println("connected...yeey :)");
  }

  //WiFi.mode(WIFI_STA);
  //WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  pinMode(LED_BUILTIN, OUTPUT);
}
void loop()
{
  // digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  // delay(1000);                       // wait for a second
  // digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  // delay(1000);                       // wait for a second
  repeatedCall();
}