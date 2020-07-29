// ---------------------------------------------------------------------------
// Created by John Valai on 20/08/11.
// Copyright (C) - 2020
//
// ---------------------------------------------------------------------------
// OROBOROS AUDIO SYSTEMS
// SwitcherOne
//
// @author J. V. Kamaly - hi@jvk.to
// ---------------------------------------------------------------------------

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// CONSTANT definitions
// ---------------------------------------------------------------------------
                
#define count(ARRAY) (sizeof(ARRAY) / sizeof(*ARRAY))
#define CUST_PIXELS_PER_LINE 8
#define CUST_DIGIT_COUNT 10

// DUE Ports
#define   D7 13
#define   D6 12
#define   D5 11
#define   D4 10
#define   EN 7
#define   RW 6
#define   RS 5

LiquidCrystal lcd(RS, RW, EN, D4, D5, D6, D7);

// Custom characters

struct Digit {
    uint8_t digit[CUST_PIXELS_PER_LINE];
    String digit_name;
};

struct DigitCollection {
    String collection_name;
    Digit digits[CUST_DIGIT_COUNT];
};

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

/* HELPER FUNCTIONS /////////////////////////////////////////////////*/

Digit *getCharset(String collection_name)  {
  for (int i = 0; i < count(customChars); i++) {
    if (customChars[i].collection_name == collection_name)
      return customChars[i].digits;
  }
}

uint8_t *invert_char(uint8_t chr[8]) {
  uint8_t temp_chr[8];

  // skip 0 as the first line should stay off
  for (int i = 1; i < 8; i += 1) {
    temp_chr[i] = ~chr[i];
  }

  return temp_chr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Serial.println(0b00000011, BIN);
void buildCustomChars(String collection_name, bool invert = false, int start = 0, int end = CUST_DIGIT_COUNT) {
  Digit *glyphs = getCharset(collection_name);

  // Max. 8 characters in CGRAM
  for (int i = start; i < 8 && i < end + 1; i++) {

    if (invert == true) {
      uint8_t temp_chr[8];
      // skip 0 as the first line should stay off
      for (int i = 1; i < 8; i += 1) {
        temp_chr[i] = *glyphs[i].digit;
      }

      Serial.print("::");
      Serial.println(temp_chr[2], BIN);

      lcd.createChar(i, temp_chr);
      // lcd.createChar(i, invert_char(glyphs[i].digit));
      // while(1){};
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
