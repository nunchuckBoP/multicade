#ifndef _INPUT_H
#define _INPUT_H

// input library
class AnalogInput{
    const int pin;
    const int Rmin = 0;
    const int Rmax = 1023;
    const int Smin;
    const int Smax;
    public:
        AnalogInput(int attach_to, int scaled_min, int scaled_max, String description):
            pin(attach_to), 
            Smin(scaled_max), 
            Smax(scaled_max),
            desc(description)
        {
        }
        
    String desc;
    void setup(){
        // put the setup functions in here
    }
    float reading(){
        int raw_input;
        float reading;
        raw_input = analogRead(pin);
        reading = ((raw_input - Rmin) * ((Smax - Smin)/(Rmax - Rmin))) + Smin;
        return reading;
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
