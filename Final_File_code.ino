//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Arduino TFT Touchscreen DHT11
//----------------------------------------Include Library
#include <SPFD5408_Adafruit_GFX.h>    //--> Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> //--> Hardware-specific library
#include <Time.h>
#include <TimeLib.h>
#include "DHT.h" //--> DHT library


//----------------------------------------

//----------------------------------------
#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

//----------------------------------------Pin configuration and initialization for LCD TFT
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
//----------------------------------------

//----------------------------------------Defines colors
// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define ORANGE  0xFD20
#define DARKORANGE  0xFB60
#define MAROON  0x7800
#define GRAY    0x0080
//----------------------------------------

//----------------------------------------DHT11 configuration and data variables
#define DHTPIN A5     
#define DHTTYPE DHT11   
DHT dht11(DHTPIN, DHTTYPE);

int h;
float t;
float f;
float hif;
float hic;
//----------------------------------------

//----------------------------------------The x and y points for the Temperature bar
int x_bar_t = 20;
int y_bar_t = 60;
//----------------------------------------

//----------------------------------------The variable to hold the conversion value from the temperature value to the value for the temperature bar
int T_to_Bar;
//----------------------------------------

//----------------------------------------Millis variable to update the temperature and humidity values
unsigned long previousMillis = 0; //--> will store last time updated
// constants won't change:
const long interval = 2000; //--> interval 
//----------------------------------------

//========================================================================VOID SETUP()
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  tft.reset();

  tft.begin(0x9341); //--> SPFD5408/ILI9341

  tft.setRotation(135); //--> Need for the Mega, please changed for your choice or rotation initial

  tft.fillScreen(GRAY); //--> TFT LCD background color

  dht11.begin();

  //---------------------------------------Time

 setTime(18,02,00,26,6,21);

  
  //----------------------------------------Initial view  
  tft.drawLine(15, 30, 300, 30, MAGENTA); //--> tft.drawLine(x, y, x size, y size, MAROON)
  tft.drawLine(15, 29, 300, 29, MAGENTA);

  tft.setTextSize(2);
  tft.setTextColor(YELLOW);
  tft.setCursor(25, 10); //--> tft.setCursor(x, y)
  tft.print("Temperature & Humidity");
  
  draw_bar(x_bar_t, y_bar_t);

  tft.drawLine(180, 60, 180, 227, MAGENTA);
  tft.drawLine(190, 130, 300, 130, MAGENTA);

  tft.fillRect(202, 60, 100, 27, GRAY);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(205, 60);
  tft.print("Humidity");

  tft.fillRect(202, 140, 100, 43, GRAY);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(195, 145);
  tft.print("Heat Index");
  GetDHT11Data();
  delay(1000);
  //----------------------------------------
}
//========================================================================

//========================================================================VOID LOOP()
void loop() {
  // put your main code here, to run repeatedly:

  //----------------------------------------Millis to update the temperature and humidity values
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    
    GetDHT11Data();
  }
  //----------------------------------------
  //-------------------------------------------Time
 
  digitalClockDisplay();

  
  //----------------------------------------Temperature bar
  T_to_Bar = map(t/2, 0.0, 50.0, 108, 0);

  tft.fillRect(x_bar_t+8, (y_bar_t+8)+T_to_Bar, 19, 108-T_to_Bar, DARKORANGE);
  tft.fillRect(x_bar_t+8, y_bar_t+8, 19, T_to_Bar, WHITE);
  //----------------------------------------

  tft.setTextSize(2);
  tft.setTextColor(ORANGE, GRAY);
  tft.setCursor(75, 85);
  tft.print(t);
  if (t < 10) tft.print(" ");
  tft.setCursor(145, 85);
  tft.print((char)247);
  tft.println("C");

  
  tft.setCursor(75, 120);
  tft.print(f);
  if (f < 100) tft.print(" ");
  tft.setCursor(145, 120);
  tft.print((char)247);
  tft.println("F");

  tft.setTextSize(3);
  tft.setTextColor(CYAN, GRAY);
  tft.setCursor(210, 95);
  tft.print(h);
  tft.print(" %");

  tft.setTextSize(2);
  tft.setTextColor(GREEN, GRAY);
  tft.setCursor(205, 180);
  tft.print(hic);
  tft.print(" ");
  tft.print((char)247);
  tft.print("C");
  if (hic < 10) tft.print(" ");

  tft.setTextSize(2);
  tft.setTextColor(GREEN, GRAY);
  tft.setCursor(205, 215);
  tft.print(hif);
  tft.print(" ");
  tft.print((char)247);
  tft.print("F");
  if (hif < 100) tft.print(" "); 

 
}
//========================================================================

//========================================================================GetDHT11Data()
void GetDHT11Data() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht11.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht11.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  f = dht11.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  hif = dht11.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  hic = dht11.computeHeatIndex(t, h, false);
}
//========================================================================

//========================================================================draw_bar(x, y)
void draw_bar(int x_bar, int y_bar) {
  tft.fillRoundRect(x_bar, y_bar, 35, 120, 5, BLACK);
  tft.fillCircle(x_bar+17, y_bar+140, 30, BLACK);
  tft.fillRoundRect(x_bar+4, y_bar+4, 27, 120, 2, WHITE);
  tft.fillCircle(x_bar+17, y_bar+140, 25, WHITE);
  tft.fillRect(x_bar+8, y_bar+8, 19, 120, DARKORANGE);
  tft.fillCircle(x_bar+17, y_bar+140, 21, ORANGE);

  //tft.fillRect(41, 58, 19, 108, RED);
 
  tft.setTextSize(1);
  tft.setTextColor(RED);
  tft.setCursor(x_bar-18, y_bar+4);
  tft.println("100");
  
  tft.drawLine(x_bar-3, y_bar+115, x_bar, y_bar+115, RED);
  tft.setCursor(x_bar-10, y_bar+111);
  tft.println("0");
}
//========================================================================


//==========================================================================TIME

void digitalClockDisplay()
{
  tft.setTextSize(1);

  tft.setTextColor(GREEN, GRAY);
  tft.setCursor(75, 38);
  tft.print("Time : ");
  tft.setTextColor(CYAN, GRAY);
  tft.print(hour());
  printDigits(minute());
  printDigits(second());
  tft.setCursor(75, 55);
  tft.setTextColor(GREEN, GRAY);
  tft.print("Date : ");
  tft.setTextColor(CYAN, GRAY);
  tft.print(day());
  tft.print("/");
  tft.print(month());
  tft.print("/");
  tft.print(year());
  tft.println();  
}
void printDigits(int digits)
{
  tft.print(":");
  if(digits < 10)
    tft.print('0');
  tft.print(digits);  
}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
