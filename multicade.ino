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
int a_pin = 12;
bool amp_state = LOW;

// motion sensor variables
int ms_pin = 8;
int ms_state = 0;

// pir motion detector variables
boolean pirState;
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
int f_pin = 2;
float fan_on_sp = 85;
float fan_off_sp = 80;

// screen relay control variables
int s_pin = 4;

// cabinet temperature variables
int temp_pin = A0;
int temp_raw;
float temp;

int breath_state = 0;
int breath_count = 0;

void setup() {

  // start the serial 
  Serial.begin(9600);
  
  // put your setup code here, to run once:

  // assign the pins
  // motion sensor - pin X
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
  pinMode(f_pin, OUTPUT);

  // blue buttons pwm pin - pin 06
  pinMode(bb_pin, OUTPUT);

  // red buttons pwm pin - pin 05
  pinMode(rb_pin, OUTPUT);

  // first state is waiting for player
  state = 1;
  pirState = LOW;

  // calculate the delay count for
  // the delay off
  pirPRE = (pirDelayOff / scan_interval);
  
  // spin the fan to be sure that it
  // is working
  output_state(f_pin, HIGH);
  
  // delay two seconds and let the system
  // power up. Let the fan spin around for
  // a bit.
  delay(200);
  
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
    if(brightness + fade_amount <= 254){
      brightness = brightness + fade_amount; 
    }
    else{
      brightness = 254;
    }
    analogWrite(pin, brightness);
  }
  
  return brightness;
}
int fade_down(int pin, int brightness, int fade_amount){
  if(brightness > 0){
    if(brightness - fade_amount > 0){
      brightness = brightness - fade_amount;
    }
    else{
      brightness = 0;
    }
    analogWrite(pin, brightness);
  }

  return brightness;
}

void output_state(int pin_number, bool to_state){
  // current state
  bool c_state = digitalRead(pin_number);

  if(to_state != c_state){
    
    Serial.print("output state change; pin="); Serial.print(pin_number); Serial.print("; state="); Serial.println(to_state);
    
    digitalWrite(pin_number, to_state);
  }
}

void print_info(){
  
  Serial.print("state="); Serial.print(state); Serial.print(" temp="); Serial.print(temp); Serial.print(" pirState="); Serial.println(pirState);
  
}

void loop() {
  // put your main code here, to run repeatedly:

  // read the state of the motion sensor
  //ms_state = digitalRead(ms_pin);
  ms_state = 1;
  
  if(ms_state == 1){
    if(pirState == LOW){
      pirState = HIGH;
      pirACC = 0;
    }
  }
  if(ms_state == 0){
    if(pirState == HIGH){
      if(pirACC == pirPRE){
        pirState = LOW;
      }
      else{
        pirACC = pirACC + 1;
      }
    }
  }

  // check the temperature
  // get the raw temp
  temp_raw = analogRead(temp_pin);
  temp = scp(temp_raw, 0, 1023, -58.0, 842.0);
  
  // control the fan based on what the temperature
  // is in the cabinet. The setpoints are defined in
  // the begining of the program.
  if(temp > fan_on_sp){
    output_state(f_pin, HIGH);
  }
  if(temp <= fan_off_sp){
    output_state(f_pin, LOW); 
  }

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
  // go through the whole state before we turn
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

     // put the backlight color into a breathing pattern.
     // while the player is playing.
     if(breath_state == 0){
       // fade out
       b_brightness = fade_down(b_pin, b_brightness, 6);
       if(b_brightness == 0){
        if(breath_count >= 5){
          breath_state = 1;
          breath_count = 0; 
        }
        else{
          breath_count++;
        }
       }
     }
     else if(breath_state == 1){
      // fade up
      b_brightness = fade_up(b_pin, b_brightness, 3);
      if(b_brightness == 254){
        if(breath_count == 2){
          breath_state = 2;
          breath_count = 0; 
        }
        else{
          breath_count++;
        }
      }
     }
     else if(breath_state == 2){
      // fade out
      b_brightness = fade_down(b_pin, b_brightness, 3);
      if(b_brightness <= 100){
        if(breath_count == 1){
          breath_state = 3;
          breath_count = 0; 
        }
        else{
          breath_count++;
        }
      }
     }
     else if(breath_state == 3){
      // fade up
      b_brightness = fade_up(b_pin, b_brightness, 2);
      if(b_brightness == 254){
        if(breath_count = 4){
          breath_state = 0;
          breath_count = 0; 
        }
        else{
          breath_count++;
        }
      }
     }

     // debug lighting profile information.
     //Serial.print("b_brightness="); Serial.print(b_brightness); Serial.print("  breath_state="); Serial.println(breath_state);
     
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
