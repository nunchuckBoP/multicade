#include "input.h"
#include "output.h"
#include "control.h"

enum state {SLEEPING, FADE_UP, ACTIVE, FADE_OUT};
state system_state;
state last_system_state;

// declared I/O devices. First parameter is
// ALWAYS the pin number

// ANALOG INPUTS
AnalogInput temp_sensor(A0, -58.0, 842.0);

// DISCRETE INPUTS
PIR_Sensor pir(8);

// DISCRETTE OUTPUTS
DiscreteOutput status_led(13);
DiscreteOutput Fan(2);
DiscreteOutput Amp(12);
DiscreteOutput Screen(4);

// PWM OUTPUTS
LED player1(5);
LED player2(6);
LED blue_backlight(10);
LED marquee(11);

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
  
}

void loop() {
  // put your main code here, to run repeatedly:

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
        blue_backlight.fade_up(3);

        // fade up the marquee light
        marquee.fade_up(3);

        // if the lights are up, then move to the
        // next state
        if(blue_backlight.get_brightness() >= 254 & marquee.get_brightness() >= 254){
          system_state = ACTIVE;
        }
      break;
    case ACTIVE:
      
      // turn the screen on
      Screen.set_output(true);

      // turn on the audio amplifier
      Amp.set_output(true);
      
      if(pir.state == true){
        active_tmr.reset();
      }
      else{
        active_tmr.tick();
        if(active_tmr.complete){
          system_state = FADE_OUT;
          active_tmr.reset();
        }
      }
      break;
    case FADE_OUT:
      // fade out the back blue light
      blue_backlight.fade_out(2);

      // fade out the marquee
      marquee.fade_out(2);

      if(blue_backlight.get_brightness() <= 0 & marquee.get_brightness() <= 0){
        system_state = SLEEPING;
      }
      break;
  }
  if(last_system_state != system_state){
    Serial.print("system_state="); Serial.println(system_state);
    last_system_state = system_state;
  }
}
