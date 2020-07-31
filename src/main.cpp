/**
 * OROBOROS AUDIO SYSTEMS
 * SwitcherOne
 * 
 * Copyright (C)2020
 * 
 * @file main.cpp
 * @author John V. Kamaly - hi@jvk.to
 */

#include <Arduino.h>
// #include <LiquidCrystal.h>
// #include <WinstarOLEDGFX.h>
#include <Adafruit_CharacterOLED.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////////////////
// Constant definitions
/////////////////////////////////////////////////////////////////////////////////////////

#define DEBUG_MODE_CHARS 0b0001
#define DEBUG_MODE_MODE1 0b0010
#define DEBUG_MODE_MODE2 0b0011 //etc
#define DEBUG_MODE 0b0000 | DEBUG_MODE_CHARS

#define CUST_PIXELS_PER_LINE  5
#define CUST_LINES_PER_CHAR   8  
#define CUST_DIGIT_COUNT      10 // Max no. of digits to choose from
#define CUST_DIGIT_CGRAM_MAX  8  // Max. 8 characters in CGRAM

#define count(ARRAY) (sizeof(ARRAY) / sizeof(*ARRAY))

// DUE Ports
#define   D7 13
#define   D6 12
#define   D5 11
#define   D4 10
#define   EN 7
#define   RW 6
#define   RS 5

// LiquidCrystal lcd(RS, RW, EN, D4, D5, D6, D7);

// WinstarOLEDGFX lcd(RS, RW, EN, D4, D5, D6, D7);
Adafruit_CharacterOLED lcd(OLED_V2, RS, RW, EN, D4, D5, D6, D7);

/////////////////////////////////////////////////////////////////////////////////////////
// Custom 5x8 OLED Character Patterns (CGRAM data)
//
// Bit position 7 6 5 4 3 2 1 0
//              - - - 1 1 1 1 0 etc  (- denotes not used, so only bits 0..4)
/////////////////////////////////////////////////////////////////////////////////////////

struct Digit {
    String digit_name;
    uint8_t digit[CUST_LINES_PER_CHAR];
};

struct DigitCollection {
    String collection_name;
    Digit digits[CUST_DIGIT_COUNT];
};

// Character (glyph) patterns
static struct DigitCollection customChars[] = {
  { "Nums", { 
    { "0", { 0b00000,  0b00000,  0b01110,  0b01010,  0b01010,  0b01010,  0b01110,  0b00000 }},
    { "1", { 0b00000,  0b00000,  0b00100,  0b00100,  0b00100,  0b00100,  0b00100,  0b00000 }},
    { "2", { 0b00000,  0b00000,  0b01110,  0b00010,  0b01110,  0b01000,  0b01110,  0b00000 }},
    { "3", { 0b00000,  0b00000,  0b01110,  0b00010,  0b00110,  0b00010,  0b01110,  0b00000 }},
    { "4", { 0b00000,  0b00000,  0b01010,  0b01010,  0b01110,  0b00010,  0b00010,  0b00000 }},
    { "5", { 0b00000,  0b00000,  0b01110,  0b01000,  0b01110,  0b00010,  0b01110,  0b00000 }},
    { "6", { 0b00000,  0b00000,  0b01110,  0b01000,  0b01110,  0b01010,  0b01110,  0b00000 }},
    { "7", { 0b00000,  0b00000,  0b01110,  0b00010,  0b00010,  0b00010,  0b00010,  0b00000 }},
    { "8", { 0b00000,  0b00000,  0b01110,  0b01010,  0b01110,  0b01010,  0b01110,  0b00000 }},
    { "9", { 0b00000,  0b00000,  0b01110,  0b01010,  0b01110,  0b00010,  0b01110,  0b00000 }}  
  }
  },
  { "LetterSm", {
    { "L", {  0b00000,  0b00000,  0b01000,  0b01000,  0b01000,  0b01000,  0b01110,  0b00000 }},
    { "O", {  0b00000,  0b00000,  0b01110,  0b01010,  0b01010,  0b01010,  0b01110,  0b00000 }},
    { "P", {  0b00000,  0b00000,  0b01110,  0b01010,  0b01110,  0b01000,  0b01000,  0b00000 }},
    { "S", {  0b00000,  0b00000,  0b01110,  0b01000,  0b01110,  0b00010,  0b01110,  0b00000 }}
  }
  },
  { "LetterLg", { 
    { "A", {  0b01110,  0b11011,  0b11011,  0b11011,  0b11111,  0b11011,  0b11011,  0b11011 }},
    { "B", {  0b11110,  0b11011,  0b11011,  0b11111,  0b11011,  0b11011,  0b11011,  0b11110 }},
    { "C", {  0b11110,  0b11011,  0b11000,  0b11000,  0b11000,  0b11000,  0b11011,  0b11110 }}
  }
  }
};

/////////////////////////////////////////////////////////////////////////////////////////
// Helper functions
/////////////////////////////////////////////////////////////////////////////////////////

Digit *getCharset(String collection_name)  {
  for (unsigned int i = 0; i < count(customChars); i++) {
    if (customChars[i].collection_name == collection_name)
      return customChars[i].digits;
  }

  return NULL;
}

uint8_t *invert_char(uint8_t digit[CUST_DIGIT_COUNT]) {
  // declare static as address of local pointer becomes garbage outside of this scope.
  static uint8_t temp_glyph[CUST_LINES_PER_CHAR]; 

  for (int i = 0; i < CUST_LINES_PER_CHAR; i++) {
    temp_glyph[i] = ~digit[i];
  }

  return temp_glyph;
}

/**
 * Creates custom digit and places it in CGRAN
 *
 * @param collection_name Name of character pattern collection to fetch pattern (glyphs) from
 * @param invert    Option to invert pixels
 * @param char_pos Index of character collection struct array to start fetching from
 * @param num_chars How many characters 
 * @param cgram_pos The Character Generator RAM (CGRAM) is used to generate custom 5x8 character patterns. 
 *                  There are 8 available addresses: CGRAM Address 0x00 through 0x07.
 *                  CGRAM is stored in positions 0x00 through 0x07 of the font table.
 * @return nothing
 */
void buildCustomChars(String collection_name, 
                      bool invert = false, 
                      int char_pos = 0, 
                      int num_chars = CUST_DIGIT_CGRAM_MAX, 
                      int cgram_pos = 0) {
  Digit *glyphs = getCharset(collection_name);

  int cgram_i = cgram_pos;

  for (int i = char_pos; (i < num_chars + char_pos) && (i < CUST_DIGIT_CGRAM_MAX + 1); i++) {

    if (DEBUG_MODE && DEBUG_MODE_CHARS) {
      Serial.print("\nCHAR_POS:");
      Serial.print(i);
      Serial.print("|DIGIT:");
      Serial.print(glyphs[i].digit_name);
      Serial.print("|");
      Serial.print("RAM:");
      Serial.print(cgram_i);
    }

    if (invert == true) {
      uint8_t *temp_glyph;      
      temp_glyph = invert_char(glyphs[i].digit);
      lcd.createChar(cgram_i, temp_glyph);
    } else {
      lcd.createChar(cgram_i, glyphs[i].digit);
    }
    cgram_i++;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
// Display Functions
/////////////////////////////////////////////////////////////////////////////////////////

void demoNums() {

  // numbers 1-8

  int char_pos,
      num_chars,
      cgram_pos,
      pos_x;

  char_pos = 1; num_chars = 5; cgram_pos = 0; pos_x = 16 - num_chars;
  buildCustomChars("Nums", false, char_pos, num_chars, cgram_pos);
  for (int i = 0; i < num_chars; i++) {
    lcd.setCursor(pos_x + i, 1);
    lcd.write(cgram_pos + i);
  }
  delay(1000);

  // /*
  char_pos = 0; num_chars = 3; cgram_pos = 5; pos_x = 0;
  buildCustomChars("LetterLg", false, char_pos, num_chars, cgram_pos);
  for (int i = 0; i < num_chars; i++) {
    lcd.setCursor(pos_x + i, 1);
    lcd.write(cgram_pos + i);
  }
  delay(1000);
  //*/

  // invert them in sequence
  char_pos = 1; num_chars = 1; cgram_pos = 0;
  for (int i = 0; i < 5; i++) {
    buildCustomChars("Nums", true, char_pos + i, num_chars, cgram_pos + i);
       
    delay(600);
  }

  delay(2000);

  lcd.clear();

  char_pos = 1; num_chars = 8; cgram_pos = 0; pos_x = 16 - num_chars;
  buildCustomChars("Nums", false, char_pos, num_chars, cgram_pos);
  for (int i = 0; i < num_chars; i++) {
    lcd.setCursor(pos_x + i, 1);
    lcd.write(cgram_pos + i);
  }
}

//set up the welcome part
void welcome() {
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print(" Oroboros Amps  ");

  delay(1000);

  lcd.setCursor(0,1);
  lcd.print("  SwitcherOne  ");
}           

void menu() {
  lcd.clear();
  lcd.setCursor(0,0);

  lcd.print("01 JUPITER+BACK");   
   
  demoNums();
  
  lcd.setCursor(0,0);
  lcd.print("01 JUPITER+BACK");   

  delay(2000);

  lcd.setCursor(0,0);
  lcd.print("02 SERENDIPITY ");   

  buildCustomChars("Nums", true, 2, 1, 1);

  delay(2000);

  lcd.setCursor(0,0);
  lcd.print("03 CAVIAR 4 PIGS");   

  buildCustomChars("Nums", true, 6, 1, 5);

  delay(2000);
  
  lcd.setCursor(0,0);
  lcd.print("04 SHINE ON CRZY");   

  buildCustomChars("Nums", true, 4, 1, 3);
 
  delay(5000);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Main Functions
/////////////////////////////////////////////////////////////////////////////////////////

void setup() {

  Serial.begin(57600); 
  Serial.println("\n//////////////////////////////////////////////////");
  Serial.println("\nOROBOROS SWITCHERONE UI v0.2-alpha");
  Serial.println("\n//////////////////////////////////////////////////");
  
  // /*
  lcd.begin(16,2);
  lcd.clear();
  // */

  /*
  lcd.begin(16, 2, LCD_5x8DOTS, LCD_CHARACTER);
  lcd.print("hello OLED World");  
  // */
}

void loop() {
  // /*
  welcome();
  delay(2000);
  menu();
  //*/

  /*
  lcd.setCursor(0, 1);
  lcd.print(millis()/1000);
  //*/
}
