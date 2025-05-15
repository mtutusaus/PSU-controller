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

// CURSOR POSITION
int curr_col;
int curr_row;

// VOLTAGE DISPLAY (i.e. 12.3 V, 1 is voltage_disp_1, 2 is _2 and 3 is _3)
int voltage_disp_1 = 0;
int voltage_disp_2 = 0;
int voltage_disp_3 = 0;

// CURRENT DISPLAY (i.e. 250 mA, 2 is current_disp_1, 5 is _2 and 0 is _3)
int current_disp_1 = 0;
int current_disp_2 = 0;
int current_disp_3 = 0;

// OUTPUT STATE
bool output_state = false;

// LCD DEFINITION (library: LiquidCrystal_I2C@^1.1.4)
LiquidCrystal_I2C lcd(0x27,20,4); // set the LCD address to 0x27 for a 20 chars and 4 line display

// PUSH BUTTONS INITIALIZATION (library: ezButton@^1.0.6)
ezButton BTN_L(BTN_L_PIN);
ezButton BTN_R(BTN_R_PIN);
ezButton BTN_OUT(BTN_OUT_PIN);
ezButton SW(SW_PIN);

// PROJECT FUNCTIONS

void lcd_initialization(){
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

void incrementVoltageDigit(int col) {
  switch (col) {
    case 0: // tens
      voltage_disp_1++;
      if (voltage_disp_1 > 2) {
        voltage_disp_1 = 2;
      }
      break;
    case 1: // units
      voltage_disp_2++;
      if (voltage_disp_2 > 9) {
        voltage_disp_2 = 0;
        voltage_disp_1++;
        if (voltage_disp_1 > 2) {
          voltage_disp_1 = 2;
        }
      }
      break;
    case 3: // tenths
      voltage_disp_3++;
      if (voltage_disp_3 > 9) {
        voltage_disp_3 = 0;
        voltage_disp_2++;
        if (voltage_disp_2 > 9) {
          voltage_disp_2 = 0;
          voltage_disp_1++;
          if (voltage_disp_1 > 2) {
            voltage_disp_1 = 2;
          }
        }
      }
      break;
  }

  // Check total voltage and clamp to 20.0 if necessary
  float voltage = voltage_disp_1 * 10.0 + voltage_disp_2 + voltage_disp_3 / 10.0;
  if (voltage > 20.0) {
    voltage_disp_1 = 2;
    voltage_disp_2 = 0;
    voltage_disp_3 = 0;
  }
}

void decrementVoltageDigit(int col) {
  if (voltage_disp_1 == 0 && voltage_disp_2 == 0 && voltage_disp_3 == 0) return; // min reached

  switch (col) {
    case 0: // tens
      if (voltage_disp_1 > 0) voltage_disp_1--;
      break;
    case 1: // units
      voltage_disp_2--;
      if (voltage_disp_2 < 0) {
        if (voltage_disp_1 > 0) {
          voltage_disp_2 = 9;
          voltage_disp_1--;
        } else {
          voltage_disp_2 = 0;
        }
      }
      break;
    case 3: // tenths
      voltage_disp_3--;
      if (voltage_disp_3 < 0) {
        if (voltage_disp_2 > 0 || voltage_disp_1 > 0) {
          voltage_disp_3 = 9;
          voltage_disp_2--;
          if (voltage_disp_2 < 0) {
            voltage_disp_2 = 9;
            voltage_disp_1--;
          }
        } else {
          voltage_disp_3 = 0;
        }
      }
      break;
  }
}

void incrementCurrentDigit(int col) {
  switch (col) {
    case 0: // hundreds
      current_disp_1++;
      if (current_disp_1 > 5) {
        current_disp_1 = 5;
      }
      break;
    case 1: // tens
      current_disp_2++;
      if (current_disp_2 > 9) {
        current_disp_2 = 0;
        current_disp_1++;
        if (current_disp_1 > 5) {
          current_disp_1 = 5;
        }
      }
      break;
    case 2: // units
      current_disp_3++;
      if (current_disp_3 > 9) {
        current_disp_3 = 0;
        current_disp_2++;
        if (current_disp_2 > 9) {
          current_disp_2 = 0;
          current_disp_1++;
          if (current_disp_1 > 5) {
            current_disp_1 = 5;
          }
        }
      }
      break;
  }

  // Final safety clamp
  int current = current_disp_1 * 100 + current_disp_2 * 10 + current_disp_3;
  if (current > 500) {
    current_disp_1 = 5;
    current_disp_2 = 0;
    current_disp_3 = 0;
  }
}

void decrementCurrentDigit(int col) {
  if (current_disp_1 == 0 && current_disp_2 == 0 && current_disp_3 == 0) return; // min reached

  switch (col) {
    case 0: // hundreds
      if (current_disp_1 > 0) current_disp_1--;
      break;
    case 1: // tens
      current_disp_2--;
      if (current_disp_2 < 0) {
        if (current_disp_1 > 0) {
          current_disp_2 = 9;
          current_disp_1--;
        } else {
          current_disp_2 = 0;
        }
      }
      break;
    case 2: // units
      current_disp_3--;
      if (current_disp_3 < 0) {
        if (current_disp_2 > 0 || current_disp_1 > 0) {
          current_disp_3 = 9;
          current_disp_2--;
          if (current_disp_2 < 0) {
            current_disp_2 = 9;
            current_disp_1--;
          }
        } else {
          current_disp_3 = 0;
        }
      }
      break;
  }
}

void updateVoltageDisplay() {
  lcd.setCursor(0,1);
  lcd.print(voltage_disp_1);
  lcd.print(voltage_disp_2);
  lcd.print(".");
  lcd.print(voltage_disp_3);
  lcd.setCursor(curr_col, curr_row);
}

void updateCurrentDisplay() {
  lcd.setCursor(0,2);
  lcd.print(current_disp_1);
  lcd.print(current_disp_2);
  lcd.print(current_disp_3);
  lcd.setCursor(curr_col, curr_row);
}

void handleRotaryEncoder() {
  CLK_PIN_state = (CLK_PIN_state << 1) | digitalRead(CLK_PIN) | 0xE000;
  if (CLK_PIN_state == 0xF000) {
    CLK_PIN_state = 0x0000;

    if (curr_row == 1) { // Voltage row
      if (digitalRead(DT_PIN)) incrementVoltageDigit(curr_col);
      else decrementVoltageDigit(curr_col);
      updateVoltageDisplay();
    } 
    else if (curr_row == 2) { // Current row
      if (digitalRead(DT_PIN)) incrementCurrentDigit(curr_col);
      else decrementCurrentDigit(curr_col);
      updateCurrentDisplay();
    }
  }
}

void changeSelectedVariable() {
  if (SW.isPressed()) {
    switch (curr_row) {
      case 1: // move from voltage row to current row
        curr_row = 2;
        if (curr_col == 3) curr_col = 2;
        break;
      case 2: // move from current row to voltage row
        curr_row = 1;
        if (curr_col == 2) curr_col = 3;
        break;
    }
    lcd.setCursor(curr_col, curr_row);
  }
}

void moveCursorRight() {
  if (BTN_R.isPressed()) {
    switch (curr_row) {
      case 1:
        switch (curr_col){
          case 0: curr_col++; break;
          case 1: curr_col = 3; break;
          case 3: curr_col = 3;
        } break;
      case 2:
        curr_col++;
        if (curr_col > 2) curr_col = 2;
        break;
    }
    lcd.setCursor(curr_col, curr_row);
    lcd.cursor();
  }
}

void moveCursorLeft() {
  if (BTN_L.isPressed()) {
    switch (curr_row){
      case 1:
        switch (curr_col){
          case 0: curr_col = 0; break;
          case 1: curr_col--; break;
          case 3: curr_col = 1; break;
        } break;
      case 2:
        curr_col--;
        if (curr_col < 0) curr_col = 0;
        break;
    }
    lcd.setCursor(curr_col, curr_row);
    lcd.cursor();
  }
}

void toggleOutput() {
  if (BTN_OUT.isPressed()) {
    output_state = !output_state;
    lcd.setCursor(17, 1);
    if (output_state) {
      lcd.print("   ");  // Clear previous
      lcd.setCursor(17, 1);
      lcd.print("ON");
    } else {
      lcd.print("  ");  // Clear previous
      lcd.setCursor(17, 1);
      lcd.print("OFF");
    }
    lcd.setCursor(curr_col, curr_row);
  }
}

void handleInputs() {
  changeSelectedVariable();
  moveCursorRight();
  moveCursorLeft();
  toggleOutput();
}

void setup() {
  Serial.begin(9600);

  // Button debounce time setting in ms
  BTN_L.setDebounceTime(50);
  BTN_R.setDebounceTime(50);
  BTN_OUT.setDebounceTime(50);
  SW.setDebounceTime(50);

  // Input pins configuration
  pinMode(CLK_PIN,INPUT);
  pinMode(DT_PIN,INPUT);
  
  // LCD initialization
  lcd_initialization();
}

void loop() {
  // Push button check state loop
  BTN_L.loop();
  BTN_R.loop();
  BTN_OUT.loop();
  SW.loop();

  // Rotary encoder control
  handleRotaryEncoder();

  // Input buttons control
  handleInputs();
}