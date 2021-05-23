#include "TM1637.h"

#define LED_ON HIGH
#define LED_OFF LOW
#define SW_ON LOW
#define SW_OFF HIGH
#define PIN_LED 0
#define PIN_SW1 1
#define PIN_CLK 7
#define PIN_DIO 8
#define INTERVAL_UPDATE 1000
#define INTERVAL_LOOP 1
#define THRESHOLD 800
#define SERIAL_TX_MARGIN 13
#define STRING_MAX 4

typedef unsigned char uchar;
typedef unsigned int uint;

TM1637 tm = TM1637(PIN_CLK, PIN_DIO);

static char sc_strings[STRING_MAX + 1] = {0};

void setup()
{
    int dummy = 0;

    digitalWrite(LED_BUILTIN, LED_OFF);
    digitalWrite(PIN_LED, LED_OFF);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_SW1, INPUT);

    Serial.begin(9600);
    /*
    while (!Serial)
    {
        ; // wait for serial port to connect
    }
    */
    tm.init();

    usb_mhz14a_init();
    usb_mhz14a_co2_request();
    delay(100);
    usb_mhz14a_get_co2(&dummy);
    digitalWrite(LED_BUILTIN, LED_ON);
}

void loop()
{
    int value = 0;
    int status = 0;
    uchar arr_size = 0;
    static int interval_cnt = INTERVAL_UPDATE;

    if (0 >= interval_cnt)
    {
        usb_mhz14a_co2_request();
    }
    else if (0 == usb_mhz14a_co2_is_ready())
    {
        status = usb_mhz14a_get_co2(&value);
        Serial.print("co2=");
        if (0 == status)
        {
            if (value < THRESHOLD)
            {
                digitalWrite(PIN_LED, LED_OFF);
            }
            else
            {
                digitalWrite(PIN_LED, LED_ON);
            }
            Serial.print(value, DEC);
            int_to_char_arr(value, sc_strings, &arr_size);
            tm.clearDisplay();
            for (uchar uc_loop_cnt = 0; uc_loop_cnt < arr_size; uc_loop_cnt++)
            {
                tm.setDigit(STRING_MAX - uc_loop_cnt - 1, sc_strings[uc_loop_cnt] - 0x30, false);
            }
            buff_clr(sc_strings, STRING_MAX + 1);
        }
        Serial.print(";status=");
        Serial.print(status);
        Serial.print("\n");
    }

    if (0 >= interval_cnt)
    {
        interval_cnt = INTERVAL_UPDATE - SERIAL_TX_MARGIN;
    }
    interval_cnt -= INTERVAL_LOOP;

    delay(INTERVAL_LOOP);
}

void int_to_char_arr(int num, char *buff, uchar *size)
{
    if (num >= 1000)
    {
        buff[3] = (num / 1000) + 0x30;
        *size += 1;
    }
    if (num >= 100)
    {
        buff[2] = ((num / 100) % 10) + 0x30;
        *size += 1;
    }
    if (num >= 10)
    {
        buff[1] = ((num / 10) % 10) + 0x30;
        *size += 1;
    }
    if (num >= 0)
    {
        buff[0] = (num % 10) + 0x30;
        *size += 1;
    }
}

void buff_clr(char *buff, uchar cnt)
{
    for (uchar i = 0; i < cnt; i++)
    {
        buff[i] = 0;
    }
}