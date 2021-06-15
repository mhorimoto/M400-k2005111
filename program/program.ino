#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <LiquidCrystal_I2C.h>

#define VERSION  "A28"

uint8_t mcusr_mirror __attribute__ ((section (".noinit")));
void get_mcusr(void)	 \
  __attribute__((naked)) \
  __attribute__((section(".init3")));
void get_mcusr(void) {
  mcusr_mirror = MCUSR;
  MCUSR = 0;
  wdt_disable();
}


const char version[] PROGMEM = VERSION;

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
char lcdtext[17];
char lcdtitle[17];
long int lc;
unsigned long pmillsec,dmillsec,cmillsec; 
int tl;
int inabp, inbbp, instopp, insts;
int ain, delayA, delayB;   // Delay time counter for A,B
boolean forceMode,breakForce;

void setup(void) {
  configure_wdt();
  pinMode(A0,INPUT);        // Delay Time Adjuster
  pinMode(0,INPUT_PULLUP);  // INPUT-B OPTCOUPLE
  pinMode(1,INPUT_PULLUP);  // INPUT-A OPTCOUPLE
  pinMode(10,INPUT_PULLUP); // STOP    BUTTON
  pinMode(11,INPUT_PULLUP); // INPUT-B BUTTON
  pinMode(12,INPUT_PULLUP); // INPUT-A BUTTON
  pinMode(2,OUTPUT);        // INPUT-A RELAY
  pinMode(3,OUTPUT);        // INPUT-B RELAY
  pinMode(4,OUTPUT);        // OUTPUT-SW RELAY
  pinMode(5,OUTPUT);        // OUTPUT-SIGNAL  RELAY
  digitalWrite(2,HIGH);     // RELAY-A
  digitalWrite(3,HIGH);     // RELAY-B
  digitalWrite(4,HIGH);     // RELAY-SW OUTPUT
  digitalWrite(5,HIGH);     // RELAY-SIGNAL OUTPUT
  inabp = digitalRead(12);   // A BUTTON
  inbbp = digitalRead(11);   // B BUTTON
  instopp = digitalRead(10); // STOP BUTTON
  wdt_reset();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(13,0);
  lcd.print(VERSION);
  lc = 0;
  forceMode = false;
  breakForce = false;
  wdt_reset();
  //  Serial.begin(115200); // Remove when A28
  lcd.setCursor(14,1);
  lcd.print("RM");
  insts = 0;     // input status 0:OFF 1:A 2:B 4:C(if exist)
  pmillsec = 0;
  dmillsec = 0;
  cmillsec = millis();
}

void loop(void) {
  int inab, inbb, instop;
  int inao, inbo;
  boolean aflg,bflg,stopflg;

  wdt_reset();
  //
  //  LOW is ASSERT
  //
  inao = digitalRead(1);
  inbo = digitalRead(0);
  inab = digitalRead(12);
  inbb = digitalRead(11);
  instop = digitalRead(10);
  ain = analogRead(A0);
  ain = (ain*10)/1023;
  sprintf(lcdtext,"DELAY=%2dsec",ain);
  lcd.setCursor(0,0);
  lcd.print(lcdtext);
  if ((inab*inbb)==0) {
    forceMode = true;
    lcd.setCursor(14,1);
    lcd.print("FM");
  }
  if (breakForce) {
    forceMode = false;
    breakForce = false;
    lcd.setCursor(14,1);
    lcd.print("RM");
  }
  lcd.setCursor(0,1);
  if (forceMode) {
    wdt_reset();
    //
    //*********** FORCE MODE *************
    //
    if (inab==LOW) {       // A BUTTON
      forceMode = true;
      lc=0;
      lcd.print("IN=A  OUT=A");
      digitalWrite(3,HIGH);
      digitalWrite(2,LOW);
      delay(70);
      digitalWrite(5,LOW);
      delay(70);
      digitalWrite(4,LOW);
    }
    if (instop==LOW) {      // STOP BUTTON
      forceMode = true;
      lcd.print("IN=N  OUT=N");
      digitalWrite(4,HIGH);
      digitalWrite(5,HIGH);
      digitalWrite(2,HIGH);
      digitalWrite(3,HIGH);
      lc++;
      if (lc>100) {
	forceMode = false;
	breakForce = true;
	lc=0;
      }
      
    }
    if (inbb==LOW) {         // B BUTTON
      forceMode = true;
      lc=0;
      lcd.print("IN=B  OUT=B");
      digitalWrite(2,HIGH);
      digitalWrite(3,LOW);
      delay(70);
      digitalWrite(5,LOW);
      delay(70);
      digitalWrite(4,LOW);
    }
  } else {
    //
    //*********** REMOTE MODE *************
    //
    cmillsec = millis();
    dmillsec = cmillsec - pmillsec; // 1000mSecを計算する
    if (dmillsec>1000) {
      if (delayA>0) {
	delayA--;
      }
      if (delayB>0) {
	delayB--;
      }
      pmillsec = cmillsec;
    }
    wdt_reset();
    insts = 0; // Reset status
    if (inao==LOW) { insts  = 1; }
    if (inbo==LOW) { insts |= 2; }
    // if (inco==LOW) { insts |= 4; }
    lcd.setCursor(0,1);
    lcd.print("IN=X LCK=");
    switch(insts) {
    case 0:
      digitalWrite(4,HIGH);
      delay(70);
      digitalWrite(5,HIGH);
      delay(70);
      digitalWrite(2,HIGH);
      digitalWrite(3,HIGH);
      if (delayA>0) {
	sprintf(lcdtext,"A/%02d",delayA);
	lcd.setCursor(9,1);
	lcd.print(lcdtext);
      }
      if (delayB>0) {
	sprintf(lcdtext,"B/%02d",delayB);
	lcd.setCursor(9,1);
	lcd.print(lcdtext);
      }
      if ((delayA==0)&&(delayB==0)) {
	lcd.setCursor(9,1);
	lcd.print("X   ");
      }
      break;
    case 1: // IN A
      if (delayA>0) {
	break;
      }
      lcd.setCursor(3,1);
      lcd.print("A");
      lcd.setCursor(9,1);
      lcd.print("B   ");
      digitalWrite(3,HIGH);
      digitalWrite(2,LOW);
      delay(70);
      digitalWrite(5,LOW);
      delay(70);
      digitalWrite(4,LOW);
      delayB = ain;
      break;      
    case 2: // IN B
      if (delayB>0) {
	break;
      }
      lcd.setCursor(3,1);
      lcd.print("B");
      lcd.setCursor(9,1);
      lcd.print("A   ");
      digitalWrite(2,HIGH);
      digitalWrite(3,LOW);
      delay(70);
      digitalWrite(5,LOW);
      delay(70);
      digitalWrite(4,LOW);
      delayA = ain;
      break;
    default:
      break;
    }
  }
}



void configure_wdt(void) {
  cli();                           // disable interrupts for changing the registers
  MCUSR = 0;                       // reset status register flags
                                   // Put timer in interrupt-only mode:
  WDTCSR |= 0b00011000;            // Set WDCE (5th from left) and WDE (4th from left) to enter config mode,
                                   // using bitwise OR assignment (leaves other bits unchanged).
  WDTCSR =  0b00001000 | 0b100000; // clr WDIE: interrupt enabled
                                   // set WDE: reset disabled
                                   // and set delay interval (right side of bar) to 8 seconds
  sei();                           // re-enable interrupts
                                   // reminder of the definitions for the time before firing
                                   // delay interval patterns:
                                   //  16 ms:     0b000000
                                   //  500 ms:    0b000101
                                   //  1 second:  0b000110
                                   //  2 seconds: 0b000111
                                   //  4 seconds: 0b100000
                                   //  8 seconds: 0b100001
}
