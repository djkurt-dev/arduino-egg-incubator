#include <LiquidCrystal.h>
#include <Servo.h>
#include <DHT.h>;

#define DHTPIN 11
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5 

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
Servo myservo;

int RelayBulb = 1;
int RelayFan = 2;
int lcd_key = 0;
int adc_key_in = 0;
int pos = 0;
float minTemp = 37.00;
float maxTemp = 37.80;
float minHumidity=55.00;
float maxHumidity=60.00;
int minTempReached = 0;
int maxHumReached = 0;
int ventOpen = 1;
int count = 0;
int chk;
float hum, temp, uptime;
char ch, unit;

int read_LCD_buttons()
{
  adc_key_in = analogRead(0);
  if (adc_key_in > 1500) return btnNONE;
  if (adc_key_in < 50) return btnRIGHT;
  if (adc_key_in < 195) return btnUP;
  if (adc_key_in < 380) return btnDOWN;
  if (adc_key_in < 500) return btnLEFT;
  if (adc_key_in < 700) return btnSELECT;
  return btnNONE;
}


void setup() {
	pinMode(RelayBulb, OUTPUT);
  pinMode(RelayFan, OUTPUT);
  myservo.attach(3);
  dht.begin();
  
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Egg Incubator");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("by: Wino");
  delay(2000);
  lcd.clear();

  lcd.setCursor(3, 0);
  lcd.print("ALL RIGHTS");
  delay(1000);
  lcd.setCursor(4, 1);
  lcd.print("RESERVED");
  delay(2000);
  lcd.clear();
  
  for (pos = 180; pos >= 0; pos -= 1) { 
      if(pos == 0){ // done closing
        ventOpen = 0; // closed 
        break;
      }
      
      myservo.write(pos);              
      delay(10);                       
    }
}



void loop() {
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  unsigned long t1 = millis();

  lcd.setCursor(0, 0);
  lcd.print("Temp:  ");
  lcd.print(temp);
  lcd.setCursor(0, 1);
  lcd.print("Hum:  ");
  lcd.print(hum);

  if(temp <= minTemp){
    minTempReached = 1;
  }
  if(temp >= maxTemp) {
    minTempReached = 0;
  }

  if(minTempReached == 1){
    digitalWrite(RelayBulb, LOW);     
  } else {
    digitalWrite(RelayBulb, HIGH);   
  }

  if(hum <= minHumidity){
    maxHumReached = 0;

    if(ventOpen == 1){
      for (pos = 180; pos >= 0; pos -= 1) { 
    
        if(pos == 0){ // done closing
          ventOpen = 0; // closed 
          break;
        }
        
        myservo.write(pos);              
        delay(10);                       
      }      
    }
    
  } 
  
  if(hum >= maxHumidity){
    maxHumReached = 1;
    
    if(ventOpen == 0){
      for (pos = 0; pos <= 180; pos += 1) { 
        if(pos == 180){ // done opening
          ventOpen = 1; // open 
          break;
        }
        
        myservo.write(pos);              
        delay(10);                        
      }      
    }
    
  } 

  uptime = t1 * 1e-3;
  unit = 's';
  if (uptime > 60) {
    uptime /= 60;
    unit = 'm';
    if (uptime > 60) {
      uptime /= 60;
      unit = 'h';
      if (uptime > 24) {
        uptime /= 24;
        unit = 'd';
      }
    }
  }

  lcd_key = read_LCD_buttons();

  switch(lcd_key)
  {
    case btnRIGHT:{
      break;
    }
    case btnLEFT:{
      lcd.clear();
      lcd.print("Uptime:  ");
      lcd.print(uptime);
      lcd.print(unit);
      break;
    }
    case btnSELECT:{
      setup();
    }
  }
}