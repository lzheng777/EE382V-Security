#include <stdlib.h>
#include <stdio.h>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string.h>

char ip_addr[] = "127.0.0.2";
int port = 12000;

int main(){
    int clientSock = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addrPort = {
        .sin_family = AF_INET,
        .sin_port = htons(port)
    };

    if (!inet_pton(AF_INET, ip_addr, &addrPort.sin_addr)){
        printf("Not a valid ip address\n");
        return 0;
    }

    if (connect(clientSock, (struct sockaddr *) &addrPort, sizeof(addrPort)) < 0){
        printf("Socket failed to connect\n");
    }

    char buf[] = "Input lowercase sentence:";

    if (send(clientSock, buf, strlen(buf), 0) != strlen(buf)){
        printf("Did not send entire string: %s\n", buf);
    }

    ssize_t count = recv(clientSock, buf, 1024, 0);
    buf[count] = '\0';
    printf("From Server: %s\n", buf);

    close(clientSock);

    return 0;
}
