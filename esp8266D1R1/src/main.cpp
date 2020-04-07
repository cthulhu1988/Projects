#include <painlessMesh.h>
#include <Hash.h>
#include "ESP8266WiFi.h"
#include <SPI.h>
#include <MFRC522.h>
#include "Arduino.h"
#include "FS.h"
#include "blockchain.h"
#include "block.h"
<<<<<<< HEAD

=======
>>>>>>> 4ade4cb30ea7f5b82c8d595a6ab3897e4bb51b0f
/////////////////////////////////////////////////////////////////////////////////////
#define   LED             2    // GPIO number of connected LED
#define   BLINK_PERIOD    3000 // milliseconds until cycle repeat
#define   BLINK_DURATION  200  // milliseconds LED is on for

// These are network credentials unique to the peer-to-peer network.
#define   MESH_SSID       "NETGEAR94"
#define   MESH_PASSWORD   "shinycarrot"
#define   MESH_PORT       5555

// Output pins for the RFID SCANNER
constexpr uint8_t RST_PIN =  0; constexpr uint8_t SS_PIN =  15; MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
//////////////////////////////////////////////////////////////////////////////

/////////////////////////// GENESIS BLOCK LOGIC /////////////////
bool isGenesisBlock = true;
bool newAssetTag = false;

//////////////////// PAINLESS MESH Function Prototypes /////////////////////////
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);
void delayReceivedCallback(uint32_t from, int32_t delay);
void sendMessage() ;
Task taskSendMessage( TASK_SECOND * 2, TASK_FOREVER, &sendMessage ); // start with a two second interval

/////////////////////// TRUSTED MEMBER NODES ///////////////////////////////
long long int trustedNodes[3] = {2731010923, 2731822602, 2731822745};

////////////////////////SPIFFS Prototypes ////////////////////////////////
void ReadFlashFile();
void DeleteFlashFiles();
void writeFlashFiles(String s);
void ReadLastLine();
//////////////Class instantiation OF MESH ///////////////////////////
Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;

////////////////// Variables /////////////////////////////////////
bool calc_delay = false;
SimpleList<uint32_t> nodes;
uint32 chipId = system_get_chip_id();
Task blinkNoNodes;
bool onFlag = false;
bool success = false;
bool scan = true;
String inStringHex = "";
String thisNodeStr = "";

int blockCount;
bool flashToSend = false;
String lineToSend ="";

// Blockchain Object //
blockchain newChain;
//////////////////////////////////////////////////////////////////////////////////////////////////
////////// SETUP LOOP ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  delay(2000);
  blockCount = 0;

  //// RFID SCANNER
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details

  //////////////////////////
  // Try to mount the SPIFFS system.
  if(!SPIFFS.begin()){
    Serial.println("ERROR mounting system");
    return;
  } else {Serial.println("Spiffs (Flash Storage) MOUNTED");}

  // Act on the mesh instantiation //
  mesh.setDebugMsgTypes(ERROR | DEBUG);  // set before init() so that you can see error messages
  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);
  userScheduler.addTask( taskSendMessage ); taskSendMessage.enable();

  // Blinks number of times to indicate how many nodes are connected to the mesh
  pinMode(LED, OUTPUT);
  blinkNoNodes.set(BLINK_PERIOD, (mesh.getNodeList().size() + 1) * 2, []() {
      onFlag ? onFlag = false : onFlag = true;
      blinkNoNodes.delay(BLINK_DURATION);
      if (blinkNoNodes.isLastIteration()) {
        blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
        blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
      }
  });
  userScheduler.addTask(blinkNoNodes);
  blinkNoNodes.enable();
  randomSeed(analogRead(A0));
  thisNodeStr += String(mesh.getNodeId());
}

////////////////////////////////////////////////////////////////////////////////////
///////////////////////// END SETUP LOOP ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////START MAIN LOOP ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
void loop() {
  mesh.update();
  digitalWrite(LED, !onFlag);

  // RFID sensor returns if no new tag is read.
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
  //delay(50);
  //mesh.update();
  return;
  }
    ///Read RFID data from card or fob ///
    inStringHex = "";
    for (byte i = 0; i < 4; i++) {
    inStringHex += String(mfrc522.uid.uidByte[i], HEX);
    }

    /// READ DATA FROM FLASH FILE
    if(inStringHex == "d6ac5923"){
      Serial.println("READING FLASH FILE:");
      ReadFlashFile();
      delay(100);
    }

    if(inStringHex == "44c38d23"){
      Serial.println("DELETING FLASH FILE:");
      DeleteFlashFiles();
      delay(100);
    }

    if(inStringHex == "199219e5"){
      Serial.println("PRINTING CHAIN :");
      newChain.printChain();
      delay(100);
    }


                  /// Delete Card //            /// Read flash files //       /// print blockchain ///
    if(inStringHex != "44c38d23" && inStringHex != "d6ac5923" && inStringHex != "199219e5" ){

      Serial.print("Asset Tag::");
      Serial.println(inStringHex);
      Serial.println();

      newAssetTag = true;
      String hex = inStringHex.c_str();

      if (isGenesisBlock) {
        newChain = blockchain();
        String record = newChain.GetLastRecord();
        writeFlashFiles(record);
        writeFlashFiles("\n");
        ReadLastLine();
      } else {
        block nBlock = block(blockCount, hex);
        newChain.AddBlock(nBlock);
        String record = newChain.GetLastRecord();
        writeFlashFiles(record);
        writeFlashFiles("\n");
        ReadLastLine();
      }
      mesh.update();
    }
    mfrc522.PICC_HaltA();
    if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
      return;
  }
///////////////////////// END RFID FUNCTIONS /////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////END MAIN LOOP ////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



//////////////////////////READ, WRITE, DELETE TO and from DISK //////////////////
void ReadFlashFile(){
  File f = SPIFFS.open("/chain.txt", "r");
  while(f.available()){
    Serial.write(f.read());
  }
  f.close();
  Serial.println();
}

void ReadLastLine(){
  String lastLine = "";
  File f = SPIFFS.open("/chain.txt", "r");
  while(f.available()){
    lastLine += char(f.read());
  }
  int fileSize = f.size();
  lineToSend = lastLine.substring((fileSize-99), fileSize);
  flashToSend = true;
  f.close();
}

void DeleteFlashFiles(){
  SPIFFS.remove("/chain.txt");
}

void writeFlashFiles(String s){
  File file = SPIFFS.open("/chain.txt", "a");
  file.print(s);
  if(!isGenesisBlock){
    Serial.println(s);
  } else {
    Serial.println("The following GENESIS BLOCK was written: ");
    isGenesisBlock = false;
    Serial.println(s);
  }
  file.close();
}

////////////////////////////END WRITE TO DISK ///////////////////////////////////////////////



////////////////////////// Painless Mesh Functions /////////////////////////////////////
void sendMessage() {
  if (newAssetTag & flashToSend) {
    mesh.sendBroadcast(lineToSend);
    lineToSend = "";
    newAssetTag = false;
    flashToSend = false;
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
}

// from the onReceive method, from is the node that is sending. The message can be anything.
void receivedCallback(uint32_t from, String & msg) {
  String prev_hash = msg.substring(1,41);
  String data_rec = msg.substring(45,53);
  String block_hash = msg.substring(57,97);
  //Serial.println(prev_hash);
  //Serial.println(data_rec);
  //Serial.println(block_hash);

  Serial.printf("Node Number of Sender: %u -- Message: %s\n", from, msg.c_str());
  bool memberNode = false;
  for(int i = 0; i < 3; i++){
    if(from == trustedNodes[i]){ memberNode = true; }   }
  memberNode == true ? Serial.println("Sender IS a member Node") : Serial.println("Sender is NOT a member Node");

  if (memberNode && isGenesisBlock)
  {
      isGenesisBlock = false;
      // start blockchain
      newChain = blockchain();
      writeFlashFiles(msg);
      writeFlashFiles("\n");
  }
  else if(memberNode && !isGenesisBlock)
  {
      block nBlock = block(blockCount,data_rec);
      newChain.AddBlock(nBlock);
      writeFlashFiles(msg);
      writeFlashFiles("\n");
  }
}

// When a new node is connected Fires everytime a node makes a new connection //
void newConnectionCallback(uint32_t nodeId) {
  onFlag = false;
  blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
  blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
  Serial.printf("New Connection, %s\n", mesh.subConnectionJson(true).c_str());
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
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

void nodeTimeAdjustedCallback(int32_t offset) {}
  //Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
//}
void delayReceivedCallback(uint32_t from, int32_t delay) {}
  //Serial.printf("Delay to node %u is %d us\n", from, delay);}
    //msg += mesh.getNodeId();
    //msg += " myFreeMemory: " + String(ESP.getFreeHeap());
