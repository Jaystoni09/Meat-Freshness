#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "RTClib.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);
#define MQ137_PIN 35
#define MQ136_PIN 34
const int sensorPin = 32;  // Analog pin connected to MQ137 AOUT
#define RL_VALUE1 47   // Load resistance (47KΩ)
#define RL_VALUE2 47
const float RL_VALUE3 = 6000.0;
// Use the Ro value from your calibration run
#define RO_VALUE1 72   // **IMPORTANT: Replace this with your specific Ro value**
#define RO_VALUE2 72

// Logarithmic curve coefficients for MQ137 based on typical datasheet plots
#define M_VALUE1 -0.243
#define B_VALUE1 0.323
#define M_VALUE2 -0.243
#define B_VALUE2 0.323
const float GAS_PPM_ALPHA = 1000; // Example Alpha value (adjust based on datasheet)
const float GAS_PPM_BETA = -0.6; 
#define led1 12
#define led2 13
#define led3 14
#define buzzer 2
#define SD_CS_PIN 5
const char btn1 = 25;
const char btn2 = 26;
const char btn3 = 27;

bool pressed = false;
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS3231 rtc;
File dataFile;

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
lcd.init();
lcd.backlight();
// Initialize Serial communication and check for SD card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD Card initialization failed!");
    while (1); // Stop here if SD card initialization fails
  }
  Serial.println("SD Card initialized.");

  lcd.setCursor(1, 0);
  lcd.print("System Loading");
   for (int a = 0; a <= 15; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(500);
  }
  lcd.clear();
    lcd.print("Select Meat Test");
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }
Serial.begin(115200);
if(!rtc.begin()) {
  Serial.print("Couldn't find RTC");
  while(1);
}
if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set, uncomment the following line and upload
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Sets the RTC to the current compile time
    // Or manually set the time, for example:
    // rtc.adjust(DateTime(2025, 10, 19, 9, 30, 0)); // Year, Month, Day, Hour, Minute, Second
  }

pinMode(buzzer, OUTPUT);
pinMode(btn1,INPUT_PULLUP);
pinMode(btn2,INPUT_PULLUP);
pinMode(btn3,INPUT_PULLUP);
pinMode(led3,OUTPUT);
pinMode(led2,OUTPUT);
pinMode(led1,OUTPUT);
pinMode(sensorPin, INPUT);


String dataString =String("   Date  ") + "," + String("   Time   " )+"," +String("MQ136 ")+","+String("   MQ137 ")+","+ String("   Color Temp ")+","+String("   Lux  ") + "," + String("   R   " )+ ","+String("  G ")+","+String("   B ")+","+ String("   C ")+","+String("  Status ")+","+ String("  Meat ")+"\r\n" /*String(now_00.year()) + "-" + String(now_00.month()) + "-" + String(now_00.day()) 
                      +","+ String(now_00.hour()) + ":" + String(now_00.minute()) + ":" + String(now_00.second())   +","+ String(ppm1) 
                       +","+ String(ppm2) + "," + String(colorTemp)+","+ String(lux)+"," + String(r)+","+String(g)+","+ String(b)+","+ String(c)+","+ String("Spoiled")+","+String("CHICKEN MEAT")*/;
  
                      dataFile = SD.open("/data.csv", FILE_APPEND);
                      if (dataFile) {
    dataFile.println(dataString); // Write the data to the file
    dataFile.close(); // Close the file
    Serial.println("Data saved to SD card: " + dataString);
    Serial.print("Data has Save");
    }else {
    Serial.println("Error opening Sample.txt for writing");
  }
                      }
void loop() {
  // put your main code here, to run repeatedly:
float sensor_volt1;
  float RS1;
  float ratio1;
  float ppm1;
  float sensor_value1 = 0;

  float sensor_volt2;
  float RS2;
  float ratio2;
  float ppm2;
  float sensor_value2 = 0;
  
for (int x = 0; x < 100; x++) {
    sensor_value1 = sensor_value1 + analogRead(MQ136_PIN);
    sensor_value2 = sensor_value2 + analogRead(MQ137_PIN);
    

  }
  sensor_value1 = sensor_value1 / 100.0;
  // Convert analog reading to voltage
  sensor_volt1 = sensor_value1 * (3.3 / 4095.0);
  // Calculate the sensor resistance (Rs)
  RS1 = ((3.3 * RL_VALUE1) / sensor_volt1) - RL_VALUE1;
  // Calculate the ratio (Rs/Ro)
  ratio1 = RS1 / RO_VALUE1;
  // Use the logarithmic formula to get PPM
  ppm1 = pow(10, ((log10(ratio1) - B_VALUE1) / M_VALUE1));

  sensor_value2 = sensor_value2 / 100.0;
  // Convert analog reading to voltage
  sensor_volt2 = sensor_value2 * (3.3 / 4095.0);
  // Calculate the sensor resistance (Rs)
  RS2 = ((3.3 * RL_VALUE2) / sensor_volt2) - RL_VALUE2;
  // Calculate the ratio (Rs/Ro)
  ratio2 = RS2 / RO_VALUE2;
  // Use the logarithmic formula to get PPM
  ppm2 = pow(10, ((log10(ratio2) - B_VALUE2) / M_VALUE2));


  DateTime now_00=rtc.now();
  uint16_t r, g, b, c, colorTemp, lux;
  tcs.getRawData(&r, &g, &b, &c);
  colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);

  Serial.println(ppm1);
  Serial.println(ppm2);
  Serial.println(colorTemp);
  Serial.println(lux);
  Serial.println(r);
  Serial.println(g);
  Serial.println(b);
  Serial.println(c);
  
bool currentState1 = digitalRead(btn2);
bool currentState2 = digitalRead(btn3);
bool currentState3 = digitalRead(btn4);
\
  
  
if(currentState1 == pressed){

  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("  PORK  MEAT");
  delay(3000);
  
  lcd.clear();
  lcd.print("   PROCESSING");
   for (int b = 0; b <= 15; b++) {
    lcd.setCursor(b, 1);
    lcd.print(".");
    delay(1000);
   
  }
  for (int x = 0; x < 100; x++) {
    sensor_value1 = sensor_value1 + analogRead(MQ136_PIN);
    sensor_value2 = sensor_value2 + analogRead(MQ137_PIN);
    

  }
  sensor_value1 = sensor_value1 / 100.0;
  // Convert analog reading to voltage
  sensor_volt1 = sensor_value1 * (3.3 / 4095.0);
  // Calculate the sensor resistance (Rs)
  RS1 = ((3.3 * RL_VALUE1) / sensor_volt1) - RL_VALUE1;
  // Calculate the ratio (Rs/Ro)
  ratio1 = RS1 / RO_VALUE1;
  // Use the logarithmic formula to get PPM
  ppm1 = pow(10, ((log10(ratio1) - B_VALUE1) / M_VALUE1));

  sensor_value2 = sensor_value2 / 100.0;
  // Convert analog reading to voltage
  sensor_volt2 = sensor_value2 * (3.3 / 4095.0);
  // Calculate the sensor resistance (Rs)
  RS2 = ((3.3 * RL_VALUE2) / sensor_volt2) - RL_VALUE2;
  // Calculate the ratio (Rs/Ro)
  ratio2 = RS2 / RO_VALUE2;
  // Use the logarithmic formula to get PPM
  ppm2 = pow(10, ((log10(ratio2) - B_VALUE2) / M_VALUE2));


  DateTime now_00=rtc.now();
  uint16_t r, g, b, c, colorTemp, lux;
  tcs.getRawData(&r, &g, &b, &c);
  colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);
  // ppm1 stands for MQ136 and ppm2 stands for MQ137
  if (ppm1 >= 3 && ppm2 >= 13 && colorTemp >=7000) //Set the threshold range for the Spoiled Pork Meat 
  {
    digitalWrite(buzzer, HIGH);
digitalWrite(led2,HIGH);
lcd.clear();
lcd.print("    Spoiled   ");
delay(4000);
Serial.println("Spoiled");
lcd.clear();
digitalWrite(buzzer, LOW);
digitalWrite(led2,LOW);
Serial.println("MQ136 :");
Serial.println(ppm1);
lcd.setCursor(0, 0);
lcd.print("MQ136:");
lcd.print(ppm1);
Serial.println("MQ137 :");
Serial.println(ppm2);
lcd.setCursor(0, 1);
lcd.print("MQ137:");
lcd.print(ppm2);
delay(2500);
lcd.clear();
lcd.print("ColorTemp: ");lcd.print(colorTemp,DEC);lcd.print("K  ");
lcd.setCursor(0,1);
lcd.print("L: ");lcd.print(lux,DEC);lcd.print(" ");
lcd.print("R: ");lcd.print(r,DEC);lcd.print(" ");
delay(2500);
lcd.clear();
lcd.print("G: ");lcd.print(g,DEC);lcd.print(" ");
lcd.print("B: ");lcd.print(b,DEC);
lcd.setCursor(0,1);
lcd.print("C: ");lcd.print(c,DEC);lcd.print(" ");
delay(2500);
lcd.clear();   
lcd.print("Date ");
lcd.print(now_00.year(),DEC);
lcd.print("-");
lcd.print(now_00.month(),DEC);
lcd.print("-");
lcd.print(now_00.day(),DEC);
lcd.setCursor(0,1);
lcd.print("Time ");
lcd.print(now_00.hour(),DEC);
lcd.print(":");
lcd.print(now_00.minute(),DEC);
lcd.print(":");
lcd.print(now_00.second(),DEC);
delay(2500);

    String dataString =/*String("Date  ") + "," + String("   Time   " )+ String("MQ136: ")+","+String("   MQ137: ")+","+ String("   TGS: ")+"\r\n"+ */String(now_00.year()) + "-" + String(now_00.month()) + "-" + String(now_00.day()) 
                      +","+ String(now_00.hour()) + ":" + String(now_00.minute()) + ":" + String(now_00.second())   +","+ String(ppm1) 
                       +","+ String(ppm2) + "," + String(colorTemp)+","+ String(lux)+"," + String(r)+","+String(g)+","+ String(b)+","+ String(c)+","+ String("Spoiled")+","+String("PORK MEAT");
                      dataFile = SD.open("/data.csv", FILE_APPEND);
                      if (dataFile) {
    dataFile.println(dataString); // Write the data to the file
    dataFile.close(); // Close the file
    Serial.println("Data saved to SD card: " + dataString);
    Serial.print("Data has Save");
    lcd.clear();
    lcd.print("Data has saved");
    delay(3000);
    lcd.clear();
    lcd.print("Select Meat Test");
  } else {
    Serial.println("Error opening Sample.txt for writing");
  }
  }
   // ppm1 stands for MQ136 and ppm2 stands for MQ137
  else if (ppm1 >= 2 && ppm2 >= 10 && colorTemp >=6700 ) //Set the threshold range for the not fresh Pork Meat
  {
    digitalWrite(buzzer, HIGH);
    digitalWrite(led3,HIGH);
    lcd.clear();
    lcd.print("   Not Fresh");
    delay(4000);
    Serial.println("Not Fresh");
    lcd.clear();
     digitalWrite(buzzer, LOW);
    digitalWrite(led3,LOW);
  Serial.println("MQ136 :");
  Serial.println(ppm1);
  lcd.setCursor(0, 0);
  lcd.print("MQ136:");
  lcd.print(ppm1);
  Serial.println("MQ137 :");
  Serial.println(ppm2);
  lcd.setCursor(0, 1);
  lcd.print("MQ137:");
  lcd.print(ppm2);
  delay(2500);
  lcd.clear();

  lcd.print("ColorTemp: ");lcd.print(colorTemp,DEC);lcd.print("K  ");
  lcd.setCursor(0,1);
  lcd.print("L: ");lcd.print(lux,DEC);lcd.print(" ");
  lcd.print("R: ");lcd.print(r,DEC);lcd.print(" ");
    delay(2500);
  lcd.clear();

  lcd.print("G: ");lcd.print(g,DEC);lcd.print(" ");
  lcd.print("B: ");lcd.print(b,DEC);
  lcd.setCursor(0,1);
  lcd.print("C: ");lcd.print(c,DEC);lcd.print(" ");

  delay(2500);
  lcd.clear();
    
lcd.print("Date ");
lcd.print(now_00.year(),DEC);
lcd.print("-");
lcd.print(now_00.month(),DEC);
lcd.print("-");
lcd.print(now_00.day(),DEC);
lcd.setCursor(0,1);
lcd.print("Time ");
lcd.print(now_00.hour(),DEC);
lcd.print(":");
lcd.print(now_00.minute(),DEC);
lcd.print(":");
lcd.print(now_00.second(),DEC);
delay(2500);

    String dataString =/*String("Date  ") + "," + String("   Time   " )+ String("MQ136: ")+","+String("   MQ137: ")+","+ String("   TGS: ")+"\r\n"+ */String(now_00.year()) + "-" + String(now_00.month()) + "-" + String(now_00.day()) 
                      +","+ String(now_00.hour()) + ":" + String(now_00.minute()) + ":" + String(now_00.second())   +","+ String(ppm1) 
                       +","+ String(ppm2) + "," + String(colorTemp)+","+ String(lux)+"," + String(r)+","+String(g)+","+ String(b)+","+ String(c)+","+ String("Not Fresh")+","+String("PORK MEAT");
                      dataFile = SD.open("/data.csv", FILE_APPEND);
                      if (dataFile) {
    dataFile.println(dataString); // Write the data to the file
    dataFile.close(); // Close the file
    Serial.println("Data saved to SD card: " + dataString);
    Serial.print("Data has Save");
    lcd.clear();
    lcd.print("Data has saved");
    delay(3000);
    lcd.clear();
    lcd.print("Select Meat Test");
  } else {
    Serial.println("Error opening Sample.txt for writing");
  }
  }
   else {
    digitalWrite(led1,HIGH);
    lcd.clear();
    lcd.print("     Fresh   ");
    delay(4000);
    Serial.println("     Fresh");
    digitalWrite(led1, LOW);
    digitalWrite(buzzer, LOW);
    lcd.clear();
  Serial.println("MQ136 :");
  Serial.println(ppm1);
  lcd.setCursor(0, 0);
  lcd.print("MQ136:");
  lcd.print(ppm1);
  Serial.println("MQ137 :");
  Serial.println(ppm2);
  lcd.setCursor(0, 1);
  lcd.print("MQ137:");
  lcd.print(ppm2);
  delay(2500);
  lcd.clear();
  
  lcd.print("ColorTemp: ");lcd.print(colorTemp,DEC);lcd.print("K  ");
  lcd.setCursor(0,1);
  lcd.print("L: ");lcd.print(lux,DEC);lcd.print(" ");
  lcd.print("R: ");lcd.print(r,DEC);lcd.print(" ");
    delay(2500);
  lcd.clear();

  lcd.print("G: ");lcd.print(g,DEC);lcd.print(" ");
  lcd.print("B: ");lcd.print(b,DEC);
  lcd.setCursor(0,1);
  lcd.print("C: ");lcd.print(c,DEC);lcd.print(" ");
  delay(2500);
  
    lcd.clear();
lcd.print("Date ");
lcd.print(now_00.year(),DEC);
lcd.print("-");
lcd.print(now_00.month(),DEC);
lcd.print("-");
lcd.print(now_00.day(),DEC);
lcd.setCursor(0,1);
lcd.print("Time ");
lcd.print(now_00.hour(),DEC);
lcd.print(":");
lcd.print(now_00.minute(),DEC);
lcd.print(":");
lcd.print(now_00.second(),DEC);
delay(2500);

  

    String dataString =/*String("Date  ") + "," + String("   Time   " )+ String("MQ136: ")+","+String("   MQ137: ")+","+ String("   TGS: ")+"\r\n"+ */String(now_00.year()) + "-" + String(now_00.month()) + "-" + String(now_00.day()) 
                      +","+ String(now_00.hour()) + ":" + String(now_00.minute()) + ":" + String(now_00.second())   +","+ String(ppm1) 
                       +","+ String(ppm2) + "," + String(colorTemp)+","+ String(lux)+"," + String(r)+","+String(g)+","+ String(b)+","+ String(c)+","+String("Fresh")+","+String("PORK MEAT");
                      dataFile = SD.open("/data.csv", FILE_APPEND);
                      if (dataFile) {
    dataFile.println(dataString); // Write the data to the file
    dataFile.close(); // Close the file
    Serial.println("Data saved to SD card: " + dataString);
    Serial.print("Data has Save");
    lcd.clear();
    lcd.print("Data has saved");
    delay(3000);
    lcd.clear();
    lcd.print("Select Meat Test");
  } else {
    Serial.println("Error opening Sample.txt for writing");
  }
  }
  Serial.print("ColorTemp: "); Serial.print(colorTemp, DEC); Serial.print("K");
  Serial.print("Lux: "); Serial.print(lux, DEC); 
  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
  Serial.println(" ");
  
  
   
  
}
if(currentState2 == pressed){
    
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print(" CHICKEN  MEAT");
  delay(3000);
  
  lcd.clear();
  lcd.print("   PROCESSING");
   for (int b = 0; b <= 15; b++) {
    lcd.setCursor(b, 1);
    lcd.print(".");
    delay(2000);
   
  }
  for (int x = 0; x < 100; x++) {
    sensor_value1 = sensor_value1 + analogRead(MQ136_PIN);
    sensor_value2 = sensor_value2 + analogRead(MQ137_PIN);
  

  }
  sensor_value1 = sensor_value1 / 100.0;
  // Convert analog reading to voltage
  sensor_volt1 = sensor_value1 * (3.3 / 4095.0);
  // Calculate the sensor resistance (Rs)
  RS1 = ((3.3 * RL_VALUE1) / sensor_volt1) - RL_VALUE1;
  // Calculate the ratio (Rs/Ro)
  ratio1 = RS1 / RO_VALUE1;
  // Use the logarithmic formula to get PPM
  ppm1 = pow(10, ((log10(ratio1) - B_VALUE1) / M_VALUE1));

  sensor_value2 = sensor_value2 / 100.0;
  // Convert analog reading to voltage
  sensor_volt2 = sensor_value2 * (3.3 / 4095.0);
  // Calculate the sensor resistance (Rs)
  RS2 = ((3.3 * RL_VALUE2) / sensor_volt2) - RL_VALUE2;
  // Calculate the ratio (Rs/Ro)
  ratio2 = RS2 / RO_VALUE2;
  // Use the logarithmic formula to get PPM
  ppm2 = pow(10, ((log10(ratio2) - B_VALUE2) / M_VALUE2));

  DateTime now_00=rtc.now();
  uint16_t r, g, b, c, colorTemp, lux;
  tcs.getRawData(&r, &g, &b, &c);
  colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);
   // ppm1 stands for MQ136 and ppm2 stands for MQ137
  if (ppm1 >= 2 && ppm2 >= 25 && colorTemp >=6800) //set the threshold range value of Spoiled Chicken Meat
   {
    digitalWrite(buzzer, HIGH);
    digitalWrite(led2,HIGH);
    lcd.clear();
    lcd.print("    Spoiled   ");
    delay(4000);
    Serial.println("Spoiled");
    digitalWrite(buzzer, LOW);
    digitalWrite(led2,LOW);
    lcd.clear();
  Serial.println("MQ136 :");
  Serial.println(ppm1);
  lcd.setCursor(0, 0);
  lcd.print("MQ136:");
  lcd.print(ppm1);
  Serial.println("MQ137 :");
  Serial.println(ppm2);
  lcd.setCursor(0, 1);
  lcd.print("MQ137:");
  lcd.print(ppm2);
  delay(2500);
  lcd.clear();

  lcd.print("ColorTemp: ");lcd.print(colorTemp,DEC);lcd.print("K  ");
  lcd.setCursor(0,1);
  lcd.print("L: ");lcd.print(lux,DEC);lcd.print(" ");
  lcd.print("R: ");lcd.print(r,DEC);lcd.print(" ");
    delay(2500);
  lcd.clear();

  lcd.print("G: ");lcd.print(g,DEC);lcd.print(" ");
  lcd.print("B: ");lcd.print(b,DEC);
  lcd.setCursor(0,1);
  lcd.print("C: ");lcd.print(c,DEC);lcd.print(" ");

  delay(2500);
  
    lcd.clear();
lcd.print("Date ");
lcd.print(now_00.year(),DEC);
lcd.print("-");
lcd.print(now_00.month(),DEC);
lcd.print("-");
lcd.print(now_00.day(),DEC);
lcd.setCursor(0,1);
lcd.print("Time ");
lcd.print(now_00.hour(),DEC);
lcd.print(":");
lcd.print(now_00.minute(),DEC);
lcd.print(":");
lcd.print(now_00.second(),DEC);
delay(2500);

    String dataString =/*String("Date  ") + "," + String("   Time   " )+ String("MQ136: ")+","+String("   MQ137: ")+","+ String("   TGS: ")+"\r\n"+ */String(now_00.year()) + "-" + String(now_00.month()) + "-" + String(now_00.day()) 
                      +","+ String(now_00.hour()) + ":" + String(now_00.minute()) + ":" + String(now_00.second())   +","+ String(ppm1) 
                       +","+ String(ppm2) + "," + String(colorTemp)+","+ String(lux)+"," + String(r)+","+String(g)+","+ String(b)+","+ String(c)+","+ String("Spoiled")+","+String("CHICKEN MEAT");
                      dataFile = SD.open("/data.csv", FILE_APPEND);
                      if (dataFile) {
    dataFile.println(dataString); // Write the data to the file
    dataFile.close(); // Close the file
    Serial.println("Data saved to SD card: " + dataString);
    Serial.print("Data has Save");
    lcd.clear();
    lcd.print("Data has saved");
    delay(3000);
    lcd.clear();
    lcd.print("Select Meat Test");
  } else {
    Serial.println("Error opening Sample.txt for writing");
  }
  } 
   // ppm1 stands for MQ136 and ppm2 stands for MQ137
  else if (ppm1 >= 1.5 && ppm2 >= 16 && colorTemp >=6380 ) //set the threshold range value of Not Fresh Chicken Meat
   {
    digitalWrite(buzzer, HIGH);
    digitalWrite(led3,HIGH);
    lcd.clear();
    lcd.print("   Not Fresh   ");
    delay(4000);
    Serial.println("Not Fresh");
    digitalWrite(buzzer, LOW);
    digitalWrite(led3,LOW);
    lcd.clear();
  Serial.println("MQ136 :");
  Serial.println(ppm1);
  lcd.setCursor(0, 0);
  lcd.print("MQ136:");
  lcd.print(ppm1);
  Serial.println("MQ137 :");
  Serial.println(ppm2);
  lcd.setCursor(0, 1);
  lcd.print("MQ137:");
  lcd.print(ppm2);
  delay(2500);
  lcd.clear();

  lcd.print("ColorTemp: ");lcd.print(colorTemp,DEC);lcd.print("K  ");
  lcd.setCursor(0,1);
  lcd.print("L: ");lcd.print(lux,DEC);lcd.print(" ");
  lcd.print("R: ");lcd.print(r,DEC);lcd.print(" ");
    delay(2500);
  lcd.clear();

  lcd.print("G: ");lcd.print(g,DEC);lcd.print(" ");
  lcd.print("B: ");lcd.print(b,DEC);
  lcd.setCursor(0,1);
  lcd.print("C: ");lcd.print(c,DEC);lcd.print(" ");

  delay(2500);
  
    lcd.clear();
lcd.print("Date ");
lcd.print(now_00.year(),DEC);
lcd.print("-");
lcd.print(now_00.month(),DEC);
lcd.print("-");
lcd.print(now_00.day(),DEC);
lcd.setCursor(0,1);
lcd.print("Time ");
lcd.print(now_00.hour(),DEC);
lcd.print(":");
lcd.print(now_00.minute(),DEC);
lcd.print(":");
lcd.print(now_00.second(),DEC);
delay(2500);

    String dataString =/*String("Date  ") + "," + String("   Time   " )+ String("MQ136: ")+","+String("   MQ137: ")+","+ String("   TGS: ")+"\r\n"+ */String(now_00.year()) + "-" + String(now_00.month()) + "-" + String(now_00.day()) 
                      +","+ String(now_00.hour()) + ":" + String(now_00.minute()) + ":" + String(now_00.second())   +","+ String(ppm1) 
                       +","+ String(ppm2) + "," + String(colorTemp)+","+ String(lux)+"," + String(r)+","+String(g)+","+ String(b)+","+ String(c)+","+ String("Not Fresh")+","+String("CHICKEN MEAT");
                      dataFile = SD.open("/data.csv", FILE_APPEND);
                      if (dataFile) {
    dataFile.println(dataString); // Write the data to the file
    dataFile.close(); // Close the file
    Serial.println("Data saved to SD card: " + dataString);
    Serial.print("Data has Save");
    lcd.clear();
    lcd.print("Data has saved");
    delay(3000);
    lcd.clear();
    lcd.print("Select Meat Test");
  } else {
    Serial.println("Error opening Sample.txt for writing");
  }
  } 
  
  else {
    digitalWrite(led1,HIGH);
    lcd.clear();
    lcd.print("     Fresh   ");
    delay(4000);
    Serial.println("     Fresh");
    digitalWrite(led1, LOW);
    digitalWrite(buzzer, LOW);
    lcd.clear();
  Serial.println("MQ136 :");
  Serial.println(ppm1);
  lcd.setCursor(0, 0);
  lcd.print("MQ136:");
  lcd.print(ppm1);
  Serial.println("MQ137 :");
  Serial.println(ppm2);
  lcd.setCursor(0, 1);
  lcd.print("MQ137:");
  lcd.print(ppm2);
  delay(2500);
  lcd.clear();
  
  lcd.print("ColorTemp: ");lcd.print(colorTemp,DEC);lcd.print("K  ");
  lcd.setCursor(0,1);
  lcd.print("L: ");lcd.print(lux,DEC);lcd.print(" ");
  lcd.print("R: ");lcd.print(r,DEC);lcd.print(" ");
    delay(2500);
  lcd.clear();

  lcd.print("G: ");lcd.print(g,DEC);lcd.print(" ");
  lcd.print("B: ");lcd.print(b,DEC);
  lcd.setCursor(0,1);
  lcd.print("C: ");lcd.print(c,DEC);lcd.print(" ");
  delay(2500);
  
    lcd.clear();
lcd.print("Date ");
lcd.print(now_00.year(),DEC);
lcd.print("-");
lcd.print(now_00.month(),DEC);
lcd.print("-");
lcd.print(now_00.day(),DEC);
lcd.setCursor(0,1);
lcd.print("Time ");
lcd.print(now_00.hour(),DEC);
lcd.print(":");
lcd.print(now_00.minute(),DEC);
lcd.print(":");
lcd.print(now_00.second(),DEC);
delay(2500);

  

    String dataString =/*String("Date  ") + "," + String("   Time   " )+ String("MQ136: ")+","+String("   MQ137: ")+","+ String("   TGS: ")+"\r\n"+ */String(now_00.year()) + "-" + String(now_00.month()) + "-" + String(now_00.day()) 
                      +","+ String(now_00.hour()) + ":" + String(now_00.minute()) + ":" + String(now_00.second())   +","+ String(ppm1) 
                       +","+ String(ppm2) + "," + String(colorTemp)+","+ String(lux)+"," + String(r)+","+String(g)+","+ String(b)+","+ String(c)+","+String("Fresh")+","+String("CHICKEN MEAT");
                      dataFile = SD.open("/data.csv", FILE_APPEND);
                      if (dataFile) {
    dataFile.println(dataString); // Write the data to the file
    dataFile.close(); // Close the file
    Serial.println("Data saved to SD card: " + dataString);
    Serial.print("Data has Save");
    lcd.clear();
    lcd.print("Data has saved");
    delay(3000);
    lcd.clear();
    lcd.print("Select Meat Test");
  } else {
    Serial.println("Error opening Sample.txt for writing");
  }
  }
  Serial.print("ColorTemp: "); Serial.print(colorTemp, DEC); Serial.print("K");
  Serial.print("Lux: "); Serial.print(lux, DEC); 
  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
  Serial.println(" ");
  

  


  while(digitalRead(btn1) == pressed){

  }
  while(digitalRead(btn2) == pressed){
  }
}
}
