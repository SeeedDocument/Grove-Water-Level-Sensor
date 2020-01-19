#include <Wire.h>
#include <Arduino.h>
#include <U8x8lib.h>

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ A0, /* data=*/ A1, /* reset=*/ U8X8_PIN_NONE);         // Digispark ATTiny85

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#define SERIAL SerialUSB
#else
#define SERIAL Serial
#endif

const int ledPin = 2; 
const int buzzerPin = 3; 
int value = 0;



unsigned char low_data[8] = {0};
unsigned char high_data[12] = {0};




#define NO_TOUCH       0xFE

#define THRESHOLD      100

#define ATTINY1_HIGH_ADDR   0x78
#define ATTINY2_LOW_ADDR   0x77



void getHigh12SectionValue(void)
{
  memset(high_data, 0, sizeof(high_data));
  Wire.requestFrom(ATTINY1_HIGH_ADDR, 12);
  while (12 != Wire.available());

  for (int i = 0; i < 12; i++) {
    high_data[i] = Wire.read();
  }

  delay(10);
}


void getLow8SectionValue(void)
{
  memset(low_data, 0, sizeof(low_data));
  Wire.requestFrom(ATTINY2_LOW_ADDR, 8);
  while (8 != Wire.available());

  for (int i = 0; i < 8 ; i++) {
    low_data[i] = Wire.read(); // receive a byte as character
  }


  delay(10);
}



void check()
{


  int sensorvalue_min = 250;
  int sensorvalue_max = 255;
  int low_count = 0;
  int high_count = 0;
  while (1)
  {
    uint32_t touch_val = 0;
    uint8_t trig_section = 0;
    low_count = 0;
    high_count = 0;
    getLow8SectionValue();
    getHigh12SectionValue();

    Serial.println("low 8 sections value = ");
    for (int i = 0; i < 8; i++)
    {
      Serial.print(low_data[i]);
      Serial.print(".");
      if (low_data[i] >= sensorvalue_min && low_data[i] <= sensorvalue_max)
      {
        low_count++;
      }
      if (low_count == 8)
      {
        Serial.print("      ");
        Serial.print("PASS");
      }
    }
    Serial.println("  ");
    Serial.println("  ");
    Serial.println("high 12 sections value = ");
    for (int i = 0; i < 12; i++)
    {
      Serial.print(high_data[i]);
      Serial.print(".");

      if (high_data[i] >= sensorvalue_min && high_data[i] <= sensorvalue_max)
      {
        high_count++;
      }
      if (high_count == 12)
      {
        Serial.print("      ");
        Serial.print("PASS");
      }
    }
    Serial.println("  ");
    Serial.println("  ");




    for (int i = 0 ; i < 8; i++) {
      if (low_data[i] > THRESHOLD) {
        touch_val |= 1 << i;

      }
    }
    for (int i = 0 ; i < 12; i++) {
      if (high_data[i] > THRESHOLD) {
        touch_val |= (uint32_t)1 << (8 + i);
      }
    }

    while (touch_val & 0x01)
    {
      trig_section++;
      touch_val >>= 1;
    }
    value = trig_section * 5;
    SERIAL.print("water level = ");
    SERIAL.print(value);
    SERIAL.println("% ");
    SERIAL.println(" ");
    SERIAL.println("*********************************************************");
      u8x8.setFont(u8x8_font_7x14B_1x2_r);
      u8x8.setCursor(0,1);
      u8x8.print("water level:");
      u8x8.setCursor(3,20);
      u8x8.print(value);
      u8x8.print("%        ");
      delay(50);

    if(trig_section * 5 == 100)
    { u8x8.setCursor(3,20);
      u8x8.print("overflow!");
      delay(100);
      digitalWrite(ledPin, HIGH);
      analogWrite(buzzerPin, 256);
      delay(50);
      digitalWrite(ledPin, LOW);    
      analogWrite(buzzerPin, LOW);
      delay(50);}
    else 
    {digitalWrite(ledPin, LOW);
    analogWrite(buzzerPin, LOW);}
  }
}



void setup() {
  pinMode(ledPin, OUTPUT);
  SERIAL.begin(115200);
  Wire.begin();
    u8x8.begin();
    u8x8.setPowerSave(0);
}

void loop()
{
  check();
}
