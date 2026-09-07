#ifndef ACCESS_CONTROL_H
#define ACCESS_CONTROL_H

#include <stdint.h>
#include "stm32f446xx.h"

typedef enum {
    ACCESS_STATE_IDLE = 0U,
    ACCESS_STATE_PERSON_DETECTED,
    ACCESS_STATE_AUTHENTICATION,
    ACCESS_STATE_ACCESS_GRANTED,
    ACCESS_STATE_ACCESS_DENIED,
    ACCESS_STATE_INTRUSION_ALERT
} AccessState_t;

uint8_t AccessControl_Init(void);
void AccessControl_Run(void);
AccessState_t AccessControl_GetState(void);

#endif
