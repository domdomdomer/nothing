#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define MAX_RESOURCES 10
#define MAX_PROCESSES 10

pthread_mutex_t resource_lock[MAX_RESOURCES];
pthread_mutex_t probe_lock;
bool deadlock_detected = false;

int resource_state[MAX_RESOURCES] = {0}; // 0 = Free, 1 = Held
int process_waiting[MAX_PROCESSES] = {0}; // 0 = Not waiting, 1 = Waiting
int num_processes = 0;
int num_resources = 0;

void acquire_resource(int process_id, int resource_id) {
    pthread_mutex_lock(&resource_lock[resource_id]);
    while (resource_state[resource_id] != 0) {
        pthread_mutex_unlock(&resource_lock[resource_id]);
        printf("Process %d is waiting for Resource %d\n", process_id, resource_id);
        sleep(1);
        pthread_mutex_lock(&resource_lock[resource_id]);
    }
    resource_state[resource_id] = 1;
    printf("Process %d acquired Resource %d\n", process_id, resource_id);
    pthread_mutex_unlock(&resource_lock[resource_id]);
}

void release_resource(int process_id, int resource_id) {
    pthread_mutex_lock(&resource_lock[resource_id]);
    resource_state[resource_id] = 0;
    printf("Process %d released Resource %d\n", process_id, resource_id);
    pthread_mutex_unlock(&resource_lock[resource_id]);
}

void *probe_function(void *arg) {
    while (!deadlock_detected) {
        sleep(3); // Probe every 3 seconds

        printf("Probe message sent...\n");

        // Check for potential deadlocks
        pthread_mutex_lock(&probe_lock); // Acquire probe lock to check state
        int deadlock_detected_local = 0;
        for (int i = 0; i < num_processes; i++) {
            if (process_waiting[i]) {
                printf("Potential deadlock detected! Process %d is waiting.\n", i);
                deadlock_detected_local = 1;
                break;
            }
        }
        pthread_mutex_unlock(&probe_lock); // Release probe lock

        if (deadlock_detected_local) {
            printf("Deadlock detected!\n");
            deadlock_detected = true;
            exit(0);
            break;
        }
    }
    return NULL;
}

void *process_function(void *arg) {
    int process_id = *((int *) arg);
    while (!deadlock_detected) {
        int resource_id = rand() % num_resources;
        pthread_mutex_lock(&probe_lock); // Acquire probe lock to update process_waiting
        process_waiting[process_id] = 1; // Mark process as waiting
        pthread_mutex_unlock(&probe_lock); // Release probe lock
        acquire_resource(process_id, resource_id);
        pthread_mutex_lock(&probe_lock); // Acquire probe lock to update process_waiting
        process_waiting[process_id] = 0; // Reset process waiting status
        pthread_mutex_unlock(&probe_lock); // Release probe lock
        sleep(rand() % 3);
        release_resource(process_id, resource_id);
        sleep(rand() % 3);
    }
    return NULL;
}

int main() {
    printf("Enter the number of processes: ");
    scanf("%d", &num_processes);
    printf("Enter the number of resources: ");
    scanf("%d", &num_resources);

    printf("Enter the deadlock scenario (process ID and resource ID, -1 to end):\n");
    int process_id, resource_id;
    while (1) {
        scanf("%d", &process_id);
        if (process_id == -1) break;
        scanf("%d", &resource_id);
        pthread_mutex_lock(&resource_lock[resource_id]);
        resource_state[resource_id] = 1;
        pthread_mutex_unlock(&resource_lock[resource_id]);
    }

    pthread_t processes[MAX_PROCESSES];
    pthread_mutex_init(&probe_lock, NULL);
    for (int i = 0; i < num_resources; i++) {
        pthread_mutex_init(&resource_lock[i], NULL);
    }
    int process_ids[MAX_PROCESSES];
    for (int i = 0; i < num_processes; i++) {
        process_ids[i] = i;
        pthread_create(&processes[i], NULL, process_function, &process_ids[i]);
    }

    pthread_t probe_thread;
    pthread_create(&probe_thread, NULL, probe_function, NULL);

    for (int i = 0; i < num_processes; i++) {
        pthread_join(processes[i], NULL);
    }
    pthread_join(probe_thread, NULL);

    pthread_mutex_destroy(&probe_lock);
    for (int i = 0; i < num_resources; i++) {
        pthread_mutex_destroy(&resource_lock[i]);
    }
    return 0;
}

