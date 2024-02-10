#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>


//some gibberish declarations
struct sockaddr_in abc, sender, tosend;
int sock;
int coordinator, max, mynode;


//ring message
struct msg_node{
    int ar[6]; //list of processed in the form of boolean array where each process switches the index corresponsing to their process number to 1
    char msg[100]; //used to make appropriate actions
    int node_no; //node number/process number of the sender node
};

//making the messages
char* elect_ini = "elect_initi"; 
char* me_elected = "me_elected";

void meelc(void* arg){
    // for getting the election done autmatically with the arg msg_node
    struct msg_node *tem = (struct msg_node*)arg;
    socklen_t len = sizeof(sender);
    tem->ar[mynode] = 1; //current process polling its index to 1 to mkae its entry in the boolean array
    tem->node_no = mynode; //sender nodes number
    strcpy(tem->msg, elect_ini); //inititating the election
    for(int i = 0; i<=max; i++){
        if (i!=mynode){ //sending to all nodes except for itself
            tosend.sin_port = htons(4500+i);
            printf("sending message to %d\n", i);
            tem->node_no = mynode;  //redundant, can remove
            sendto(sock, tem, sizeof(struct msg_node), 0, (struct sockaddr*)&sender, sizeof(sender));
            sleep(8);
        }
    }
}

void* cord(){
    // for announcing the coordinator if its itself is a coordinator
    printf("cordn = %d", coordinator);
    socklen_t len = sizeof(sender);
    struct msg_node *tem = malloc(sizeof(struct msg_node)); //getting empty msg_node as there is no arg to pass
    strcpy(tem->msg, me_elected); //putting message me_elected, therefore declaring itself to be a coordinator
    tem->node_no = coordinator; //node number of sender which is the coordinator itself
    for(int i = 0; i<=max; i++){ //sending to all the nodes(incl. itself)
        tosend.sin_port = htons(4500+i);
        sendto(sock, tem, sizeof(struct msg_node), 0, (struct sockaddr*)&sender, len);
    }
}

void *fun(){
    //for getting the election done manually
    struct msg_node *tem = malloc(sizeof(struct msg_node));
    socklen_t len = sizeof(sender);
    int input;
    while(1){
        printf("Enter 1 to check coordinator: ");
        scanf("%d", &input);
        if(input==1){
            //checking if the currernt coordinator is good or not
            tosend.sin_port = htons(4500+coordinator);
            strcpy(tem->msg, "alive");
            tem->node_no = mynode;
            sendto(sock, tem, sizeof(struct msg_node),0, (struct sockaddr*)&tosend, len);
            sleep(8);
            if(strcmp(tem->msg, "salive")!=0){
                //if the coord is dead
                printf("dead coord\n");
                for(int i = 0; i<=max; i++){
                    tem->ar[i] = 0;
                }
                //reset the array
                tem->ar[mynode] = 1;
                tem->node_no = mynode;
                strcpy(tem->msg, elect_ini);
                for(int i = 0; i<=max; i++){
                    //send the election to all the nodes
                    tosend.sin_port = htons(4500+i);
                    printf("sending to %d\n", i);
                    sendto(sock, tem, sizeof(struct msg_node), 0, (struct sockaddr*)&tosend, len);
                    sleep(8); 
                }
            }
        }
    }
    free(tem);
}


int main(int argc, char* argv[]){
    struct timeval timeout; //setting the timeout interval
    pthread_t t1, t2 = 0, t3; //creating the threads
    int a; //this will be used later for getting the new coordinator (line 155)
    coordinator = atoi(argv[2]); //getting the coordinator from the user(./ring 3 3 ---- the second 3 is the coordinator and also the max number of nodes)
    //other meaning, only node 3 is active and node1, node2 are ded
    mynode = atoi(argv[1]); //getting the current node number(./ring 2 3 ------ means the terminal is node 2 and the coordinator given by user is node 3)
    max = coordinator; // max process number automatically is the coordinator
    sock = socket(AF_INET, SOCK_DGRAM, 0); //creating a socket duh
    timeout.tv_sec = 5; //can change the value, will change the time taken between 2 timeout messages in a terminal
    timeout.tv_usec = 0;

    //current node intitalization
    abc.sin_addr.s_addr = INADDR_ANY;
    abc.sin_family = AF_INET;
    abc.sin_port = htons(mynode + 4500);

    bind(sock, (struct sockaddr*)&abc, sizeof(abc));
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    //coordinator node initialization
    tosend.sin_addr.s_addr = INADDR_ANY;
    tosend.sin_family = AF_INET;
    tosend.sin_port = htons(coordinator + 4500);

    struct msg_node* tem = (struct msg_node*)malloc(sizeof(struct msg_node));
    socklen_t len = sizeof(sender);
    pthread_create(&t1, NULL, fun, NULL); //starting a thread on fun, as the code has to detect the "1" given by the user at any point of time
    // to know the current coordinator or (in case the coordinator is ded) to intitate the election algorithm

    while(1){
        ssize_t bytesRead = recvfrom(sock, tem, sizeof(struct msg_node), 0, (struct sockaddr*)&sender, &len);
        if (bytesRead == -1)
        //timout part IMP as we get the info as current node and coordinator node number
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
                // Handle timeout (no data received within the specified time)
                printf("\nTimeout: No data received within the specified time.mynode = %d cord = %d\n", mynode, coordinator);
            }
            continue;
        }
        printf("%s\n", tem->msg);
        if (strcmp(elect_ini, tem->msg) == 0)
        // in case the node recieves the election initiation message from some other node
        {
            // if(tem->node_no==mynode){
            //if election message is recieved
            strcpy(tem->msg, "OK");
            tosend.sin_port = htons(tem->node_no + 4500); //sending it back to the sender
            printf(" sending okay to %d node\n ", tem->node_no);
            tem->node_no = mynode;
            sendto(sock, tem, sizeof(struct msg_node), 0, (struct sockaddr *)&tosend, len);
            strcpy(tem->msg, "O");

            if (tem->ar[mynode]==1) // this condition is important because if the current node is already marked as 1 in the boolean array, then it means the message has completed a round
            {
                for (int i = 0; i <= max; i++)
                {
                    if (tem->ar[i] == 1)
                    {
                        a = i; //hence get the highest process number (see, here the "a" variable is used)
                    }
                }
                coordinator = a; //  and assign it as the corrdinator
                printf("cordinator %d\n", a);
                pthread_create(&t3, NULL, cord, NULL); //announcing the coordinator
                continue;
            }
            tem->node_no = mynode;
            pthread_create(&t2, NULL, meelc, (void *)tem); //restarting the election process
            //}
        }

        if(strcmp(me_elected, tem->msg) == 0){
            if(t2!=0){
                pthread_cancel(t2); //cancelling the ongoing election process if any
                t2 = 0;
            }
            coordinator = tem->node_no; //assigning the current node as coordinator
            pthread_cancel(t1); //restarting the manual election to come out of while loop
            pthread_create(&t1, NULL, fun, NULL);
        }

        if(strcmp("alive", tem->msg) == 0){
            // see from coordinator node prespective, if its alive then it will revert with a postive message thats its alive
            tosend.sin_port = htons(4500+tem->node_no); //sending it to node which asked whether the coordinator is alive
            strcpy(tem->msg, "salive");
            tem->node_no = mynode;
            sendto(sock, tem, sizeof(struct msg_node), 0, (struct sockaddr*)&tosend, len);
        }

        if(strcmp(tem->msg, "salive")){

            //in case the sender node receives is alive message from the coordinator, meaning the coordinator is alive
            printf("restarting the thread\n"); 
            /*restarting the thread responsible for the while loop as the coord is alive and the while loop 
            contains the election algo, causing mynode to send the election to next node and causing 
            indefinite loop, so we restart the thread to come out of it
            */
           pthread_cancel(t1);
           pthread_create(&t1,NULL, fun, NULL);
        }

        if(strcmp(tem->msg, "OK") == 0){
            printf("killing as ok recieved from %d\n", tem->node_no);
            //stopping the election algo under the while loop
            pthread_cancel(t1);
            pthread_create(&t1, NULL, fun, NULL);
            if(t2!=0){
                //cancelling ongoing automated election too
                pthread_cancel(t2);
                t2 = 0;
            }
        }
    }

}