#include <Arduino.h>

#include "StatusLeds.h"
#include "Common.h"

/*
 * init GPIO to control attached leds
 */
void initStatusLeds() {
  pinMode(STATUS_LED_WIFI   , OUTPUT);
  pinMode(STATUS_LED_MQTT   , OUTPUT);
  pinMode(STATUS_LED_IN     , OUTPUT);
  pinMode(STATUS_LED_OUT    , OUTPUT);

  pinMode(13, INPUT);
  pinMode(15, OUTPUT);

  controlStatusLed(13,LOW);
  controlStatusLed(15,LOW);

  controlStatusLed(STATUS_LED_WIFI, LOW);
  controlStatusLed(STATUS_LED_MQTT, LOW);
  controlStatusLed(STATUS_LED_IN  , LOW);
  controlStatusLed(STATUS_LED_OUT , LOW);
}


/*
 * set state of given Led's pin
 */
void controlStatusLed(int pin, int state) {
  digitalWrite(pin, state);
}
