#include "UDP.h"

EthernetUDP udp;
IPAddress senderIP;
IPAddress ip;
uint16_t senderPort = 12345;
uint32_t kDHCPTimeout = 10000;  //waiting time 15 seconds
uint16_t kPort = 5190;          // Chat port
int message_size = 0;
int t;
bool fisrt_message;
bool UDPactive = false;
// Setup UDP

void setupUDP() {
  Ethernet.begin();
  Ethernet.onLinkState([](bool state) {});  // No debug output
  if (!Ethernet.waitForLocalIP(kDHCPTimeout)) {
    return;  // Exit if no IP address is assigned
  }

  UDPactive = true;
  fisrt_message = false;

  ip = Ethernet.localIP();
  Serial.printf("    Local IP     = %u.%u.%u.%u\r\n", ip[0], ip[1], ip[2], ip[3]);
  udp.begin(kPort);
}

void set_sender_info() {
  senderIP = udp.remoteIP();  // enlever et les définir une seule fois  fct first message
  // senderPort = udp.remotePort();
  Serial.print("Sender Port : ");
  Serial.println(senderPort);
}

// Communication functions

void reply(byte* message, uint16_t size) {  // send the message send
  // byte* instructions = new byte[size];
  // for (int j = 0; j < size; j++) {
  //   instructions[j] = message[j];
  //   Serial.printf("%#04x ", instructions[j]);
  // }
  udp.send(senderIP, senderPort, message, size);
}

void convert(const char* message, uint16_t length, int type) {
  if (type == 0) {
    uint16_t totalLength = 4 + length;

    // Create a buffer on the stack
    uint8_t buffer[totalLength];

    // Add the 4-byte header
    buffer[0] = 0xDD;
    buffer[1] = 0xDD;
    buffer[2] = 0xDD;
    buffer[3] = 0xDD;

    // Add the message bytes (ASCII characters)
    for (uint16_t i = 0; i < length; i++) {
      buffer[4 + i] = static_cast<uint8_t>(message[i]);
    }
    // Send via UDP
    udp.send(senderIP, senderPort, buffer, totalLength);
  } else if (type == 1) {
    uint16_t totalLength = 4 + length;

    // Create a buffer on the stack
    uint8_t buffer[totalLength];

    // Add the 4-byte header
    buffer[0] = 0xCC;
    buffer[1] = 0xCC;
    buffer[2] = 0xCC;
    buffer[3] = 0xCC;

    // Add the message bytes (ASCII characters)
    for (uint16_t i = 0; i < length; i++) {
      buffer[4 + i] = static_cast<uint8_t>(message[i]);
    }

    // ---- AFFICHAGE DU MESSAGE ENVOYÉ ----
    Serial.print("UDP type=1 totalLength=");
    Serial.print(totalLength);
    Serial.print(" header=CC CC CC CC message=\"");

    // affiche la partie message en ASCII
    for (uint16_t i = 0; i < length; i++) {
      char ch = (char)buffer[4 + i];
      // option : remplace les non-imprimables
      if (ch < 32 || ch > 126) ch = '.';
      Serial.print(ch);
    }
    Serial.println("\"");

    // affiche tout le buffer en HEX
    Serial.print("Buffer HEX: ");
    for (uint16_t i = 0; i < totalLength; i++) {
      if (buffer[i] < 0x10) Serial.print('0');
      Serial.print(buffer[i], HEX);
      Serial.print(' ');
    }
    Serial.println();
    // -------------------------------------
    // Send via UDP
    udp.send(senderIP, senderPort, buffer, totalLength);
  }
}

void send_string(const String& msg, int type) {
  convert(msg.c_str(), msg.length(), type);
}

void send_data(void* payload, uint16_t size) {
  udp.send(senderIP, senderPort, (byte*)payload, size);
}

Packet receivePacket() {
  int size = udp.parsePacket();

  if (size <= 0) {
    // Pas de paquet reçu → on retourne un buffer avec une seule valeur nulle
    byte* instructions = new byte[1]{ 0 };
    return { instructions, 1 };
  }

  if (!fisrt_message) {
    set_sender_info();
    fisrt_message = true;
  }

  const uint8_t* data = udp.data();

  printf("[%u.%u.%u.%u][%d] ", senderIP[0], senderIP[1], senderIP[2], senderIP[3], size);

  byte* instructions = new byte[size];
  for (int j = 0; j < size; j++) {
    instructions[j] = data[j];
    Serial.printf("%#04x ", instructions[j]);
  }
  Serial.println();
  return { instructions, size };  // on retourne bien {data, taille}
}