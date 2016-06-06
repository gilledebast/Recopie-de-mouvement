/* ----------------------------------------------------------------------------------------------------
 * RECOPIE DE MOUVEMENT (copy of motion), 2016
 * Update: 06/06/16
 *
 * Installation connecting...
 * with EMG Grove > Arduino > Relay > TENS3000
 * 
 * V1.0
 * Written by Bastien DIDIER
 * more info : http://one-billion-cat.com/
 *
 * ----------------------------------------------------------------------------------------------------
 */ 
 
#include <Adafruit_NeoPixel.h>

#define BUTTON_PIN 3
#define  RELAY_PIN 4
#define    LED_PIN 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, LED_PIN, NEO_GRB + NEO_KHZ800);

uint32_t led_barGraph[8] = {
  strip.Color(0,255,0),
  strip.Color(25,225,0),
  strip.Color(75,200,0),
  strip.Color(125,125,0),
  strip.Color(150,125,0),
  strip.Color(200,75,0),
  strip.Color(225,25,0),
  strip.Color(255,0,0)
};

//SENSIBILITY
int SensitivitiesIndex = 2;
int max_data[7];   // max analog data

//STORE EMG VALUE
int emg;

int static_analog_dta   = 0;                // static analog data
//??
int max_analog_dta      = 350;              // max analog data
int min_analog_dta      = 100;              // min analog data


// get analog emgue
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

    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);

    pinMode(BUTTON_PIN, INPUT);

    strip.begin();
    strip.setBrightness(64);
    strip.show(); //Initialize led_barGraph to 'off'
    
    long sum = 0;
          
    for(int i=0; i<=10; i++){
        for(int j=0; j<100; j++){
          sum += getAnalog(A0);
          delay(1);
        }

        int wait = map(i, 0,10, 0,strip.numPixels());
        strip.setPixelColor(wait, strip.Color(0,0,255));
        strip.show();
    }
    
    sum /= 1100;
    
    static_analog_dta = sum;

    Serial.print("Static Analog Data : ");
    Serial.println(static_analog_dta); 

    Serial.println("Setup Max Analog Data:");
    for(int i=0; i<7; i++){
      max_data[i] = static_analog_dta + (i+1)*10;
      Serial.println(max_data[i]);
    }
    
    Serial.println("Ready!");
}

void loop(){
  
    //-----------------------  Switch sensitivity
    
    Sensibility();
    
    //-----------------------------  Measure EMG
    
    emg = analogRead(A0);
    
    //---------------------- Show EMG strength on LED
    
    if(emg>static_analog_dta){
      emg = map(emg, static_analog_dta, max_data[SensitivitiesIndex], 0, 7);
    } else {
      emg=-1;
    }

    for(int i=0; i<=emg; i++){
      strip.setPixelColor(i, led_barGraph[i]);
      strip.show();
    }
    for(int i=emg+1; i<strip.numPixels(); i++){
      strip.setPixelColor(i, strip.Color(0,0,0));
      strip.show();
    }

    //---------------------- Turn ON relay for TENS if emg > THRESHOLD
    
    if(emg>=7){
      digitalWrite(RELAY_PIN, HIGH);
      delay(50);
      digitalWrite(RELAY_PIN, LOW);
    }
    
    delay(10);
}

void Sensibility(){
  if (digitalRead(BUTTON_PIN)){

    //turn off all the LEDs in LED bar
    for(int i=0; i<=strip.numPixels(); i++){
      strip.setPixelColor(i, strip.Color(0,0,0));
      strip.show();
    }
    
    SensitivitiesIndex++;
    
    if( SensitivitiesIndex == strip.numPixels() ){
      SensitivitiesIndex = 0;
    }
    
    //light up LED at lastSensitivitiesIndex position for visual feedback
    strip.setPixelColor(SensitivitiesIndex, strip.Color(255,255,255));
    strip.show();
        
    while (digitalRead(BUTTON_PIN)){  
      delay(10);
    }       
    //whait a bit more so that LED light feedback is always visible
    delay(100);
  }
}
