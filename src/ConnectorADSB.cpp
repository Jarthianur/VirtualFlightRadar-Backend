#include "ConnectorADSB.h"

ConnectorADSB::ConnectorADSB(const char* srchost, int srcport, int dstport)
: response(""),
  linebuffer(""),
  adsb_in_host(srchost),
  adsb_in_port(srcport),
  nmea_out_port(dstport)
{
   memset(buffer,0,sizeof(buffer));
}

ConnectorADSB::~ConnectorADSB()
{
   close();
}

const char* ConnectorADSB::getSrcHost() const
{
   return adsb_in_host;
}

void ConnectorADSB::close()
{
   ::close(adsb_in_sock);
   ::close(nmea_out_sock);
   ::close(xcs_cli_sock);
}

int ConnectorADSB::readLine()
{
#define EOL "\r\n"
   int eol = linebuffer.find(EOL);
   if (eol == -1) {
      if (recv(adsb_in_sock, buffer, 2047, 0) == -1) {
         return -1;
      }
      linebuffer.append(buffer);
      eol = linebuffer.find(EOL);
   }
   eol += 2;
   try {
      response = linebuffer.substr(0,eol);
   } catch (const std::out_of_range& e) {
      std::cout << e.what();
   }
   linebuffer.clear();
   if (response.length() == 0) {
      return -2;
   }
   return response.length();
}

const int ConnectorADSB::getSrcPort() const
{
   return adsb_in_port;
}

int ConnectorADSB::connect()
{
   std::cout << "connect() ..." << std::endl;
   if ((adsb_host_info = gethostbyname(adsb_in_host)) == NULL) {
      std::cout << "Could not resolve Hostname!" << std::endl;
      return -1;
   }

   if ((adsb_in_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      std::cout << "Could not create socket!" << std::endl;
      return -1;
   }

   memset(&adsb_in_addr, 0, sizeof(adsb_in_addr));
   adsb_in_addr.sin_family = AF_INET;
   adsb_in_addr.sin_port = htons(adsb_in_port);
   adsb_in_addr.sin_addr = *((struct in_addr*) adsb_host_info->h_addr);

   std::cout << "address.fam= " << adsb_in_addr.sin_family << std::endl;
   std::cout << "address.port= " << ntohs(adsb_in_addr.sin_port) << std::endl;
   std::cout << "address.addr= " << inet_ntoa(xcs_cli_addr.sin_addr) << std::endl;
   if (::connect(adsb_in_sock, (struct sockaddr*) &adsb_in_addr, sizeof(struct sockaddr)) == -1) {
      std::cout << "Could not connect to server!" << std::endl;
      close();
      return -1;
   }

   std::cout << "connected to ADS-B" << std::endl;

   // now try to accept connection from xcsoar
   std::cout << "connecting to xcsoar..." << std::endl;

   if ((nmea_out_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      std::cout << "Could not create socket!" << std::endl;
      return -1;
   }

   if (setsockopt(nmea_out_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      std::cout << "Could not set socketopt REUSEADDR!" << std::endl;
      return -1;
   }

   memset(&nmea_out_addr, 0, sizeof(nmea_out_addr));
   nmea_out_addr.sin_family = AF_INET;
   nmea_out_addr.sin_port = htons(nmea_out_port);
   nmea_out_addr.sin_addr.s_addr = INADDR_ANY;

   if (bind(nmea_out_sock, (struct sockaddr*)&nmea_out_addr, sizeof(struct sockaddr)) == -1) {
      std::cout << "Could not bind socket!" << std::endl;
      return -1;
   }

   if (listen(nmea_out_sock, 20) == -1) {
      std::cout << "Could not listen to socket!" << std::endl;
      return -1;
   }
   int con = 0;
   //now wait for xcsoar to connect
   while (con == 0) {
      std::cout << "wait for xcsoar..."<< std::endl;
      unsigned int sin_s = sizeof(struct sockaddr);
      xcs_cli_sock = accept(nmea_out_sock, (struct sockaddr*)&xcs_cli_addr, &sin_s);
      if (xcs_cli_sock == -1) {
         std::cout << "Could not accept connection!" << std::endl;
         return -1;
      }
      char* addr = inet_ntoa(xcs_cli_addr.sin_addr);
      if (std::strcmp(addr, "127.0.0.1") != 0) {
         std::cout << "Do not accept connection from " << addr << std::endl;
         return -1;
      }
      std::cout<< "connection from " << addr <<std::endl;
      con = 1;
   }
   std::cout << "connected to xcsoar"<< std::endl;

   return 0;
}

int ConnectorADSB::sendMsg(std::string& msg)
{
   if (send(xcs_cli_sock, msg.c_str(), msg.length(), 0) > 0) {
      std::cout << "sent msg" << std::endl;
   }
   return 0;
}

const int ConnectorADSB::getDstPort() const
{
   return nmea_out_port;
}
