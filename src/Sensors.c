/**************************************************************
* Class: CSC-615-01 Spring 2020
* Name: Team Shartcar
* Student ID: 
* Github ID: 
* Project: Drive On
*
* File: Sensors.c
*
* Description: Implementation file for all sensor routines in
*           Sensors.h. Takes readings that are output from 
*           line and infrared sensors. 
*
**************************************************************/
// #include <stdio.h>
// #include <stdlib.h>
// #include <wiringPi.h>
// #include "Sensors.h"
// #include "DEV_Config.h"
#include "Include.h"

void sensors_init() {
    //DEV_ModuleInit();
    //pinMode(pin, INPUT);

    // Line Sensors
    pinMode(LINESENSOR_A, INPUT); // A, far left lateral
    pinMode(LINESENSOR_B, INPUT); // B, middle left
    pinMode(LINESENSOR_C, INPUT); // C, middle center
    pinMode(LINESENSOR_D, INPUT); // D, middle right
    pinMode(LINESENSOR_E, INPUT); // E, far right lateral

    // IR Sensors
    pinMode(IRSENSOR_A, INPUT); // A, far left lateral
    pinMode(IRSENSOR_B, INPUT); // B, middle left
    pinMode(IRSENSOR_C, INPUT); // C, middle right
    pinMode(IRSENSOR_D, INPUT); // D, far right lateral
}

void getIrSensorData(int sensor) {
    //sensors_init(IRSENSOR_A);
    //printf("Pin 1 Ir Sensor!\n");

    // labels for debugging output
    char s = 'Z';
    if(sensor == IRSENSOR_A) s = 'A';
    if(sensor == IRSENSOR_B) s = 'B';
    if(sensor == IRSENSOR_C) s = 'C';
    if(sensor == IRSENSOR_D) s = 'D';

    // Infinite loop to read IR sensor
    while(1) {
        // if low pulse from sensor, there is obstacle
        if(digitalRead(sensor) == 0) {
            //*isObstacle = 1;
            if(sensor == IRSENSOR_A) obstacleIR_A = 1;
            if(sensor == IRSENSOR_B) obstacleIR_B = 1;
            if(sensor == IRSENSOR_C) obstacleIR_C = 1;
            if(sensor == IRSENSOR_D) obstacleIR_D = 1;
            //printf("IRSENSOR_%c: OBSTACLE AHEAD\n", s);
            delay(100);
        }
        else {
            //*isObstacle = 0;
            if(sensor == IRSENSOR_A) obstacleIR_A = 0;
            if(sensor == IRSENSOR_B) obstacleIR_B = 0;
            if(sensor == IRSENSOR_C) obstacleIR_C = 0;
            if(sensor == IRSENSOR_D) obstacleIR_D = 0;
            //printf("IRSENSOR_%c: CLEAR\n", s);

            delay(100);
        }
    }
}

void getLineSensorData(int sensor) {
    //initSensors(LINESENSOR1);
    // int* lineDetection;
    // lineDetection = (int*) arg;

   // labels for debugging output
    char s = 'Z';
    if(sensor == LINESENSOR_A) s = 'A';
    if(sensor == LINESENSOR_B) s = 'B';
    if(sensor == LINESENSOR_C) s = 'C';
    if(sensor == LINESENSOR_D) s = 'D';
    if(sensor == LINESENSOR_E) s = 'E';

    while(1) {
        // if low pulse on sensor, we are over a line
        if(digitalRead(sensor) == 1) {
            //printf("LINESENSOR_%c is: ON\n", s);
            //*lineDetection = 1;
            if(sensor == LINESENSOR_A) onLine_A = 1;
            if(sensor == LINESENSOR_B) onLine_B = 1;
            if(sensor == LINESENSOR_C) onLine_C = 1;
            if(sensor == LINESENSOR_D) onLine_D = 1;
            if(sensor == LINESENSOR_E) onLine_E = 1;
            delay(100);
        }
        else {
            //printf("LINESENSOR_%c is: OFF\n", s);
            //*lineDetection = 0;
            if(sensor == LINESENSOR_A) onLine_A = 0;
            if(sensor == LINESENSOR_B) onLine_B = 0;
            if(sensor == LINESENSOR_C) onLine_C = 0;
            if(sensor == LINESENSOR_D) onLine_D = 0;
            if(sensor == LINESENSOR_E) onLine_E = 0;
            delay(100);
        }
    }
}
