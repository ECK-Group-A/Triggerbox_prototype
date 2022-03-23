#include <Arduino.h>

volatile uint16_t timer1_msb = 0;
volatile uint16_t timerval = 0;
volatile uint16_t degree = 0;
volatile uint16_t degrees = 0;
volatile uint16_t pad_decimal = 0;
volatile bool im_global = false;

const uint8_t high_time = 10;

const uint16_t camera_position[6] = {0, 120, 240, 0xFF, 0xFF, 0xFF};
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
  TCCR1B = (1 << CS10);   // Timer/counter 1 clock prescaler = 1
  TCCR1B |= (1 << ICES1); // Timer/counter 1 Input Capture on Rising Edge
  TIMSK1 |= (1 << ICIE1) | (1 << TOIE1); // Timer/counter 1  interrupt when input capture event occurs and when overflow
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
  uint32_t timerval = (timer1_msb << 16) | ICR1;
  timer1_msb = 0;
  TCNT1 = 0;
  TCNT3 = 0;
  degrees = 0;
  degree = timerval / 3600;
  pad_decimal = 3600 / (timerval % 3600);
  OCR3A = degree;
  update_outputs();
}

ISR(TIMER1_OVF_vect) {
  timer1_msb++;
}

ISR(TIMER3_COMPA_vect)
{
  ++degrees % pad_decimal ? OCR3A = degree : OCR3A = degree+1;
  update_outputs();
}

void update_outputs() {
  if (degrees < 3600) {
    uint16_t angle = degrees % 360;
    PORTF = ((angle >= camera_position[0] && angle < camera_position[0] + high_time) << camera_pin[0]) | ((angle >= camera_position[1] && angle < camera_position[1] + high_time) << camera_pin[1]) | ((angle >= camera_position[2] && angle < camera_position[2] + high_time) << camera_pin[2]) | ((angle >= camera_position[3] && angle < camera_position[3] + high_time) << camera_pin[3]) | ((angle >= camera_position[4] && angle < camera_position[4] + high_time) << camera_pin[4]) | ((angle >= camera_position[5] && angle < camera_position[5] + high_time) << camera_pin[5]);
  }
}
