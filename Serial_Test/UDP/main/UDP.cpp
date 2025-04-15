#include "UDP.h"

EthernetUDP udp;
IPAddress senderIP;
uint16_t senderPort = 12345;
uint32_t kDHCPTimeout = 15000;  //waiting time 15 seconds
uint16_t kPort = 5190;          // Chat port
int message_size = 0;
int t;
bool fisrt_message;
uint8_t mac[6] = { 0x00, 0x1A, 0xB6, 0x03, 0x2B, 0x77 };
// Setup UDP

void setupUDP() {
  Ethernet.begin();
  Ethernet.onLinkState([](bool state) {});  // No debug output
  if (!Ethernet.waitForLocalIP(kDHCPTimeout)) {
    return;  // Exit if no IP address is assigned
  }

  fisrt_message = false;

  IPAddress ip = Ethernet.localIP();
  Serial.printf("    Local IP     = %u.%u.%u.%u\r\n", ip[0], ip[1], ip[2], ip[3]);
  udp.begin(kPort);
}

// void set_sender_info() {
//   senderIP = udp.remoteIP();  // enlever et les définir une seule fois  fct first message
//   // senderPort = udp.remotePort();
//   Serial.print("Sender Port : ");
//   Serial.println(senderPort);
// }

// Communication functions

// void reply(byte* message, uint16_t size) {  // send the message send
//   udp.send(senderIP, senderPort, message, size);
// }


// void send_data(void* payload, uint16_t size) {// debug
//   Serial.print("Payload (hex): ");
//   byte* data = (byte*)payload;
//   for (uint16_t i = 0; i < size; i++) {
//     if (data[i] < 0x10) Serial.print("0");
//     Serial.print(data[i], HEX);
//     Serial.print(" ");
//   }
//   Serial.println();

//   udp.send(senderIP, senderPort, data, size);
// }

// void send_data(void* payload, uint16_t size) {
//   udp.send(senderIP, senderPort, (byte*)payload, size);
// }

// void send_string(const char* message, uint16_t length) {
//   udp.send(senderIP, senderPort, (byte*)message, length);
// }

// Packet receivePacket() {
//   int size = udp.parsePacket();

//   if (size <= 0) {
//     // Pas de paquet reçu → on retourne un buffer avec une seule valeur nulle
//     byte* instructions = new byte[1]{ 0 };
//     return { instructions, 1 };
//   }

//   if (!fisrt_message) {
//     set_sender_info();
//     fisrt_message = true;
//   }

//   const uint8_t* data = udp.data();

//   printf("[%u.%u.%u.%u][%d] ", senderIP[0], senderIP[1], senderIP[2], senderIP[3], size);

//   byte* instructions = new byte[size];
//   for (int j = 0; j < size; j++) {
//     instructions[j] = data[j];
//     Serial.printf("%#04x ", instructions[j]);
//   }
//   Serial.println();
//   return { instructions, size };  // on retourne bien {data, taille}
}




//////Old Code ////////////


// uint16_t assembleUInt16(uint8_t lowByte, uint8_t highByte) { // to assemble 2 byte
//   return (static_cast<uint16_t>(highByte) << 8) | static_cast<uint16_t>(lowByte);
// }

// void old_reply(int tracker, byte* index, int size) {
//     if(tracker == 1) { // data
//       byte message[size+4];
//       message[0] = 0xFF;
//       message[1] = 0xFF;
//       message[2] = 0xFF;
//       message[3] = 0xFF;
//       for (int j = 0; j < size; j++) {message[j+4] = index[j]; }  // copy data
//       udp.send(senderIP, senderPort, message, sizeof(message));}
//     else if (tracker == 2) { // Start test
//       byte message[size+4];
//       message[0] = 0xDD;
//       message[1] = 0xDD;
//       message[2] = 0xDD;
//       message[3] = 0xDD;
//       for (int j = 0; j < size; j++) { // copy data
//       message[j+4] = index[j]; }
//       udp.send(senderIP, senderPort, message, sizeof(message));}
//     else if (tracker == 3) { // Confirm test
//         Serial.println("Test confirmed");}
//     else if (tracker == 4) { // Abort test
//       byte message[size+4];
//       message[0] = 0xCC;
//       message[1] = 0xCC;
//       message[2] = 0xCC;
//       message[3] = 0xCC;
//       for (int j = 0; j < size; j++) { // copy data
//       message[j+4] = index[j]; }
//       udp.send(senderIP, senderPort, message, sizeof(message));}
//     else if (tracker == 5) { // Abort test
//       byte message[size+4];
//       message[0] = 0xBB;
//       message[1] = 0xBB;
//       message[2] = 0xBB;
//       message[3] = 0xBB;
//       for (int j = 0; j < size; j++) { // copy data
//       message[j+4] = index[j]; }
//       udp.send(senderIP, senderPort, message, sizeof(message));}
// }