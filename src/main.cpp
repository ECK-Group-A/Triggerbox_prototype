#include <Arduino.h>

volatile uint16_t timerval = 0;
volatile uint16_t degrees = 0;
volatile bool im_global = false;

void setup()
{
  cli(); // Disable global interrupts

  /////////////
  // TIMER 1 //
  /////////////

  DDRD &= ~(1 << 4);      // Set PD4 input
  TCCR1A = 0x00;          // Timer/counter 1 normal mode
  TCCR1B = (1 << CS12);   // Timer/counter 1 clock prescaler = 256
  TCCR1B |= (1 << ICES1); // Timer/counter 1 Input Capture on Rising Edge
  TIMSK1 |= (1 << ICIE1); // Timer/counter 1  interrupt when input capture event occurs
  TCNT1 = 0;              // Timer/counter 1 reset to zero

  /////////////
  // TIMER 3 //
  /////////////

  TCCR3A = 0x00;           // Timer/counter 3 (mode 4) UP to OCR3A
  TCCR3B = (1 << CS30);    // Timer/counter 3 clock prescaler = 1
  TCCR3B |= (1 << WGM32);  // Timer/counter 3 (mode 4) UP to OCR3A
  TIMSK3 |= (1 << OCIE3A); // Timer/counter 3 interrupt interrupt whenever (TCNT3 matches OCR3A)
  TCNT3 = 0;               // Timer/counter 3 reset to zero

  sei(); // Free the interrupts!

  Serial.begin(9600);
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (im_global == true)
  {
    Serial.println("Hi");
    im_global = false;
  }
}

ISR(TIMER1_CAPT_vect)
{
  TCNT1 = 0;
  OCR3A = (uint32_t)ICR1 * 256 / 3600;
  TCNT3 = 0;
}

ISR(TIMER3_COMPA_vect)
{
  if (degrees < 359)
  {
    degrees++;
  }
  else
  {
    degrees = 0;
    im_global = true;
  }
}