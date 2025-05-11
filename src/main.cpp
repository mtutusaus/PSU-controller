#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ezButton.h>

// ENCODER VOLTAGE
#define CLK_V_PIN 17
#define DT_V_PIN 16
#define SW_V_PIN 4
uint16_t CLK_V_PIN_state; 

// ENCODER current
#define CLK_A_PIN 19
#define DT_A_PIN 18
#define SW_A_PIN 23
uint16_t CLK_A_PIN_state;

// DAC
#define DAC_V_PIN 25
#define DAC_A_PIN 26

// PUSH BUTTONS
#define BTN_L_PIN 32
#define BTN_R_PIN 33
#define BTN_OUT_PIN 27

int curr_col;
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

bool BTN_L_state;
bool BTN_R_state;
bool BTN_OUT_state;

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line display
ezButton BTN_L(BTN_L_PIN);
ezButton BTN_R(BTN_R_PIN);
ezButton BTN_OUT(BTN_OUT_PIN);
ezButton SW_V(SW_V_PIN);
ezButton SW_A(SW_A_PIN);

void setup() {
  Serial.begin(9600);
  // Button debounce time setting
  BTN_L.setDebounceTime(50);
  BTN_R.setDebounceTime(50);
  SW_V.setDebounceTime(50);
  SW_A.setDebounceTime(50);
  // Input pins config
  pinMode(CLK_V_PIN,INPUT);
  pinMode(DT_V_PIN,INPUT);
  pinMode(CLK_A_PIN,INPUT);
  pinMode(DT_A_PIN,INPUT);
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
  lcd.print("00.0 V"); // TO DO: IMPLEMENT ADC MEASUREMENT TO DISPLAY SENSE
  lcd.setCursor(8,2);
  lcd.print("000 mA");
  lcd.setCursor(15,0); lcd.print("|");
  lcd.setCursor(15,1); lcd.print("|");
  lcd.setCursor(15,2); lcd.print("|");
  lcd.setCursor(15,3); lcd.print("|");
  lcd.setCursor(16,0);
  lcd.print("Out");
  lcd.setCursor(17,1);
  lcd.print("ON"); // TO DO: IMPLEMENT OUTPUT BUTTON TO CHANGE OUTPUT ON/OFF
  curr_col = 3;
  lcd.setCursor(curr_col,3);
  lcd.cursor();
  // dacWrite(DAC_V_PIN,0); // set DAC to 0V
}

void loop() {
  BTN_L.loop();
  BTN_R.loop();
  SW_V.loop();
  SW_A.loop();
  // ROTARY ENCODER VOLTAGE
  CLK_V_PIN_state = (CLK_V_PIN_state<<1) | digitalRead(CLK_V_PIN) | 0xe000; // detects stable falling edge
  if (CLK_V_PIN_state == 0xf000){
    CLK_V_PIN_state = 0x0000;
    if (digitalRead(DT_V_PIN)){ // CW
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
    if (voltage_disp_1>9) {voltage_disp_1=9;}
    if (voltage_disp_2<0) {voltage_disp_2=0;}
    if (voltage_disp_2>9) {voltage_disp_2=9;}
    if (voltage_disp_3<0) {voltage_disp_3=0;}
    if (voltage_disp_3>9) {voltage_disp_3=9;}
    lcd.setCursor(0,1);
    lcd.print(voltage_disp_1);
    lcd.print(voltage_disp_2);
    lcd.print(".");
    lcd.print(voltage_disp_3);
    lcd.setCursor(curr_col,3);

    // voltage_DAC = voltage_disp_1 * 10 + voltage_disp_2 + voltage_disp_3 * 0.1 + voltage_disp_4 * 0.01; // convert what is on the LCD screen to a voltage
    // if (voltage_DAC > 3.3) {voltage_DAC = 3.3;}
    // DAC_bits = round((voltage_DAC / 3.3) * 255);
    // dacWrite(DAC_V_PIN,DAC_bits);
  }

  CLK_A_PIN_state = (CLK_A_PIN_state<<1) | digitalRead(CLK_A_PIN) | 0xe000; // detects stable falling edge
  if (CLK_A_PIN_state == 0xf000){
    CLK_A_PIN_state = 0x0000;
    if (digitalRead(DT_A_PIN)){ // CW
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
    lcd.setCursor(curr_col,3);

    // voltage_DAC = voltage_disp_1 * 10 + voltage_disp_2 + voltage_disp_3 * 0.1 + voltage_disp_4 * 0.01; // convert what is on the LCD screen to a voltage
    // if (voltage_DAC > 3.3) {voltage_DAC = 3.3;}
    // DAC_bits = round((voltage_DAC / 3.3) * 255);
    // dacWrite(DAC_V_PIN,DAC_bits);
  }

  if(SW_V.isPressed()){
    // ROTARY ENCODER VOLTAGE SWITCH PRESSED
    voltage_disp_1 = 0;
    voltage_disp_2 = 0;
    voltage_disp_3 = 0;
    // dacWrite(DAC_V_PIN,0);
    lcd.setCursor(0,1);
    lcd.print(voltage_disp_1);
    lcd.print(voltage_disp_2);
    lcd.print(".");
    lcd.print(voltage_disp_3);
    curr_col = 3;
    lcd.setCursor(curr_col,3);
    lcd.cursor();
  }

  if(SW_A.isPressed()){
    // ROTARY ENCODER CURRENT SWITCH PRESSED
    current_disp_1 = 0;
    current_disp_2 = 0;
    current_disp_3 = 0;
    // dacWrite(DAC_V_PIN,0);
    lcd.setCursor(0,2);
    lcd.print(current_disp_1);
    lcd.print(current_disp_2);
    lcd.print(current_disp_3);
    curr_col = 3;
    lcd.setCursor(curr_col,3);
    lcd.cursor();
  }

  // MOVE CURSOR
  if(BTN_R.isPressed())
  {
    curr_col++;
    if (curr_col>3) {curr_col=3;}
    lcd.setCursor(curr_col,3);
  }
  if(BTN_L.isPressed())
  {
    curr_col--;
    if (curr_col<0) {curr_col=0;}
    lcd.setCursor(curr_col,3);
  }
}