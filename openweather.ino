#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Define Variables
#define OLED_MOSI   13 //D1
#define OLED_CLK   14 //D0
#define OLED_DC    12 //DC
#define OLED_CS    15 //CS
#define OLED_RESET 2//RES
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

const char* ssid     = "namelessneko";           // insert your SSID
const char* password = "0917643580";   // insert your password

WiFiClient client;

const char* server = "api.openweathermap.org";  // server's address
const char* resource = "/data/2.5/weather?id=1668341&units=metric&APPID=7f9abc6839ab446bf8d5894bdae77685"; // insert your API key

char response[600]; // this fixed sized buffers works well for this project using the NodeMCU.

void setup(){
    // initialize serial
    Serial.begin(115200);
    Serial.print("system start... ");
    while(!Serial){
        delay(100);
        }
      
    // initialize WiFi
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);  
    }
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.setTextWrap(false);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0,10);
  display.print("start");
  display.display();
}



void loop(){
    
        // connect to server  
        bool ok = client.connect(server, 80);
        bool beginFound = false;
        
        while(!ok){
            Serial.print(".");
            delay(500);        
        }
    
        delay(500);
    
        //Send request to resource
        client.print("GET ");
        client.print(resource);
        client.println(" HTTP/1.1");
        client.print("Host: ");
        client.println(server);
        client.println("Connection: close");
        client.println();
    
        delay(100);
    
        //Reading stream and remove headers
        client.setTimeout(10000);
   
        bool ok_header = ok_header = client.find("\r\n\r\n");
    
        while(!ok_header){
        // wait
        }
         
        client.readBytes(response, 800);
        
        // uncomment line below to see data received for debugging purposes or just fun    
        // Serial.println(response); 
        
        // process JSON
        DynamicJsonBuffer jsonBuffer;
        
        // But.....make sure the stream header is valid
        // Sometime OWM includes invalid data after the header
        // Parsing fails if this data is not removed
        
        if(int(response[0]) != 123){
            Serial.println("Wrong start char detected");
        int i = 0;
        while(!beginFound){
            if(int(response[i]) == 123){ // check for the "{" 
            beginFound = true;
            Serial.print("{ found at ");
            Serial.println(i);
            }
            i++;
        }

            int eol = sizeof(response);
            Serial.print("Length = ");
            Serial.println(eol);
  
    
            //restructure by shifting the correct data
            Serial.println("restructure");
            for(int c=0; c<(eol-i); c++){
                response[c] = response[((c+i)-1)];
                Serial.print(response[c]);
            }
     
            Serial.println("Done...");

                
            }
        
        
        JsonObject& root = jsonBuffer.parseObject(response);
        
        if (!root.success()) {
        Serial.println("JSON parsing failed!");
        } 
        else {
        //Serial.println("JSON parsing worked!");
        }
        
        const char* location = root["name"]; 
        const char* weather = root["weather"][0]["description"];
        double temp = root["main"]["temp"];
        
        // Print data to Serial
        Serial.print("*** ");
        Serial.print(location);
        Serial.println(" ***");
        Serial.print("Type: ");
        Serial.println(weather);
        Serial.print("Temp ");
        Serial.print(temp);
        Serial.println("C");
        Serial.println("----------"); 
        
       display.clearDisplay();
       display.setTextColor(WHITE);
       display.setTextSize(1);
       display.setCursor(0,0);
       display.print("*** ");
       display.setTextSize(1);
       display.setCursor(70,0);
       display.print(location);
       display.print("***");
       display.setTextSize(1);
       display.setCursor(0,10);
       display.print("TEMP: ");
       display.setTextSize(1);
       display.setCursor(70,10);
       display.print(temp);
       display.print("C ");
       display.setTextSize(1);
       display.setCursor(0,20);
       display.print("Type: ");
       display.setTextSize(1);
       display.setCursor(70,20);
       display.print(weather);
       display.print(" ");
       display.display();
        client.stop(); // disconnect from server
    
        for(int x=0; x<10; x++){ // wait for new connection with progress indicator
            Serial.print(".");
            delay(99999); // the OWM free plan API does NOT allow more then 60 calls per minute
        }
        
        Serial.println("");

    }
    
