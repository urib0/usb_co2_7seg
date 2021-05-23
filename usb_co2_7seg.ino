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
#define INTERVAL_LOOP 10
#define SW_LONG_PUSH_TIME 3
#define THRESHOLD 800
#define SERIAL_TX_MARGIN 13
#define STRING_MAX 4
#define RESULT_OK 0
#define RESULT_NG 1

typedef unsigned char uchar;
typedef unsigned int uint;

TM1637 tm = TM1637(PIN_CLK, PIN_DIO);

static char co2_strings[STRING_MAX + 1] = {0};

void setup()
{
    int co2_dummy = 0;

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
    usb_mhz14a_get_co2(&co2_dummy);
    digitalWrite(LED_BUILTIN, LED_ON);
}

void loop()
{
    static int co2_value = 0;
    static int co2_status = 0;
    static int interval_cnt = INTERVAL_UPDATE;

    // ポーリング処理
    if (0 == usb_mhz14a_co2_is_ready())
    {
        co2_status = usb_mhz14a_get_co2(&co2_value);
        if (0 == co2_status)
        {
            writeIndicator(oo2_value);
        }
    }

    // インターバル処理
    if (0 >= interval_cnt)
    {
        writeSerial(co2_value, co2_status);
        writeDisplay(co2_value);
        usb_mhz14a_co2_request();
        if (RESULT_OK == check_sw_long_push(PIN_SW1))
        {
            digitalWrite(PIN_LED, LED_ON);
        }
        else
        {
            digitalWrite(PIN_LED, LED_OFF);
        }
    }

    if (0 >= interval_cnt)
    {
        interval_cnt = INTERVAL_UPDATE - SERIAL_TX_MARGIN;
    }
    interval_cnt -= INTERVAL_LOOP;

    delay(INTERVAL_LOOP);
}

void writeIndicator(int co2_value)
{
    if (co2_value < THRESHOLD)
    {
        digitalWrite(PIN_LED, LED_OFF);
    }
    else
    {
        digitalWrite(PIN_LED, LED_ON);
    }
}

void writeSerial(int co2_val, int co2_status)
{
    Serial.print("co2=");
    Serial.print(co2_val, DEC);
    Serial.print(";status=");
    Serial.print(co2_status);
    Serial.print("\n");
}

void writeDisplay(int co2_val)
{
    uchar string_size = 0;

    int_to_char_arr(co2_val, co2_strings, &string_size);
    tm.clearDisplay();
    for (uchar uc_loop_cnt = 0; uc_loop_cnt < string_size; uc_loop_cnt++)
    {
        tm.setDigit(STRING_MAX - uc_loop_cnt - 1, co2_strings[uc_loop_cnt] - 0x30, false);
    }
    buff_clr(co2_strings, STRING_MAX + 1);
}

int check_sw_long_push(int pin)
{
    static int long_push_cnt = SW_LONG_PUSH_TIME;
    int ret = RESULT_NG;

    if (SW_ON == digitalRead(pin))
    {
        long_push_cnt--;
    }
    else
    {
        long_push_cnt = SW_LONG_PUSH_TIME;
    }

    if (0 >= long_push_cnt)
    {
        ret = RESULT_OK;
    }

    return ret;
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