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
#include <LiquidCrystal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

                
#define count(ARRAY) (sizeof(ARRAY) / sizeof(*ARRAY))
/////////////////////////////////////////////////////////////////////////////////////////
// Constant definitions
/////////////////////////////////////////////////////////////////////////////////////////

#define CUST_PIXELS_PER_LINE  5
#define CUST_LINES_PER_CHAR   8  
#define CUST_DIGIT_COUNT      10 // Max no. of digits to choose from
#define CUST_DIGIT_MAX        8  // Max. 8 characters in CGRAM

// DUE Ports
#define   D7 13
#define   D6 12
#define   D5 11
#define   D4 10
#define   EN 7
#define   RW 6
#define   RS 5

LiquidCrystal lcd(RS, RW, EN, D4, D5, D6, D7);

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
    { "B", {  0b11110,  0b11011,  0b11011,  0b11011,  0b11111,  0b11011,  0b11011,  0b11110 }},
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

  for (int i = start; i < CUST_DIGIT_MAX && i < end + 1; i++) {

    if (invert == true) {
      uint8_t *temp_glyph;
      
      // Invert bits
      temp_glyph = invert_char(glyphs[i].digit);

      lcd.createChar(i, temp_glyph);
    } else {
      lcd.createChar(i, glyphs[i].digit);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void demoLettersS() {
  buildCustomChars("LetterSm");
  
  lcd.setCursor(12, 1);
  for (int i = 0; i < 4; i++)  {
    lcd.write(i);    
  }

  delay(1000);

  // invert them in sequence
  for (int i = 0; i < 4; i++) {
    buildCustomChars("LetterSm", true, i, i + 1);
    
    lcd.setCursor(i + 12, 1);
    lcd.write(i);
    
    delay(600);
  }

  // reset chars
  // buildCustomNumChars();
}
/////////////////////////////////////////////////////////////////////////////////////////
// Display Functions
/////////////////////////////////////////////////////////////////////////////////////////

void demoNums() {
  int offset = 1;

  // numbers 1-8
  // buildCustomChars(customNumChars, offset);
  buildCustomChars("Nums");
  
  for (int i = 0; i < 8; i++)  {
    lcd.setCursor(i + 8, 1);
    lcd.write(i);    
  }

  delay(1000);

  // invert them in sequence
  for (int i = 0; i < 8; i += 1) {
    // lcd.createChar(i, invert_char(customNumChars[i + offset].digit));
    
    lcd.setCursor(i + 8, 1);
    lcd.write(i);
    
    delay(600);
  }

  // reset chars
  // buildCustomNumChars();
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
   
  // demoLettersS();
  return;

  delay(1000);
  
  demoNums();
  
  delay(2000);

  lcd.setCursor(0,0);
  lcd.print("02 SERENDIPITY ");   

  // lcd.createChar(2, invert_char(customChars[2+1]));
  lcd.setCursor(2 + 8, 1);
  lcd.write(2);  

  delay(2000);

  lcd.setCursor(0,0);
  lcd.print("03 CAVIAR 4 PIGS");   

  // lcd.createChar(6, invert_char(customChars[6+1]));
  lcd.setCursor(6 + 8, 1);
  lcd.write(6);    

  delay(2000);
  
  lcd.setCursor(0,0);
  lcd.print("04 SHINE ON CRZY");   

  // lcd.createChar(4, invert_char(customChars[4+1]));
  lcd.setCursor(4 + 8, 1);
  lcd.write(4);    
  
  delay(5000);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// Main Functions
/////////////////////////////////////////////////////////////////////////////////////////

void setup() {

  Serial.begin(57600); 
  Serial.write("MOTHAFUCKA!\n");
  lcd.begin(16,2);
  lcd.clear();
}

void loop() {
  // welcome();
  // delay(3000);
  menu();

  buildCustomChars("LetterSm", false, 0, 3);
  buildCustomChars("Nums", false, 4, 8);
 
  lcd.setCursor(0,1);

  lcd.write((int)0);
  lcd.write(1);
  lcd.write(1);
  lcd.write(2);
  lcd.write(3);

  lcd.write(4);
  lcd.write(5);
  lcd.write(6);
  lcd.write(7);

  delay(1000);

  buildCustomChars("LetterSm", true, 0, 0);

  while(1){};
}
