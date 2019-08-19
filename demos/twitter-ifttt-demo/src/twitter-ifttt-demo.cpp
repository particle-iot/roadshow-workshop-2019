/*
 * Project twitter-ifttt-demo
 * Description: Demo of using IFTTT to Send a Tweet whenever I turn on my Grove LED
 * Author: Brandon Satrom <brandon@particle.io>
 * Date: 7/31/2019
 */
#include "Particle.h"
#include "Grove_ChainableLED.h"

ChainableLED leds(A4, A5, 1);

int toggleLed(String args);

void setup()
{
  leds.init();
  leds.setColorHSB(0, 0.0, 0.0, 0.0);

  Particle.function("toggleLed", toggleLed);
}

int toggleLed(String args)
{
  leds.setColorHSB(0, 0.0, 1.0, 0.5);

  Particle.publish("led-on", NULL, PRIVATE);

  delay(1000);

  leds.setColorHSB(0, 0.0, 0.0, 0.0);

  return 1;
}

void loop()
{
}