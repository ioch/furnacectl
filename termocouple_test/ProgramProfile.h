#ifndef __PROGRAM_PROFILE
#define __PROGRAM_PROFILE

#if (ARDUINO < 100)
#include "WProgram.h"
#else
#include "Arduino.h"
#endif

#define STEP_NONE 255
#define PROGRAM_IMVOCATION_INTERVAL 1000

typedef enum{
  PHASE_RAMP,
  PHASE_SOAK
} ProgramStepPhase_t;

typedef struct{
  float rampDegreesPerMinute;
  float targetTemp;
  uint16_t minutesAtTarget;
} ProgamStep_t;

class ProgramProfile
{
public:
	ProgramProfile(void (*spCallback)(uint16_t), void (*pdCallback) (void)) {
		setpointCallback = spCallback;
		programDoneCallback = pdCallback;
	}
	void start(float currTemp);
	void compute(float currTemp);

private:
	void (*setpointCallback)(float);
	void (*programDoneCallback) (void);
	float stepStartTemperature = 0;
	ProgramStepPhase_t programStepPhase = PHASE_RAMP;
	uint8_t currentStep = STEP_NONE;

	uint32_t lastProgramInvocationTs = 0;
	uint32_t stepStartTs = 0;
	uint8_t programIsDone = 0;
	ProgamStep_t program[1] = {{1.5, 120, 30}}; //{{0.83, 120, 30}}
	uint8_t programStepsTotal = sizeof(program) / sizeof(program[0]);

	uint8_t programShouldAdvance();
	void stepInit(float currTemp);
	uint8_t isMoreStepsLeft();
	uint8_t isTimeToRamp();
	float computeSetpoint();
	uint8_t isTargetSetpointReached(uint16_t currTemp);
};

#endif
