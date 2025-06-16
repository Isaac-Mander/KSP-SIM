#include "TM1637.h"
#include <TM1637TinyDisplay6.h>

#include <KerbalSimpit.h>
//Store the current action status, as recevied by simpit.
byte currentActionStatus = 0;

KerbalSimpit mySimpit(Serial);

const int MAIN_DISP_DIO = A3;
const int MAIN_DISP_CLK = A2;
TM1637TinyDisplay6 main_disp(MAIN_DISP_CLK, MAIN_DISP_DIO);

const int FIRST_DISP_DIO = 11;
const int FIRST_DISP_CLK = 10;
TM1637 first_disp(FIRST_DISP_CLK, FIRST_DISP_DIO);

const int SEC_DISP_DIO = A4;
const int SEC_DISP_CLK = A5;
TM1637 sec_disp(SEC_DISP_CLK, SEC_DISP_DIO);


const int KEYPAD_ROW[] = {5,4,3,2};
const int KEYPAD_COL[] = {8,9,7,6};


float velocity;
float current_g_force;



void connect_to_ksp() {
  // Set up the build in LED, and turn it on.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  // Start serial
  Serial.begin(115200);
  // This loop continually attempts to handshake with the plugin.
  // It will keep retrying until it gets a successful handshake.
  while (!mySimpit.init()) {
    delay(100);
  }
  
  // Turn off the built-in LED to indicate handshaking is complete.
  digitalWrite(LED_BUILTIN, LOW);
  
  // Display a message in KSP to indicate handshaking is complete.
  mySimpit.printToKSP("Connected", PRINT_TO_SCREEN);
  // Sets our callback function. The KerbalSimpit library will
  // call this function every time a packet is received.
  mySimpit.inboundHandler(messageHandler);
  // Send a message to the plugin registering for the Action status channel.
  // The plugin will now regularly send Action status  messages while the
  // flight scene is active in-game.

  // mySimpit.registerChannel(AIRSPEED_MESSAGE);
  mySimpit.registerChannel(TARGETINFO_MESSAGE);
  
}

void messageHandler(byte messageType, byte msg[], byte msgSize) {
  switch(messageType) {
    case VELOCITY_MESSAGE:
    if (msgSize == sizeof(velocityMessage)) {
      velocityMessage velocityMessage_info;
      velocityMessage_info = parseMessage<velocityMessage>(msg);
      surface_v = (velocityMessage_info.surface);
      orbital_v = (velocityMessage_info.orbital);
      vertical_v = (velocityMessage_info.vertical);
    }
    case TARGETINFO_MESSAGE:
    if (msgSize == sizeof(targetMessage)) {
      targetMessage targetMessage_info;
      targetMessage_info = parseMessage<targetMessage>(msg);
      target_rel_v = (targetMessage_info.velocity);
    }
  }
}

void switch_scan() {
    // for (int row = 0; row <= 4; row++)
  // {
    digitalWrite(KEYPAD_ROW[0],LOW);
    delay(10);
    for (int col = 0; col <= 4; col++)
    {
      if(digitalRead(KEYPAD_COL[0]) == LOW)
      {
        Serial.print(0);
        Serial.print(":");
        Serial.println(col);
      }
    }
  // }
  
}

void update_display(int program, int mode, int main_display) {
  main_disp.showNumber(main_display);
  first_disp.displayNum(program);
  sec_disp.displayNum(mode);
}


/* The infomation display program 
1. Velocity
2. Orbit
3. Target
4. Maneuver
5. Raw Rotation
6. Action groups
7. Fuel
8. 
9. 
*/
void display_infomation(int mode) {
  if(mode == 1) {update_display(1,mode,surface_v);} //Surface Velocity
  else if(mode == 2) {update_display(1,mode,orbital_v);} //Orbital Velocity
  else if(mode == 3) {update_display(1,mode,surface_v);} //Vertical Velocity


  else if(mode == 10) {update_display(1,mode,surface_v);}
  else if(mode == 2) {update_display(1,mode,surface_v);}
}


void setup() {
  connect_to_ksp();
  randomSeed(analogRead(0));

  main_disp.setBrightness(2);
  main_disp.clear();

  first_disp.init();
  first_disp.set(BRIGHT_TYPICAL);

  sec_disp.init();
  sec_disp.set(BRIGHT_TYPICAL);


  // Serial.begin(9600);
  //Start initial display states
  main_disp.showString("bootnG");
  first_disp.displayNum(8888);
  sec_disp.displayNum(8888);
  delay(2000);
  main_disp.clear();
  first_disp.displayNum(0);
  sec_disp.displayNum(0);

  for (int i = 0; i <= 4; i++)
  {
    pinMode(KEYPAD_ROW[i],OUTPUT);
    digitalWrite(KEYPAD_ROW[i],HIGH);
    pinMode(KEYPAD_COL[i],INPUT_PULLUP);
  }
}
int index = 0;
long test_val = 0;
void loop() {
  mySimpit.update();
  delay(100);
}
