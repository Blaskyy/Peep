#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT    1235
#define LENGTH  1518
struct sockaddr_in control_add;
struct sockaddr_in remote_add;
int sockfd, length = sizeof(struct sockaddr_in);
char datagram[LENGTH];

void printError(int sort, char *err) {
    switch (sort) {
        case 0:
            printf("\e[33mERR_bind:");break;
        case 1:
            printf("\e[33mERR_recvfrom:");break;
        case 2:
            printf("\e[33mERR_sendto:");break;
        default:
            printf("\e[33mERR:");break;
    }
    printf(" %s\n\e[0m", err);
    exit(-1);
}

void recvOnline() {
    int x, z;

    puts("Waiting for connection...");
    while(1) {
        bzero(datagram, LENGTH);
        z = recvfrom(sockfd, datagram, LENGTH, 0, (struct sockaddr *)&remote_add, &length);
        if (z == -1)
            printError(1, strerror(errno));
        else if (strcmp(datagram, "ok") == 0) {
            puts("Target is on-line.");
            x = sendto(sockfd, "ok", 2, 0, (struct sockaddr *)&remote_add, sizeof(remote_add));
            if (x == -1)
                printError(2, strerror(errno));
            break;
        }
    }
}

int main(int argc, char const *argv[]){
    int x, z;
    char command[16], tranfile[256], savefile[256];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&control_add, sizeof(control_add));
    control_add.sin_family = AF_INET;
    control_add.sin_port = htons(PORT);
    control_add.sin_addr.s_addr = htonl(INADDR_ANY);

    z = bind(sockfd, (struct sockaddr *)&control_add, sizeof(control_add));
    if (z == -1)
        printError(0, strerror(errno));

    // Waiting for the target to send on-line information
    recvOnline(sockfd);
    while(1) {
        printf("\n\e[1;36m=> \e[0m");
        bzero(datagram, LENGTH);
        fgets(datagram, LENGTH, stdin);

        if (strcmp(datagram, "\n") == 0) {
            continue;
        }
        else if (strcmp(datagram, "quit\n") == 0) {
            x = sendto(sockfd, datagram, strlen(datagram), 0, (struct sockaddr *)&remote_add, sizeof(remote_add));
            if (x == -1)
                printError(2, strerror(errno));
            break;
        }

        // Here the command is sent to remote machine through UDP
        x = sendto(sockfd, datagram, strlen(datagram), 0, (struct sockaddr *)&remote_add, sizeof(remote_add));
        if(x != -1){
            puts("Command sent. Waiting for response...");
            puts("\e[0;30m------------------------------------------------------------\e[0m");
        }else
            printError(2, strerror(errno));

        // Here the output of the command is collected from the remote machine
        sscanf(datagram, "%[^ ]", command);
        printf("%s",command);
        if (strcmp(command, "tran") == 0) {
            sscanf(datagram, "%*s %s %s", tranfile, savefile);
            printf("Transfer file: %s, save to %s\n", tranfile, savefile);
            FILE *fp;
            fp = fopen(savefile, "w");
            while(1) {
                bzero(datagram, LENGTH);
                z = recvfrom(sockfd, datagram, LENGTH, 0, (struct sockaddr *)&remote_add, &length);
                if (z == -1) {
                    printf("ERR_recvfrom: %s\n", strerror(errno));
                    exit(-1);
                }
                if(strncmp(datagram, "___EOF___\n", 10) == 0){
                    fclose(fp);
                    break;
                }
                fwrite(datagram, z, 1, fp);
            }
            continue;
        } //end transfer file
        bzero(datagram, LENGTH);

        while(1) {
            bzero(datagram, sizeof(datagram));
            z = recvfrom(sockfd, datagram, LENGTH, 0, (struct sockaddr *)&remote_add, &length);
            if (z == -1)
                printError(1, strerror(errno));
            datagram[z] = 0;
            if (strncmp(datagram, "___EOF___\n", 10) == 0)
                break;
            printf("%s", datagram);
        }
    }
    close(sockfd);
    return;
}