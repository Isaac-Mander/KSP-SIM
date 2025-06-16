#include <KerbalSimpit.h>

KerbalSimpit mySimpit(Serial);

//Store the current action status, as recevied by simpit.
byte currentActionStatus = 0;

float current_g_force;




// Define column and row pins
const int colPins[] = {43, 45, 37, 41, 35, 26};   // Column pins
const int rowPins[] = {32, 28, 30, 24}; // Row pins

// Set the number of rows and columns
const int numRows = 4;
const int numCols = 6;

// Define a structure to store button information
struct Button {
  int buttonNum;  // Button number (e.g., 10)
  int col;        // Column number (e.g., 4)
  int row;        // Row number (e.g., 3)
};

Button buttons[] = {
  {1,1,3},
  {2,0,3},
  {3,2,3},
  {4,2,3},
  {5,5,3},
  {6,5,1},
  {7,4,1},
  {8,3,1},
  {9,3,3},
  {10,4,3},
  {11,1,1},
  {12,0,1},
  {13,2,1},
  {14,3,2},
  {15,4,2},
  {16,1,0},
  {17,2,0},
  {18,0,0},
  {19,3,0},
  {20,4,0}
};


// Set the pin numbers:
const int PITCH_PIN = A1;    // the pin used for controlling pitch
const int YAW_PIN = A2;     // the pin used for controlling roll

const int action_group_leds[] = {5,6,7,8,9,10,11,12};   // LED PINS
const int num_action_group_leds = sizeof(action_group_leds) / sizeof(action_group_leds[0]);

//AUX Led panel pin definitions
const int gear_led_pin = A10;
const int brake_led_pin = A13;
const int gear_switch_pin = A12;
const int low_g_led_pin = A11;
const int high_g_led_pin = A14;


//General use connector (OVERLAPS WITH AUX PANEL AS IT IS CURRENTLY NOT IN USE)
const int general_pin_1 = A8;
const int general_pin_2 = A9;
const int general_pin_3 = A10;
const int general_pin_4 = A13;
const int general_pin_5 = A12;
const int general_pin_6 = A11;
const int general_pin_7 = A14;


// Define SAS and RCS LED pins
const int sas_leds[] = {53}; // Example with multiple LEDs
const int rcs_leds[] = {51};
const int num_sas_leds = sizeof(sas_leds) / sizeof(sas_leds[0]);
const int num_rcs_leds = sizeof(rcs_leds) / sizeof(rcs_leds[0]);

// Function to update SAS and RCS LEDs based on action status
void update_sas_rcs_leds() {
    bool sas_state = currentActionStatus & SAS_ACTION;
    bool rcs_state = currentActionStatus & RCS_ACTION;
    
    for (int i = 0; i < num_sas_leds; i++) {
        digitalWrite(sas_leds[i], sas_state);
    }
    for (int i = 0; i < num_rcs_leds; i++) {
        digitalWrite(rcs_leds[i], rcs_state);
    }
}

//Switch numbers
const unsigned int rcs_switch = 1;
const unsigned int sas_switch = 2;
const unsigned int gear_switch = 15;
const unsigned int light_switch = 14;
const unsigned int brake_switch = 16;
const unsigned int stage_switch = 20;
const unsigned int a1_switch = 5;
const unsigned int a2_switch = 6;
const unsigned int a3_switch = 7;
const unsigned int a4_switch = 8;
const unsigned int a5_switch = 10;
const unsigned int a6_switch = 11;
const unsigned int a7_switch = 12;
const unsigned int a8_switch = 13;

bool custom_group_state[] = {false,false,false,false,false,false,false,false};
  
const int numButtons = sizeof(buttons) / sizeof(buttons[0]);

// Function to find the button number based on the row and column
int findButtonNumber(int col, int row) {
  for (int i = 0; i < numButtons; i++) {
    // Check if the current button's row and column match the input
    if (buttons[i].row == row && buttons[i].col == col) {
      return buttons[i].buttonNum;
    }
  }
  return -1;  // Return -1 if no matching button is found (shouldn't happen)
}

int switch_scan(int result[]) {
  //Set all col HIGH
  for (int i = 0; i < numCols; i++) {
    digitalWrite(colPins[i], HIGH);  // Set all columns to HIGH initially
  }
  
  //COL loop
  for (int current_col = 0; current_col < numCols; current_col++) {
    //Pull col down and scan rows
    digitalWrite(colPins[current_col], LOW);
    delay(1);
    for (int current_row = 0; current_row < numRows; current_row++) {

      int state = digitalRead(rowPins[current_row]);
      int switch_id = findButtonNumber(current_col,current_row);
      result[switch_id-1] = !state;
    }
    digitalWrite(colPins[current_col], HIGH);
    }
}

void main_switch_matrix_setup() {
    // Initialize column pins as output
  for (int i = 0; i < numCols; i++) {
    pinMode(colPins[i], OUTPUT);
    digitalWrite(colPins[i], HIGH);  // Set all columns to HIGH initially
  }

  // Initialize row pins as input with pull-up resistors enabled
  for (int i = 0; i < numRows; i++) {
    pinMode(rowPins[i], INPUT_PULLUP);
  }
}

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
  mySimpit.registerChannel(ACTIONSTATUS_MESSAGE);
  mySimpit.registerChannel(AIRSPEED_MESSAGE);
}

void messageHandler(byte messageType, byte msg[], byte msgSize) {
  switch(messageType) {
  case ACTIONSTATUS_MESSAGE:
    // Checking if the message is the size we expect is a very basic
    // way to confirm if the message was received properly.
    if (msgSize == 1) {
      currentActionStatus = msg[0];
    }
    break;
  case AIRSPEED_MESSAGE:
    if (msgSize == sizeof(airspeedMessage)) {
      airspeedMessage airspeedMessage_info;
      airspeedMessage_info = parseMessage<airspeedMessage>(msg);
      current_g_force = airspeedMessage_info.gForces;
    }

  }
}

void update_action(bool new_state, uint32_t action) {
  bool current_state = currentActionStatus & action;

  if (new_state && !current_state) {
    mySimpit.activateAction(action);
  }
  if (!new_state && current_state) {
    mySimpit.deactivateAction(action);
  }
}

void update_ag(bool new_state, int custom_group) {
  bool current_state = custom_group_state[custom_group-1];

  if (new_state && !current_state) {
    mySimpit.activateCAG(custom_group);
  }
  if (!new_state && current_state) {
    mySimpit.deactivateCAG(custom_group);
  }
  custom_group_state[custom_group-1] = new_state;
  int led_pin = action_group_leds[custom_group-1];
  update_action_group_led(led_pin,new_state);
}

void update_action_groups(int states[]) {
  // Update SAS
  bool sas_switch_state = states[sas_switch - 1];
  update_action(sas_switch_state, SAS_ACTION);

  // Update RCS
  bool rcs_switch_state = states[rcs_switch - 1];
  update_action(rcs_switch_state, RCS_ACTION);

  // Update action groups 1 through 8
  bool a1_switch_state = states[a1_switch - 1];
  update_ag(a1_switch_state, 1);
  
  bool a2_switch_state = states[a2_switch - 1];
  update_ag(a2_switch_state, 2);
  
  bool a3_switch_state = states[a3_switch - 1];
  update_ag(a3_switch_state, 3);
  
  bool a4_switch_state = states[a4_switch - 1];
  update_ag(a4_switch_state, 4);
  
  bool a5_switch_state = states[a5_switch - 1];
  update_ag(a5_switch_state, 5);
  
  bool a6_switch_state = states[a6_switch - 1];
  update_ag(a6_switch_state, 6);
  
  bool a7_switch_state = states[a7_switch - 1];
  update_ag(a7_switch_state, 7);
  
  bool a8_switch_state = states[a8_switch - 1];
  update_ag(a8_switch_state, 8);
}

void update_action_group_led(int pin_num, bool state) {
  digitalWrite(pin_num,state);
}

void led_panel_setup() {
    for (int i = 0; i < num_sas_leds; i++) {
        pinMode(sas_leds[i], OUTPUT);
        digitalWrite(sas_leds[i], LOW);  // Ensure LEDs start OFF
    }
    for (int i = 0; i < num_rcs_leds; i++) {
        pinMode(rcs_leds[i], OUTPUT);
        digitalWrite(rcs_leds[i], LOW);  // Ensure LEDs start OFF
    }
    for (int i = 0; i < num_action_group_leds; i++) {
        pinMode(action_group_leds[i], OUTPUT);
        digitalWrite(action_group_leds[i], LOW);  // Ensure LEDs start OFF
    }


}

void soyuz_panel_setup() {
  //Set up the matrix grid
  pinMode(general_pin_1,OUTPUT);
}


//This aux panel is currently not being used
void aux_led_panel_setup() {
    //Aux leds
    pinMode(gear_led_pin,OUTPUT);
    digitalWrite(gear_led_pin,LOW);
    pinMode(gear_led_pin,OUTPUT);
    digitalWrite(gear_led_pin,LOW);
    pinMode(brake_led_pin,OUTPUT);
    digitalWrite(brake_led_pin,LOW);
    pinMode(low_g_led_pin,OUTPUT);
    digitalWrite(low_g_led_pin,LOW);
    pinMode(high_g_led_pin,OUTPUT);
    digitalWrite(high_g_led_pin,LOW);

    pinMode(gear_switch_pin,INPUT_PULLUP);
}
//This aux panel is currently not being used
void update_aux_panel(float current_g_force) {
  // Update GEAR based on switch
  bool gear_switch_state = !digitalRead(gear_switch_pin); //Reversed input so gear is down when switch is tied high
  update_action(gear_switch_state, GEAR_ACTION);
  digitalWrite(gear_led_pin,gear_switch_state);

  //Brake LED
  bool break_state = currentActionStatus & BRAKES_ACTION;
  digitalWrite(brake_led_pin,break_state);
  //Low G LED
  if(current_g_force >= 0.05) {digitalWrite(low_g_led_pin,HIGH);}
  else {{digitalWrite(low_g_led_pin,LOW);}}
  //High G LED
  if(current_g_force >= 9.0) {digitalWrite(high_g_led_pin,HIGH);}
  else {{digitalWrite(high_g_led_pin,LOW);}}
}

void test_mode() {
  //Flash all leds 
}

void setup() {
//   // SWITCH MATRIX SETUP
//   main_switch_matrix_setup();
// //  aux_led_panel_setup();/
//   led_panel_setup();
//   connect_to_ksp();
pinMode(general_pin_1,OUTPUT);
pinMode(general_pin_2,OUTPUT);
pinMode(general_pin_3,OUTPUT);
pinMode(general_pin_4,OUTPUT);
pinMode(general_pin_5,OUTPUT);
pinMode(general_pin_6,OUTPUT);
pinMode(general_pin_7,OUTPUT);
digitalWrite(general_pin_1,HIGH);
digitalWrite(general_pin_2,HIGH);
digitalWrite(general_pin_3,HIGH);
digitalWrite(general_pin_4,HIGH);
digitalWrite(general_pin_5,HIGH);
digitalWrite(general_pin_6,HIGH);
digitalWrite(general_pin_7,HIGH);
}

void loop() {
  // mySimpit.update();
  // int switch_states[20]; //Reset the switch state array back to empty
  // switch_scan(switch_states);
  // update_action_groups(switch_states);
  // update_sas_rcs_leds();
}
