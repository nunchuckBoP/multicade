#ifndef _INPUT_H
#define _INPUT_H

#include "control.h"

// input library
class AnalogInput{
    const int pin;
    const int Rmin = 0;
    const int Rmax = 1023;
    const int Smin;
    const int Smax;
    Timer PrintTMR;
    
    public:
        AnalogInput(int attach_to, int scaled_min, int scaled_max, String description):
            pin(attach_to), 
            Smin(scaled_min), 
            Smax(scaled_max),
            desc(description),
            PrintTMR(3000)
        {
        }
        // declare the public variables
        String desc;
        void setup(){
            // put the setup functions in here
        }
        float reading(){
            // Rmin = 0
            // Rmax = 1023
            // Smin = -58
            // Smax = 842
            // last raw reading 122 @ about 50 degrees in garage
            
            int raw_input;
            float reading;
    
            // get the raw input
            raw_input = analogRead(pin);
    
            // calculates the reading based on the parameters
            float n = Smax - Smin;
            float d = Rmax - Rmin;
            float s = (n / d);
            reading =(s * raw_input) + Smin;

            if(PrintTMR.complete){
              print_info(reading);
              PrintTMR.reset();
            }
            else{
              PrintTMR.tick();
            }

            return reading;
        }
        void print_info(float reading){
            Serial.print("Analog Input ("); Serial.print(desc); Serial.print("): "); Serial.println(reading);
        }
};

class PIR_Sensor{
    // discrete PIR motion sensor
    // this has built in debouncing capabilities
    // so no need to
    const int pin;
    int prev_state;

    public:
        PIR_Sensor(int attach_to):
            pin(attach_to)
        {
        }

        // public variables
        bool state;
    
        void print_info(){
            // print the information
            Serial.print("PIR_Sensor:"); Serial.print(pin);
            Serial.print("  Current State:"); Serial.print(state);
            // finish the line
            Serial.println("");
        }
        void setup(){
            pinMode(pin, INPUT);
            state = false;
        }

        void loop(){

            int value;
            value = digitalRead(pin);

            if(value == HIGH){
              state = true;
            }
            else{
              state = false;
            }
            
            if(prev_state != state){
                print_info();
                prev_state = state;
            }
        }
};
#endif
