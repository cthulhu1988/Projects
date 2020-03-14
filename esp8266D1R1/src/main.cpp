#include <painlessMesh.h>
#include <Hash.h>
#include "ESP8266WiFi.h"
#include <SPI.h>
#include <MFRC522.h>

#define   LED             2       // GPIO number of connected LED, ON ESP-12 IS GPIO2
#define   BLINK_PERIOD    3000 // milliseconds until cycle repeat
#define   BLINK_DURATION  100  // milliseconds LED is on for
#define   MESH_SSID       "NETGEAR94"
#define   MESH_PASSWORD   "shinycarrot"
#define   MESH_PORT       5555

//////////////////// Function Prototypes /////////////////////////
void sendMessage();
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);
void delayReceivedCallback(uint32_t from, int32_t delay);
void sendMessage() ;
Task taskSendMessage( TASK_SECOND * 1, TASK_FOREVER, &sendMessage ); // start with a one second interval
//////////////////////////////////////////////////////////////////

//// Need NTP clock to get time stamp

struct block {
  int timestamp;
  char* assetTag;
  int dataHash;
  int prevHash;
};


//////////////Class instantiation ?///////////////////////////
Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;


//////////////////Variables /////////////////////////////////////
bool calc_delay = false;
SimpleList<uint32_t> nodes;
uint32 chipId = system_get_chip_id();
Task blinkNoNodes;
bool onFlag = false;

//////////////////////////////////////
////////// SETUP LOOP ////////////////
//////////////////////////////////////
void setup() {

  struct block genesis;
  genesis.timestamp = 0;
  genesis.prevHash = 0;
  printf("size of genesis is %d\n", sizeof(block) );


  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  // Act on the mesh instantiation //
  mesh.setDebugMsgTypes(ERROR | DEBUG);  // set before init() so that you can see error messages
  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();

  blinkNoNodes.set(BLINK_PERIOD, (mesh.getNodeList().size() + 1) * 2, []() {
      onFlag ? onFlag = false : onFlag = true;
      blinkNoNodes.delay(BLINK_DURATION);

      if (blinkNoNodes.isLastIteration()) {
        // Finished blinking. Reset task for next run
        // blink number of nodes (including this node) times
        blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
        // Calculate delay based on current mesh time and BLINK_PERIOD
        // This results in blinks between nodes being synced
        blinkNoNodes.enableDelayed(BLINK_PERIOD -
            (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
      }
  });
  userScheduler.addTask(blinkNoNodes);
  blinkNoNodes.enable();
  randomSeed(analogRead(A0));
}
///////////////////////// END SETUP LOOP ////////////////////////////////////////////
void loop() {
  //printf("size of genesis is %d\n", sizeof(block) );
  mesh.update();
  digitalWrite(LED, !onFlag);
}


//////////////////////////////// Painless Mesh Functions /////////////////////////////////////
void sendMessage() {
  String msg = "Hello from node ";
  msg += mesh.getNodeId();
  msg += " myFreeMemory: " + String(ESP.getFreeHeap());
  mesh.sendBroadcast(msg);

  // Send a node to a packet to meashure the trip delay //
  if (calc_delay) {
    SimpleList<uint32_t>::iterator node = nodes.begin();
    while (node != nodes.end()) {
      mesh.startDelayMeas(*node);
      node++;
    }
    calc_delay = false;
  }

  Serial.printf("Sending message: %s\n", msg.c_str());
  // set an interval to send a message at random times.
  taskSendMessage.setInterval( random(TASK_SECOND * 1, TASK_SECOND * 5));  // between 1 and 5 seconds
}

// from the onReceive method, from is the node that is sending. The message can be anything.
void receivedCallback(uint32_t from, String & msg) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

// When a new node is connected Fires everytime a node makes a new connection //
void newConnectionCallback(uint32_t nodeId) {
  // Reset blink task
  onFlag = false;
  blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
  blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);

  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
  Serial.printf("--> startHere: New Connection, %s\n", mesh.subConnectionJson(true).c_str());
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
  // Reset blink task
  onFlag = false;
  blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
  blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);

  nodes = mesh.getNodeList();

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
  calc_delay = true;
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}


// void setup() {
//   Serial.begin(115200);
//
// }
//
// void loop() {
//   uint32 chipId = system_get_chip_id();
//   Serial.printf("%d\n", chipId);
//   // usage as String
//   // SHA1:a9993e364706816aba3e25717850c26c9cd0d89d
//   Serial.print("SHA1:");
//   Serial.println(sha1("abc"));
//   // usage as ptr
//   SHA1:a94a8fe5ccb19ba61c4c0873d391e987982fbbd3
//   uint8_t hash[20];
//   sha1("test", &hash[0]);
//   Serial.print("SHA1:");
//   for (uint16_t i = 0; i < 20; i++) {
//     Serial.printf("%02x", hash[i]);
//   }
//   Serial.println();
//   delay(1000);
// }
