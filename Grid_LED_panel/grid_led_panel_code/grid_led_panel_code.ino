#include <KerbalSimpit.h>

KerbalSimpit mySimpit(Serial);
//Store the current action status, as recevied by simpit.
byte currentActionStatus = 0;

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
  
  // Turn off the built-in LED to indicate handshaking is co                                            mplete.
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

void update_action(bool new_state, int custom_group) {
  if (new_state) {
    mySimpit.activateAction(custom_group);
  }
  if (!new_state) {
    mySimpit.deactivateAction(custom_group);
  }
}






const int abort_sw = 10, stage_sw = 11;
const int abort_led = 3, high_g_led = 4, stage_led = 5, low_g_led = 8;

bool is_abort_active = false;
unsigned long start_of_abort = 0;
unsigned long previousMillis = 0;
int ledState = LOW;

bool stage_sw_latch = true;



const bool DEBUG = false;
void setup() {
  Serial.begin(9600);

  pinMode(abort_led,OUTPUT);
  pinMode(high_g_led,OUTPUT);
  pinMode(stage_led,OUTPUT);
  pinMode(low_g_led,OUTPUT);

  pinMode(abort_sw,INPUT_PULLUP);
  pinMode(stage_sw,INPUT_PULLUP);

  if(!DEBUG) {connect_to_ksp();}
}

void loop() {
  //Abort mode
  bool abort_sw_state = !digitalRead(abort_sw);
  if (abort_sw_state == true) {
    is_abort_active = true;
  }
  else
  {
    is_abort_active = false;
  }

  if (is_abort_active) {
    if (millis() - previousMillis >= 100) {
      // save the last time you blinked the LED
      previousMillis = millis();

      // if the LED is off turn it on and vice-versa:
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      digitalWrite(abort_led, ledState);
    }
  }
  else {
    start_of_abort = millis();
    previousMillis = start_of_abort;
  }

  if(!DEBUG) {update_action(abort_sw_state,ABORT_ACTION);}


  //Stage Switch
  bool stage_sw_state = digitalRead(stage_sw);
  if(stage_sw_state && !stage_sw_latch)
  {
    update_action(true,STAGE_ACTION);
    delay(1); //SLIGHT DELAY SO THE ACTION IS REGISTERED IN THE GAME
    stage_sw_latch = true;
  }
  else if(!stage_sw_state && stage_sw_latch)
  {
    stage_sw_latch = false;
  }
  digitalWrite(stage_led,stage_sw_latch);




  //Low G LED
  if(current_g_force >= 0.05 && current_g_force < 1) {digitalWrite(low_g_led,HIGH);}
  else {{digitalWrite(low_g_led,LOW);}}
  //High G LED
  if(current_g_force >= 9.0) {digitalWrite(high_g_led,HIGH);}
  else {{digitalWrite(high_g_led,LOW);}}

  
  if(!DEBUG) {mySimpit.update();}
}
