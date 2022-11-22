////
// Button Actions
//
// This library contains the actions to perform when the buttons and quad encoders are actuated.
////

#include "buttonActions.h"

// Other SiLabs headers
#include "pin_config.h"
#include "app_log.h"

// Project Libraries
#include "buttons.h"
#include "timers_HW.h"

// TODO: Quick and dirty test to see that everything is wired
// A button press will increase the counter of button presses and a button release will print a message
// A quad rotating will increase a count starting on 4000 if turned CW and decrease it if turned CCW.
// This all will be replaced, so I won't comment it further.

volatile uint32_t buttonCount = 0;
volatile uint32_t rotaryCount = 4000;

void gpioCallbackButton1(uint8_t intNo, void* ctx) {
    (void)intNo;
    // set debounce timer
    setDebounceTimerBnt1((button_t*)ctx);
    // set sampling timer
    setSamplingTimerBtn1((button_t*)ctx);
}

void gpioCallbackQuad1(uint8_t intNo, void* ctx) {
    (void)intNo;
    // Trivial, but makes it easier to read
    quad_encoder_t* quadPtr = (quad_encoder_t*) ctx;
    // The pin 0 has triggered a falling interrupt, read pin 1 to determine if we are CW or CCW
    if (readPin(quadPtr->pin1Port, quadPtr->pin1No) == 1) {
        quadPtr->clockWiseAction();
    } else {
        quadPtr->counterClockWiseAction();
    }
}

void button1Pressed(void) {
    buttonCount++;
}

void button1Released(void) {
    app_log_info("Btn1 Released\r\n");
}

void quad1ClockWise(void) {
    rotaryCount++;
}

void quad1CounterClockWise(void) {
    rotaryCount--;
}

uint32_t getRotary(void) {
    return rotaryCount;
}

uint32_t getButton(void) {
    return buttonCount;
}
