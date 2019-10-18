#ifndef _OUTPUT_H
#define _OUTPUT_H

#include <control.h>

// output library

class DiscreteOutput{
    const int pin;
    public:
        DiscreteOutput(int attach_to, String description):
            pin(attach_to),
            desc(description)
        {
        }
        // public variables.
        bool state;
        String desc;
        void setup(){
          pinMode(pin, OUTPUT);
        };
        void set_output(bool value){
          if(state != value){
            digitalWrite(pin, value);
            state = value;
            // digital out (desc) State: state
            Serial.print("Digital Out ("); Serial.print(desc); Serial.print(")  "); Serial.print(" State:"); Serial.println(state);
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
    int profile_step = 0;
    Timer TMR(3000);

    public:
        LED(int attach_to, String description):
            pin(attach_to),
            desc(description)
        {
        }

        String desc;
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
                print_info();
            }
            analogWrite(pin, brightness);
        }
        void fade_out(int fade_amount){
            if(brightness - fade_amount < fade_min){
                brightness = fade_min;
            }
            else{
                brightness = brightness - fade_amount;
                print_info();
            }
            analogWrite(pin, brightness);
        }
        void fade_profile1(){
            // fade profile 1 - breathing
            // effect
            // fade out, fade up, wait, then repeat
            int fade_amounts[] = {5, 2, 0};
            int step_levels[] = {0, 254, 0};
            double wait_times[] = {0, 0, 3000};
            if(profile_step == 0){
                // fade out
                fade_out(fade_amounts[profile_step]);
                if(brightness <= step_levels[profile_step]){
                    // increment the profile step
                    profile_step = profile_step + 1;
                }
            }
            else if(profile_step == 1){
                // fade up
                fade_up(fade_amounts[profile_step]);
                if(brightness >= step_levels[profile_step]){
                    profile_step = profile_step + 1;
                }
            }
            else if(profile_step == 2){
                // wait a time
                TMR.tick();
                if (TMR.complete){
                    TMR.reset();
                    profile_step = 0;
                }
            }
        }
        int get_brightness(){
          return brightness;
        }
        void print_info(){
          Serial.print("LED "); Serial.print(desc); Serial.print("  brightness:"); Serial.println(brightness);
        }
};
#endif
