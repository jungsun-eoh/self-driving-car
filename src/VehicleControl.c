/**************************************************************
* Class: CSC-615-01 Spring 2020
* Name: Team Shartcar
* Student ID: 
* Github ID: 
* Project: Drive On
*
* File: VehicleControl.c
*
* Description: Control interface for the vehicle that handles
*           the processing of incoming data and controls the
*           car's maneuvering.
*
**************************************************************/
#include "Include.h"

/**************************************************************
Initialize all components and structures by calling their
init functions in each module
**************************************************************/
#define ECHO 22             // Echo sensor for GPIO 23
#define TRIGGER 27          // Echo sensor for GPIO 18
#define ECHO_SIDE 24        // Echo sensor for GPIO 24
#define TRIGGER_SIDE 18     // Echo sensor for GPIO 18
void init() {
    // initialize wiringPi
    if (wiringPiSetupGpio() < 0) {
        printf("error with wiringPiSetupGpio()!\n");
    }
    pinMode(BUTTON, INPUT); // initialize button

    // Initialize echo sensors
    pinMode(ECHO, INPUT);
    pinMode(ECHO_SIDE, INPUT);
    pinMode(TRIGGER, OUTPUT);
    pinMode(TRIGGER_SIDE, OUTPUT);

    // settle the echo sensors
    digitalWrite(TRIGGER, LOW);
    digitalWrite(TRIGGER_SIDE, LOW);
    delay(1000); // wait 1 second

    globals_init();     // initialize all global variables
    sensors_init();     // initialize all sensors
    motor_init();       // initialize motors
    speedometer_init(); // initialize SPI for speedometer
    // initialize lidar
}

/**************************************************************
Initiate a change in direction of vehicle travel. Operates by
changing the speed of one front wheel to be faster 

return: 1 for success, 0 for failure
***************************************************************/
int veer(int goLeft, int speedA, int speedB) {
    resume();

    // Veer right
    if(goLeft == OFF) {
        setupForward(speedA, MOTOR_A); // slow down right wheel
        setupForward(speedB, MOTOR_B); // speed up left wheel
    }
    // Veer left
    else if(goLeft == ON) {
        setupForward(speedA, MOTOR_A); // slow down right wheel
        setupForward(speedB, MOTOR_B); // speed up left wheel
    }
    delay(150);
    resume(); // continue straight
    return 1;
}

/**************************************************************
Evaluate the IR input and determine if an obstacle is 
within x-distance of the front of the vehicle. 

return: 1 for obstacle in the vehicle's path, 0 for all-clear
***************************************************************/
int obstacleIsAhead() {
    if((obstacleIR_B == ON) || (obstacleIR_C == ON))
        return 1;
    return 0;
}

/**************************************************************
If the car gets stuck on a line via edge case, this is the
redundancy method to seek out the line via rotation.

Edge case: Line is positioned between the left or right-most
middle line sensor and one of the side lateral line sensors
***************************************************************/
int getUnstuck() {

    /**********************
    RIGO ALGORITHM
    **********************/
    // twist right a tiny bit
    setupForward(DEFAULT_POWER_B ,MOTOR_B);
    setupBackward(DEFAULT_POWER_A, MOTOR_A);
    delay(250);
    halt();

    // Check status of SENSOR_D
    if(onLine_D == ON) return 1;
    else {
        while(car_activated == ON) {
            setupBackward(DEFAULT_POWER_B, MOTOR_B);
            setupForward(DEFAULT_POWER_A, MOTOR_A);
            delay(250);
            halt();

            if(onLine_D) return 1;
        }
    }
        
    return 0;
}
/**************************************************************
Evaluate the group of three line sensors on the front middle
bumper and if we don't have "ON ON ON" then steering is 
required. This method will trigger turn() function to steer
the car depending on the configuration of the sensors
input.

If all three display "0 0 0" meaning that all three have left
the line, this is indicative of either the end of the course,
or a 90 degree left or right turn required, and this
method will look for the line continuing on either side by
looking at the left and right lateral line sensor data. If 
either is found, the method will trigger sharpTurn() for that
direction.

return: 1 for success, 0 for error
***************************************************************/
int correctHeading() {
    //int AGGRESSIVE = 20;
    //int VEER = 10;

    // if we are totally off the line, get back on 
    // 0 0 0
    if((onLine_B == OFF) && (onLine_C == OFF) && (onLine_D == OFF)) { // if all sensors off line, then stop
        halt(); // We're not on the line at all, so stop to reassess
        if(onLine_E == ON) //hard right
            sharpTurn(OFF, DEFAULT_POWER_A - VEER, DEFAULT_POWER_B + VEER + 4);
        
        else if(onLine_A == ON) //hard left
            sharpTurn(ON, (DEFAULT_POWER_A + VEER + 8), (DEFAULT_POWER_B - VEER));

        // Check the IR sensors in the front
        if((obstacleIR_B == OFF) && (obstacleIR_C == OFF)){
            // Check the Echo distance
            if(distance > 20){
                // If the above are also OFF, then call the stuck function
                getUnstuck();
            }
        }    
    }

    // 0 0 1
    while((onLine_B == OFF) && (onLine_C == OFF) && (onLine_D == ON)) {
        output();
        //Initiate a medium veer right maneuver
        //printf("Medium Veer Right\n");
        if(!obstacleIsAhead()) {
            veer(OFF, (DEFAULT_POWER_A - AGGRESSIVE - 2), (DEFAULT_POWER_B + AGGRESSIVE + 6));
        }
        else {
            // printf("Halting ln279\n");
            // halt();
        }
    }

    // 0 1 1
    while((onLine_B == OFF) && (onLine_C == ON) && (onLine_D == ON)) {
        output();
        //Initiate a slight veer right maneuver
        //printf("Slight Veer Right\n");
        if(!obstacleIsAhead()) {
            veer(ON, (DEFAULT_POWER_A - VEER), (DEFAULT_POWER_B + VEER));
        }
        else {
            // printf("Halting ln291\n");
            // halt();
        }
    }

    // 1 1 0
    while((onLine_B == ON) && (onLine_C == ON) && (onLine_D == OFF)) {
        output();
        //initiate a slight veer left maneuver
        //printf("Slight Veer Left\n");
        if(!obstacleIsAhead()) {
            veer(ON, (DEFAULT_POWER_A + VEER), (DEFAULT_POWER_B - VEER));
        }
    }

    // 1 0 0
    while((onLine_B == ON) && (onLine_C == OFF) && (onLine_D == OFF)) {
        output();
        //initiate a medium veer left maneuver
        //printf("Medium Veer Left\n");
        if(!obstacleIsAhead()) {
            veer(ON, (DEFAULT_POWER_A + AGGRESSIVE + 7), (DEFAULT_POWER_B - AGGRESSIVE - 4));
        }
        else {
            //printf("Halting ln311\n");
            //halt();
        }
    }

    // 1 0 1
    if((onLine_B == ON) && (onLine_C == OFF) && (onLine_D == ON)) {
        //Error, this indicates two thin lines. Bad  
    }

    // 0 1 0
    else if((onLine_B == OFF) && (onLine_C == ON) && (onLine_D == OFF)) {
        //Error, this indicates a thin line. Bad. But continue on LOL
        if((!obstacleIsAhead()) && (halted == OFF))
            resume(); 
    }

    // 1 1 1
    else if((onLine_B == ON) && (onLine_C == ON) && (onLine_D == ON)) {
        //printf("We are on the line!\n");
        if((!obstacleIsAhead()) && (halted == OFF))
            resume();
    }


    return 1;
}

/**************************************************************
The car has detected an obstacle and needs to initiate a
sequence to go around the object. It uses echo sensors on the
front and left side of the car. The left sensor monitors the
initial distance and the difference between the current and
last reading. If the change is huge, this means the middle of
the car has passed the obstacle and it can begin to turn for 
the next leg of the pattern:

Leg 1: turn right and move parallel to the front of the object
Leg 2: turn left and move parallel to the side of the object
Leg 3: turn left and find the line
Leg 4: turn right to get back on the line

Note: strikes is a variable intended to prevent problems with
      random wild readings on the echo sensor. It will 
      increment to 3 for each sequential wild reading and if
      so, we will know that it's a genuine large delta that
      should trigger the next step of the sequence.
***************************************************************/
int goAround() {
    //int AGGRESSIVE = 20;
    //int VEER = 10;
    int prevSideDist = 999999;
    int curSideDist = 999999;
    int thresh = 20;            // stop 20cm in front of object
    int strikes = 0;  
    int maxStrikes = 3;          

    // Since we are halted, resume vehicle movement
    resume();

    // Make the first right turn
    setupForward(DEFAULT_POWER_B + VEER + 4, MOTOR_B); //speed up left wheel
    setupBackward(DEFAULT_POWER_A - VEER, MOTOR_A); //reverse right wheel
    delay(950); //give time to turn

    // Continue Straight for First Leg
    resume(); // continue straight
    delay(500);
    prevSideDist = side_distance;
    while(ON) {
        curSideDist = side_distance; // look at the current side distance
        int delta = abs(curSideDist - prevSideDist);
        //printf("curSideDist: %d  |  ", curSideDist);
        //printf("prevSideDist: %d  |  ", prevSideDist);
        //printf("delta: %d\n", delta);
        if(delta > thresh) {
            strikes++;
            if(strikes >= maxStrikes) {
                strikes = 0;
                //printf("First Leg Complete!\n");
                delay(850); // clear the rest of the vehicle before turning
                break; // if we passed the obstacle on the side
            }//if inner
        }//if outer
    }//while

    // FIRST LEFT TURN
    //printf("FIRST SHARP LEFT around object...\n");
    setupBackward(0, MOTOR_B); //back the left wheel
    setupForward((DEFAULT_POWER_A + VEER + 14), MOTOR_A); //speed up the right wheel
    delay(1000); // give it a moment to turn

    // Continue Straight For Second Leg
    resume(); //continue straight
    delay(1000); //keep going straight for a bit
    prevSideDist = side_distance;
    while(ON) {
        curSideDist = side_distance; // look at the current side distance
        int delta = abs(curSideDist - prevSideDist);
        //printf("curSideDist: %d  |  ", curSideDist);
        //printf("prevSideDist: %d  |  ", prevSideDist);
        //printf("delta: %d\n", delta);
        if(delta > thresh) {
            strikes++;
            //printf("Second Leg Complete!\n");
            if(strikes >= maxStrikes) {
                strikes = 0;
                delay(1500); // clear the rest of the vehicle before turning
                printf("Second Leg Complete!\n");
                break; // if we passed the obstacle on the side
            }
        }
    }
            
    // SECOND LEFT TURN
    //printf("SECOND SHARP LEFT around object...\n");
    setupBackward(0, MOTOR_B); //back the left wheel
    setupForward((DEFAULT_POWER_A + VEER + 14), MOTOR_A); //speed up the right wheel
    delay(1000); // give it a moment to correct
    prevSideDist = side_distance;

    // Continue Straight until we find the line
    resume();
    while((onLine_B == OFF) && (onLine_C == OFF) && (onLine_D == OFF)) {}

    // turn right 90 degrees back onto the line
    //printf("Turning right to get back on line\n");
    sharpTurn(OFF, DEFAULT_POWER_A - VEER - 8, DEFAULT_POWER_B + VEER + 4);

    return 1;
}

/**************************************************************
Make a 90 degree turn left or right, depending on the 
arguments passed.

Parameters:
    goLeft: boolean. If true, vehicle turns left. If false,
        vehicle will turn right.

return: 1 for success, 0 for error
***************************************************************/
int sharpTurn(int goLeft, int speedA, int speedB) {
    // Sharp Right Turn 90 degrees
    if(goLeft == OFF) {
        setupForward(speedB, MOTOR_B); //left wheel forwards
        setupBackward(speedA, MOTOR_A); //back the right wheel
        delay(750); // give it a moment to correct
    }
    // Sharp Left Turn 90 degrees
    else if(goLeft == ON) {
        setupBackward(speedB, MOTOR_B); //back the left wheel
        setupForward(speedA, MOTOR_A); //speed up the right wheel
        delay(850); // give it a moment to correct
    }
    resume();   //continue straight
    return 1;
}

/**************************************************************
Halt stops the car periodically for different reasons, such
as to evaluate a situation before continuing, or prior to
termination of program.
***************************************************************/
void halt() {
    stopMotor(MOTOR_A);
    stopMotor(MOTOR_B);
    halted = ON;
}
/**************************************************************
Resume triggers the car to continue traveling forward at
its default speed.
***************************************************************/
void resume() {
    setupForward(DEFAULT_POWER_A, MOTOR_A);
    setupForward(DEFAULT_POWER_B, MOTOR_B);
    halted = OFF;
}

/**************************************************************
Output all of our most important data to the console
Data: global variables for speed
***************************************************************/
void output() {
    printf("L: %.2f cm/s | R: %.2f cm/s | ", cur_speed_b, cur_speed_a);
    printf("Sensors: %d %d%d%d %d | ", onLine_A, onLine_B, onLine_C, onLine_D, onLine_E);
    printf("Dist_FR: %dcm | Dist_SD: %dcm        \r", distance, side_distance);
    fflush(stdout);
}

/**************************************************************
THREAD FUNCTIONS
***************************************************************/
//button thread function
void* buttonThread(void* args){
    //runs forever until return or breakout
    while (1){
        //If the button gets pressed
        if(digitalRead(BUTTON) == ON) {
            //If the car is activated
            if(car_activated == ON) {
                car_activated = OFF;
                printf("Lets exit our threads now...\n");
                break;
            }
            //If the car is not activated
            else {
                car_activated = ON;
            }
        }
        delay(500);
    }
    halted = ON;
    stopMotor(MOTOR_A);
    stopMotor(MOTOR_B);
}

/**************************************************************
Line thread continuously gets readings for one individual
line sensor and updates its respective global variable
***************************************************************/
void* lineThread(void* args) {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    
    // unpack arguments
    Args* arg = (Args*)args;
    int sensor = arg->lineSensor;

    // call sensor readings method
    getLineSensorData(sensor);
}

/**************************************************************
IR thread continuously gets readings for one individual
infrared obstacle avoidance sensor and updates its 
respective global variable
***************************************************************/
void* irThread(void* args) {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    // unpack arguments
    Args* arg = (Args*)args;
    int sensor = arg->irSensor;
    
    // call ir readings method
    getIrSensorData(sensor);
}

/**************************************************************
Speed thread takes readings from the LS7366R quadrature
encoder chip connected to one motor and then calculates the
speed that wheel is traveling, and updates its global variable. 
All speed readings are in the units cm/s
***************************************************************/
void* speedThread(void* args) {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    Args* arg = (Args*)args;
    int motor = arg->motor;
    int chip_enable;
    if(motor == MOTOR_B) chip_enable = CE0;
    else chip_enable = CE1;
    readCounter(chip_enable, motor);
}

/**************************************************************
Lidar thread is intended to take distance readings from the
Lidar unit.
***************************************************************/
void* lidarThread(void* args) {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    // unpack arguments
    // call readLidar mmm,m,mm,m
}

/**************************************************************
Drive thread is the main driving thread that handles the
maneuvering of the vehicle based on data read from sensors.
Operates in an infinite loop unless the car is deactivated.
Drive thread is the only thread that does not get canceled but
rather quits on its own.
***************************************************************/
void* driveThread(void* args) {
    int distanceThresh = 20; // the min distance we'll get to an obstacle ahead

    resume(); // initially start the car going straight at default speeds

    while(car_activated == ON) {
        // output all the sensors
        output();
        //printf("Obstacle A->D: %d  %d%d  %d |\t", obstacleIR_A, obstacleIR_B, obstacleIR_C, obstacleIR_D);
        //printf("Line sensors A->E: %d  %d%d%d  %d\n", onLine_A, onLine_B, onLine_C, onLine_D, onLine_E);
        //printf("Distance: %d cm  |  Side Distance: %d cm\n ", distance, side_distance);

        // if our middle sensors flag an obstacle, stop the motors
        if((obstacleIsAhead() == ON) || (distance <= distanceThresh)) {
            //printf("obstacle in way! stopping now...\n");
            stopMotor(MOTOR_A);
            stopMotor(MOTOR_B);
            delay(5000); // Wait 5 sec for obstacle to pass, if its mobile
            if((obstacleIsAhead() == ON) || (distance <= distanceThresh)) { // if we get to here, its a stationairy object
                goAround();
            }
        }
        else if((!obstacleIsAhead()) && (halted == OFF)){ // if no obstacle and we are fully on line, go
            resume();
        }
        correctHeading();
    }//while
}//driveThread

/**************************************************************
Distance thread is responsible for keeping track of the distance
readings between one echo sensor and the nearest obstacle in
front of it. It updates its respective global variable
***************************************************************/
void* distanceThread(void* args) {
    printf("starting distanceThread..\n");
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    clock_t start, end; // for timing the echo response
    double cpu_time_used, local_distance; // time elapsed and distance recorded
    int isFrontSensor;
    int trigger;
    int echo;

    // get arguments
    Args* arg = (Args*)args;
    isFrontSensor = arg->isFrontSensor;

    // if this is for front sensor use front sensor GPIO
    if(isFrontSensor) {
        trigger = TRIGGER;
        echo = ECHO;
    }
    // otherwise use side sensor GPIO
    else {
        trigger = TRIGGER_SIDE;
        echo = ECHO_SIDE;
    }

    // trigger an initial low pulse to settle the sensor
    digitalWrite(trigger, LOW);
    delay(2000); // wait 2 seconds
    
    while(1) {
        
        // trigger a high pulse
        digitalWrite(trigger, HIGH);
        delay(0.01); // wait 0.00001 seconds
        digitalWrite(trigger, LOW); // trigger a low pulse output
    
        // start timer 
        while(digitalRead(echo) == LOW) {
            //printf("echo = LOW\n");
            start = clock();
        }

        // stop timer
        while(digitalRead(echo) == HIGH) {
            //printf("echo = HIGH\n");
            end = clock();
        }

        // calculate duration
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; // duration = end time - start time
    
        // calculate distance
        local_distance = cpu_time_used * US_MULTIPLIER; // distance = duration * 17150

        // write distance to global shared distance variable
        if(isFrontSensor) {
            distance = (int)local_distance; // truncate distance for simplicity
            //printf("Distance in cm: %.2f\n", local_distance);
        }
        else {
            side_distance = (int)local_distance;
            //printf("Side_Distance in cm: %.2f\n", local_distance);
        }
        
        delay(100);
    }//while
    
}

