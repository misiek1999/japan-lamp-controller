#include <FastLED.h>
#include <Arduino.h>

// LED Matrix Settings
#define LED_PIN     D4        // Data pin connected to WS2812B
#define MATRIX_WIDTH  8
#define MATRIX_HEIGHT 8
#define NUM_LEDS    (MATRIX_WIDTH * MATRIX_HEIGHT)
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS  100        // 0-255

// Button Settings
#define BUTTON_PIN  D2        // Button pin (connect to GND when pressed)
#define DEBOUNCE_DELAY 50     // Debounce time in milliseconds

// LED Array
CRGB leds[NUM_LEDS];

// Pattern Variables
int currentPattern = 0;
const int NUM_PATTERNS = 8;   // Total number of patterns

// Button Variables
bool lastButtonState = HIGH;
bool buttonState = HIGH;
unsigned long lastDebounceTime = 0;

// Function Prototypes
void checkButton();
int XY(int x, int y);
void solidColor(CRGB color);
void rainbowPattern();
void movingDot();
void checkerboard();
void colorWave();
void randomSparkle();

void setup() {
  Serial.begin(115200);

  // Initialize LED Matrix
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();

  // Initialize Button
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.println("WS2812B 8x8 Matrix Ready!");
}

void loop() {
  // Check button state
  checkButton();

  // Display current pattern
  switch(currentPattern) {
    case 0:
      solidColor(CRGB::Red);
      break;
    case 1:
      solidColor(CRGB::Green);
      break;
    case 2:
      solidColor(CRGB::Blue);
      break;
    case 3:
      rainbowPattern();
      break;
    case 4:
      movingDot();
      break;
    case 5:
      checkerboard();
      break;
    case 6:
      colorWave();
      break;
    case 7:
      randomSparkle();
      break;
  }

  FastLED.show();
  delay(50);
}

// Button Handler with Debouncing
void checkButton() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading != buttonState) {
      buttonState = reading;

      // Button pressed (LOW because of INPUT_PULLUP)
      if (buttonState == LOW) {
        currentPattern = (currentPattern + 1) % NUM_PATTERNS;
        Serial.print("Pattern changed to: ");
        Serial.println(currentPattern);
        FastLED.clear();
      }
    }
  }

  lastButtonState = reading;
}

// Helper function to get LED index from X,Y coordinates
int XY(int x, int y) {
  // Adjust this based on your matrix wiring
  // This is for zigzag wiring pattern
  int index;
  if (y % 2 == 0) {
    // Even rows go left to right
    index = y * MATRIX_WIDTH + x;
  } else {
    // Odd rows go right to left
    index = y * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - x);
  }
  return index;
}

// ========== PATTERN FUNCTIONS ==========

// Pattern 0-2: Solid Colors
void solidColor(CRGB color) {
  fill_solid(leds, NUM_LEDS, color);
}

// Pattern 3: Rainbow Pattern
void rainbowPattern() {
  static uint8_t hue = 0;
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue + (i * 4), 255, 255);
  }
  hue++;
}

// Pattern 4: Moving Dot
void movingDot() {
  static int pos = 0;
  FastLED.clear();

  int x = pos % MATRIX_WIDTH;
  int y = pos / MATRIX_WIDTH;
  leds[XY(x, y)] = CRGB::White;

  pos = (pos + 1) % NUM_LEDS;
}

// Pattern 5: Checkerboard
void checkerboard() {
  static bool toggle = false;
  static unsigned long lastToggle = 0;

  if (millis() - lastToggle > 500) {
    toggle = !toggle;
    lastToggle = millis();
  }

  for(int y = 0; y < MATRIX_HEIGHT; y++) {
    for(int x = 0; x < MATRIX_WIDTH; x++) {
      bool isEven = (x + y) % 2 == 0;
      if (isEven == toggle) {
        leds[XY(x, y)] = CRGB::Purple;
      } else {
        leds[XY(x, y)] = CRGB::Orange;
      }
    }
  }
}

// Pattern 6: Color Wave
void colorWave() {
  static uint8_t hue = 0;
  for(int y = 0; y < MATRIX_HEIGHT; y++) {
    for(int x = 0; x < MATRIX_WIDTH; x++) {
      leds[XY(x, y)] = CHSV(hue + (x * 10) + (y * 10), 255, 255);
    }
  }
  hue += 2;
}

// Pattern 7: Random Sparkle
void randomSparkle() {
  // Fade all LEDs
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i].fadeToBlackBy(20);
  }

  // Add random sparkles
  if(random(10) > 7) {
    leds[random(NUM_LEDS)] = CHSV(random(255), 255, 255);
  }
}