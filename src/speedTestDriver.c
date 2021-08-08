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
// #include <pthread.h>

// #include "Speedometer.h"
#include "Include.h"

typedef struct Args {
    int motor;
} Args;

void* motor(void*);
void* speed(void*);

int main() {
    speedometer_init();
    motor_init();

    Args args1, args2;
    args1.motor = MOTOR_A;
    args2.motor = MOTOR_B;

    pthread_t motorA, motorB, speedA, speedB;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); 

    // start motor thread
    // int ret1 = pthread_create(&motorA, &attr, &motor, (void*)&args1);
    // if(ret1 != 0) {
    //     printf("error: Cannot create thread1!\n");
    // }
    // int ret2 = pthread_create(&speedA, &attr, &speed, (void*)&args1);
    // if(ret2 != 0) {
    //     printf("error: Cannot create thread2!\n");
    // }
    int ret3 = pthread_create(&motorB, &attr, &motor, (void*)&args2);
    if(ret3 != 0) {
        printf("error: Cannot create thread2!\n");
    }
    int ret4 = pthread_create(&speedB, &attr, &speed, (void*)&args2);
    if(ret4 != 0) {
        printf("error: Cannot create thread2!\n");
    }


    // // join our threads
    // int join1 = pthread_join(motorA, NULL);
    // int s = pthread_cancel(speedA);
    // int join2 = pthread_join(speedA, NULL);
    // if (join1 != 0) {
    //     printf("error %d: Cannot join thread1\n", join1);
    // }
    // if (join2 != 0) {
    //     printf("error %d: Cannot join thread2\n", join2);
    // }

    // join our threads
    int join1 = pthread_join(motorB, NULL);
    int s = pthread_cancel(speedB);
    int join2 = pthread_join(speedB, NULL);
    if (join1 != 0) {
        printf("error %d: Cannot join thread1\n", join1);
    }
    if (join2 != 0) {
        printf("error %d: Cannot join thread2\n", join2);
    }

    return 0;
}

void* motor(void* ptr) {
    Args* args = (Args*) ptr;
    int motor = args->motor;

    printf("Beginning tests for MOTOR_");
    if(motor == MOTOR_A) printf("A...\n");
    else printf("B...\n");

    // Test Case 1: accelerate to max speed forwards
    if(accelerate(motor, MAX_FREQ, FORWARD) != 1)
        printf("Test Case 1: FAIL\n");
    else printf("Test Case 1: PASS\n");
    delay(1000);

    // Test Case 2: decelerate from max speed to 50%
    if(decelerate(motor, 50, FORWARD) != 1)
        printf("Test Case 2: FAIL\n");
    else printf("Test Case 2: PASS\n"); 
    delay(1000);       

    // Test Case 3: accelerate to 75% forwards
    if(accelerate(motor, 75, FORWARD) != 1)
        printf("Test Case 3: FAIL\n");
    else printf("Test Case 3: PASS\n");
    delay(1000);

    // Test Case 4: attempt rapid direction change to 75% backwards (should fail)
    if(accelerate(motor, 75, BACKWARD) == 1)
        printf("Test Case 4: FAIL\n");
    else printf("Test Case 4: PASS\n");
    delay(1000);

    // Test Case 5: decelerate to 2% speed and then stop
    if(decelerate(motor, 2, FORWARD) != 1)
        printf("Test Case 5: FAIL\n");
    else {
        stopMotor(motor);
        printf("Test Case 5: PASS\n");
    }
    delay(1000);

    // Test Case 6: direction change to backwards from 0 speed to 25%
    if(accelerate(motor, 25, BACKWARD) != 1)
        printf("Test Case 6: FAIL\n");
    else printf("Test Case 6: PASS\n");
    delay(1000);

    // Test Case 7: accelerate to 100% backwards 
    if(accelerate(motor, MAX_FREQ, BACKWARD) != 1)
        printf("Test Case 7: FAIL\n");
    else printf("Test Case 7: PASS\n");
    delay(1000);

    // Test Case 8: rapid stop from 100%
    stopMotor(motor);

    printf("\nTesting Complete.\n");
}

void* speed(void* ptr) {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    Args* args = (Args*) ptr;
    int motor = args->motor;
    int chip_enable;
    if(motor == MOTOR_B) chip_enable = CE0;
    else chip_enable = CE1;
    readCounter(chip_enable);
}