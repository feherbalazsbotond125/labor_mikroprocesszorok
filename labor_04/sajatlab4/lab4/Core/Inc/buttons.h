#ifndef BUTTONS_H
#define BUTTONS_H

#include "main.h"

typedef enum
{
    BUTTON_NONE = 0,
    BUTTON_PREVIOUS,
    BUTTON_INVERT,
    BUTTON_NEXT
} ButtonEvent;

void Buttons_Init(void);
ButtonEvent Buttons_GetEvent(void);

#endif /* BUTTONS_H */
