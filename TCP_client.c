#define _WIN32_WINNT 0x0600
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <string.h>
#include <stdio.h>
#include <conio.h>

int main(int argc, char *argv[]) {
   
   WSADATA d;
  
   if (WSAStartup(MAKEWORD(2, 2), &d)) {
      fprintf(stderr, "Error: %d\n", WSAGetLastError);
      return 1;
   }
   
   if (argc < 3) {
      fprintf(stderr, "Not enough arguments. Error: %d\n", WSAGetLastError);
      return 1;
   }

   printf("Configure remote address..\n");
   struct addrinfo hints;
   struct addrinfo *peer_address;
   hints.ai_socktype = SOCK_STREAM;
   memset(&hints, 0, sizeof(hints));
   getaddrinfo(argv[1], argv[2], &hints, &peer_address);   
   
   printf("The remote address is: \n");
   char buffer_address[100];
   char buffer_service[100];
   getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen, buffer_address, sizeof(buffer_address), buffer_service, sizeof(buffer_service), NI_NUMERICHOST);
   printf("%s %s\n", buffer_address, buffer_service); 
  
   SOCKET listen_socket = socket(peer_address->ai_family, peer_address->ai_protocol, IPPROTO_TCP);
  
   printf("Connecting.\n");
   connect(listen_socket, peer_address->ai_addr, peer_address->ai_addrlen);

   printf("Connection successful.\n");
   printf("Start sending data.\n");
    
   while(1) {
      fd_set set;
      FD_ZERO(&set);
      FD_SET(listen_socket, &set);
      
      struct timeval timeout;
      timeout.tv_sec = 0.1;
      
      int selecting = select(listen_socket+1, &set, 0, 0, &timeout);
      if (selecting < 0) {
         fprintf(stderr, "Error: %d\n", WSAGetLastError);
         return 1; 
      }
    
      if(FD_ISSET(listen_socket, &set)) {
         char reading[4096];
         int receiving = recv(listen_socket, reading, sizeof(reading), 0);
         if (receiving < 1) {
            fprintf(stderr, "Error: %d\n", WSAGetLastError);
            return 1;
         }
         printf("Received (%d bytes): %.*s", receiving, receiving, reading);
      } 
   
      if(_kbhit()) {
      char reading[4096];
      fgets(reading, sizeof(reading), stdin);
      int send_bytes = send(listen_socket, reading, strlen(reading), 0);
      }
   } 

   closesocket(listen_socket);
   WSACleanup();
   return 0;
}

//This is a TCP client. Enter "CTRL+C" to exit

