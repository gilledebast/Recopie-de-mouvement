/* ----------------------------------------------------------------------------------------------------
 * RECOPIE DE MOUVEMENT (copy of motion), 2016
 * Update: 10/01/17
 *
 * Installation that allows a person to take control of the arm of another person
 * with EMG Grove > Arduino > Relay > TENS3000
 * 
 * V1.02
 * Base on BackyardBrain shield
 * Written by Bastien DIDIER
 * more info : http://one-billion-cat.com/
 *
 * ----------------------------------------------------------------------------------------------------
 */ 

// Load arduino library for neopixel led bar, and set pin variables
#include <Adafruit_NeoPixel.h>

#define BUTTON_PIN  3
#define  RELAY_PIN  4
#define    LED_PIN  6
#define   LED2_PIN 13

// initialize led bar pins
Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, LED_PIN, NEO_GRB + NEO_KHZ800);

// Color for LEDs in the bar graph, it moves from green towards red
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

//Sensitivity index, to be changed by the button
int SensitivitiesIndex = 2;
int max_data[7];   //a table variable that registers the reference values of the person from minimum to maximum

//STORE EMG VALUE
int emg;

int static_analog_dta   = 0;                // static analog data

int max_analog_dta      = 350;              // max analog data
int min_analog_dta      = 100;              // min analog data


// get analog emg
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

    // initialize pins 
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);

    // initialize ledbar
    strip.begin();
    strip.setBrightness(64);
    strip.show(); //Initialize led_barGraph to 'off'
    
    long sum = 0;
          
    for(int i=0; i<=10; i++){
        for(int j=0; j<100; j++){
          sum += getAnalog(A0);
          delay(1);
        }

        int wait = map(i, 0,10, 0,strip.numPixels()); // to display where we are in the acquisition process through led bar
        strip.setPixelColor(wait, strip.Color(0,0,255)); // depending on the number of waits we display the right amound of blue leds
        strip.show();
    }
    
    sum /= 1100;  // you do the mean value of the 1100 readings
    
    static_analog_dta = sum; // This is the reference value of the person

    Serial.print("Static Analog Data : "); //display reference value in the console
    Serial.println(static_analog_dta); 

    // Starting from the person's reference data, we fill-in the table variable max_data
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

    // if the read value is bigger than my rest reference value
    if(emg>static_analog_dta){
      emg = map(emg, static_analog_dta, max_data[SensitivitiesIndex], 0, 7);
    } else {
      emg=-1; // in case we have lower values than the reference (because muscle is more relaxed) we ignore it by giving it a negative value
    }

    // loop to display LED Bar on loop lights up, and the other loop turns off the LED
    for(int i=0; i<=emg; i++){
      strip.setPixelColor(i, led_barGraph[i]);
      strip.show();
    }
    for(int i=emg+1; i<strip.numPixels(); i++){
      strip.setPixelColor(i, strip.Color(0,0,0));
      strip.show();
    }

    // Up to here was the User Interface programming, and this last part is what translates the EMG read value into voltage for the tens machine
    //---------------------- Turn ON relay for TENS if emg > THRESHOLD
    
    if(emg>=7){
      digitalWrite(RELAY_PIN, HIGH); // When you reach the maximum number of LEDS only then you set off the current to the tens
      digitalWrite(LED2_PIN, HIGH);
      delay(50); // send current for 50milliseconds
      digitalWrite(RELAY_PIN, LOW);
    } else {
      digitalWrite(LED2_PIN, LOW);   
    }
    
    delay(10); // to slow down things so that it doesn't go to quickly, which with all the things to be done would ruin arduino memory
}

// If you push the sensitivity button, it turns off al the leds and lights up the LED corresponding to the new increased level of sensitivity 
void Sensibility(){
  if (digitalRead(BUTTON_PIN)){

    //turn off all the LEDs in LED bar
    for(int i=0; i<=strip.numPixels(); i++){
      strip.setPixelColor(i, strip.Color(0,0,0));
      strip.show();
    }
    
    SensitivitiesIndex++;

    //if we were at maximum level, it returns to minimum again
    if( SensitivitiesIndex == strip.numPixels() ){
      SensitivitiesIndex = 0;
    }
    
    //light up LED at lastSensitivitiesIndex position for visual feedback
    strip.setPixelColor(SensitivitiesIndex, strip.Color(255,255,255));
    strip.show();

    // wait until you lift the finger so there's not a false multiple read
    while (digitalRead(BUTTON_PIN)){  
      delay(10);
    }       
    //whait a bit more so that LED light feedback is always visible
    delay(100);
  }
}
