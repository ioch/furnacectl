#include "ProgramProfile.h"

uint8_t ProgramProfile::programShouldAdvance() {
  return !programIsDone &&
         (PHASE_SOAK == programStepPhase && 
          (millis() - stepStartTs) / 60000 > program[currentStep].minutesAtTarget);
}

void ProgramProfile::stepInit(uint16_t currTemp) {
  stepStartTemperature = currTemp;
  programStepPhase = PHASE_RAMP;
  stepStartTs = millis();
  lastProgramInvocationTs = millis();
}

uint8_t ProgramProfile::isMoreStepsLeft() {
  return currentStep + 1 < programStepsTotal;
}

uint8_t ProgramProfile::isTimeToRamp() {
  return PHASE_RAMP == programStepPhase && 
         millis() - lastProgramInvocationTs > PROGRAM_IMVOCATION_INTERVAL;
}

uint16_t ProgramProfile::computeSetpoint() {
  return stepStartTemperature + (millis() - stepStartTs) / (float) 60000 * program[currentStep].rampDegreesPerMinute;
}

uint8_t ProgramProfile::isTargetSetpointReached(uint16_t currTemp) {
  return currTemp >= program[currentStep].targetTemp;
}

void ProgramProfile::start(uint16_t currTemp) {
  currentStep = 0;
  programIsDone = 0;
  stepInit(currTemp);
}

void ProgramProfile::compute(uint16_t currTemp) {
  if(programShouldAdvance()) {
    if(isMoreStepsLeft()) {
      currentStep = currentStep + 1;
      stepInit(currTemp);
    } else {
      programIsDone = 1;
      programDoneCallback();
    }
  } else if(isTimeToRamp()){
    if(isTargetSetpointReached(currTemp)) {
      programStepPhase = PHASE_SOAK;
      stepStartTs = millis();
      setpointCallback(newSetpoint);
    } else {
      uint16_t newSetpoint = computeSetpoint();
      if(newSetpoint > program[currentStep].targetTemp) {
        newSetpoint = program[currentStep].targetTemp;
      }
      setpointCallback(newSetpoint);
      lastProgramInvocationTs = millis();
    }
  } 
}

/**
5°/h iki 120 
prie 120 30min
išjungti
**/
