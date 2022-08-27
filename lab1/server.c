#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string.h>
#include <ctype.h>

char ip_addr[] = "127.0.0.2";
int port = 12000;
int queueLen = 1;

int main(){
    int sockId = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addrPort = {
        .sin_family = AF_INET,
        .sin_port = htons(port)
    };

    if (!inet_pton(AF_INET, ip_addr, &addrPort.sin_addr)){
        printf("Not a valid ip address\n");
        return 0;
    }
    
    bind(sockId, (struct sockaddr *) &addrPort, sizeof(addrPort));

    listen(sockId, queueLen);
    
    struct sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);

    char buf[1024];
    int clientSock;

    printf("The server is ready to receive\n");

    while (1) {
        if ((clientSock = accept(sockId, (struct sockaddr *) &clientAddr, &addrLen)) < 0){
            printf("Failed to accept client socket");
        }

        ssize_t count = recv(clientSock, buf, 1024, 0);
                
        buf[count + 1] = '\0';

        for (int i = 0; i < count + 1; i++){
            buf[i] = toupper(buf[i]);
        }

        if (send(clientSock, buf, strlen(buf), 0) != strlen(buf)){
            printf("Did not send entire string to client\n");
        }

        close(clientSock);
    }

    close(sockId);

    return 0;
}
