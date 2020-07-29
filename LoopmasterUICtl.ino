/*

Misson Control

*/

#include  <LiquidCrystal.h>

//const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

const int D7 = 13, 
                D6 = 12, 
                D5 = 11, 
                D4 = 10,
                EN = 7,
                RS = 6;
                
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

int y = 1000;

// drawing the pixels for each 8x5 part of the LCD 
byte char0[8] =     {  B00000,  B00000,  B01110, B01010, B01010, B01010, B01110, B00000 };
byte char1[8] =     {  B00000,  B00000,  B00100, B00100, B00100, B00100, B00100, B00000 };
byte char2[8] =     {  B00000,  B00000,  B01110, B00010, B01110, B01000, B01110, B00000 };
byte char3[8] =     {  B00000,  B00000,  B01110, B00010, B00110, B00010, B01110, B00000 };
byte char4[8] =     {  B00000,  B00000,  B01010,  B01010,  B01110,  B00010,  B00010,  B00000 };
byte char5[8] =     {  B00000,  B00000,  B01110,  B01000,  B01110,  B00010,  B01110,  B00000 };
byte char6[8] =     {  B00000,  B00000,  B01110,  B01000,  B01110,  B01010,  B01110,  B00000 };
byte char7[8] =     {  B00000,  B00000,  B01110,  B00010,  B00010,  B00010,  B00010,  B00000 };
byte char8[8] =     {  B00000,  B00000,  B01110,  B01010,  B01110,  B01010,  B01110,  B00000 };
byte char9[8] =     {  B00000,  B00000,  B01110,  B01010,  B01110,  B00010,  B01110,  B00000 };
byte charA[8] =     {  B01110,  B11011,  B11011,  B11011,  B11111,  B11011,  B11011,  B11011 };
byte charL_s[8] =   {  B00000,  B00000,  B01000,  B01000,  B01000,  B01000,  B01110,  B00000 };
byte charO_s[8] =   {  B00000,  B00000,  B01110,  B01010,  B01010,  B01010,  B01110,  B00000 };
byte charP_s[8] =   {  B00000,  B00000,  B01110,  B01010,  B01110,  B01000,  B01000,  B00000 };
byte charS_s[8] =   {  B00000,  B00000,  B01110,  B01000,  B01110,  B00010,  B01110,  B00000 };

byte customChars[][8] = { 
  {  B00000,  B00000,  B01110,  B01010,  B01010,  B01010,  B01110,  B00000 }, // 0
  {  B00000,  B00000,  B00100,  B00100,  B00100,  B00100,  B00100,  B00000 }, // 1
  {  B00000,  B00000,  B01110,  B00010,  B01110,  B01000,  B01110,  B00000 }, // 2
  {  B00000,  B00000,  B01110,  B00010,  B00110,  B00010,  B01110,  B00000 }, // 3
  {  B00000,  B00000,  B01010,  B01010,  B01110,  B00010,  B00010,  B00000 }, // 4
  {  B00000,  B00000,  B01110,  B01000,  B01110,  B00010,  B01110,  B00000 }, // 5
  {  B00000,  B00000,  B01110,  B01000,  B01110,  B01010,  B01110,  B00000 }, // 6
  {  B00000,  B00000,  B01110,  B00010,  B00010,  B00010,  B00010,  B00000 }, // 7
  {  B00000,  B00000,  B01110,  B01010,  B01110,  B01010,  B01110,  B00000 }, // 8
  {  B00000,  B00000,  B01110,  B01010,  B01110,  B00010,  B01110,  B00000 }, // 9
  
  {  B01110,  B11011,  B11011,  B11011,  B11111,  B11011,  B11011,  B11011 }, // A
  
  {  B00000,  B00000,  B01000,  B01000,  B01000,  B01000,  B01110,  B00000 }, // L small
  {  B00000,  B00000,  B01110,  B01010,  B01010,  B01010,  B01110,  B00000 }, // O small
  {  B00000,  B00000,  B01110,  B01010,  B01110,  B01000,  B01000,  B00000 }, // P small
  {  B00000,  B00000,  B01110,  B01000,  B01110,  B00010,  B01110,  B00000 }  // S small
};

byte charInv;
byte temp_chr[8];

void createNumchars(){
  // numbers 1-8
  for (int i = 0; i < 8; i += 1)
    lcd.createChar(i, customChars[i+1]);
}

//set up the welcome part
void welcome(){
  lcd.clear();
  delay(250);
  lcd.setCursor(0,0);
  lcd.print(" OROBOROS AMPS");
  
  delay(1000);

  lcd.setCursor(0,1);
  lcd.print("MISSION CTL v1.0");
  
  delay(3000);
}           

void menu(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("01 JUPITER+BACK");   
   
  delay(1000);
 
  // numbers 1-8
  for (int i = 0; i < 8; i += 1) {       
    lcd.setCursor(i + 8, 1);
    lcd.write(i);    
  }

  delay(1000);

  // invert them in sequence
  for (int i = 0; i < 8; i += 1) {
  
    lcd.createChar(i, invert_char(customChars[i+1]));
    
    lcd.setCursor(i + 8, 1);
    lcd.write(i);
    
    delay(600);
  }

  // reset chars
  createNumchars();
  
  delay(2000);

  lcd.setCursor(0,0);
  lcd.print("02 SERENDIPITY ");   

  lcd.createChar(2, invert_char(customChars[2+1]));
  lcd.setCursor(2 + 8, 1);
  lcd.write(2);    
  delay(2000);

  lcd.setCursor(0,0);
  lcd.print("03 CAVIAR 4 PIGS");   

  lcd.createChar(6, invert_char(customChars[6+1]));
  lcd.setCursor(6 + 8, 1);
  lcd.write(6);    
  delay(2000);
  
  lcd.setCursor(0,0);
  lcd.print("04 SHINE ON CRZY");   

  lcd.createChar(4, invert_char(customChars[4+1]));
  lcd.setCursor(4 + 8, 1);
  lcd.write(4);    
  
  delay(5000);

}

byte *invert_char(byte chr[]) {

  // skip 0 as the first line should stay off
  for (int i = 1; i < 8; i += 1) {
    temp_chr[i] = ~chr[i];
  }

  return temp_chr;
  
}

void setup() {
  lcd.begin(16,2);
  lcd.clear();

  createNumchars();
}

void loop() {
  welcome();
  menu();
}
