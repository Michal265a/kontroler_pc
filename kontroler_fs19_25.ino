#include <pcf8574.h>
#include <Wire.h>
#include <Keyboard.h>
#include <Mouse.h>

/*  PODPIĘCIE:
    D2 - PCF15 (SDA)
    D3 - PCF14 (SCL)
    D4(A6) - mode (rotate switch)
    D5 - joystick button 1
    D6 - joystick button 2
    D7 - joystick button 3
    D8 - switches column 1
    D9 - switches column 2
    D10 - switches column 3
    D11 - switches column 4
    D12 - switches column 5
    A0 - joystick 1 X axle
    A1 - joystick 1 Y axle
    A2 - joystick 2 X axle
    A3 - joystick 2 Y axle
    A4 - joystick 3 X axle
    A5 - joystick 3 Y axle
*/

PCF8574 expander(0x20);

const int joystick_dead_zone = 256;
int pressed_shift = 0;

bool p_tab_sw[9][5] = {false};
bool tab_sw[9][5] = {false};
uint8_t mode = 0;
const int32_t interval = 50;
int32_t p_t_stamp = 0;
int32_t t_stamp = 0;
uint16_t mode_analog = 0;

void read_controller();
void set_action_fs19();
void set_action_fs25();

void setup() {
  Keyboard.begin();
  Mouse.begin();
  for (int i = 5; i < 8; ++i)
    pinMode(i, INPUT_PULLUP);
  for (int i = 8; i < 13; ++i)
    pinMode(i, INPUT);
  for (int i = 0; i < 6; ++i) {
    pinMode(expander, i, OUTPUT);
    digitalWrite(expander, i, HIGH);
  }
  //read mode
  mode_analog = analogRead(A6);
  mode = map(mode_analog, -170, 1194, 1, 5); //scale to 1-2-3-4
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 5; ++j) {
      p_tab_sw[i][j] = true;
      tab_sw[i][j] = true;
    }
  }
}

void loop() {
  while (t_stamp - p_t_stamp <= interval) {
    t_stamp = millis();
  }
  p_t_stamp = t_stamp;
  mode_analog = analogRead(A6);
  mode = map(mode_analog, -170, 1194, 1, 5); //scale to 1-2-3-4

  read_controller();
  if (mode == 1)
    set_action_fs19();
  else if (mode == 2)
    set_action_fs25();

  //rewrite tab
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 5; ++j) {
      p_tab_sw[i][j] = tab_sw[i][j];
    }
  }
}

void read_controller() {
  //read switches
  for (int i = 0; i < 6; ++i) {
    digitalWrite(expander, i, LOW);
    for (int j = 0; j < 5; ++j) {
      tab_sw[i][j] = digitalRead(j + 8);
    }
    digitalWrite(expander, i, HIGH);
  }
  //read joysticks
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 2; ++j) {
      int16_t axis = analogRead(2 * i + 18 + j) - 512;
      if (axis < -joystick_dead_zone) {
        //directions: up,down,right,left,push
        tab_sw[6 + i][2 * j + 0] = 1;
        tab_sw[6 + i][2 * j + 1] = 0;
      }
      else if (axis > joystick_dead_zone) {
        //directions: 5 lines above
        tab_sw[6 + i][2 * j + 0] = 0;
        tab_sw[6 + i][2 * j + 1] = 1;
      }
      else {
        tab_sw[6 + i][2 * j + 0] = 1;
        tab_sw[6 + i][2 * j + 1] = 1;
      }
    }
    tab_sw[6 + i][4] = digitalRead(i + 5);
  }
}

void set_action_fs19() {
  if (p_tab_sw[0][0] == 1 && tab_sw[0][0] == 0) { //wciśnięto A1
    Keyboard.write(KEY_RETURN); //enter
  }
  if (p_tab_sw[0][1] == 1 && tab_sw[0][1] == 0) { //wciśnięto A2
    Keyboard.press('z'); //z
  }
  if (p_tab_sw[0][1] == 0 && tab_sw[0][1] == 1) { //puszczono A2
    Keyboard.release('z'); //z
  }
  if (p_tab_sw[0][2] == 1 && tab_sw[0][2] == 0) { //wciśnięto A3
    Keyboard.write('r'); //r
  }
  if (p_tab_sw[0][3] == 1 && tab_sw[0][3] == 0) { //wciśnięto A4
    Keyboard.write('t'); //t
  }
  if (p_tab_sw[0][4] == 1 && tab_sw[0][4] == 0) { //wciśnięto A5
    Keyboard.write('i'); //i
  }
  if (p_tab_sw[1][0] == 1 && tab_sw[1][0] == 0) { //wciśnięto B1
    Keyboard.write('y'); //y
  }
  if (p_tab_sw[1][1] == 1 && tab_sw[1][1] == 0) { //wciśnięto B2
    Keyboard.write('o'); //o
  }
  if (p_tab_sw[1][2] == 1 && tab_sw[1][2] == 0) { //wciśnięto B3
    Keyboard.write('q'); //q
  }
  if (p_tab_sw[1][3] == 1 && tab_sw[1][3] == 0) { //wciśnięto B4
    Keyboard.write('u'); //u
  }
  if (p_tab_sw[1][4] == 1 && tab_sw[1][4] == 0) { //wciśnięto B5
    Keyboard.write('g'); //g
  }
  if (p_tab_sw[2][0] == 1 && tab_sw[2][0] == 0) { //wciśnięto C1
    Keyboard.write('v'); //v
  }
  if (p_tab_sw[2][1] == 1 && tab_sw[2][1] == 0) { //wciśnięto C2
    Keyboard.write('b'); //b
  }
  if (p_tab_sw[2][2] == 1 && tab_sw[2][2] == 0) { //wciśnięto C3
    Keyboard.write('x'); //x
  }
  if (p_tab_sw[2][3] == 1 && tab_sw[2][3] == 0) { //wciśnięto C4
    Keyboard.write(KEY_TAB); //tab
  }
  if (p_tab_sw[2][4] == 1 && tab_sw[2][4] == 0) { //wciśnięto C5
    Keyboard.press(KEY_LEFT_SHIFT);
    Keyboard.write(KEY_TAB); //shift+tab
    Keyboard.release(KEY_LEFT_SHIFT);
  }
  if (p_tab_sw[3][0] == 1 && tab_sw[3][0] == 0) { //wciśnięto D1
    Keyboard.write(92 + 136); //num4
  }
  if (p_tab_sw[3][1] == 1 && tab_sw[3][1] == 0) { //wciśnięto D2
    Keyboard.write(98 + 136); //num0
  }
  if (p_tab_sw[3][2] == 1 && tab_sw[3][2] == 0) { //wciśnięto D3
    Keyboard.write(93 + 136); //num5
  }
  if (p_tab_sw[3][3] == 1 && tab_sw[3][3] == 0) { //wciśnięto D4
    Keyboard.write(94 + 136); //num6
  }
  if (p_tab_sw[3][4] == 1 && tab_sw[3][4] == 0) { //wciśnięto D5
    Keyboard.write(KEY_DELETE); //delete
  }
  if (p_tab_sw[4][0] == 1 && tab_sw[4][0] == 0) { //wciśnięto E1
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.write('c'); //alt+c
    Keyboard.release(KEY_LEFT_ALT);
  }
  if (p_tab_sw[4][1] == 1 && tab_sw[4][1] == 0) { //wciśnięto E2
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.write('x'); //alt+x
    Keyboard.release(KEY_LEFT_ALT);
  }
  if (p_tab_sw[4][2] == 1 && tab_sw[4][2] == 0) { //wciśnięto E3
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_PAGE_UP); //alt+PgUp
  }
  if (p_tab_sw[4][2] == 0 && tab_sw[4][2] == 1) { //puszczono E3
    Keyboard.release(KEY_PAGE_UP); //alt+PgUp
    Keyboard.release(KEY_LEFT_ALT);
  }
  if (p_tab_sw[4][3] == 1 && tab_sw[4][3] == 0) { //wciśnięto E4
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_PAGE_DOWN); //alt+PgDn
  }
  if (p_tab_sw[4][3] == 0 && tab_sw[4][3] == 1) { //puszczono E4
    Keyboard.release(KEY_PAGE_DOWN); //alt+PgDn
    Keyboard.release(KEY_LEFT_ALT);
  }
  if (p_tab_sw[4][4] == 1 && tab_sw[4][4] == 0) { //wciśnięto E5
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.write(KEY_DELETE); //alt+delete
    Keyboard.release(KEY_LEFT_ALT);
  }
  if (p_tab_sw[5][0] == 1 && tab_sw[5][0] == 0) { //wciśnięto F1
    Keyboard.write('c'); //c
  }
  if (p_tab_sw[5][1] == 1 && tab_sw[5][1] == 0) { //wciśnięto F2
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.write('s'); //ctrl+s
    Keyboard.release(KEY_LEFT_CTRL);
  }
  if (p_tab_sw[5][2] == 1 && tab_sw[5][2] == 0) { //wciśnięto F3
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.write(KEY_INSERT); // alt+ins
    Keyboard.release(KEY_LEFT_ALT);
  }
  if (p_tab_sw[5][3] == 1 && tab_sw[5][3] == 0) { //wciśnięto F4
    Keyboard.write(KEY_ESC); //esc
  }
  if (p_tab_sw[5][4] == 1 && tab_sw[5][4] == 0) { //wciśnięto F5
    Keyboard.press(KEY_LEFT_CTRL); //ctrl
  }
  if (p_tab_sw[5][4] == 0 && tab_sw[5][4] == 1) { //puszczono F5
    Keyboard.release(KEY_LEFT_CTRL); //ctrl
  }
  if (p_tab_sw[6][0] == 1 && tab_sw[6][0] == 0) { //wciśnięto Joy1↑
    Keyboard.press('j'); //j
  }
  if (p_tab_sw[6][0] == 0 && tab_sw[6][0] == 1) { //puszczono Joy1↑
    Keyboard.release('j'); //j
  }
  if (p_tab_sw[6][1] == 1 && tab_sw[6][1] == 0) { //wciśnięto Joy1↓
    Keyboard.press('n'); //n
  }
  if (p_tab_sw[6][1] == 0 && tab_sw[6][1] == 1) { //puszczono Joy1↓
    Keyboard.release('n'); //n
  }
  if (p_tab_sw[6][2] == 1 && tab_sw[6][2] == 0) { //wciśnięto Joy1→
    Keyboard.press('k'); //k
  }
  if (p_tab_sw[6][2] == 0 && tab_sw[6][2] == 1) { //puszczono Joy1→
    Keyboard.release('k'); //k
  }
  if (p_tab_sw[6][3] == 1 && tab_sw[6][3] == 0) { //wciśnięto Joy1←
    Keyboard.press('m'); //m
  }
  if (p_tab_sw[6][3] == 0 && tab_sw[6][3] == 1) { //puszczono Joy1←
    Keyboard.release('m'); //m
  }
  if (p_tab_sw[7][0] == 1 && tab_sw[7][0] == 0) { //wciśnięto Joy2↑
    pressed_shift++;
    if (pressed_shift == 1) {
      Keyboard.press(KEY_LEFT_SHIFT);
    }
    Keyboard.press('j'); //shift+j
  }
  if (p_tab_sw[7][0] == 0 && tab_sw[7][0] == 1) { //puszczono Joy2↑
    Keyboard.release('j'); //shift+j
    pressed_shift--;
    if (pressed_shift == 0) {
      Keyboard.release(KEY_LEFT_SHIFT);
    }
  }
  if (p_tab_sw[7][1] == 1 && tab_sw[7][1] == 0) { //wciśnięto Joy2↓
    pressed_shift++;
    if (pressed_shift == 1) {
      Keyboard.press(KEY_LEFT_SHIFT);
    }
    Keyboard.press('n'); //shift+n
  }
  if (p_tab_sw[7][1] == 0 && tab_sw[7][1] == 1) { //puszczono Joy2↓
    Keyboard.release('n'); //shift+n
    pressed_shift--;
    if (pressed_shift == 0) {
      Keyboard.release(KEY_LEFT_SHIFT);
    }
  }
  if (p_tab_sw[7][2] == 1 && tab_sw[7][2] == 0) { //wciśnięto Joy2→
    pressed_shift++;
    if (pressed_shift == 1) {
      Keyboard.press(KEY_LEFT_SHIFT);
    }
    Keyboard.press('k'); //shift+k
  }
  if (p_tab_sw[7][2] == 0 && tab_sw[7][2] == 1) { //puszczono Joy2→
    Keyboard.release('k'); //shift+k
    pressed_shift--;
    if (pressed_shift == 0) {
      Keyboard.release(KEY_LEFT_SHIFT);
    }
  }
  if (p_tab_sw[7][3] == 1 && tab_sw[7][3] == 0) { //wciśnięto Joy2←
    pressed_shift++;
    if (pressed_shift == 1) {
      Keyboard.press(KEY_LEFT_SHIFT);
    }
    Keyboard.press('m'); //shift+m
  }
  if (p_tab_sw[7][3] == 0 && tab_sw[7][3] == 1) { //puszczono Joy2←
    Keyboard.release('m'); //shift+m
    pressed_shift--;
    if (pressed_shift == 0) {
      Keyboard.release(KEY_LEFT_SHIFT);
    }
  }
  if (p_tab_sw[8][0] == 1 && tab_sw[8][0] == 0) { //wciśnięto Joy3↑
    Keyboard.press(KEY_UP_ARROW); //↑
  }
  if (p_tab_sw[8][0] == 0 && tab_sw[8][0] == 1) { //puszczono Joy3↑
    Keyboard.release(KEY_UP_ARROW); //↑
  }
  if (p_tab_sw[8][1] == 1 && tab_sw[8][1] == 0) { //wciśnięto Joy3↓
    Keyboard.press(KEY_DOWN_ARROW); //↓
  }
  if (p_tab_sw[8][1] == 0 && tab_sw[8][1] == 1) { //puszczono Joy3↓
    Keyboard.release(KEY_DOWN_ARROW); //↓
  }
  if (p_tab_sw[8][2] == 1 && tab_sw[8][2] == 0) { //wciśnięto Joy3→
    Keyboard.press(KEY_RIGHT_ARROW); //→
  }
  if (p_tab_sw[8][2] == 0 && tab_sw[8][2] == 1) { //puszczono Joy3→
    Keyboard.release(KEY_RIGHT_ARROW); //→
  }
  if (p_tab_sw[8][3] == 1 && tab_sw[8][3] == 0) { //wciśnięto Joy3←
    Keyboard.press(KEY_LEFT_ARROW); //←
  }
  if (p_tab_sw[8][3] == 0 && tab_sw[8][3] == 1) { //puszczono Joy3←
    Keyboard.release(KEY_LEFT_ARROW); //↑
  }
}

void set_action_fs25() {
  if (p_tab_sw[0][0] == 1 && tab_sw[0][0] == 0) { //wciśnięto A1
    Keyboard.write(KEY_RETURN); //enter
  }
  if (p_tab_sw[0][1] == 1 && tab_sw[0][1] == 0) { //wciśnięto A2
    Keyboard.write('z'); //z
  }
  if (p_tab_sw[0][2] == 1 && tab_sw[0][2] == 0) { //wciśnięto A3
    Keyboard.write('r'); //r
  }
  if (p_tab_sw[0][3] == 1 && tab_sw[0][3] == 0) { //wciśnięto A4
    Keyboard.write('t'); //t
  }
  if (p_tab_sw[0][4] == 1 && tab_sw[0][4] == 0) { //wciśnięto A5
    Keyboard.write('i'); //i
  }
  if (p_tab_sw[1][0] == 1 && tab_sw[1][0] == 0) { //wciśnięto B1
    Keyboard.write('g'); //g
  }
  if (p_tab_sw[1][1] == 1 && tab_sw[1][1] == 0) { //wciśnięto B2
    Keyboard.write('o'); //o
  }
  if (p_tab_sw[1][2] == 1 && tab_sw[1][2] == 0) { //wciśnięto B3
    Keyboard.write('y'); //y
  }
  if (p_tab_sw[1][3] == 1 && tab_sw[1][3] == 0) { //wciśnięto B4
    Keyboard.write('u'); //u
  }
  if (p_tab_sw[1][4] == 1 && tab_sw[1][4] == 0) { //wciśnięto B5
    Keyboard.press('h'); //h
  }
  if (p_tab_sw[1][4] == 1 && tab_sw[1][4] == 1) { //puszczono B5
    Keyboard.release('h'); //h
  }
  if (p_tab_sw[2][0] == 1 && tab_sw[2][0] == 0) { //wciśnięto C1
    Keyboard.write('v'); //v
  }
  if (p_tab_sw[2][1] == 1 && tab_sw[2][1] == 0) { //wciśnięto C2
    Keyboard.write('b'); //b
  }
  if (p_tab_sw[2][2] == 1 && tab_sw[2][2] == 0) { //wciśnięto C3
    Keyboard.write('x'); //x
  }
  if (p_tab_sw[2][3] == 1 && tab_sw[2][3] == 0) { //wciśnięto C4
    Keyboard.write('q'); //q
  }
  if (p_tab_sw[2][4] == 1 && tab_sw[2][4] == 0) { //wciśnięto C5
    Keyboard.write(KEY_TAB); //tab
  }
  if (p_tab_sw[3][0] == 1 && tab_sw[3][0] == 0) { //wciśnięto D1
    Keyboard.write(92 + 136); //num4
  }
  if (p_tab_sw[3][1] == 1 && tab_sw[3][1] == 0) { //wciśnięto D2
    Keyboard.write(98 + 136); //num0
  }
  if (p_tab_sw[3][2] == 1 && tab_sw[3][2] == 0) { //wciśnięto D3
    Keyboard.write(93 + 136); //num5
  }
  if (p_tab_sw[3][3] == 1 && tab_sw[3][3] == 0) { //wciśnięto D4
    Keyboard.write(94 + 136); //num6
  }
  if (p_tab_sw[3][4] == 1 && tab_sw[3][4] == 0) { //wciśnięto D5
    Keyboard.write(KEY_DELETE); //delete
  }
  if (p_tab_sw[4][0] == 1 && tab_sw[4][0] == 0) { //wciśnięto E1
    Keyboard.write('e'); //Num/
  }
  if (p_tab_sw[4][1] == 1 && tab_sw[4][1] == 0) { //wciśnięto E2
    Keyboard.write(KEY_KP_PLUS); //Num+
  }
  if (p_tab_sw[4][2] == 1 && tab_sw[4][2] == 0) { //wciśnięto E3
    Keyboard.write(KEY_KP_MINUS); //Num-
  }
  if (p_tab_sw[4][3] == 1 && tab_sw[4][3] == 0) { //wciśnięto E4
    Keyboard.write(KEY_KP_ASTERISK); //Num*
  }
  if (p_tab_sw[4][4] == 1 && tab_sw[4][4] == 0) { //wciśnięto E5
    Keyboard.write(KEY_KP_SLASH); //Num/
  }
  if (p_tab_sw[5][0] == 1 && tab_sw[5][0] == 0) { //wciśnięto F1
    Keyboard.write('c'); //c
  }
  if (p_tab_sw[5][1] == 1 && tab_sw[5][1] == 0) { //wciśnięto F2
    Keyboard.write('p'); //p
  }
  if (p_tab_sw[5][2] == 1 && tab_sw[5][2] == 0) { //wciśnięto F3
    Keyboard.write(KEY_ESC); // esc
  }
  if (p_tab_sw[5][3] == 1 && tab_sw[5][3] == 0) { //wciśnięto F4
    Keyboard.press(KEY_LEFT_CTRL); //ctrl
  }
  if (p_tab_sw[5][3] == 0 && tab_sw[5][3] == 1) { //puszczono F4
    Keyboard.release(KEY_LEFT_CTRL); //ctrl
  }
  if (p_tab_sw[5][4] == 1 && tab_sw[5][4] == 0) { //wciśnięto F5
    Keyboard.write(' '); //space
  }
  if (p_tab_sw[6][0] == 1 && tab_sw[6][0] == 0) { //wciśnięto Joy1↑
    Keyboard.press('j'); //j
  }
  if (p_tab_sw[6][0] == 0 && tab_sw[6][0] == 1) { //puszczono Joy1↑
    Keyboard.release('j'); //j
  }
  if (p_tab_sw[6][1] == 1 && tab_sw[6][1] == 0) { //wciśnięto Joy1↓
    Keyboard.press('n'); //n
  }
  if (p_tab_sw[6][1] == 0 && tab_sw[6][1] == 1) { //puszczono Joy1↓
    Keyboard.release('n'); //n
  }
  if (p_tab_sw[6][2] == 1 && tab_sw[6][2] == 0) { //wciśnięto Joy1→
    Keyboard.press('k'); //k
  }
  if (p_tab_sw[6][2] == 0 && tab_sw[6][2] == 1) { //puszczono Joy1→
    Keyboard.release('k'); //k
  }
  if (p_tab_sw[6][3] == 1 && tab_sw[6][3] == 0) { //wciśnięto Joy1←
    Keyboard.press('m'); //m
  }
  if (p_tab_sw[6][3] == 0 && tab_sw[6][3] == 1) { //puszczono Joy1←
    Keyboard.release('m'); //m
  }
  if (p_tab_sw[7][0] == 1 && tab_sw[7][0] == 0) { //wciśnięto Joy2↑
    pressed_shift++;
    if (pressed_shift == 1) {
      Keyboard.press(KEY_LEFT_SHIFT);
    }
    Keyboard.press('j'); //shift+j
  }
  if (p_tab_sw[7][0] == 0 && tab_sw[7][0] == 1) { //puszczono Joy2↑
    Keyboard.release('j'); //shift+j
    pressed_shift--;
    if (pressed_shift == 0) {
      Keyboard.release(KEY_LEFT_SHIFT);
    }
  }
  if (p_tab_sw[7][1] == 1 && tab_sw[7][1] == 0) { //wciśnięto Joy2↓
    pressed_shift++;
    if (pressed_shift == 1) {
      Keyboard.press(KEY_LEFT_SHIFT);
    }
    Keyboard.press('n'); //shift+n
  }
  if (p_tab_sw[7][1] == 0 && tab_sw[7][1] == 1) { //puszczono Joy2↓
    Keyboard.release('n'); //shift+n
    pressed_shift--;
    if (pressed_shift == 0) {
      Keyboard.release(KEY_LEFT_SHIFT);
    }
  }
  if (p_tab_sw[7][2] == 1 && tab_sw[7][2] == 0) { //wciśnięto Joy2→
    pressed_shift++;
    if (pressed_shift == 1) {
      Keyboard.press(KEY_LEFT_SHIFT);
    }
    Keyboard.press('k'); //shift+k
  }
  if (p_tab_sw[7][2] == 0 && tab_sw[7][2] == 1) { //puszczono Joy2→
    Keyboard.release('k'); //shift+k
    pressed_shift--;
    if (pressed_shift == 0) {
      Keyboard.release(KEY_LEFT_SHIFT);
    }
  }
  if (p_tab_sw[7][3] == 1 && tab_sw[7][3] == 0) { //wciśnięto Joy2←
    pressed_shift++;
    if (pressed_shift == 1) {
      Keyboard.press(KEY_LEFT_SHIFT);
    }
    Keyboard.press('m'); //shift+m
  }
  if (p_tab_sw[7][3] == 0 && tab_sw[7][3] == 1) { //puszczono Joy2←
    Keyboard.release('m'); //shift+m
    pressed_shift--;
    if (pressed_shift == 0) {
      Keyboard.release(KEY_LEFT_SHIFT);
    }
  }
  if (p_tab_sw[8][0] == 1 && tab_sw[8][0] == 0) { //wciśnięto Joy3↑
    Keyboard.press(KEY_UP_ARROW); //↑
  }
  if (p_tab_sw[8][0] == 0 && tab_sw[8][0] == 1) { //puszczono Joy3↑
    Keyboard.release(KEY_UP_ARROW); //↑
  }
  if (p_tab_sw[8][1] == 1 && tab_sw[8][1] == 0) { //wciśnięto Joy3↓
    Keyboard.press(KEY_DOWN_ARROW); //↓
  }
  if (p_tab_sw[8][1] == 0 && tab_sw[8][1] == 1) { //puszczono Joy3↓
    Keyboard.release(KEY_DOWN_ARROW); //↓
  }
  if (p_tab_sw[8][2] == 1 && tab_sw[8][2] == 0) { //wciśnięto Joy3→
    Keyboard.press(KEY_RIGHT_ARROW); //→
  }
  if (p_tab_sw[8][2] == 0 && tab_sw[8][2] == 1) { //puszczono Joy3→
    Keyboard.release(KEY_RIGHT_ARROW); //→
  }
  if (p_tab_sw[8][3] == 1 && tab_sw[8][3] == 0) { //wciśnięto Joy3←
    Keyboard.press(KEY_LEFT_ARROW); //←
  }
  if (p_tab_sw[8][3] == 0 && tab_sw[8][3] == 1) { //puszczono Joy3←
    Keyboard.release(KEY_LEFT_ARROW); //↑
  }
 }