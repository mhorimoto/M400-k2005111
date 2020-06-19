#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
char lcdtext[17];
char lcdtitle[17];
long int lc;
int inabp, inbbp, instopp;

void setup(void) {
  pinMode(A0,INPUT);
  pinMode(0,INPUT_PULLUP);  // INPUT-B OPTCOUPLE
  pinMode(1,INPUT_PULLUP);  // INPUT-A OPTCOUPLE
  pinMode(10,INPUT_PULLUP); // STOP    BUTTON
  pinMode(11,INPUT_PULLUP); // INPUT-B BUTTON
  pinMode(12,INPUT_PULLUP); // INPUT-A BUTTON
  pinMode(2,OUTPUT);        // INPUT-A RELAY
  pinMode(3,OUTPUT);        // INPUT-B RELAY
  pinMode(5,OUTPUT);        // OUTPUT  RELAY
  digitalWrite(2,HIGH);     // RELAY-A
  digitalWrite(3,HIGH);     // RELAY-B
  digitalWrite(5,HIGH);     // RELAY-OUTPUT
  inabp = digitalRead(12);   // A BUTTON
  inbbp = digitalRead(11);   // B BUTTON
  instopp = digitalRead(10); // STOP BUTTON
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("K2005111-2");
  lc = 0;
}

void loop(void) {
  int inab, inbb, instop;
  boolean aflg,bflg,stopflg;

  inab = digitalRead(12);
  inbb = digitalRead(11);
  instop = digitalRead(10);
  lcd.setCursor(0,1);
  if (inab==LOW) {       // A BUTTON
    lc=0;
    lcd.print("L");
    digitalWrite(3,HIGH);
    digitalWrite(2,LOW);
    delay(100);
    digitalWrite(5,LOW);
  } else {
    lcd.print("H");
  }
  lcd.setCursor(2,1);
  if (instop==LOW) {      // STOP BUTTON
    lcd.print("L");
    digitalWrite(5,HIGH);
    digitalWrite(2,HIGH);
    digitalWrite(3,HIGH);
    lc++;
  } else {
    lcd.print("H");
    lcd.setCursor(6,1);
    lcd.print(lc);
    lc=0;
  }
  lcd.setCursor(4,1);
  if (inbb==LOW) {         // B BUTTON
    lc=0;
    lcd.print("L");
    digitalWrite(2,HIGH);
    digitalWrite(3,LOW);
    delay(100);
    digitalWrite(5,LOW);
  } else {
    lcd.print("H");
  }
}
