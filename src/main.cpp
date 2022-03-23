#include <Arduino.h>

// The current angle of the LIDAR, from 0 to 259 degrees.
volatile uint16_t angle = 0;
// The number of clock cycles in the duration of a degree.
volatile uint16_t degree = 0;
// The current number of degrees the LIDAR has moved.
volatile uint16_t degrees = 0;
// The number of degrees before an extra cycle must be added.
volatile uint16_t pad_decimal = 0;

// The number of "degrees" the trigger signal must be high.
const uint8_t high_time = 10;

// Camera positions in degrees, set to 0xFF when not in use.
const uint16_t camera_position[6] = {0, 120, 240, 0xFF, 0xFF, 0xFF};

// Pins for camera trigger:
// Pin A5, Pin A4, Pin A3, Pin A2, Pin A1, Pin A0
const uint16_t camera_pin[6] = {PINF0, PINF1, PINF4, PINF5, PINF6, PINF7};

void update_outputs();

void setup() {
  // Set the camera pins as output pins
  DDRF = (1 << camera_pin[0]) | (1 << camera_pin[1]) | (1 << camera_pin[2]) |
         (1 << camera_pin[3]) | (1 << camera_pin[4]) | (1 << camera_pin[5]);

  cli();  // Disable global interrupts

  /////////////
  // TIMER 1 //
  /////////////

  DDRD &= ~(1 << 4);       // Set PD4 as input capture input
  TCCR1A = 0x00;           // Timer/counter 1 normal mode
  TCCR1B = (1 << CS12);    // Timer/counter 1 clock prescaler = 256
  TCCR1B |= (1 << ICES1);  // Timer/counter 1 Input Capture on Rising Edge
  TIMSK1 |=
      (1
       << ICIE1);  // Timer/counter 1  interrupt when input capture event occurs
  TCNT1 = 0;       // Timer/counter 1 reset to zero

  /////////////
  // TIMER 3 //
  /////////////

  TCCR3A = 0x00;            // Timer/counter 3 (mode 4) UP to OCR3A
  TCCR3B = (1 << CS30);     // Timer/counter 3 clock prescaler = 1
  TCCR3B |= (1 << WGM32);   // Timer/counter 3 (mode 4) UP to OCR3A
  TIMSK3 |= (1 << OCIE3A);  // Timer/counter 3 interrupt interrupt whenever
                            // (TCNT3 matches OCR3A)
  TCNT3 = 0;                // Timer/counter 3 reset to zero

  sei();  // Free the interrupts!
}

void loop() {
  // Nothing to be done here.
}

ISR(TIMER1_CAPT_vect) {
  TCNT1 = 0;  // Set Timer 1 to zero
  TCNT3 = 0;  // Set Timer 3 to zero
  angle = 0;
  degrees = 0;
  update_outputs();
  // Calculate the number of clock cycles per degree of LIDAR rotation
  degree = (uint32_t)ICR1 * 256 / 3600;
  // Calculate interval in degrees where a clock cycle must be padded to
  // compensate for the division error.
  pad_decimal = 3600 / (((uint32_t)ICR1 * 256) % 3600);
  // Set Timer 3 compare match register A to the number of clock cycles per
  // degree.
  OCR3A = degree;
}

ISR(TIMER3_COMPA_vect) {
  // Increment degrees and add a clock cycle in necessary.
  ++degrees % pad_decimal ? OCR3A = degree : OCR3A = degree + 1;
  // Increment angle in range 0 to 359
  angle < 359 ? angle++ : angle = 0;
  update_outputs();
}

void update_outputs() {
  if (degrees < 3600) {
    PORTF =
        ((angle >= camera_position[0] && angle < camera_position[0] + high_time)
         << camera_pin[0]) |
        ((angle >= camera_position[1] && angle < camera_position[1] + high_time)
         << camera_pin[1]) |
        ((angle >= camera_position[2] && angle < camera_position[2] + high_time)
         << camera_pin[2]) |
        ((angle >= camera_position[3] && angle < camera_position[3] + high_time)
         << camera_pin[3]) |
        ((angle >= camera_position[4] && angle < camera_position[4] + high_time)
         << camera_pin[4]) |
        ((angle >= camera_position[5] && angle < camera_position[5] + high_time)
         << camera_pin[5]);
  }
}
