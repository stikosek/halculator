#include "SPI.h"
#include "TFT_eSPI.h"
#include "Keypad.h"
#include "tinyexpr.h"

TFT_eSPI tft = TFT_eSPI();

const byte rows = 7;
const byte cols = 6;
char keys[rows][cols] = {
  {'M'  ,  'w'  ,  ' '  ,  ' '  ,  ' '  ,  ' '},
  {'a'  ,  'o'  ,  'd'  ,  ' '  ,  ' '  ,  ' '},
  {'H'  ,  's'  ,  ' '  ,  '+'  ,  ' '  ,  ' '},
  {'7'  ,  '8'  ,  '9'  ,  '-'  ,  'p'  ,  ' '},
  {'4'  ,  '5'  ,  '6'  ,  '*'  ,  '^'  ,  ','},
  {'1'  ,  '2'  ,  '3'  ,  '/'  ,  'r'  ,  'C'},
  {'0'  ,  '.'  ,  '='  ,  '('  ,  ')'  ,  'R'}
};

char calculable[20] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '(', ')', '^', '*', '/', '-', '+', '.', '√', ','};

byte rowPins[rows] = {PB13, PB14, PB15, PB3, PB4, PB5, PB8};
byte colPins[cols] = {PB0, PB1, PB2, PB10, PB11, PB12};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols );

int screens_count = 2;
int screens[2] = {1,3};
int screens_index = 0;
int screen = 1;

int last_rendered_screen = 0;
String equasion = "";

void setup() {
  tft.init();//
  tft.fillScreen(TFT_BLUE);
  tft.setRotation(1);
  tft.setTextSize(5);
  tft.println("Halculator");
  tft.setTextSize(2);
  tft.println("\nstikosek.xyz");
  delay(1000);
}

bool is_calculable(char key) {
  // i forgor how to check the length of an array in arduino lolol
  for (int i = 0; i < 20; i++) {
      if (key == calculable[i]) {
          return true;
      }
  }
  return false;
}

void render() {
  switch (screen) {
      case 1:
        render_calculate();
        break;
      case 2:
        render_h();
        break;
  }
}



void render_calculate() {
  tft.fillScreen(TFT_BLUE);
  tft.setTextColor(TFT_BLACK, TFT_BLUE);
  tft.setCursor(0, 0);
  tft.setTextSize(2);
  rerender_calculate();
}

void rerender_calculate() {
  // Blankout current EQ
  tft.fillRect(0,0,320,220, TFT_BLUE);

  // Write EQ
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setCursor(0,0);
  tft.println("Expression:\n");
  tft.print(equasion);

  // REnder banner
  render_calc_banner(0);
}

void render_calc_banner(float value) {
  tft.fillRect(0,220, 320, 20, TFT_CYAN);
  tft.setTextColor(TFT_BLACK, TFT_GREEN);
  tft.setCursor(2, 222);
  tft.setTextSize(2);
  tft.print(" CALC ");
  tft.setTextColor(TFT_BLACK, TFT_CYAN);
  tft.print(" ="+String(calculate_equasion()));
}

void render_cheatsheet() {
  tft.fillScreen(TFT_BLUE);
  tft.fillRect(0,220, 320, 20, TFT_CYAN);
  tft.setTextColor(TFT_BLACK, TFT_YELLOW);
  tft.setCursor(2, 222);
  tft.setTextSize(2);
  tft.print(" CHEAT-SHEET ");

  // Render things
  tft.setCursor(0, 0);
}

void handle_keypress(char key) {
  // Handle global keys

  if (key == 'M'){
    // Switch to next mode
    if (screens_index + 1 == screens_count) {
      // Is on the end, switch to start
      screens_index = 0;
    } else {
      screens_index++;
    }
    screen = screens[screens_index];


    render();
    return;
  }


  if (screen == 1) { // Calc screen
    // Add char into EQ if it is "calculable"¨
    bool calculable = is_calculable(key);
    if (calculable == true) {
      // Append this character to the EQ
      equasion = equasion + key;
      rerender_calculate();
    } else {
      // Handle special functions
      if (key == 'C') {
        equasion = equasion[equasion.length() - 1] = '\0';
        rerender_calculate();
      }
      if (key == 'R') {
        int l = equasion.length() - 1;
        equasion.remove(l,1);
        rerender_calculate();
      }
       if (key == 'r') {
        equasion = equasion + "sqrt(";
        rerender_calculate();
      }
       if (key == 'p') {
        equasion = equasion + "pi()";
        rerender_calculate();
      }
    }


  }
}

double calculate_equasion() {
  int error = 0;
  char eq[equasion.length() +1];
  equasion.toCharArray(eq, sizeof(eq));
  double done = te_interp(eq, &error);
  return done;
}


void loop() {
  // Insure the correct screen is rendered
  if (last_rendered_screen != screen) {
      render();
      last_rendered_screen = screen;
  }

  // Handle keys
  char key = keypad.getKey();
  if(key != NO_KEY) {
    //tft.println(key);
    handle_keypress(key);
  }
}


