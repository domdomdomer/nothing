#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

struct sockaddr_in abc, sender, tosend;
int cordinator, max, mynode;
int soc;

struct msg_node
{
    int ar[6];
    char msg[100];
    int node_no;
};

void meelc(void* arg)
{
    struct msg_node *tem = (struct msg_node *)arg;
    char elect_inti[100] = "elect_inti";
    char me_elected[100] = "me_elected";
    socklen_t len = sizeof(sender);
    tem->ar[mynode] = 1;
    tem->node_no = mynode;
    strcpy(tem->msg, elect_inti);
    for (int i = mynode + 1; i <= max; i++)
    {
        tosend.sin_port = htons(i + 4500);
        printf("sending to %d\n", i);
        strcpy(tem->msg, elect_inti);
        tem->node_no = mynode;
        sendto(soc, tem, sizeof(struct msg_node), 0, (struct sockaddr *)&tosend, len);
        sleep(8);
    }

    for (int i = 0; i < mynode; i++)
    {
        tosend.sin_port = htons(i + 4500);
        strcpy(tem->msg, elect_inti);
        tem->node_no = mynode;
        sendto(soc, tem, sizeof(struct msg_node), 0, (struct sockaddr *)&tosend, len);
        printf("sending to %d\n", i);
        sleep(8);
    }
}

void *cord()
{
    printf("cordni = %d", cordinator);
    socklen_t len = sizeof(sender);
    struct msg_node *tem = malloc(sizeof(struct msg_node));
    strcpy(tem->msg, "me_elected");
    tem->node_no = cordinator;
    for (int i = 0; i <= max; i++)
    {
        tosend.sin_port = htons(i + 4500);
        sendto(soc, tem, sizeof(struct msg_node), 0, (struct sockaddr *)&tosend, len);
    }
}

void *fun()
{
    // printf("sdcv");
    struct msg_node *tem = malloc(sizeof(struct msg_node));
    socklen_t len = sizeof(sender);
    char elect_inti[100] = "elect_inti";
    char me_elected[100] = "me_elected";
    int inpt;
    while (1)
    {
        printf("enter 1 to check cordinator: ");
        scanf("%d", &inpt);
        if (inpt == 1)
        {
            tosend.sin_port = htons(cordinator + 4500);
            strcpy(tem->msg, "alive");
            tem->node_no = mynode;
            sendto(soc, tem, sizeof(struct msg_node), 0, (struct sockaddr *)&tosend, len);
            // recvfrom(soc,tem,sizeof(struct msg_node),0,(struct sockaddr *)&sender,&len);
            sleep(8);
            if (strcmp(tem->msg, "salive") != 0)
            {
                printf("\ndead cordinator\n");
                for (int i = 0; i <= max; i++)
                {
                    tem->ar[mynode] = 0;
                }
                tem->ar[mynode] = 1;
                tem->node_no = mynode;
                strcpy(tem->msg, elect_inti);
                for (int i = mynode + 1; i <= max; i++)
                {
                    tosend.sin_port = htons(i + 4500);
                    printf("sending to %d\n", i);
                    strcpy(tem->msg, elect_inti);
                    tem->node_no = mynode;
                    sendto(soc, tem, sizeof(struct msg_node), 0, (struct sockaddr *)&tosend, len);
                    sleep(8);
                }

                for (int i = 0; i <= mynode; i++)
                {
                    tosend.sin_port = htons(i + 4500);
                    printf("sending to %d\n", i);
                    strcpy(tem->msg, elect_inti);
                    tem->node_no = mynode;
                    sendto(soc, tem, sizeof(struct msg_node), 0, (struct sockaddr *)&tosend, len);
                    sleep(8);
                }
            }
        }
    }
    free(tem);
}

int main(int argc, char *argv[])
{
    struct timeval timeout;
    pthread_t thread1, t2 = 0, t3;
    int a;
    cordinator = atoi(argv[2]);
    max = cordinator;
    mynode = atoi(argv[1]);
    soc = socket(AF_INET, SOCK_DGRAM, 0);
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    abc.sin_addr.s_addr = INADDR_ANY;
    abc.sin_family = AF_INET;
    abc.sin_port = htons(atoi(argv[1]) + 4500);
    bind(soc, (struct sockaddr *)&abc, sizeof(abc));
    setsockopt(soc, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
    tosend.sin_addr.s_addr = INADDR_ANY;
    tosend.sin_family = AF_INET;
    tosend.sin_port = htons(cordinator + 4500);
    struct msg_node *tem = malloc(sizeof(struct msg_node));
    char elect_inti[100] = "elect_inti";
    char me_elected[100] = "me_elected";
    socklen_t len = sizeof(sender);
    pthread_create(&thread1, NULL, fun, NULL);
    while (1)
    {
        ssize_t bytesRead = recvfrom(soc, tem, sizeof(struct msg_node), 0, (struct sockaddr *)&sender, &len);
        if (bytesRead == -1)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
                // Handle timeout (no data received within the specified time)
                printf("\nTimeout: No data received within the specified time.mynode = %d cord = %d\n", mynode, cordinator);
            }
            continue;
        }
        printf("%s\n", tem->msg);
        if (strcmp(elect_inti, tem->msg) == 0)
        {
            // if(tem->node_no==mynode){
            strcpy(tem->msg, "OK");
            tosend.sin_port = htons(tem->node_no + 4500);
            printf(" sending okay to %d node\n ", tem->node_no);
            tem->node_no = mynode;
            sendto(soc, tem, sizeof(struct msg_node), 0, (struct sockaddr *)&tosend, len);
            strcpy(tem->msg, "O");
            for (int i = 0; i <= max; i++)
            {
                if (tem->ar[i] == 1)
                {
                    a = i;
                    break;
                }
            }

            if (a == mynode)
            {
                for (int i = 0; i <= max; i++)
                {
                    if (tem->ar[i] == 1)
                    {
                        a = i;
                    }
                }
                cordinator = a;
                printf("cordinator %d\n", a);
                pthread_create(&t3, NULL, cord, NULL);
                continue;
            }
            tem->node_no = mynode;
            pthread_create(&t2, NULL, meelc, (void *)tem);
            //}
        }

        if (strcmp(me_elected, tem->msg) == 0)
        {
            if (t2 != 0)
            {
                pthread_cancel(t2);
                t2 = 0;
            }
            cordinator = tem->node_no;
            pthread_cancel(thread1);
            pthread_create(&thread1, NULL, fun, NULL);
        }

        if (strcmp("alive", tem->msg) == 0)
        {
            printf("killing\n");
            tosend.sin_port = htons(tem->node_no + 4500);
            strcpy(tem->msg, "salive");
            tem->node_no = mynode;
            sendto(soc, tem, sizeof(struct msg_node), 0, (struct sockaddr *)&tosend, len);
        }

        if (strcmp("salive", tem->msg) == 0)
        {
            printf("killing as alive \n");
            pthread_cancel(thread1);
            pthread_create(&thread1, NULL, fun, NULL);
        }

        if (strcmp(tem->msg, "OK") == 0)
        {

            printf("killing as ok recieved from %d\n", tem->node_no);
            pthread_cancel(thread1);
            pthread_create(&thread1, NULL, fun, NULL);
            if (t2 != 0)
            {
                pthread_cancel(t2);
                t2 = 0;
            }
        }
    }

    // strcpy(msg,"abcd");

    // sendto(soc,tem,sizeof(struct msg_node),0,(struct sockaddr *)&sender,len);
}

// Hey sollu if any errors ,mostly fine than edachina solllu daaaaaaaaaaaaaa
