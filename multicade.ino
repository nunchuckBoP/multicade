// declare the variables
// -------------state------------
// 1 = waiting player
// 2 = limbo_up
// 3 = running
// 4 = limbo_down
//-------------------------------
int state;
bool motion_detected;

// used for pid control
double tc_et;
float tc_sp;
float tc_kp;
float tc_ki;
float tc_kd;

void setup() {
  // put your setup code here, to run once:

  // assign the pins

  // motion sensor

  // screen ac relay

  // marquee pwm pin

  // backlight pwm pin

  // buttons pwm pin

  // temperature sensor

  // fan FET driver

}

float scp(int rmin, int rmax, float smin, float smax){
  // this is a scaling routine. Returns the scaled
  // value.
  
}
double compute_pid(double inp, float setpoint){
  // computes the temperature PID algorythm
  // outputs the control variable value
  
}
void loop() {
  // put your main code here, to run repeatedly:

  // check the temperature
  // compute the pid output
  // assign the output of the fan to the
  // pid output.

  // check the state of the machine
  // if it is state is waiting for player
  // then wait for the motion sensor to 
  // detect an entity

  // if it is running, wait for the motion
  // sensor to not detect a human

  // if it is limbo-down, and a entity is not
  // detected, then increase the timed value. If 
  // an entity is detected, then go to libo_up
  // state

}
