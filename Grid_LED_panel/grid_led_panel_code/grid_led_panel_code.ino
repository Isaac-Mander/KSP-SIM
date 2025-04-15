#include <KerbalSimpit.h>

KerbalSimpit mySimpit(Serial);
//Store the current action status, as recevied by simpit.
byte currentActionStatus = 0;

float current_g_force;

class LedButtonPanel {
  private:
    int* input_pins;
    int input_pin_count;
    int* output_pins;
    int output_pin_count;
  public:
    template <size_t N, size_t M>
    LedButtonPanel(int (&in_pins)[N], int (&out_pins)[M]) {
      input_pins = in_pins;
      input_pin_count = N;
      output_pins = out_pins;
      output_pin_count = M;
    }
  
    void init() {
      //Set the input pins as inputs
      for (int i = 0; i < input_pin_count; i++) {
        pinMode(input_pins[i], INPUT_PULLUP);
      }

      //Set the output pins as outputs
      for (int i = 0; i < output_pin_count; i++) {
        int pin = output_pins[i];
        pinMode(pin, OUTPUT);
        digitalWrite(pin,LOW);
      }
    }

    bool read_button(int index) {
      //Invert the output as LOW means pressed
      return !digitalRead(input_pins[index]);
    }

    void set_LED(int index, bool state) {
      digitalWrite(output_pins[index],state);
    }
};

int input_pins[2] = {A2,A1};
int output_pins[7] = {3,7,4,8,5,9,2};

LedButtonPanel LBO(input_pins,output_pins);

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


void set_led(int pin, bool state)
{

}


void setup() {
  connect_to_ksp();
  
}

void loop() {
  mySimpit.update();
  delay(100);
}