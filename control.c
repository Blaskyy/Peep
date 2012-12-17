#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 1235
#define LENGTH  512

int main(int argc, char **argv){
    int x, z, sockfd;
    char *remote_ip = "127.0.0.1";
    char datagram[LENGTH];
    int length = LENGTH;

    struct sockaddr_in control_add;
    struct sockaddr_in remote_add;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&remote_add, sizeof(remote_add));
    remote_add.sin_family = AF_INET;
    remote_add.sin_port = htons(PORT);
    remote_add.sin_addr.s_addr = inet_addr(remote_ip);

    while(1) {
        printf("Command: ");
        bzero(datagram, 512);
        fgets(datagram, 512, stdin);

        if (strcmp(datagram, "\n") == 0) {
            continue;
        }
        else if (strcmp(datagram, "quit\n") == 0) {
            break;
        }

        // Here the command is sent to remote machine through UDP
        x = sendto(sockfd, datagram, strlen(datagram), 0, (struct sockaddr *)&remote_add, sizeof(remote_add));
        if(x != -1){
            puts("Command sent. Waiting for response...");
        }else
            printf("ERR_sendto: %s\n", strerror(errno));
        bzero(datagram, 512);

        // Here the output of the command is collected from the remote machine
        do {
            bzero(datagram, sizeof(datagram));
            z = recvfrom(sockfd, datagram, 512, 0, (struct sockaddr *)&control_add, &length);
            if (z == -1)
                printf("ERR_recvfrom: %s\n", strerror(errno));

            datagram[z] = 0;
            printf("%s", datagram);
        }while(strncmp(datagram, "___EOF___\n", 10) != 0);
    }
    close(sockfd);
    return 0;
}