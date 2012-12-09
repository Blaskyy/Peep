#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {
    int x, z;
    char *host_ip = "127.0.0.1";
    short host_port = 1234;
    char *remote_ip = "127.0.0.1";
    short remote_port = 1235;

    int host_sock, remote_sock;
    char datagram[512], command[16], path[128];
    int remote_len = 512;

    struct sockaddr_in host_add;
    struct sockaddr_in remote_add;
    FILE *fp;  // it will open a stream for the output of command

    // Initialize the host machine
    // popen("chmod 755 *", "r");
    // popen("./init.sh", "r");

    host_sock = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&host_add, sizeof(host_add));
    host_add.sin_family = AF_INET;
    host_add.sin_port = htons(host_port);
    host_add.sin_addr.s_addr = inet_addr(host_ip);

    z = bind(host_sock, (struct sockaddr *)&host_add, sizeof(host_add));
    if (z == -1)
        puts("bind error");

    while(1) {
        puts("Waiting for command...");
        bzero(datagram, 512);

        // Here we are waiting for receiving command from remote machine
        z = recvfrom(host_sock, datagram, 512, 0, (struct sockaddr *)&remote_add, &remote_len);
        if (z == -1)
            puts("recvfrom error");

        datagram[z] = 0;
        printf("Command:%s", datagram);

        sscanf(datagram, "%[^ ]", command);
        // use function chdir() to replacement command 'cd'
        if (strcmp(command, "cd") == 0) {
            bzero(command, sizeof(command));
            sscanf(datagram, "%*3s%s", path);
            if (chdir(path) == -1) {
                printf("%s\n", strerror(errno));
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

        remote_sock = socket(AF_INET, SOCK_DGRAM, 0);
        bzero(&remote_add, sizeof(remote_add));
        remote_add.sin_family = AF_INET;
        remote_add.sin_port = htons(remote_port);
        remote_add.sin_addr.s_addr = inet_addr(remote_ip);

        // collecting the output and sending to the remote
        while(fgets(datagram, sizeof(datagram)-1, fp) != NULL) {
            x = sendto(remote_sock, datagram, strlen(datagram), 0, (struct sockaddr *)&remote_add, sizeof(remote_add));
            if (x == -1)
                puts("sendto error");

            bzero(datagram, sizeof(datagram));
        }
        // marking the end of output of command
        strcpy(datagram, "___EOF___\n");
        x = sendto(remote_sock, datagram, strlen(datagram), 0, (struct sockaddr *)&remote_add, sizeof(remote_add));
        if (x == -1)
            puts("sendto 'EOF' error");

        close(remote_sock);
    }
    close(host_sock);
    return 0;
}