#define LED_ON LOW
#define LED_OFF HIGH
#define PIN_SW1 3
#define PIN_SW2 2
#define INTERVAL_UPDATE 1000
#define INTERVAL_LOOP 1
#define SERIAL_TX_MARGIN 13

void setup() {
  int dummy = 0;

  digitalWrite(LED_BUILTIN, LED_OFF);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_SW1, INPUT);
  pinMode(PIN_SW2, INPUT);

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect
  }
  usb_mhz14a_init();
  usb_mhz14a_co2_request();
  delay(100);
  usb_mhz14a_get_co2( &dummy );
/*
  if( digitalRead( PIN_SW1 ) ){
    digitalWrite(LED_BUILTIN, LED_ON);
    usb_mhz14a_zero_calibration();
    digitalWrite(LED_BUILTIN, LED_OFF);
  }
*/

  digitalWrite(LED_BUILTIN, LED_ON);
}

void loop() {
  int value = 0;
  int status = 0;
  static int interval_cnt = INTERVAL_UPDATE;

  if( 0 >= interval_cnt ){
    usb_mhz14a_co2_request();
  }
  else if( 0 == usb_mhz14a_co2_is_ready() ){
    status = usb_mhz14a_get_co2( &value );
    Serial.print("co2=");
    if( 0 == status ){
      Serial.print(value, DEC);
    }
    Serial.print(";status=");
    Serial.print(status);
    Serial.print("\n");
  }

  if( 0 >= interval_cnt ){
    interval_cnt = INTERVAL_UPDATE - SERIAL_TX_MARGIN;
  }
  interval_cnt -= INTERVAL_LOOP;

  delay( INTERVAL_LOOP );
}
