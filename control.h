#ifndef _CONTROL_H
#define _CONTROL_H

// control library
enum control_action{high, low};
enum switch_output {on, off};

class Timer{
    double PRE;
    double start_time;
    bool enabled = false;
    public:
      Timer(double preset_ms):
        PRE(preset_ms)
      {
      }
      // public variables
      double ACC = 0;
      bool complete = false;
      void tick(){
        if(enabled & not(complete)){
          ACC = millis() - start_time;
          Serial.print("Timer Timing: "); Serial.println(ACC);
          if(ACC >= PRE){
            complete = true;   
          }
        }
        else{
          start_time = millis();
          enabled = true;
        }
      }
      void reset(){
        ACC = 0;
        complete = false;
        start_time = 0;
      }
};

class SoftwareSwitch{
    // software switch is a deadband
    // type control. The input is a float
    // and the setpoints are also floats. The
    // output switches based on where the input
    // is. The control action sets the active
    // high or active low functionality.
    control_action ca;
    const float on_sp;
    const float off_sp;
    bool active = false;
    
    public:
        SoftwareSwitch(control_action control_action, float on_setpoint, float off_setpoint):
            ca(control_action),
            on_sp(on_setpoint),
            off_sp(off_setpoint)
        {
            // start by telling the switch
            // to be low.
            active = off;
        }
        int check(float input){
            if(ca == control_action::high){
                // looking for a value to be high
                if(active == true){
                    if(input > on_sp){
                        active = true;
                    }
                    else{
                        active = false;
                    }
                }
                else{
                    if(input < off_sp){
                        active = false;
                    }
                    else{
                        active = true;
                    }
                }
            }
            else{
                if(active == false){
                    if(input < on_sp){
                        active = true;
                    }
                    else{
                        active = false;
                    }
                }
                else{
                    if(input > off_sp){
                        active = false;
                    }
                    else{
                        active = true;
                    }
                }
            }
            return active;
        }
};
#endif
