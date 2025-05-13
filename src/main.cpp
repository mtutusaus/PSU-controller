#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ezButton.h>

// ENCODER
#define CLK_PIN 17
#define DT_PIN 16
#define SW_PIN 4
uint16_t CLK_PIN_state; 

// DAC
#define DAC_V_PIN 25
#define DAC_A_PIN 26

// PUSH BUTTONS
#define BTN_L_PIN 32
#define BTN_R_PIN 33
#define BTN_OUT_PIN 27

int curr_col;
int curr_row;
// i.e. 12.3 V, 1 is voltage_disp_1, 2 is _2 and 3 is _3
int voltage_disp_1 = 0;
int voltage_disp_2 = 0;
int voltage_disp_3 = 0;

// i.e. 250 mA, 2 is current_disp_1, 5 is _2 and 0 is _3
int current_disp_1 = 0;
int current_disp_2 = 0;
int current_disp_3 = 0;

// float voltage_DAC = 0.0; // desired voltage at the DAC output
// uint8_t DAC_bits; // voltage_DAC converter to a 8 bits value (DAC_scaled rounded)

// to toggle output state depending on previous state
bool output_state = false;
bool input_state = false;

// set the LCD address to 0x27 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd(0x27,20,4);  

ezButton BTN_L(BTN_L_PIN);
ezButton BTN_R(BTN_R_PIN);
ezButton BTN_OUT(BTN_OUT_PIN);
ezButton SW(SW_PIN);



void setup() {
  Serial.begin(9600);
  // Button debounce time setting
  BTN_L.setDebounceTime(50);
  BTN_R.setDebounceTime(50);
  BTN_OUT.setDebounceTime(50);
  SW.setDebounceTime(50);

  // Input pins config
  pinMode(CLK_PIN,INPUT);
  pinMode(DT_PIN,INPUT);
  
  // LCD initialization
  lcd.init();
  lcd.clear();         
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Set");
  lcd.setCursor(0,1);
  lcd.print(voltage_disp_1);
  lcd.print(voltage_disp_2);
  lcd.print(".");
  lcd.print(voltage_disp_3);
  lcd.setCursor(5,1);
  lcd.print("V");
  lcd.setCursor(0,2);
  lcd.print(current_disp_1);
  lcd.print(current_disp_2);
  lcd.print(current_disp_3);
  lcd.setCursor(4,2);
  lcd.print("mA");
  lcd.setCursor(7,0); lcd.print("|");
  lcd.setCursor(7,1); lcd.print("|");
  lcd.setCursor(7,2); lcd.print("|");
  lcd.setCursor(7,3); lcd.print("|");
  lcd.setCursor(8,0);
  lcd.print("Sense");
  lcd.setCursor(8,1);
  lcd.print("00.0 V");
  lcd.setCursor(8,2);
  lcd.print("000 mA");
  lcd.setCursor(15,0); lcd.print("|");
  lcd.setCursor(15,1); lcd.print("|");
  lcd.setCursor(15,2); lcd.print("|");
  lcd.setCursor(15,3); lcd.print("|");
  lcd.setCursor(16,0);
  lcd.print("Out");
  lcd.setCursor(17,1);
  lcd.print("OFF");
  curr_col = 3;
  curr_row = 1;
  lcd.setCursor(curr_col,curr_row);
  lcd.cursor();
}

void loop() {
  BTN_L.loop();
  BTN_R.loop();
  BTN_OUT.loop();
  SW.loop();

  // ROTARY ENCODER
  CLK_PIN_state = (CLK_PIN_state<<1) | digitalRead(CLK_PIN) | 0xe000; // detects stable falling edge
  if (CLK_PIN_state == 0xf000){
    CLK_PIN_state = 0x0000;
    if (curr_row == 1){ // VOLTAGE
      if (digitalRead(DT_PIN)){ // CW
        switch(curr_col){
          case 0: voltage_disp_1++; break;
          case 1: voltage_disp_2++; break;
          case 3: voltage_disp_3++; break;
          default: break;
        }
      }
      else{
        switch(curr_col){
          case 0: voltage_disp_1--; break;
          case 1: voltage_disp_2--; break;
          case 3: voltage_disp_3--; break;
          default: break;
        }
      }
      // UPDATE VOLTAGE DISPLAY
      if (voltage_disp_1<0) {voltage_disp_1=0;}
      if (voltage_disp_1>1) {voltage_disp_1=1;}
      if (voltage_disp_2<0) {voltage_disp_2=0;}
      if (voltage_disp_2>9) {voltage_disp_2=9;}
      if (voltage_disp_3<0) {voltage_disp_3=0;}
      if (voltage_disp_3>9) {voltage_disp_3=9;}
      lcd.setCursor(0,1);
      lcd.print(voltage_disp_1);
      lcd.print(voltage_disp_2);
      lcd.print(".");
      lcd.print(voltage_disp_3);
      lcd.setCursor(curr_col,curr_row);
    }
    else{ // CURRENT
      if (digitalRead(DT_PIN)){ // CW
        switch(curr_col){
          case 0: current_disp_1++; break;
          case 1: current_disp_2++; break;
          case 2: current_disp_3++; break;
          default: break;
        }
      }
      else{
        switch(curr_col){
          case 0: current_disp_1--; break;
          case 1: current_disp_2--; break;
          case 2: current_disp_3--; break;
          default: break;
        }
      }
      // UPDATE CURRENT DISPLAY
      if (current_disp_1<0) {current_disp_1=0;}
      if (current_disp_1>9) {current_disp_1=9;}
      if (current_disp_2<0) {current_disp_2=0;}
      if (current_disp_2>9) {current_disp_2=9;}
      if (current_disp_3<0) {current_disp_3=0;}
      if (current_disp_3>9) {current_disp_3=9;}
      lcd.setCursor(0,2);
      lcd.print(current_disp_1);
      lcd.print(current_disp_2);
      lcd.print(current_disp_3);
      lcd.setCursor(curr_col,curr_row);
    }
  }


  if(SW.isPressed()){
    switch(curr_row){
      case 1: curr_row = 2; break;
      case 2: curr_row = 1; break;
    }
    lcd.setCursor(curr_col,curr_row);
  }
  // MOVE CURSOR
  if(BTN_R.isPressed())
  {
    curr_col++;
    if (curr_col>3) {curr_col=3;}
    lcd.setCursor(curr_col,curr_row);
    lcd.cursor();
  }
  if(BTN_L.isPressed())
  {
    curr_col--;
    if (curr_col<0) {curr_col=0;}
    lcd.setCursor(curr_col,curr_row);
    lcd.cursor();
  }

  // OUTPUT ENABLE
  if(BTN_OUT.isPressed())
  {
    switch(output_state){
      case false: 
        output_state = true;
        lcd.setCursor(17,1);
        lcd.print(" "); lcd.print(" "); lcd.print(" ");
        lcd.setCursor(17,1);
        lcd.print("ON");
        lcd.setCursor(curr_col,curr_row);
        break;
      case true: 
        output_state = false; 
        lcd.setCursor(17,1);
        lcd.print(" "); lcd.print(" ");
        lcd.setCursor(17,1);
        lcd.print("OFF");
        lcd.setCursor(curr_col,curr_row);
        break;
    }
  }
}