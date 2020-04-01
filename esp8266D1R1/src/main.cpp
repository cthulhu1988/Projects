#include <painlessMesh.h>
#include <Hash.h>
#include "ESP8266WiFi.h"
#include <SPI.h>
#include <MFRC522.h>
#include "Arduino.h"
#include "FS.h"
/////////////////////////////////////////////////////////////////////////////////////
#define   LED             2       // GPIO number of connected LED, ON ESP-12 IS GPIO2
#define   BLINK_PERIOD    3000 // milliseconds until cycle repeat
#define   BLINK_DURATION  200  // milliseconds LED is on for
// These are network credentials unique to the peer-to-peer network.
#define   MESH_SSID       "NETGEAR94"
#define   MESH_PASSWORD   "shinycarrot"
#define   MESH_PORT       5555
//////////////////////////////RFID VARIABLES //////////////////////////////////
// Output pins for the RFID SCANNER
constexpr uint8_t RST_PIN =  0; constexpr uint8_t SS_PIN =  15; MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
//////////////////////////////////////////////////////////////////////////////
bool isGenesisBlock = false;
bool newAssetTag = false;
//////////////////// PAINLESS MESH Function Prototypes /////////////////////////
//void sendMessage();
// messages sent to this node:
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);
void delayReceivedCallback(uint32_t from, int32_t delay);

void sendMessage() ;
Task taskSendMessage( TASK_SECOND * 1, TASK_FOREVER, &sendMessage ); // start with a one second interval
//void sendUpdateToNodes();
//Task taskSendDataBlock(TASK_SECOND * 1, TASK_FOREVER, &sendUpdateToNodes);
//////////////////////////////////////////////////////////////////

///// RFID function prototypes ///////
void dump_byte_array(byte *buffer, byte bufferSize);

////// Blockchain Struct //////////
struct block {
  int timestamp;
  String assetTag;
  int dataHash;
  int prevHash;
};


//int readSize = file.readBytes((byte*) block, sizeof(block));

//////////////Class instantiation OF MESH ///////////////////////////
Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;


//////////////////Variables /////////////////////////////////////
bool calc_delay = false;
SimpleList<uint32_t> nodes;
uint32 chipId = system_get_chip_id();
Task blinkNoNodes;
bool onFlag = false;
bool success = false;
bool scan = true;

String inStringHex = "";
//////////////////////////////////////////////////////////////////////////////////////////////////
////////// SETUP LOOP ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  delay(4000);
  //// RFID SCANNER
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  //////////////////////////


  // Try to mount the SPIFFS system.
  if(!SPIFFS.begin()){
    Serial.println("ERROR mounting system");
    return;
  } else {Serial.println("Spiffs MOUNTED");}
  // Open file to write to
  File f = SPIFFS.open("/chain.txt", "w");
  if (!f) {
    Serial.println("file open failed");
  }

  // Check to see if genesis block has been written to memory.
  // Construct Genesis block in ONE node.

  struct block genesis;
  genesis.timestamp = 0;
  genesis.prevHash = 0;

  pinMode(LED, OUTPUT);
  // Act on the mesh instantiation //
  mesh.setDebugMsgTypes(ERROR | DEBUG);  // set before init() so that you can see error messages
  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
  // Sets a callback routine for any messages addressed to this node
  mesh.onReceive(&receivedCallback);
  // fires everytime a new connection is made.
  mesh.onNewConnection(&newConnectionCallback);
  // fires when there is a change in network topology.
  mesh.onChangedConnections(&changedConnectionCallback);
  // fires every time local time is adjused to match the mesh.
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);

// Adding tasks to the queue.
  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();

  // userScheduler.addTask(taskSendDataBlock);
  // taskSendDataBlock.enable();

  blinkNoNodes.set(BLINK_PERIOD, (mesh.getNodeList().size() + 1) * 2, []() {
      onFlag ? onFlag = false : onFlag = true;
      blinkNoNodes.delay(BLINK_DURATION);
      if (blinkNoNodes.isLastIteration()) {
        // Finished blinking. Reset task for next run // blink number of nodes (including this node) times
        blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
        // Calculate delay based on current mesh time and BLINK_PERIOD
        blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
      }
  });
  userScheduler.addTask(blinkNoNodes);
  blinkNoNodes.enable();
  randomSeed(analogRead(A0));
}
///////////////////////// END SETUP LOOP ////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////START MAIN LOOP ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
void loop() {
  //printf("size of genesis is %d\n", sizeof(block) );
  // update runs various maintainance funtions
  mesh.update();
  digitalWrite(LED, !onFlag);

          /// RFID ///
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())
  {
    newAssetTag = true;
    inStringHex = "";
    for (byte i = 0; i < 4; i++) {
    inStringHex += String(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.print(inStringHex);

  }

}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////END MAIN LOOP ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////



/////////////////////// END RFID FUNCTIONS /////////////////////////////////////////


//////////////////////////WRITE TO DISK /////////////////////////////////////////////

void ReadFlashFile(){
  File f = SPIFFS.open("/chain.txt", "r");
  while(f.available()){
    Serial.write(f.read());
  }
}

void DeleteFlashFiles(){
  SPIFFS.remove("/chain.txt");
}
////////////////////////////END WRITE TO DISK ///////////////////////////////////////////////



//////////////////////////////// Painless Mesh Functions /////////////////////////////////////


void sendMessage() {
  if (newAssetTag) {
    Serial.print("THis is now set to true    ");


    Serial.print(inStringHex);
    String msg = "Asset Tag: ";
    msg += inStringHex;
    //msg += mesh.getNodeId();
    //msg += " myFreeMemory: " + String(ESP.getFreeHeap());
    mesh.sendBroadcast(msg);
    }

    // Send a node to a packet to meashure the trip delay //
    if (calc_delay) {
      SimpleList<uint32_t>::iterator node = nodes.begin();
      while (node != nodes.end()) {
        mesh.startDelayMeas(*node);
        node++;
      }
      calc_delay = false;
    }
    newAssetTag = false;

  //Serial.printf("Sending message: %s\n", msg.c_str());

  // set an interval to send a message at random times. DO NOT HAVE TO USE
  taskSendMessage.setInterval( random(TASK_SECOND * 5, TASK_SECOND * 10));  // between 1 and 5 seconds
}

// from the onReceive method, from is the node that is sending. The message can be anything.
void receivedCallback(uint32_t from, String & msg) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
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
