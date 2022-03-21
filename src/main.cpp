#include <Arduino.h>

volatile uint16_t timerval = 0;
volatile uint16_t degrees = 0;
volatile bool im_global = false;

const uint8_t high_time = 10;

const uint16_t camera_position[6] = {0, 90, 180, 0xFF, 0xFF, 0xFF};
const uint16_t camera_pin[6] = {PINF0, PINF1, PINF4, PINF5, PINF6, PINF7};

void update_outputs();

void setup()
{
  DDRF = (1 << camera_pin[0]) | (1 << camera_pin[1]) | (1 << camera_pin[2]) | (1 << camera_pin[3]) | (1 << camera_pin[4]) | (1 << camera_pin[5]);

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
}

ISR(TIMER1_CAPT_vect)
{
  TCNT1 = 0;
  TCNT3 = 0;
  degrees = 0;
  update_outputs();
  OCR3A = (uint32_t)ICR1 * 256 / 3600;
}

ISR(TIMER3_COMPA_vect)
{
  degrees < 359 ? degrees++ : degrees = 0;
  update_outputs();
}

void update_outputs() {
  PORTF = ((degrees >= camera_position[0] && degrees < camera_position[0] + high_time) << camera_pin[0]) | ((degrees >= camera_position[1] && degrees < camera_position[1] + high_time) << camera_pin[1]) | ((degrees >= camera_position[2] &&  degrees < camera_position[2] + high_time) << camera_pin[2]) | ((degrees >= camera_position[3] && degrees < camera_position[3] + high_time) << camera_pin[3]) | ((degrees >= camera_position[4] && degrees < camera_position[4] + high_time) << camera_pin[4]) | ((degrees >= camera_position[5] && degrees < camera_position[5] + high_time) << camera_pin[5]);
}
