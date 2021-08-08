/**************************************************************
* Class: CSC-615-01 Spring 2020
* Name: Jennifer Finaldi
* Student ID: 920290420
* Github ID: jfinaldi
* Project: Drive On
*
* File: motorTestDriver.c
*
* Description: Test driver for motor.c and motor.h files
*
**************************************************************/
// #include <stdio.h>      
// #include <stdlib.h>

// #include "Motor.h"
#include "Include.h"

int main(int argc, char** argv) {

    motor_init();

    // This test uses only MOTOR_A
    printf("Beginning tests for MOTOR_A...\n");

    // Test Case 1: accelerate to max speed forwards
    if(accelerate(MOTOR_A, MAX_FREQ, FORWARD) != 1)
        printf("Test Case 1: FAIL\n");
    else printf("Test Case 1: PASS\n");
    delay(1000);

    // Test Case 2: decelerate from max speed to 50%
    if(decelerate(MOTOR_A, 50, FORWARD) != 1)
        printf("Test Case 2: FAIL\n");
    else printf("Test Case 2: PASS\n"); 
    delay(1000);       

    // Test Case 3: accelerate to 75% forwards
    if(accelerate(MOTOR_A, 75, FORWARD) != 1)
        printf("Test Case 3: FAIL\n");
    else printf("Test Case 3: PASS\n");
    delay(1000);

    // Test Case 4: attempt rapid direction change to 75% backwards (should fail)
    if(accelerate(MOTOR_A, 75, BACKWARD) == 1)
        printf("Test Case 4: FAIL\n");
    else printf("Test Case 4: PASS\n");
    delay(1000);

    // Test Case 5: decelerate to 2% speed and then stop
    if(decelerate(MOTOR_A, 2, FORWARD) != 1)
        printf("Test Case 5: FAIL\n");
    else {
        stopMotor(MOTOR_A);
        printf("Test Case 5: PASS\n");
    }
    delay(1000);

    // Test Case 6: direction change to backwards from 0 speed to 25%
    if(accelerate(MOTOR_A, 25, BACKWARD) != 1)
        printf("Test Case 6: FAIL\n");
    else printf("Test Case 6: PASS\n");
    delay(1000);

    // Test Case 7: accelerate to 100% backwards 
    if(accelerate(MOTOR_A, MAX_FREQ, BACKWARD) != 1)
        printf("Test Case 7: FAIL\n");
    else printf("Test Case 7: PASS\n");
    delay(1000);

    // Test Case 8: rapid stop from 100%
    stopMotor(MOTOR_A);

    printf("\nTesting Complete.\n");

    return 0;
}