#include <PID_v1.h>

// declare the variables
// -------------state------------
// 1 = waiting player
// 2 = limbo_up
// 3 = running
// 4 = limbo_down
//-------------------------------
int state;

// scan interval (mS)
int scan_interval = 10;

// amplifier control variables
int a_pin = 8;
bool amp_state = LOW;

// temperature setpoint
double temp_setpoint = 80;

// pid variables
double tcv_pv;
double tcv_sp;
double tcv_cv;
double tcv_kp = 2;
double tcv_ki = 5;
double tcv_kd = 1;
PID tcv_pid(&tcv_pv, &tcv_cv, &tcv_sp, tcv_kp, tcv_ki, tcv_kd, DIRECT);

// motion sensor variables
int ms_pin = 13;
int ms_state = 0;
bool pirState;
int pirDelayOff = 30000; // in mS
int pirPRE;
int pirACC = 0;

// marquee fade effect variables
int m_pin = 11;
int m_brightness = 0;
int m_fadeamount = 2;

// backlight fade effect variable
int b_pin = 10;
int b_brightness = 0;
int b_fadeamount = 2;

// blue button light fade effects
int bb_pin = 6;
int bb_brightness = 0;
int bb_fadeamount = 2;

// red button light fade effects
int rb_pin = 5;
int rb_brightness = 0;
int rb_fadeamount = 2;

// fan control variables
int f_pin = 9;

// screen relay control variables
int s_pin = 12;

// cabinet temperature variables
int temp_pin = 0;
int rtemp;
float temp;

void setup() {

  // start the serial 
  Serial.begin(9600);
  
  // put your setup code here, to run once:

  // assign the pins
  // motion sensor - pin 13
  pinMode(ms_pin, INPUT);

  // amplifier relay - pin 08
  pinMode(a_pin, OUTPUT);
  
  // screen ac relay - pin 12
  pinMode(s_pin, OUTPUT);

  // marquee pwm pin - pin 11
  pinMode(m_pin, OUTPUT);

  // backlight pwm pin - pin 10
  pinMode(b_pin, OUTPUT);
  
  // fan FET driver - pin 09

  // blue buttons pwm pin - pin 06
  pinMode(bb_pin, OUTPUT);

  // red buttons pwm pin - pin 05
  pinMode(rb_pin, OUTPUT);

  // setup the pid
  tcv_pid.SetMode(AUTOMATIC);

  // calculate the setpoint
  tcv_sp = scp(temp_setpoint, -40.0, 302.0, 0, 1023);

  // first state is waiting for player
  state = 1;
  pirState = LOW;

  // calculate the delay count for
  // the delay off
  pirPRE = (pirDelayOff / scan_interval);
  
}

float scp(int input, int rmin, int rmax, int smin, int smax){
  // this routine takes in a raw input value
  // plus parameters and will return the scaled 
  // output
  float slope;
  float slope_numerator;
  float slope_denominator;
  float inter;
  float output;

  // slope is rise over run
  slope_numerator = smax - smin;
  slope_denominator = rmax - rmin;
  slope = slope_numerator / slope_denominator;

  // intercept calculation
  inter = smax - slope*rmax;

  output = input*slope+inter;

  // return value (line formula)
  return output;
}

int fade_up(int pin, int brightness, int fade_amount){
  if(brightness < 255){
    brightness = brightness + fade_amount;
    analogWrite(pin, brightness);
  }
  return brightness;
}
int fade_down(int pin, int brightness, int fade_amount){
  if(brightness > 0){
    brightness = brightness - fade_amount;
    analogWrite(pin, brightness);
  }
  return brightness;
}

void output_state(int pin_number, bool to_state){
  // current state
  bool c_state = digitalRead(pin_number);

  if(to_state != c_state){
    
    Serial.print("output state change; pin=");
    Serial.print(pin_number);
    Serial.print("; state=");
    Serial.print(to_state);
    
    digitalWrite(pin_number, to_state);
  }
}

void print_info(){
  
  Serial.print("state=");
  Serial.print(state);
  Serial.print(" temp=");
  Serial.print(temp);
  Serial.print(" pirState=");
  Serial.print(pirState);
  
}

void loop() {
  // put your main code here, to run repeatedly:

  // read the state of the motion sensor
  ms_state = digitalRead(ms_pin);
  
  if(ms_state == 1){
    if(pirState = LOW){
      pirState = HIGH;
      pirACC = 0;
    }
  }
  if(ms_state == 0){
    if(pirState == HIGH){
      if(pirACC == pirPRE){
        pirState == LOW;
      }
      else{
        pirACC = pirACC + 1;
      }
    }
  }

  // check the temperature
  // get the raw temp
  tcv_pv = analogRead(temp_pin);
  temp = scp(rtemp, 0, 1023, -40.0, 302.0);
  
  // compute the pid output
  // assign the output of the fan to the
  // pid output.
  tcv_pid.Compute();

  // write the pid output to the analog
  // value
  int cv = scp(tcv_cv, 0, 1023, 0, 254);
  analogWrite(f_pin, cv);

  // check the state of the machine
  // if it is state is waiting for player
  // then wait for the motion sensor to 
  // detect an entity
  if(state == 1){
    
    if(pirState == HIGH){
      state = 2;
    }
    
  }

  // if the state is limbo-up, then we will
  // go through the whole state before we
  if(state == 2){

    // turn on the relays
    output_state(a_pin, HIGH);
    output_state(s_pin, HIGH);

    // fade up the lights
    m_brightness = fade_up(m_pin, m_brightness, m_fadeamount);
    b_brightness = fade_up(b_pin, b_brightness, b_fadeamount);
    bb_brightness = fade_up(bb_pin, bb_brightness, bb_fadeamount);
    rb_brightness = fade_up(rb_pin, rb_brightness, rb_fadeamount);

    // after the lights are up
    // we switch to the next state
    if(m_brightness == 254 and b_brightness == 254 and bb_brightness == 254 and rb_brightness == 254){
      state = 3;
    }
    
  }

  // if it is running, wait for the motion
  // sensor to not detect a human
  if(state == 3){

     // if a human is not detected in the
     // timeframe, it moves to limbo_down
     // state     

     // read the sensor
     if (pirState == LOW){
      state = 4;
     }
    
  }
  else{
    // do things  in here that need
    // to happen in any other state than
    // state 3.
    pirACC = 0;
  
  }

  // if it is limbo-down, and a entity is not
  // detected, then increase the timed value. If 
  // an entity is detected, then go to libo_up
  // state
  if(state == 4){
    
    // turn off the relays
    output_state(a_pin, LOW);
    output_state(s_pin, LOW);

    // if we don't get the sensor, fade the light
    // down
    m_brightness = fade_down(m_pin, m_brightness, m_fadeamount);
    b_brightness = fade_down(b_pin, b_brightness, b_fadeamount);
    bb_brightness = fade_down(bb_pin, bb_brightness, bb_fadeamount);
    rb_brightness = fade_down(rb_pin, rb_brightness, rb_fadeamount);

    // if the fades are all at zero, then we move to
    // state 1
    if(m_brightness == 0 and b_brightness == 0 and bb_brightness == 0 and rb_brightness == 0){
      state = 1;
    }
    
    // if we get the sensor again, fade back up
    if(pirState == HIGH){
      state = 2;
    }
    
  }

}
