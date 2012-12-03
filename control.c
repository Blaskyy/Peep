/*
    Filename: control.c
    Description: A remote control program using UDP and sockets in linux through C language.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv){
    int x, z;
    char *host_ip = "127.0.0.1";
    short host_port = 1235;
    char *remote_ip = "127.0.0.1";
    short remote_port = 1234;

    int host_sock, remote_sock;
    char datagram[512];
    int remote_len;

    struct sockaddr_in host_add;
    struct sockaddr_in remote_add;

    host_sock = socket(AF_INET, SOCK_DGRAM, 0);

    host_add.sin_family = AF_INET;
    host_add.sin_port = htons(host_port);
    host_add.sin_addr.s_addr = inet_addr(host_ip);

    z = bind(host_sock, (struct sockaddr *)&host_add, sizeof(host_add));

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

        remote_sock = socket(AF_INET, SOCK_DGRAM, 0);
        remote_add.sin_family = AF_INET;
        remote_add.sin_port = htons(remote_port);
        remote_add.sin_addr.s_addr = inet_addr(remote_ip);

        // Here the command is sent to remote machine through UDP
        x = sendto(remote_sock, datagram, strlen(datagram), 0, (struct sockaddr *)&remote_add, sizeof(remote_add));
        if(x != -1){
            puts("Command sent. Waiting for response...");
        }
        close(remote_sock);
        bzero(datagram, 512);

        // Here the output of the command is collected from the remote machine
        do {
            bzero(datagram, sizeof(datagram));
            z = recvfrom(host_sock, datagram, 512, 0, (struct sockaddr *)&remote_add, &remote_len);
            datagram[z] = 0;
            printf("%s", datagram);
        }while(strncmp(datagram, "___EOF___\n", 10) != 0);

    }
    close(host_sock);
    return 0;
}