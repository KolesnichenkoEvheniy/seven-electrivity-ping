#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32httpUpdate.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>

String FirmwareVer = "1.3"; // current firmware version.
WiFiClientSecure client;
const char* string1 = FirmwareVer.c_str();

// Comment it if you use your private url.
#define URL_fw_Version "https://raw.githubusercontent.com/KolesnichenkoEvheniy/seven-electrivity-ping/main/ota/version.txt"
#define URL_fw_Bin "https://raw.githubusercontent.com/KolesnichenkoEvheniy/seven-electrivity-ping/main/ota/latest.bin"
#define LED_BUILTIN 2
#define SITE_FINGERPRINT "09:01:0C:CE:9B:72:21:55:C7:E6:86:B0:77:39:D3:D2:DC:06:05:DE:A1:A4:98:4A:0B:96:5E:18:77:77:26:B5"

const char* test_root_ca= \
  "-----BEGIN CERTIFICATE-----\n" \
  "MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\n" \
  "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
  "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n" \
  "MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
  "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
  "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\n" \
  "9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\n" \
  "2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\n" \
  "1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\n" \
  "q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz\n" \
  "tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ\n" \
  "vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP\n" \
  "BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\n" \
  "5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY\n" \
  "1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4\n" \
  "NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG\n" \
  "Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91\n" \
  "8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe\n" \
  "pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl\n" \
  "MrY=\n" \
  "-----END CERTIFICATE-----\n";


HTTPClient http;
ESP32HTTPUpdate httpUpdate;

// const char* ssid = "Your_SSID"; // Put your wifi name here
// const char* password = "your_password"; // put your wifi password here, if it's open then only declare = ""

void FirmwareUpdate()
{
  Serial.print("\n Current Version is: ");
  Serial.print(string1);
  
  // check version URL, Thumbprint(Fingerprint) of Website and thumbprint is change in some months.
  // you can find website thumbprint using this website : https://www.grc.com/fingerprints.htm
  // client.setInsecure();
  client.setCACert(test_root_ca);
  http.begin(URL_fw_Version);

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

      httpUpdate.rebootOnUpdate(false); // remove automatic update

      t_httpUpdate_return ret = ESPhttpUpdate.update(URL_fw_Bin);

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
      switch (ret) {
          case HTTP_UPDATE_FAILED:
            Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
            Serial.println(F("Retry in 10secs!"));
            delay(10000); // Wait 10secs
            break;
    
          case HTTP_UPDATE_NO_UPDATES:
            Serial.println("HTTP_UPDATE_NO_UPDATES");
            break;
    
          case HTTP_UPDATE_OK:
            Serial.println("HTTP_UPDATE_OK");
            delay(1000); // Wait a second and restart
            ESP.restart();
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

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

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

  Serial.print(F("Firmware version "));
  Serial.println(FirmwareVer);

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.print(F("Inizializing FS..."));
  if (SPIFFS.begin()){
      Serial.println(F("done."));
  }else{
      Serial.println(F("fail."));
  }
 
  String fileSystemVersion = "Not set!";
  Serial.print(F("FileSystem version "));
  File versionFile = SPIFFS.open(F("/version.txt"), "r");
  if (versionFile) {
      fileSystemVersion = versionFile.readString();
      versionFile.close();
  }
  Serial.println(fileSystemVersion);
  delay(2000);
}
void loop()
{
  // digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  // delay(1000);                       // wait for a second
  // digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  // delay(1000);                       // wait for a second
  repeatedCall();
}