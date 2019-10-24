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
    const int pin;
    const String desc;
    Timer FadeTMR;
    Timer WaitTMR;
    Timer PrintTMR;
    int brightness = 0;
    int max_out = 254;
    int profile_step = 0;
    bool is_verbose;
    public:
      LED(int attach_to, String description, bool enable_print):
          pin(attach_to),
          desc(description),
          is_verbose(enable_print),
          FadeTMR(1500),
          WaitTMR(0),
          PrintTMR(5000)
          {
                          
          }
      void setup(){
        pinMode(pin, OUTPUT);
      }
      void fade(bool up, double fade_time, double wait_time){

        // put the preset into the fade timer
        FadeTMR.set_pre(fade_time);

        // put the preset into the wait timer
        WaitTMR.set_pre(wait_time);

        // starts the timer.
        if(FadeTMR.complete == false){
          FadeTMR.tick();
        }
        else{
          if(WaitTMR.get_pre() > 0){
            if(WaitTMR.complete == false){
              WaitTMR.tick();
            }
          }        
        }
        
        // calculates the buffer values
        float bup = (max_out / fade_time) * FadeTMR.ACC;
        float bd = ((max_out / fade_time) * (-1) * FadeTMR.ACC) + max_out;
        
        if(up){
          int bup_int = floor(bup) - brightness;
          if(bup_int > 0){
            brightness = brightness + bup_int;
          }
        }
        else{
          int bd_int = brightness - floor(bd);
          if(bd_int > 0){
            brightness = brightness - bd_int; 
          }
        }
        
        // print information control
        PrintTMR.tick();
        if(PrintTMR.complete){
          // print the information
          print_info();
          
          // reset the print timer
          PrintTMR.reset();
        }

        if(brightness < 0){
          brightness = 0;
        }
        if(brightness > 254){
          brightness = 254;
        }
        analogWrite(pin, brightness);
      }
      void print_info(){
        if(is_verbose){
          Serial.print("LED Output:"); Serial.print(desc); Serial.print("  brightness="); Serial.print(brightness);
          Serial.print(" profile_step="); Serial.print(profile_step); 
          Serial.print("  FadeTMR.TT="); Serial.print(FadeTMR.TT); Serial.print(" FadeTMR.ACC="); Serial.print(FadeTMR.ACC);
          Serial.print("  WaitTMR.TT="); Serial.print(WaitTMR.TT); Serial.print(" WaitTMR.ACC="); Serial.println(WaitTMR.ACC);
        }
        
      }
      void fade_profile1(){
        if(profile_step == 0){
          // fade down, wait 500 ms
          fade(false, 1500, 500);
        }
        else if(profile_step == 1){
          // fade up, wait 5000 ms
          fade(true, 3000, 5000);
        }
        else{
          // don't do anything
        }
        if(fade_complete()){
          if(profile_step < 1){
            reset_profile();
            profile_step = profile_step + 1;
          }
          else{
            reset_profile();
          }
        }
      }
      bool fade_complete(){
        if((FadeTMR.complete && WaitTMR.complete) || (FadeTMR.complete && WaitTMR.get_pre() <= 0)){
          return true;
        }
        else{
          return false;
        }
      }
      int get_brightness(){
        return brightness;
      }
      void reset(){
        FadeTMR.reset();
        WaitTMR.reset();
      }
      void reset_profile(){
        reset();
        profile_step = 0;
      }
};
#endif
