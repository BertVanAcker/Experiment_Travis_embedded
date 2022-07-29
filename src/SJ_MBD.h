#include <Arduino.h>

/***********************************************************************************************************************
*                                          DEVICE SPECIFIC DEFINES
***********************************************************************************************************************/
#define DEVICE_ID ARDUINO_ID

#define AT_ANALOG 3
#define AT_MODE_NOT_SET 255
#define MAX_DIGITAL_PINS_SUPPORTED 20
#define MAX_ANALOG_PINS_SUPPORTED 15


/*
*   Pins unknown to the board
*/

#ifndef A8
#define A8 2047
#endif

#ifndef A9
#define A9 2047
#endif

#ifndef A10
#define A10 2047
#endif

#ifndef PIN_A11
#define A11 2047
#endif

#ifndef PIN_A12
#define A12 2047
#endif

#ifndef PIN_A13
#define A13 2047
#endif

#ifndef PIN_A14
#define A14 2047
#endif

#ifndef PIN_A15
#define A15 2047
#endif


/***********************************************************************************************************************
*                                          GENERAL DEFINES
***********************************************************************************************************************/
#define MAX_COMMAND_LENGTH 30

unsigned long current_millis;  // for analog input loop
unsigned long previous_millis; // for analog input loop
uint8_t analog_sampling_interval = 19;


/***********************************************************************************************************************
*                                          STEAM-JACK DEFINES
***********************************************************************************************************************/
#define FIRMWARE_MAJOR 0
#define FIRMWARE_MINOR 1

#define ARDUINO_ID 1
#define MICROBITv1_ID 2
#define MICROBITv2_ID 3
#define RPI_PICO 4

#define SERIAL_BAUD 115200

//function IDs
#define SERIAL_LOOP_BACK 0
#define SET_PIN_MODE 1
#define DIGITAL_WRITE 2
#define ANALOG_WRITE 3
#define GET_FIRMWARE_VERSION 4
#define ARE_U_THERE 5
#define CUSTOM_FUNCTION 6

//reporting IDs
#define DIGITAL_REPORT 2
#define ANALOG_REPORT 3
#define FIRMWARE_REPORT 5
#define I_AM_HERE 6
#define DEBUG_PRINT 99


/***********************************************************************************************************************
*                                           Data Structures
***********************************************************************************************************************/
struct command_descriptor
{
    // API function pointer
    void (*command_func)(void);
};

// a descriptor for digital pins
struct pin_descriptor
{
    byte pin_number;
    byte pin_mode;
    bool reporting_enabled; // If true, then send reports if an input pin
    int last_value;         // Last value read for input mode
};

// a descriptor for digital pins
struct analog_pin_descriptor
{
    byte pin_number;
    byte pin_mode;
    bool reporting_enabled; // If true, then send reports if an input pin
    int last_value;         // Last value read for input mode
    int differential;       // difference between current and last value needed
                            // to generate a report
};
