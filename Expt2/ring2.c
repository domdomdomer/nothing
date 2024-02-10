#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>

#define MSG_CONFIRM 0
#define TRUE 1
#define FALSE 0

typedef struct list
{
    int size;
    int arr[20];
} list;

int connect_to_port(int connect_to) {
    int sock_id = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_id < 0)
    {
        perror("Unable to create a socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(connect_to);

    // Bind the socket to a specific address and port
    if (bind(sock_id, (const struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    return sock_id;
}

void send_to_id(int id, int sock_id, list llist) {
    struct sockaddr_in client_address;
    memset(&client_address, 0, sizeof(client_address));

    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = INADDR_ANY;
    client_address.sin_port = htons(id);

    sendto(sock_id, &llist, sizeof(list), MSG_CONFIRM, (const struct sockaddr *)&client_address, sizeof(client_address));
}

void election(int id, int self, int next) {
    list llist;
    llist.arr[0] = self;
    llist.size = 1;
    printf("Forwading to: %d\n", next);
    send_to_id(next, id, llist);
}

void print(list l) {
    for (int i = 0; i < l.size; i++) {
        printf("%d ", l.arr[i]);
    }
}

int main(int argc, char *argv[]) {
    int self = atoi(argv[1]), next = atoi(argv[3]);

    bool start_at = atoi(argv[2]) == 1 ? true : false;

    printf("Creating a node at %d %d\n", self, start_at);
    int sock_id = connect_to_port(self);

    if (start_at)
    {
        election(sock_id, self, next);
    }

    struct sockaddr_in from;
    int len;
    list l;

    while (true) {
        memset(&from, 0, sizeof(from));
        int n = recvfrom(sock_id, &l, sizeof(list), MSG_WAITALL, (struct sockaddr *)&from, &len);

        if (l.arr[0] == -1) {
            printf("New Co - Ordinator: %d\n", l.arr[1]);
            if (l.arr[2] == next) {
                break;
            }
            send_to_id(next, sock_id, l);
            break;
        }
        else if (l.arr[0] == self) {
            int max = l.arr[0];
            for (int i = 1; i < l.size; i++) {
                if (l.arr[i] > max)
                    max = l.arr[i];
            }

            l.arr[0] = -1;
            l.arr[1] = max;
            l.arr[2] = self;

            printf("New Co-ordinator: %d\n", l.arr[1]);

            send_to_id(next, sock_id, l);
            break;
        }

        else
        {
            printf("Received list: ");
            print(l);
            printf("\n\n");

            l.arr[l.size] = self;
            l.size++;

            printf("Sending: ");
            print(l);
            printf(" To %d\n\n", next);

            send_to_id(next, sock_id, l);
        }
    }
}
