#include "access_control.h"

int main(void)
{
    if (AccessControl_Init() != DRIVER_OK) {
        for (;;) {
            /* Initialization failure: remain safely stopped. */
        }
    }

    for (;;) {
        AccessControl_Run();
    }
}
