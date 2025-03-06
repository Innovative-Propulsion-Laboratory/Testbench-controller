#include <QNEthernet.h>
using namespace qindesign::network;
constexpr uint32_t kDHCPTimeout = 15'000;  //waiting time 15 seconds
constexpr uint16_t kPort = 5190;  // Chat port
uint16_t senderPort; 
IPAddress senderIP;

EthernetUDP udp;
int t;

void setup() {
  Serial.begin(11500);


  /////////////////////////////////////////////////////
  // Initilisation and setup //
  printf("Starting...\r\n");
  Ethernet.begin();
  Ethernet.onLinkState([](bool state) {
    printf("[Ethernet] Link %s\r\n", state ? "ON" : "OFF");
  });
  if (!Ethernet.waitForLocalIP(kDHCPTimeout)) {
    printf("Failed to get IP address\r\n");
    return;
  }
  IPAddress ip = Ethernet.localIP();
  printf("    Local IP     = %u.%u.%u.%u\r\n", ip[0], ip[1], ip[2], ip[3]);
  udp.begin(kPort);
  /////////////////////////////////////////////////////

}


void loop() {
  receivePacket();
}

void reply(int tracker, byte* index) {
    
    if tracker) {
        case 0: // Confirmation
          framesize = index.sizeof() + 4;
          byte frame[10] = concatenateArrays({0xEE, 0xEE,0XEE,0xEE},index )
          udp.send(senderIP, senderPort, message, sizeof(message)); 
        case 4: // IHM
            Serial.println("IHM control");
            break;
        
        case 5: // Start test
            Serial.println("Starting test");
            break;
        
        case 6: // Confirm test
            Serial.println("Test confirmed");
            break;
        
        case 7: // Abort test
            Serial.println("Test aborted");
            break;
        
        case 8: // Send data
            Serial.println("Sending data");
            break;
        
        case 9: // Copy of command
            Serial.println("Copying command");
            break;
        
        case 10: // Warning 1
            Serial.println("Warning 1 triggered");
            break;
        
        case 11: // Warning 2
            Serial.println("Warning 2 triggered");
            break;
    }
}

void receivePacket() {
  int size = udp.parsePacket();
  if (size < 0) { 
    return;
  }

  const uint8_t *data = udp.data();
  senderIP = udp.remoteIP();
  senderPort = udp.remotePort();
  printf("[%u.%u.%u.%u][%d] ", senderIP[0], senderIP[1], senderIP[2], senderIP[3], size);

  byte instructions[size];  
  for (int j = 0; j < size; j++) { // copy data
    instructions[j] = data[j]; 
    Serial.printf("%#04x ",instructions[j]); 
  }

  if (instructions[0] == 0xFF  && instructions[1] == 0xFF && instructions[2] == 0xFF && instructions[3] == 0xFF){ // Valve

    switch (instructions[5]) {
      case 0x00: // SV11
      case 0x01: // SV12
      case 0x02: // SV13
      case 0x03: // SV21
      case 0x04: // SV22
      case 0x05: // SV24
      case 0x06: // SV31
      case 0x07: // SV32
      case 0x08: // SV33
      case 0x09: // SV34
      case 0x10: // SV35
      case 0x11: // SV36
      case 0x12: // SV51
      case 0x13: // SV52
      case 0x14: // SV53
      case 0x15: // SV61
      case 0x16: // SV62
      case 0x17: // SV63
      case 0x18: // SVXX
        if (instructions[6] == 0x00 || instructions[6] == 0x01) {
          //setValve(req[j], req[j + 1]);  // Activer ou désactiver la valve
          // byte index[2] = {instructions[5],instructions[6]};
          reply(0,instructions);
        }
        break;
    }
  }
  if (instructions[0] == 0xFF  && instructions[1] == 0xFF && instructions[2] == 0xEE && instructions[3] == 0xEE){ // bang-bang

  }
  if (instructions[0] == 0xEE  && instructions[1] == 0xEE && instructions[2] == 0xEE && instructions[3] == 0xEE){ // Actuators

  }
  if (instructions[0] == 0xEE  && instructions[1] == 0xEE && instructions[2] == 0xDD && instructions[3] == 0xDD){ // TVC pattern

  }
  if (instructions[0] == 0xDD  && instructions[1] == 0xDD && instructions[2] == 0xDD && instructions[3] == 0xDD){ // IHM on/off

  }
  if (instructions[0] == 0xAA  && instructions[1] == 0xAA && instructions[2] == 0xAA && instructions[3] == 0xAA){ // Start test

  } 
  if (instructions[0] == 0xBB  && instructions[1] == 0xBB && instructions[2] == 0xBB && instructions[3] == 0xBB){ // Confirm test

  } 
  if (instructions[0] == 0xCC  && instructions[1] == 0xCC && instructions[2] == 0xCC && instructions[3] == 0xCC){ // Abort test

  } 

  printf("\n\r");
}

void concatenateArrays(int* arr1, int size1, int* arr2, int size2, int* result) {
  // Copy elements from the first array
  for (int i = 0; i < size1; i++) {
    result[i] = arr1[i];
  }

  // Copy elements from the second array
  for (int i = 0; i < size2; i++) {
    result[size1 + i] = arr2[i];
  }
}
