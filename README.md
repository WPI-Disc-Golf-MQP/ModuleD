# ModuleD

Module D consists of the Photobooth on the machine

## Basics of working with the serial nodes

rosserial allows us to run code on a microcontroller and make it look like a ROS node. We can communicate with the microcontroller using ROS topics.

## Python code

The photobooth feature is controlled by the code in `raspi_hal__photobooth.py`. This file contains a class named `hal__photobooth`, which inherits from the `measure_node` class in `node_templates.py`. The `measure_node` class inherits from the `serial_node` in the same file.

The Python code runs on the Raspberry Pi. `raspi_main.py` imports the `hal__photobooth` class from `raspi_hal__photobooth.py`. If `raspi_hal__photobooth.py` is run directly, then we can use code inside the ```if __name__ == `__main`:``` to test the photobooth code.

## C++ code

The C++ code runs on the SAMD21 (Arduino) and works directly with the wiring to the motors and sensors.

Each SAMD21 has a `NODE_NAME`. It is `module_d` for ModuleD. Each SAMD21 can run multiple `MODULE`s, which are defined in `std_node.cpp`. ModuleD only has one `MODULE`, named `photobooth_module`.

In general, the SAMD21 runs the `setup()` function when it starts and then keeps running the `loop()` function.

### `setup()`

This starts the ROS node.

It then creates a `MODULE` with the following:

- module_name ("photobooth")
- start_callback routine (start_photobooth)
- verify_complete_callback routine (verify_photobooth_complete)
- idle_callback routine (stop_photobooth)
- calibrate_callback routine (calibrate_photobooth)
- publishes to topic `photobooth_state` using `state_pub.publish()`
- publishes to topic `photobooth_status` using `status_pub.publish()`
- subscribes to topic `photobooth_request` and calls `process_request_callback` with messages

It then sets all of the pins to the correct mode (input/output).

 - input pin for upper limit switch
 - input pin for lower limit switch
 - output pins for the three LEDs on the SAMD21 (blue, yellow and green)
 - output pins for the lift motor (speed and direction)
 - *output pins for the stepper motor*

It then sets the initial state for all pins.

 - All LEDs on
 - lift motor stopped
 - *stepper motor stopped*

 ### `loop()`

 This uses `periodic_status()` from `std_node.cpp` to see if enough time (`STATUS_FREQ`) has passed to update status. If so, then it uses `publish_status()` to publish the status on the topic `photobooth_status`.

 It then uses spinOnce() to call any available callbacks. The `process_request_callback()` function in `std_node.cpp` is called whenever a message is received on the topic `photobooth_request`. Depending on the message, it will:

 | REQUEST         | ACTION                                                    |
 |:----------------|:----------------------------------------------------------|
 | START           | publish status IN_PROGRESS                                |
 |                 | run start_callback (start_photobooth)                     |
 | VERIFY_COMPLETE | run verify_complete_callback (verify_photobooth_complete) |
 |                 | if true, publish status COMPLETE                          |
 |                 | if false, publish status IN_PROGRESS                      |
 | STOP            | publish status IDLE                                       |
 |                 | run idle_callback (stop_photobooth)                       |
 | CALIBRATE       | publish status IN_PROGRESS                                |
 |                 | run calibrate_callback (calibrate_photobooth)             |

 It then runs `check_photobooth()` to implement the state machine.

 ### State machine

 The state machine is implemented in the `check_photobooth()` function.

 *state machine diagram*

 