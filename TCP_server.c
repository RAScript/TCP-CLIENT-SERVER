#define _WIN32_WINNT 0x0600
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[]) {

    WSADATA d;
    if(WSAStartup(MAKEWORD(2, 2), &d)){
       fprintf(stderr, "Winsock fail: %d\n", WSAGetLastError);
       return 1;
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    struct addrinfo *bind_address;
    getaddrinfo(0, "8080", &hints, &bind_address);

    SOCKET listen_socket;
    listen_socket = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
    if (listen_socket == INVALID_SOCKET) {
       fprintf(stderr, "Error in socket: %d\n", WSAGetLastError);
       return 1; 
    }

    int binding = bind(listen_socket, bind_address->ai_addr, bind_address->ai_addrlen);
    if (binding) {
       fprintf(stderr, "Error in binding: %d\n", WSAGetLastError);
       return 1;
    }
    freeaddrinfo(bind_address); 

    int listening = listen(listen_socket, 10);
    if (listening < 0) {
       fprintf(stderr, "Error in listening: %d\n", WSAGetLastError);
       return 1;
    }

    fd_set master; 
    FD_ZERO(&master);
    FD_SET(listen_socket, &master);
    SOCKET max_socket = listen_socket;
    
    while(1) { 
        
       fd_set reads;
       reads = master;
       int selecting = select(max_socket+1, &reads, 0, 0, 0);
       if (selecting < 0) {  
          fprintf(stderr, "Error in selecting: %d\n", WSAGetLastError);
          return 1;
       }

       SOCKET x; 
       char read[1024];
       int bytes_receive;
       for (x = 1; x <= max_socket; x++) {
          if (FD_ISSET(x, &reads)) {
             if (x == listen_socket) {
                 struct sockaddr client_address;
                 socklen_t client_length = sizeof(client_address);
                 SOCKET client_socket = accept(listen_socket, (struct sockaddr *)&client_address, &client_length);
                 if (client_socket == INVALID_SOCKET) {
                    fprintf(stderr, "Error: %d in accepting\n", WSAGetLastError);
                    return 1;
                 }

                 FD_SET(client_socket, &master);
                 if (client_socket > max_socket) {
                    max_socket = client_socket;
                 }
  
                 char address_buf[100];
                 char service_buf[100];
                 getnameinfo((struct sockaddr *) &client_address, client_length, address_buf, sizeof(address_buf), service_buf, sizeof(service_buf), NI_NUMERICHOST);
                 printf("There is a new connection from IP: %s.\n", address_buf);
             }
              
             else {
                 int bytes_receive = recv(x, read, sizeof(read), 0);
                 if (bytes_receive < 1) {
                    FD_CLR(x, &master);
                    closesocket(x);
                    continue;
                 }
                 int a;
                 for (a = 0; a < bytes_receive; a++)
                    read[a] = toupper(read[a]);
                 send(x, read, bytes_receive, 0);   
             }
          }   
       }
    }     

    printf("Close listen_socket socket\n");\
    printf("Programme execution successful.\n");    
    closesocket(listen_socket);
    WSACleanup();
    return 0;

}
   
//To exit the programme, enter "CTRL+C"
//Compile this programme with the command: "gcc TCP_server.c -o TCP_server -lws2_32" Make sure you remove the quotation marks, you can change the name of this programme to your choosing.
