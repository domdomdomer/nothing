#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Process structure
typedef struct {
    int id;
    bool alive;
} Process;

// Function to simulate sending a message
void sendMessage(int sender_id, int receiver_id, char* message) {
    printf("Process %d sends message '%s' to process %d\n", sender_id, message, receiver_id);
}

// Function to simulate receiving a message
void receiveMessage(int receiver_id, int sender_id, char* message) {
    printf("Process %d receives message '%s' from process %d\n", receiver_id, message, sender_id);
}

// Simulate receiving messages dynamically based on process statuses and initiator ID
void simulateReceivingMessages(Process processes[], int num_processes, int initiator_id) {
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].alive && i + 1 != initiator_id) {
            receiveMessage(i + 1, initiator_id, "ELECTION");
        }
    }
}

// Simulate receiving "OK" messages dynamically based on process statuses and initiator ID
void simulateReceivingOKMessages(Process processes[], int num_processes, int initiator_id) {
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].alive && i + 1 != initiator_id) {
            receiveMessage(initiator_id, i + 1, "OK");
        }
    }
}

// Function to initiate an election
void initiateElection(Process processes[], int num_processes, int initiator_id) {
    if (processes[initiator_id - 1].alive) {
        for (int i = initiator_id; i < num_processes; i++) {
            if (processes[i].alive) {
                sendMessage(initiator_id, i + 1, "ELECTION");
            }
        }
    }
}

// Function to handle election messages
void handleElectionMessage(Process processes[], int num_processes, int sender_id) {
    if (processes[sender_id - 1].alive) {
        for (int i = sender_id + 1; i <= num_processes; i++) {
            if (processes[i - 1].alive) {
                sendMessage(sender_id, i, "OK");
            }
        }
    }
}

// Main function
int main() {
    int num_processes;
    printf("Enter the number of processes: ");
    scanf("%d", &num_processes);

    // Dynamically allocate memory for processes
    Process *processes = (Process *)malloc(num_processes * sizeof(Process));
    if (processes == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    // Input process statuses
    for (int i = 0; i < num_processes; i++) {
        processes[i].id = i + 1;
        printf("Is process %d alive? (1 for yes, 0 for no): ", processes[i].id);
        scanf("%d", &(processes[i].alive));
    }

    // Choose an arbitrary process to initiate the election
    int initiator_id;
    printf("Enter the ID of the process to initiate the election: ");
    scanf("%d", &initiator_id);

    // Simulate receiving messages dynamically based on process statuses and initiator ID
    simulateReceivingMessages(processes, num_processes, initiator_id);
    simulateReceivingOKMessages(processes, num_processes, initiator_id);

    initiateElection(processes, num_processes, initiator_id);

    // Handle received messages
    handleElectionMessage(processes, num_processes, initiator_id);

    // Find the new coordinator
    int coordinator_id = -1;
    for (int i = num_processes - 1; i >= 0; i--) {
        if (processes[i].alive) {
            coordinator_id = processes[i].id;
            break;
        }
    }

    printf("New coordinator is process %d\n", coordinator_id);

    // Free dynamically allocated memory
    free(processes);

    return 0;
}

