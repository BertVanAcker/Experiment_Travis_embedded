
#include "SJ_MBD.h"

/***************************************************
 *              HELPER VARIABLES
 **************************************************/
bool stop_reports = false; // a flag to stop sending all report messages
// buffer to hold incoming command data
byte command_buffer[MAX_COMMAND_LENGTH];

/***************************************************
 *              DEVICE SPECIFIC VARIABLES
 **************************************************/
// integer2pin translation
int analog_read_pins[20] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15};
// Array of digital_pin_descriptors
pin_descriptor the_digital_pins[MAX_DIGITAL_PINS_SUPPORTED];
// Array of analog_pin_descriptors
analog_pin_descriptor the_analog_pins[MAX_ANALOG_PINS_SUPPORTED];

/***************************************************
 *              EXTERNAL API FUNCTIONS
 **************************************************/

extern void serial_loopback();

extern void set_pin_mode();

extern void digital_write();

extern void analog_write();

extern void get_firmware_version();

extern void are_you_there();

extern void custom_function();

//add function + extend command_table[] with 1
command_descriptor command_table[7] =       
    {
        {&serial_loopback},
        {&set_pin_mode},
        {&digital_write},
        {&analog_write},
        {&get_firmware_version},
        {&are_you_there},
        {&custom_function}
        
    };


/***************************************************
 *            API FUNCTION IMPLEMENTATION
 **************************************************/
void get_firmware_version()
{
    byte report_message[4] = {3, FIRMWARE_REPORT, FIRMWARE_MAJOR, FIRMWARE_MINOR};
    Serial.write(report_message, 4);
}

void are_you_there()
{
    byte report_message[3] = {2, I_AM_HERE, DEVICE_ID};
    Serial.write(report_message, 3);
}

void send_debug_info(byte id, int value)
{
    byte debug_buffer[5] = {(byte)4, (byte)DEBUG_PRINT, 0, 0, 0};
    debug_buffer[2] = id;
    debug_buffer[3] = highByte(value);
    debug_buffer[4] = lowByte(value);
    Serial.write(debug_buffer, 5);
}

void serial_loopback()
{
    byte loop_back_buffer[3] = {2, (byte)SERIAL_LOOP_BACK, command_buffer[0]};
    Serial.write(loop_back_buffer, 3);
}

void set_pin_mode()
{
    byte pin;
    byte mode;
    pin = command_buffer[0];
    mode = command_buffer[1];

    switch (mode)
    {
    case INPUT:
        the_digital_pins[pin].pin_mode = mode;
        the_digital_pins[pin].reporting_enabled = command_buffer[2];
        pinMode(pin, INPUT);
        break;
    case INPUT_PULLUP:
        the_digital_pins[pin].pin_mode = mode;
        the_digital_pins[pin].reporting_enabled = command_buffer[2];
        pinMode(pin, INPUT_PULLUP);
        break;
    case OUTPUT:
        the_digital_pins[pin].pin_mode = mode;
        pinMode(pin, OUTPUT);
        break;
    case AT_ANALOG:
        the_analog_pins[pin].pin_mode = mode;
        the_analog_pins[pin].differential = (command_buffer[2] << 8) + command_buffer[3];
        the_analog_pins[pin].reporting_enabled = command_buffer[4];
        break;
    default:
        break;
    }
}

void digital_write()
{
    byte pin;
    byte value;
    pin = command_buffer[0];
    value = command_buffer[1];
    digitalWrite(pin, value);
}

void analog_write()
{
    byte pin;
    byte value;
    pin = command_buffer[0];
    value = command_buffer[1];
    analogWrite(pin, value);
}

void custom_function()
{
    byte report_message[3] = {2, I_AM_HERE, CUSTOM_FUNCTION};
    Serial.write(report_message, 3);
}

/***************************************************
 *       Auto device-state reporting
 **************************************************/
void auto_reporting()
{
    /* Analog read example */
    byte report_message[5] = {4, ANALOG_REPORT, 0, 0, 0};
    int value;
    value = analogRead(A0);
    report_message[2] = (byte)A0;
    report_message[3] = highByte(value); // get high order byte
    report_message[4] = lowByte(value);
    Serial.write(report_message, 5);
    delay(50);

}

/***************************************************
 *          INTERNAL FUNCTIONS
 **************************************************/
void get_next_command()
{
    byte command;
    byte packet_length;
    command_descriptor command_entry;

    // clear the command buffer
    memset(command_buffer, 0, sizeof(command_buffer));

    // if there is no command waiting, then return
    if (not Serial.available())
    {
        return;
    }
    // get the packet length
    packet_length = (byte)Serial.read();

    while (not Serial.available())
    {
        delay(1);
    }

    // get the command byte
    command = (byte)Serial.read();

    // uncomment the next line to see the packet length and command
    //send_debug_info(packet_length, command);
    command_entry = command_table[command];

    if (packet_length > 1)
    {
        // get the data for that command
        for (int i = 0; i < packet_length - 1; i++)
        {
            // need this delay or data read is not correct
            while (not Serial.available())
            {
                delay(1);
            }
            command_buffer[i] = (byte)Serial.read();
            // uncomment out to see each of the bytes following the command
            //send_debug_info(i, command_buffer[i]);
        }
    }
    command_entry.command_func();
}

void stop_all_reports(){
    stop_reports = true;
    delay(20);
    Serial.flush();
}


/***************************************************
 *          FIRMWARE SETUP AND EXECUTION
 **************************************************/
void setup()
{
    /* digital pin descriptor */
    for (byte i = 0; i < MAX_DIGITAL_PINS_SUPPORTED; i++)
    {
        the_digital_pins[i].pin_number = i;
        the_digital_pins[i].pin_mode = AT_MODE_NOT_SET;
        the_digital_pins[i].reporting_enabled = false;
        the_digital_pins[i].last_value = 0;
    }

    /* analog pin descriptor */
    for (byte i = 0; i < MAX_ANALOG_PINS_SUPPORTED; i++)
    {
        the_analog_pins[i].pin_number = i;
        the_analog_pins[i].pin_mode = AT_MODE_NOT_SET;
        the_analog_pins[i].reporting_enabled = false;
        the_analog_pins[i].last_value = 0;
    }
    /* auto-reporting setup */
    

    Serial.begin(SERIAL_BAUD);
}

void loop()
{
    // keep processing incoming commands
    get_next_command();

    if(! stop_reports){ // stop reporting
        //auto_reporting();
    }
}