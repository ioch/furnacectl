#ifndef __MODEL_H
#define __MODEL_H

typedef enum {
    STATE_IDLE,
    STATE_SETPOINT,
    STATE_PROGRAM,
    STATE_MANUAL
} State_t;

typedef struct {
    int16_t tempCurr;
    int16_t tempSetpoint;

    uint8_t pidP;
    uint8_t pidI;
    uint8_t pidD;
    State_t state;
} Model_t;

Model_t model = {0, 115, 10, 20, 30, STATE_IDLE};

const char * const states[1][4] = {{"IDLE", "SETPOINT", "PROGRAM", "MANUAL"}};

#endif