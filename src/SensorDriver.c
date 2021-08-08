// #include <stdio.h>
// #include <stdlib.h>
// #include <pthread.h>
// #include "Sensors.h"
// #include <wiringPi.h>
#include "Include.h"


int main(void) {
    //initialize
    init();

    // declare all threading variables
    pthread_t line_A, line_B, line_C, line_D, line_E; // line sensor threads
    pthread_t obs_A, obs_B, obs_C, obs_D;             // ir sensor threads
    pthread_t output;                                 // debug output thread
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    printf("Hello world from Raspberry Pi!\n");
    printf("Testing line sensors A-E\n");

    // declare arg structs for sensors A->E
    Args args_A;
    Args args_B;
    Args args_C;
    Args args_D;
    Args args_E;

    // initialize arguments to each struct
    args_A.lineSensor = LINESENSOR_A;
    args_B.lineSensor = LINESENSOR_B;
    args_C.lineSensor = LINESENSOR_C;
    args_D.lineSensor = LINESENSOR_D;
    args_E.lineSensor = LINESENSOR_E;
    args_A.irSensor = IRSENSOR_A;
    args_B.irSensor = IRSENSOR_B;
    args_C.irSensor = IRSENSOR_C;
    args_D.irSensor = IRSENSOR_D;


    // create all line sensor threads
    int createA = pthread_create(&line_A, NULL, &lineThread, (void*)&args_A);
    int createB = pthread_create(&line_B, NULL, &lineThread, (void*)&args_B);
    int createC = pthread_create(&line_C, NULL, &lineThread, (void*)&args_C);
    int createD = pthread_create(&line_D, NULL, &lineThread, (void*)&args_D);
    int createE = pthread_create(&line_E, NULL, &lineThread, (void*)&args_E);
    int createF = pthread_create(&output, NULL, &outputThread, NULL);
    int createG = pthread_create(&obs_A, NULL, &irThread, (void*)&args_A);
    int createH = pthread_create(&obs_B, NULL, &irThread, (void*)&args_B);
    int createI = pthread_create(&obs_C, NULL, &irThread, (void*)&args_C);
    int createJ = pthread_create(&obs_D, NULL, &irThread, (void*)&args_D);

    // handle errors from pthread_create
    if(createA != 0) printf("Error creating line_A thread!\n");
    if(createB != 0) printf("Error creating line_B thread!\n");
    if(createC != 0) printf("Error creating line_C thread!\n");
    if(createD != 0) printf("Error creating line_D thread!\n");
    if(createE != 0) printf("Error creating line_E thread!\n");
    if(createF != 0) printf("Error creating output thread!\n");
    if(createG != 0) printf("Error creating obs_A thread!\n");
    if(createH != 0) printf("Error creating obs_B thread!\n");
    if(createI != 0) printf("Error creating obs_C thread!\n");
    if(createJ != 0) printf("Error creating obs_D thread!\n");

    // join all our line sensor threads
    int joinA = pthread_join(line_A, NULL);
    int joinB = pthread_join(line_B, NULL);
    int joinC = pthread_join(line_C, NULL);
    int joinD = pthread_join(line_D, NULL);
    int joinE = pthread_join(line_E, NULL);
    int joinF = pthread_join(output, NULL);
    int joinG = pthread_join(obs_A, NULL);
    int joinH = pthread_join(obs_B, NULL);
    int joinI = pthread_join(obs_C, NULL);
    int joinJ = pthread_join(obs_D, NULL);

    // handle errors from join
    if(joinA != 0) printf("Error joining line_A thread!\n");
    if(joinB != 0) printf("Error joining line_B thread!\n");
    if(joinC != 0) printf("Error joining line_C thread!\n");
    if(joinD != 0) printf("Error joining line_D thread!\n");
    if(joinE != 0) printf("Error joining line_E thread!\n");
    if(joinF != 0) printf("Error joining output thread!\n");
    if(joinG != 0) printf("Error joining obs_A thread!\n");
    if(joinH != 0) printf("Error joining obs_B thread!\n");
    if(joinI != 0) printf("Error joining obs_C thread!\n");
    if(joinJ != 0) printf("Error joining obs_D thread!\n");

    printf("All threads successfully terminated. \n");
    
    //free(data);
    return 0;

}