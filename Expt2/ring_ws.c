#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int node_id;
    int is_alive;
    struct Node* next;
} Node;

typedef struct CircularLinkedList {
    Node* head;
    Node* tail;
} CircularLinkedList;

CircularLinkedList* createCircularLinkedList() {
    CircularLinkedList* list = (CircularLinkedList*)malloc(sizeof(CircularLinkedList));
    list->head = NULL;
    list->tail = NULL;
    return list;
}

void addNode(CircularLinkedList* list, int node_id) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->node_id = node_id;
    new_node->is_alive = 1;  // Set node as alive by default
    new_node->next = NULL;
    
    if (list->head == NULL) {
        list->head = new_node;
    } else {
        list->tail->next = new_node;
    }
    list->tail = new_node;
    list->tail->next = list->head;  // circular linking
}

Node* findLeader(CircularLinkedList* list) {
    Node* current_node = list->head;
    Node* leader = current_node;
    do {
        if (current_node->is_alive && current_node->node_id > leader->node_id) {
            leader = current_node;
        }
        current_node = current_node->next;
    } while (current_node != list->head);
    return leader;
}

void handleDeadNode(CircularLinkedList* list, Node* dead_node) {
    Node* current_node = list->head;
    do {
        if (current_node->next == dead_node) {
            current_node->next = dead_node->next;  // skip over dead node
            if (dead_node == list->head) {
                list->head = dead_node->next;
            }
            if (dead_node == list->tail) {
                list->tail = current_node;
            }
            free(dead_node);  // free memory of dead node
            break;
        }
        current_node = current_node->next;
    } while (current_node != list->head);
}

void simulateDeadNode(CircularLinkedList* list, int node_id) {
    Node* current_node = list->head;
    do {
        if (current_node->node_id == node_id) {
            current_node->is_alive = 0;  // mark node as dead
            handleDeadNode(list, current_node);
            printf("Node %d is dead.\n", node_id);
            break;
        }
        current_node = current_node->next;
    } while (current_node != list->head);
}

int main() {
    CircularLinkedList* ring = createCircularLinkedList();
    
    // Adding nodes to the ring
    int n=0;
    printf("Enter number of nodes in ring: ");
    scanf("%d",&n);
    for (int i = 1; i <= n; i++) {
        addNode(ring, i);
    }
    
    // Perform election process
    Node* leader = findLeader(ring);
    printf("Node %d is the leader.\n", leader->node_id);
    
    // Simulate a dead node
    int nn=0;
    printf("Enter number of processes to kill: ");
    scanf("%d",&nn);
    int tokill=0;
    for(int i=0;i<nn;i++)
    {
        printf("Enter node number to kill: ");
        scanf("%d",&tokill);
        simulateDeadNode(ring, tokill);
    }
    //simulateDeadNode(ring, 3);
    //simulateDeadNode(ring, 5);
    
    // Perform election process again after node 3 is dead
    leader = findLeader(ring);
    printf("\nNode %d is the new leader.\n", leader->node_id);
    
    return 0;
}