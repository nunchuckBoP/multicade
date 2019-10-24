#include "input.h"
#include "output.h"
#include "control.h"

enum state {SLEEPING, FADE_UP, ACTIVE, FADE_OUT};
state system_state;
state last_system_state;

// declared I/O devices. First parameter is
// ALWAYS the pin number

// ANALOG INPUTS
AnalogInput temp_sensor(A0, -58.0, 842.0, "cabinet temp");

// DISCRETE INPUTS
PIR_Sensor pir(8);

// DISCRETTE OUTPUTS
DiscreteOutput status_led(13, "status LED");
DiscreteOutput Fan(2, "Cooling Fan");
DiscreteOutput Amp(12, "Amplifier");
DiscreteOutput Screen(4, "Monitor Screen");

// PWM OUTPUTS
LED player1(5, "Player 1 Lights", false);
LED player2(6, "Player 2 Lights", false);
LED blue_backlight(10, "Blue Backlight", true);
LED marquee(11, "Marquee Lights", false);

// CONTROL CLASS OBJECTS
SoftwareSwitch temp_ss1(high, 85.0, 80.0);
Timer active_tmr(60000);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  // set up the hardware pins
  pir.setup();
  status_led.setup();
  temp_sensor.setup();
  Fan.setup();
  player1.setup();
  player2.setup();
  blue_backlight.setup();
  marquee.setup();
  Amp.setup();
  Screen.setup();

  // initialize the variables
  system_state = SLEEPING;
  last_system_state = FADE_OUT;

  Serial.println("System Warming Up 60 sec");
  delay(10000);
  Serial.println("Warmup complete.");
  
}

void loop() {
  // put your main code here, to run repeatedly:
  double loop_start = millis();

  // get the pir state
  pir.loop();
  
  // output the status_led
  status_led.set_output(pir.state);

  // check the temperature
  float cab_temp = temp_sensor.reading();
  bool too_hot = temp_ss1.check(cab_temp);

  // control the fan relay
  if(too_hot){
    Fan.set_output(true);
  }
  else{
    Fan.set_output(false);
  }

  switch(system_state){
    case SLEEPING:
      if(pir.state == true){
        system_state = FADE_UP;

        // reset the time here
        active_tmr.reset();
      }
      else{
        // disable amplifier pin
        Amp.set_output(false);

        // disable screen pin
        Screen.set_output(false);
      }
      break;
    case FADE_UP:
        // fade up the blue_backlight
        blue_backlight.fade(true, 5000, 1000);

        // fade up the marquee light
        marquee.fade(true, 5000, 1000);
        
        // fade up player 1
        player1.fade(true, 5000, 1000);

        // fade up player 2
        player2.fade(true, 5000, 1000);

        // if the lights are up, then move to the
        // next state
        if(blue_backlight.fade_complete() && marquee.fade_complete() &
            player1.fade_complete() && player2.fade_complete()){

              // reset the fades
              blue_backlight.reset(); marquee.reset(); player1.reset(); player2.reset();
  
              // change the state to active.
              system_state = ACTIVE;
        }
        else{
          
        }
      break;
    case ACTIVE:
      
      // turn the screen on
      Screen.set_output(true);

      // turn on the audio amplifier
      Amp.set_output(true);

      // put the blue backlight 
      // LED in fade_profile1
      blue_backlight.fade_profile1();
      
      if(pir.state == true){
        active_tmr.reset();
      }
      else{
        active_tmr.tick();
        //Serial.print("active_tmr.ACC=");Serial.println(active_tmr.ACC);
        if(active_tmr.complete){
          system_state = FADE_OUT;
          active_tmr.reset();
        }
      }
      break;
    case FADE_OUT:
      // fade out the back blue light
      blue_backlight.fade(false, 5000, 1000);

      // fade out the marquee
      marquee.fade(false, 5000, 1000);

      if(blue_backlight.fade_complete() && marquee.fade_complete() <= 0){
        blue_backlight.reset(); marquee.reset();
        system_state = SLEEPING;
      }
      break;
  }
  if(last_system_state != system_state){
    Serial.print("system_state="); Serial.println(system_state);
    last_system_state = system_state;
  }

  double scan_time = millis() - loop_start;
  //Serial.print("Scan time:"); Serial.println(scan_time);

  // small delay
  delay(100);
}
