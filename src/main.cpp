#define NODE_NAME String("module_d")
// #define STATUS_FREQ 1500 // ms

#include <Arduino.h>

// #define Serial SerialUSB

// #undef min
// #undef max

#include <std_node.cpp>
// #include <std_msgs/Bool.h>
// #include <std_msgs/Empty.h>

// ---------- PHOTOBOOTH MODULE ----------
MODULE *photobooth_module;

// LEDs
int LED_BLUE = PIN_LED_13;    // HIGH for on
int LED_YELLOW = PIN_LED_RXL; // LOW for on
int LED_GREEN = PIN_LED_TXL;  // LOW for on

// I/O pins
int UPPER_LIMIT_SWITCH_PIN = 2;
int LOWER_LIMIT_SWITCH_PIN = 3;
int LIFT_MOTOR_SPEED_PIN = 9;
int LIFT_MOTOR_DIRECTION_PIN = 6;
// pins for stepper motor

// timers
long turning_start_time;

// states for state machine, initialize to PHOTOBOOTH_STATE::PHOTOBOOTH_IDLE
enum PHOTOBOOTH_STATE
{
    PHOTOBOOTH_IDLE = 0,
    PHOTOBOOTH_RISING = 1,
    PHOTOBOOTH_TURNING = 2,
    PHOTOBOOTH_LOWERING = 3
};
PHOTOBOOTH_STATE photobooth_state = PHOTOBOOTH_STATE::PHOTOBOOTH_IDLE;

// ---------- ---------- PHOTOBOOTH FUNCTIONS ---------- ----------

/**
 * Has the turntable reached the upper position
 * 
 * @return T/F
 */
bool upper_limit_switch() {  
  return (digitalRead(UPPER_LIMIT_SWITCH_PIN) == 1);
}

/**
 * Has the turntable reached the lower position
 * 
 * @return T/F
 */
bool lower_limit_switch() { 
  return (digitalRead(LOWER_LIMIT_SWITCH_PIN) == 1);
}

/**
 * The photobooth has finished if we are in the PHOTOBOOTH_IDLE state
 * 
 * @return T/F
 */
bool verify_photobooth_complete() {
  return photobooth_state == PHOTOBOOTH_STATE::PHOTOBOOTH_IDLE;
}

// ?
// bool run_yaxis_motor = false;
// long yaxis_motor_last_step = millis();
// bool yaxis_motor_last_digital_write = false;

// ?
// bool run_spin_motor = false; 
// long spin_motor_last_step = millis();
// bool spin_motor_last_digital_write = false;

// ?
// long when_spinning_started = millis();

/**
 * Starts the photobooth process by setting the lift motor to go up.
 * 
 * Changes photobooth state to PHOTOBOOTH_RISING
 * Set yaxis_motor_dir_pin to LOW to make lift go up
 * Set run_yaxis_motor to run lift motor
 */
void start_photobooth() {
  loginfo("Hi! Starting photobooth!");
  photobooth_state = PHOTOBOOTH_STATE::PHOTOBOOTH_RISING;
  // digitalWrite(yaxis_motor_dir_pin, LOW);
  // run_yaxis_motor = true; 
  // yaxis_motor_last_step = millis(); // I don't think you need this 
}

/**
 * Turn turntable routines
 */

/**
 * lift motor down
 */

/**
 * Stops the photobooth process
 * 
 * Sets run_yaxis_motor and run_spin_motor so the motors do not run
 * Change photobooth state to PHOTOBOOTH_IDLE
 */
void stop_photobooth() {
  // run_yaxis_motor = false; 
  // run_spin_motor = false;
  photobooth_state = PHOTOBOOTH_STATE::PHOTOBOOTH_IDLE; 
}

void calibrate_photobooth() {
  loginfo("calibrate_photobooth; not implemented"); //TODO: Implement calibration
}

/**
 * Sets outputs as needed by current state.
 * Implements the state machine
 * 
 * 
 */
void check_photobooth() {

  // // turn on or off the enable pins
  // if (run_yaxis_motor == true) {
  //   digitalWrite(yaxis_motor_enable_pin, HIGH);
  // } else {
  //   digitalWrite(yaxis_motor_enable_pin, LOW);
  // }

  // if (run_spin_motor == true) {
  //   // TODO: // digitalWrite() // digital write the stepper motor enable high 
  // } else {
  //   // TODO: // digial write stepper motor enable low 
  // }

  // // drive the motor if the flag has been set to run it // TODO implimet the sleep pin as well 
  // if ((yaxis_motor_last_step+0.5 < millis()) && run_yaxis_motor == true) {
  //   // digitalWrite(step_pin, HIGH);
  //   // delay(2);
  //   // digitalWrite(step_pin, LOW);
  //   // delay(2); 

  //   loginfo("Spinning yaxis motor");

  //   digitalWrite(yaxis_motor_step_pin, !yaxis_motor_last_digital_write);
  //   yaxis_motor_last_digital_write = !yaxis_motor_last_digital_write; 
  //   yaxis_motor_last_step = millis(); 
  // } 

  // if ((spin_motor_last_step+2 < millis()) && run_spin_motor == true) {
  //   // TODO: add the spin stepper motor
  // }


  switch (photobooth_state) {
    case PHOTOBOOTH_STATE::PHOTOBOOTH_IDLE:
      // Blue LED only
      digitalWrite(LED_BLUE, HIGH);
      digitalWrite(LED_YELLOW, HIGH);
      digitalWrite(LED_GREEN, HIGH);

      // do nothing
      // for testing, change state after program has been running to 2 seconds
      if (millis() > 2000) {
        photobooth_state = PHOTOBOOTH_STATE::PHOTOBOOTH_RISING;
      }
      break;
    
    case PHOTOBOOTH_STATE::PHOTOBOOTH_RISING:
      // Yellow LED only
      digitalWrite(LED_BLUE, LOW);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_GREEN, HIGH);
      
      if (upper_limit_switch() == true) {
        // run_yaxis_motor = false; 
        photobooth_state = PHOTOBOOTH_STATE::PHOTOBOOTH_TURNING;
        turning_start_time = millis();
        // when_spinning_started = millis();
        // run_spin_motor = true; 
        // spin_motor_last_step = millis(); // I dont think I need this line? 
      }
      
      break;
    
    case PHOTOBOOTH_STATE::PHOTOBOOTH_TURNING:
      // Green LED only
      digitalWrite(LED_BLUE, LOW);
      digitalWrite(LED_YELLOW, HIGH);
      digitalWrite(LED_GREEN, LOW);

      if (millis() > turning_start_time + 2000) {
        photobooth_state = PHOTOBOOTH_STATE::PHOTOBOOTH_LOWERING;
      }

      break;
    
    case PHOTOBOOTH_STATE::PHOTOBOOTH_LOWERING:
      // Blue and Yellow LEDs
      digitalWrite(LED_BLUE, HIGH);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_GREEN, HIGH);

      if (lower_limit_switch() == true) {
        // run_yaxis_motor = false; 
        photobooth_state = PHOTOBOOTH_STATE::PHOTOBOOTH_IDLE;
        photobooth_module -> publish_status(MODULE_STATUS::COMPLETE);
      }

      break;
  };

  // photobooth_module -> publish_status((int) photobooth_state);

};

// 
// 
// 
// 
// 


    // case TURNTABLE_STATE::TURNTABLE_SPINNING:
    // //TALK TO THE PI TO TAKE PICTURES
    // // TODO: do not dead recon this section, actually get confirmation from the pi that picture taking is complete 
      
    //   if (when_spinning_started+2000 < millis()) {
    //     turntable_state = TURNTABLE_STATE::TURNTABLE_LOWERING; 
    //     digitalWrite(yaxis_motor_dir_pin, HIGH);
    //     run_yaxis_motor = true; 
    //   }

    //   break;
    // case TURNTABLE_STATE::TURNTABLE_LOWERING:
    //   if (lower_limit_switched() == true) {
    //     run_yaxis_motor = false; 
    //     run_spin_motor = false; 
    //     turntable_state = TURNTABLE_STATE::TURNTABLE_IDLE; 
    //     turntable_module->publish_status(MODULE_STATUS::COMPLETE);
    //   }
      
    //   break;
    // };




// void start_top_motor(int speed = 230)
// {
//     digitalWrite(UPPER_INVERT_PIN, LOW);
//     analogWrite(UPPER_SPEED_PIN, speed); // start
// }

// void stop_top_motor()
// {
//     analogWrite(UPPER_SPEED_PIN, 0); // stop
// }

// void start_teeth_motor()
// {
//     // TODO: make teeth motor start
// }

// void stop_teeth_motor()
// {
//     // TODO: make teeth motor stop
// }

// void start_intake_motor(int speed = 230)
// {
//     digitalWrite(INTAKE_INVERT_PIN, LOW);
//     analogWrite(INTAKE_SPEED_PIN, speed); // start
//     Serial.println("intake motor started");
// }

// void stop_intake_motor()
// {
//     analogWrite(INTAKE_SPEED_PIN, 0); // stop
//     Serial.println("intake motor stopped");
// }

// // ---------- ---------- ROS INTAKE FUNCTIONS ---------- ----------

// void handle_intake_start()
// {
//     loginfo("start_intake");
//     start_intake_motor();
//     moved_to_INTAKE_RELEASE_time = millis();
//     intake_state = INTAKE_STATE::INTAKE_SEND;
// }

// void handle_stop_intake()
// {
//     stop_top_motor();
//     stop_teeth_motor();
//     stop_intake_motor();
//     intake_state = INTAKE_STATE::INTAKE_IDLE;
// }

// bool verify_intake_complete()
// {
//     return intake_state == INTAKE_STATE::INTAKE_IDLE;
// }

// void calibrate_intake()
// {
//     loginfo("calibrate_intake; not implemented"); // TODO: Implement calibration
// }

// // ---------- ---------- INTAKE TIMER CHECK & HANDLE ---------- ----------

// bool check_intake_timer()
// {
//     return moved_to_INTAKE_RELEASE_time + 2000 < millis();
// }

// void handle_intake_timer()
// {
//     if (intake_state == INTAKE_STATE::INTAKE_SEND)
//     {
//         start_top_motor();
//         start_teeth_motor();
//         stop_intake_motor();
//         intake_state = INTAKE_STATE::INTAKE_RECIEVE;
//     }
// }

// // ---------- ---------- BEAM BREAK CHECK & HANDLE ---------- ----------

// bool beam_break_val_prev = 0;
// bool check_beam_break()
// {
//     bool beam_break_val = digitalRead(BEAM_BREAK_PIN); // read beam break pin
//     if (beam_break_val != beam_break_val_prev)
//         loginfo("Intake beam break changed state to: " + String(beam_break_val)); // logging function
//     bool beam_broken = beam_break_val == 0 && beam_break_val_prev == 1;
//     beam_break_val_prev = beam_break_val; // set previous value to current value
//     return beam_broken;
// }

// void handle_beam_break()
// {
//     if (intake_state == INTAKE_STATE::INTAKE_RECIEVE)
//     {
//         stop_top_motor();
//         stop_teeth_motor();
//         intake_state = INTAKE_STATE::INTAKE_IDLE;
//         intake_module->publish_status(MODULE_STATUS::COMPLETE);
//     }
// }

// // ---------- ---------- SETUP ---------- ----------

void setup()
{
  init_std_node();
  loginfo("setup() Start");

  // initialize module with callback routines
  photobooth_module = init_module("photobooth",
    start_photobooth,
    verify_photobooth_complete,
    stop_photobooth,
    calibrate_photobooth);
  
  // set pins to input/output mode
  pinMode(UPPER_LIMIT_SWITCH_PIN, INPUT_PULLDOWN);
  pinMode(LOWER_LIMIT_SWITCH_PIN, INPUT_PULLDOWN);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LIFT_MOTOR_SPEED_PIN, OUTPUT);
  pinMode(LIFT_MOTOR_DIRECTION_PIN, OUTPUT);

  // set intial state
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LIFT_MOTOR_DIRECTION_PIN, LOW);
  analogWrite(LIFT_MOTOR_SPEED_PIN, 0);

  loginfo("setup() Complete");
}

// ---------- ---------- LOOP ---------- ----------

void loop()
{
    periodic_status();
    nh.spinOnce();
    check_photobooth();
}
