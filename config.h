/************************ Adafruit IO Config *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME  "SusanGuevara2003"
#define IO_KEY       "aio_PijP33Db47dzCu365oG0iJnSHKUg"

/******************************* WIFI **************************************/

#define WIFI_SSID "CLARO1_2B16D8"
#define WIFI_PASS "936f9wiybo"

#include "AdafruitIO_WiFi.h"

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);