#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoOTA.h>
#include <wifisetup.h>
#include <spiffsh.h>
#include <Konfiguracja.h>

#define PIN            4 //wemos
//#define PIN            5 //nodemcu
#define NUMPIXELS      61 

void setKolor2(int r, int g, int b);
void setKolor(int r, int g, int b);
byte colVal(int val);
byte * Wheel(byte WheelPos);
void setLed(int i, int r, int g, int b, int a);
void karuzela();

int ledid = 16;
int tryb = 0;
int timer = 0;

Konfiguracja kolory = Konfiguracja("kolory");

struct params
{
   	int hue;
   	int alpha;
   	int hue2;
   	int alpha2;
   	int s_val;
   	int i_val;
	int r;
   	int g;
   	int b;
};

typedef struct params Params;
Params par;

unsigned long startMillis; 
unsigned long currentMillis;
const unsigned long period = 30;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void ustawtryb(int wart)
{
	par.r = server.arg("r").toInt();
	par.g = server.arg("g").toInt();
	par.b = server.arg("b").toInt();
		
	switch (wart)
	{
		case 1000:
			par.alpha = 0;
			setKolor(255,255,240);
		break;
		
		case 1001:
			par.alpha = server.arg("alpha").toInt();
			setKolor(255, 0, 0);
		break;
		
		case 1002:
			par.alpha = server.arg("alpha").toInt();
			setKolor2(0, 255, 0);
		break;

		case 1003:
			par.alpha = server.arg("alpha").toInt();
			setKolor(0, 0, 255);
		break;

		case 1004:
			par.alpha=100;
			setKolor(par.r, par.g, par.b);
		break;
		
		case 1005:
			tryb = 2;
			par.alpha = server.arg("alpha").toInt();
			par.s_val = server.arg("s_val").toInt();
		break;
		
		case 1006:
			tryb = 3;
			par.alpha = server.arg("alpha").toInt();
			par.s_val = server.arg("s_val").toInt();
		break;
		
		case 1007:
			tryb = 4;
			par.alpha = server.arg("alpha").toInt();
			par.s_val = server.arg("s_val").toInt();
		break;
		
		case 1008:
			tryb = 1;
			par.hue = server.arg("hue").toInt();
			par.alpha = server.arg("alpha").toInt();
			par.hue2 = server.arg("hue2").toInt();
			par.alpha2 = server.arg("alpha2").toInt();
			par.s_val = server.arg("s_val").toInt();
			par.i_val = server.arg("i_val").toInt();
		break;
		
		case 1009:
			par.alpha = server.arg("alpha").toInt();
			setKolor(255,255,255);
		break;
		
		case 1100:
			
			//int l = server.arg("hue").toInt();
			par.hue = server.arg("hue").toInt();
			par.alpha = server.arg("alpha").toInt();


		break;


		default:
			tryb = 0;
		break;
	}
}

void handleFile()
{
	String n = server.uri();
	if (n == "/") n = "/index.html";
	
	File f = handleFileRead(n);
	if (f)
	{	
		String contentType = getContentType(n);
		server.streamFile(f, contentType);
	}
	else
	{
		server.send(404, "text/plain", "404: Not Found: " + n);	
	}
}

void handleSet()
{
	int wart = server.arg("tryb").toInt();
	ustawtryb(wart);

	String str="{\"hue\" : \"" + String(par.hue) +
           "\",\"alpha\":\"" + String(par.alpha) +
           "\",\"hue2\":\""  + String(par.hue2) + 
           "\",\"alfa2\":\"" + String(par.alpha2) + 
           "\",\"s_val\":\"" + String(par.s_val) + 
           "\",\"i_val\":\"" + String(par.i_val) +
		   "\",\"r\":\"" 	+ String(par.r) + 
           "\",\"g\":\"" 	+ String(par.g) + 
           "\",\"b\":\"" 	+ String(par.b) +
           "\",\"tryb\":\"" + String(tryb) + "\"}";
  	server.send(200, "text/json", str );
}


void handleRoot() 
{
	int wart = server.arg("tryb").toInt();
	ustawtryb(wart);
	handleFile();
}

void handleUpdate()
{
	HTTPUpload& upload = server.upload();
	if (upload.status == UPLOAD_FILE_START) 
	{
		Serial.setDebugOutput(true);
		WiFiUDP::stopAll();
		Serial.printf("Update: %s\n", upload.filename.c_str());
		uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
		if (!Update.begin(maxSketchSpace)) 
		{ 
			Update.printError(Serial);
		}
	} 
	else if (upload.status == UPLOAD_FILE_WRITE) 
	{
		if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) 
		{
			Update.printError(Serial);
		}
	} 
	else if (upload.status == UPLOAD_FILE_END) 
	{
		if (Update.end(true)) 
		{
			Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
		} 
		else 
		{
			Update.printError(Serial);
		}
		Serial.setDebugOutput(false);
	}
}

void handleUpdateRes()
{
	server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
}

void przycisk()
{
	if(par.alpha == 0)
	{
		par.alpha = 100;
	}
	else
	{
		par.alpha = 0;
	}

	delay(500);
	if (digitalRead(2) == LOW) 
	{
		par.alpha = 0;
		while (digitalRead(2) == LOW)
		{
			currentMillis = millis();  
			if (currentMillis - startMillis >= period)  
			{
				par.alpha += 1;
				startMillis = currentMillis; 
				setKolor(255,255,230);
			}
		}
	}
	setKolor(255,255,240);
	delay(500);
}


void setup (void) 
{
	Serial.begin (115200);
	
	WiFi.mode(WIFI_STA);   
	WiFi.disconnect();
	delay(500);
	
	SPIFFS.begin(); 

	String nazwa = konfig.czytaj(0);  
	String ssid = konfig.czytaj(1);
	String password = konfig.czytaj(2);

	Serial.println(nazwa);
	Serial.println(ssid);
	Serial.println(password);

	if(ssid != "")
	{
		WiFi.hostname(nazwa);
		WiFi.begin(ssid, password);
		int i = 0;
		while (WiFi.status() != WL_CONNECTED) 
		{
			delay(1000);
			if(i > 9) break;
			i++;
		}
	}

	if (WiFi.status() != WL_CONNECTED)
	{
		WiFi.mode(WIFI_AP);
		
		delay(100);
		IPAddress ip(192,168,99,1);
		IPAddress brama(192,168,99,1);
		IPAddress maska(255,255,255,0);
		
		WiFi.softAP(nazwa);
		WiFi.softAPConfig(ip, brama, maska);
		
	}
  
	server.on("/", handleRoot);
	server.on("/wifi", WIFIsetup);	
	server.on("/update", HTTP_POST, handleUpdateRes, handleUpdate);
	server.on("/set", HTTP_POST, handleSet);
	server.onNotFound(handleFile);
	server.begin();
	
	IPAddress ip;
	ip = WiFi.localIP();
	Serial.println(ip);

	//init params
	par.hue = 512;
	par.alpha = 0;
	par.hue2 = 512;
	par.alpha2 = 0;
	par.s_val = 10;
	par.i_val = 10;
	par.r = 254;
	par.g = 254;
	par.b = 240;

	pixels.begin();

	ArduinoOTA.setHostname("Lampak1");
	ArduinoOTA.begin();
	pinMode(2, INPUT); 


}

void loop ( void ) 
{
  	server.handleClient();
  	ArduinoOTA.handle();

  	if (digitalRead(2) == LOW) 
  	{
   		przycisk();
  	}
  
  	if (tryb == 1)
  	{
    	karuzela();
    	if (timer > 30 - par.s_val)
    	{
      		ledid++;
      		if ((ledid - par.i_val) > NUMPIXELS) ledid = 0;
      		timer = 0;
    	}
    	timer++;
  	}
  	else if (tryb == 2)
  	{
    	if (timer > (30 - par.s_val)*40)
    	{
      		byte *c;
      		for (uint16_t i = 0; i < NUMPIXELS; i++)
      		{ 
        		c = Wheel(((i * 256 / NUMPIXELS)+ledid) & 255);
        		setLed(i, *c, *(c + 1), *(c + 2), par.alpha);
      		}
      		pixels.show();
      		ledid++;
      		if (ledid >= 256 * 10) ledid = 0;
      		timer = 0;
    	}
    	timer++;
  	}
  	else if (tryb == 3)
  	{
    	if (timer > (30 - par.s_val) * 100)
    	{
      		byte *c;
     		for (uint16_t i = 0; i < NUMPIXELS; i++)
      		{
        		c = Wheel(((NUMPIXELS * 256 / NUMPIXELS)+ledid) & 255);
        		setLed(i, *c, *(c + 1), *(c + 2), par.alpha);
      		}
      		pixels.show();
			ledid++;
			if (ledid >= 256 * 10) ledid = 0;
			timer = 0;
    	}
    	timer++;
  	}
	else if (tryb == 4)
	{
		if (timer > (30 - par.s_val)*40)
		{
			byte *c;
			for (uint16_t i = 0; i < NUMPIXELS; i++)
			{ 
				c = Wheel(((i * 256 / NUMPIXELS)+ledid) & 255);
				setLed(i, *c, *c, *c, par.alpha); 
			}
			pixels.show();
			ledid++;
			if (ledid >= 256 * 10) ledid = 0;
			timer = 0;
		}
		timer++;
	}
}


void setKolor2(int r, int g, int b)
{
	for (int i = 42; i < 53; i++)
	{
		setLed(i, r, g, b, par.alpha);
		setLed( 62 - (i-42) , r, g, b, par.alpha);
		pixels.show();
		delay(750);
	}
	
	for (int i = 53; i >= 42; i--)
	{
		setLed(i, r, g, b, 0);
		
		//setLed( (1+i)  , r, g, b, 0);
		
		setLed( (53*2 - i), r, g, b, 0);

		pixels.show();
		delay(750);
	}	
	pixels.show();
	tryb = 0;
}


void setKolor(int r, int g, int b)
{
	for (int i = 0; i < NUMPIXELS; i++)
	{
		setLed(i, r, g, b, par.alpha);
	}
	pixels.show();
	tryb = 0;
}

void karuzela()
{
	for (int i = 0; i < NUMPIXELS; i++)
	{
		if (i == ledid)
		{
			int v = par.hue2;
			for (int q = 0; q <= par.i_val; q++) setLed(i - q, colVal(v+341), colVal(v), colVal(v+682), par.alpha2);
		}
		else
		{
			int v = par.hue;
			setLed(i, colVal(v+341), colVal(v), colVal(v+682), par.alpha);
		}
	}  
	pixels.show();
}

void setLed(int i, int r, int g, int b, int a)
{
	//if((i < 30) & (a > 8))a-=8;//kompems
	pixels.setPixelColor(i, pixels.Color(r*a/100, g*a/100, b*a/100));
}

byte * Wheel(byte WheelPos) 
{
	static byte c[3];
	if (WheelPos < 85) 
	{
		c[0] = WheelPos * 3;
		c[1] = 255 - WheelPos * 3;
		c[2] = 0;
	} 
	else if (WheelPos < 170) 
	{
		WheelPos -= 85;
		c[0] = 255 - WheelPos * 3;
		c[1] = 0;
		c[2] = WheelPos * 3;
	} 
	else 
	{
		WheelPos -= 170;
		c[0] = 0;
		c[1] = WheelPos * 3;
		c[2] = 255 - WheelPos * 3;
	}
  	return c;
}

byte colVal(int val)
{
	int v = val % 1024;
	if(v<=170) return (v * 3) / 2;
	if(v<=512) return 255;
	if(v<=682) return 255 - (((v%171) * 3) / 2);
	return 0;
}
