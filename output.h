#ifndef _OUTPUT_H
#define _OUTPUT_H

// output library

class DiscreteOutput{
    const int pin;
    public:
        DiscreteOutput(int attach_to):
            pin(attach_to)
        {
        }
        // public variables.
        bool state;
        void setup(){
          pinMode(pin, OUTPUT);
        };
        void set_output(bool value){
          if(state != value){
            digitalWrite(pin, value);
            state = value;
            Serial.print("Digital Out:"); Serial.print(pin); Serial.print(" State:"); Serial.println(state);
          }
        };
};

class LED{
    // LED IS ON A FADER USING
    // A PWM OUTPUT PIN. These are
    // all private class variables.
    const int pin;
    const int fade_max = 254;
    const int fade_min = 0;
    int brightness = 0;

    public:
        LED(int attach_to):
            pin(attach_to)
        {
        }
    
        void setup(){
            pinMode(pin, OUTPUT);
        }
        void update_led_brightness(){
        }
        void fade_up(int fade_amount){
            if(brightness + fade_amount > fade_max){
                brightness = fade_max;
            }
            else{
                brightness = brightness + fade_amount;
            }
            analogWrite(pin, brightness);
        }
        void fade_out(int fade_amount){
            if(brightness - fade_amount < fade_min){
                brightness = fade_min;
            }
            else{
                brightness = brightness - fade_amount;
            }
            analogWrite(pin, brightness);
        }
        int get_brightness(){
          return brightness;
        }
};
#endif
