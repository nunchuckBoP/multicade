#ifndef _OUTPUT_H
#define _OUTPUT_H

#include "control.h"

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
            //Serial.print("Digital Out ("); Serial.print(desc); Serial.print(")  "); Serial.print(" State:"); Serial.println(state);
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
    double timer_pre = 3000;
    Timer Timer1;

    public:
        LED(int attach_to, String description):
            pin(attach_to),
            desc(description),
            Timer1(3000),

            // fade profile 1 - breathing effect
            fade_profile1({FadeStep(15, false, 0, 0), FadeStep(8, true, 254, 0), FadeStep(0, false, 0, 6000)})
        {
        }

        String desc;
        FadeStep fade_profile1[3];
        int fade_profile1_step_count = 3;
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
        void fade_profile(FadeStep fade_steps[], int step_count){

            // gets the number of steps in the profile
            //int step_count = sizeof(fade_steps) / sizeof(fade_steps[0]);
            //Serial.print("step_count: "); Serial.print(step_count); Serial.print("  brightness="); Serial.println(brightness);

            // if the step index is greater then or
            // equal to the step count then reset it to
            // zero.
            if(profile_step >= (step_count-1)){
              profile_step = 0;
            }

            // get the current step
            FadeStep current_step = fade_steps[profile_step];
          
            // update the timer preset
            Timer1.set_pre(current_step.wtime);

            if(current_step.fdir){
              if(brightness >= current_step.slevel){
                // trigger timer
                if(Timer1.complete == false && Timer1.get_pre() > 0){
                  Timer1.tick();
                }
                else{
                  // increment the profile
                  // step
                  profile_step = profile_step + 1;
                  Timer1.reset();                  
                }
              }
              else{
                fade_up(current_step.famount);
              }
            }
            else{
              if(brightness <= current_step.slevel){
                // trigger the timer
                if(Timer1.complete == false && Timer1.get_pre() > 0){
                  Timer1.tick();
                }
                else{
                  // increment the profile step
                  profile_step = profile_step + 1;
                  Timer1.reset();
                }
              }
              else{
                fade_out(current_step.famount);
              }
            }  
            print_info();          
        }
        int get_brightness(){
          return brightness;
        }
        void print_info(){
          Serial.print("LED "); Serial.print(desc); Serial.print("  brightness:"); Serial.println(brightness);
        }
};
#endif
