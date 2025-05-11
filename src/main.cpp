#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ezButton.h>

// ENCODER
#define CLK_PIN 17
#define DT_PIN 16
#define BTN_PIN 4 // when pressed, change from V to A and viceversa

// DAC
#define DAC_PIN 25

// DIGIT SELECTION
#define BTN_L_PIN 32
#define BTN_R_PIN 33

uint16_t CLK_PIN_state;
int curr_col = 0;
// i.e. 12.34 V, 1 is voltage_disp_1, 2 is _2, 3 is _3 and 4 is _4
int voltage_disp_1 = 0;
int voltage_disp_2 = 0;
int voltage_disp_3 = 0;
int voltage_disp_4 = 0;

bool BTN_L_state;
bool BTN_R_state;

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line display
ezButton BTN_L(BTN_L_PIN);
ezButton BTN_R(BTN_R_PIN);
ezButton BTN(BTN_PIN);

void setup() {
  Serial.begin(9600);
  BTN_L.setDebounceTime(50);
  BTN_R.setDebounceTime(50);
  BTN.setDebounceTime(50);
  pinMode(CLK_PIN,INPUT);
  pinMode(DT_PIN,INPUT);
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
  lcd.setCursor(0,0);
  lcd.print(voltage_disp_1);
  lcd.print(voltage_disp_2);
  lcd.print(".");
  lcd.print(voltage_disp_3);
  lcd.print(voltage_disp_4);
  curr_col = 4;
  lcd.setCursor(curr_col,0);
  lcd.cursor();

  dacWrite(DAC_PIN,128);
}

void loop() {
  BTN_L.loop();
  BTN_R.loop();
  BTN.loop();
  // ROTARY ENCODER
  CLK_PIN_state = (CLK_PIN_state<<1) | digitalRead(CLK_PIN) | 0xe000; // detects stable falling edge
  if (CLK_PIN_state == 0xf000){
    CLK_PIN_state = 0x0000;
    if (digitalRead(DT_PIN)){ // CW
      switch(curr_col){
        case 0: voltage_disp_1++; break;
        case 1: voltage_disp_2++; break;
        case 3: voltage_disp_3++; break;
        case 4: voltage_disp_4++; break;
        default: break;
      }
    }
    else{
      switch(curr_col){
        case 0: voltage_disp_1--; break;
        case 1: voltage_disp_2--; break;
        case 3: voltage_disp_3--; break;
        case 4: voltage_disp_4--; break;
        default: break;
      }
    }
      // UPDATE VOLTAGE DISPLAY
  lcd.clear();
  if (voltage_disp_1<0) {voltage_disp_1=0;}
  if (voltage_disp_1>9) {voltage_disp_1=9;}
  if (voltage_disp_2<0) {voltage_disp_2=0;}
  if (voltage_disp_2>9) {voltage_disp_2=9;}
  if (voltage_disp_3<0) {voltage_disp_3=0;}
  if (voltage_disp_3>9) {voltage_disp_3=9;}
  if (voltage_disp_4<0) {voltage_disp_4=0;}
  if (voltage_disp_4>9) {voltage_disp_4=9;}
  lcd.print(voltage_disp_1);
  lcd.print(voltage_disp_2);
  lcd.print(".");
  lcd.print(voltage_disp_3);
  lcd.print(voltage_disp_4);
  lcd.setCursor(curr_col,0);
  }

  if(BTN.isPressed()){
    // ROTARY ENCODER SWITCH PRESSED
    voltage_disp_1 = 0;
    voltage_disp_2 = 0;
    voltage_disp_3 = 0;
    voltage_disp_4 = 0;
    lcd.clear();
    lcd.print(voltage_disp_1);
    lcd.print(voltage_disp_2);
    lcd.print(".");
    lcd.print(voltage_disp_3);
    lcd.print(voltage_disp_4);
    curr_col = 4;
    lcd.setCursor(curr_col,0);
    lcd.cursor();
  }

  // MOVE CURSOR
  if(BTN_R.isPressed())
  {
    curr_col++;
    if (curr_col>4) {curr_col=4;}
    lcd.setCursor(curr_col,0);
  }
  if(BTN_L.isPressed())
  {
    curr_col--;
    if (curr_col<0) {curr_col=0;}
    lcd.setCursor(curr_col,0);
  }
}