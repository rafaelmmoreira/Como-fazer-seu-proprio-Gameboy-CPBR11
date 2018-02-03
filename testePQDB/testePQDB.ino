#include "keypad.h"
#include "lcd.h"

char key; 

void loopInput()
{
  for (int i = 0; i <= 100; i++)
  {
    if (key != kpReadKey())
    {
      key = kpReadKey();
    }
  }
}



void setup() {
  // Inicializando o hardware
  kpInit();
  lcdInit();
}

void loop() {
  loopInput();
  if (key)
    lcdChar(key);
}
