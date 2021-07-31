#include <Arduino.h>

#include "davis6410.h"


// Create the wind meter for reading the 6410 using the default pins and sample time.
davis6410 wind_meter;



void log_wind_sample(float mph, int direction) {
  Serial.print(F("wind speed: mph="));
  Serial.print(mph);
  Serial.print(F(", direction="));
  Serial.println(direction);

  // Start another wind sample off.
  wind_meter.start_sample(log_wind_sample);
}

void setup() {

  Serial.begin(9600);

  Serial.println(F(""));
  Serial.println(F("Davis 6410 ==> TX20 Bridge v1.0"));
  Serial.println(F(""));

  // The 6410 interface must be initialised before use.
  wind_meter.initialise();

  // Start the 1st wind sample off.
  // From then off samples are auto triggered.
  wind_meter.start_sample(log_wind_sample);
}

void loop() {

  // Service the 6410 interface.
  // This needs to be done periodically as fast as possible.
  wind_meter.service();
}