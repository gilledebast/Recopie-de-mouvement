/* ----------------------------------------------------------------------------------------------------
 * RECOPIE DE MOUVEMENT (copy of motion), 2016
 * Update: 06/06/16
 *
 * Installation connecting two arms that one controle the other.
 * with EMG Grove > Arduino > Relay > TENS3000
 * 
 * V1
 * Written by Bastien DIDIER
 * more info : http://one-billion-cat.com/
 *
 * ----------------------------------------------------------------------------------------------------
 */ 
 
#include <Adafruit_NeoPixel.h>

#define RELAY 4

#define LED_PIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, LED_PIN, NEO_GRB + NEO_KHZ800);

    uint32_t pixels[8] = {
      strip.Color(0,255,0),
      strip.Color(25,225,0),
      strip.Color(75,200,0),
      strip.Color(125,125,0),
      strip.Color(150,125,0),
      strip.Color(200,75,0),
      strip.Color(225,25,0),
      strip.Color(255,0,0)
    };

//TODO CHANGE SENSIBILITY
int max_data[4]      = {310,330,350,370};   // max analog data
int max_analog_dta      = 350;              // max analog data
int min_analog_dta      = 100;              // min analog data
int static_analog_dta   = 0;                // static analog data

// get analog value
int getAnalog(int pin)
{
    long sum = 0;
    
    for(int i=0; i<32; i++)
    {
        sum += analogRead(pin);
    }
    
    int dta = sum>>5;
    
    max_analog_dta = dta>max_analog_dta ? dta : max_analog_dta;         // if max data
    min_analog_dta = min_analog_dta>dta ? dta : min_analog_dta;         // if min data
    
    return sum>>5;
}

void setup(){
    Serial.begin(9600);

    pinMode(RELAY, OUTPUT);
    digitalWrite(RELAY, LOW);
    
    long sum = 0;

    for(int i=0; i<=10; i++)
    {
        for(int j=0; j<100; j++)
        {
            sum += getAnalog(A0);
            delay(1);
        }
        
        //bar.setLevel(10-i);
    }
    
    sum /= 1100;
    
    static_analog_dta = sum;

    Serial.print("static_analog_dta = ");
    Serial.println(static_analog_dta); 

    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
}

int level       = 5;
int level_buf   = 5;

void loop()
{
    int val = analogRead(A0);                    // get Analog value
    
    int level2;
    
    if(val>static_analog_dta)                   // larger than static_analog_dta
    {
      val = map(val, static_analog_dta, max_data[1], 0, 7); 
      
      for(int i=0; i<=val; i++){
        strip.setPixelColor(i, pixels[i]);
        strip.show();
      }

      if(val>=7){
        digitalWrite(RELAY, HIGH);
        delay(50);
      }
    }
    /*else 
    {
        level2 = 5 - map(val, min_analog_dta, static_analog_dta, 0, 5);
    }*/
  
    
    digitalWrite(RELAY, LOW);
      
    delay(10);
  
    for(int i=0; i<=8; i++){
      strip.setPixelColor(i, strip.Color(0,0,0));
      strip.show();
    }
}
