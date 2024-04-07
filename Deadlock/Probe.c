#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_WAITING 5 // Maximum number of processes a process can wait for

struct Process {
  int id;
  int holding; // Process ID of the process holding this process
  int waiting[MAX_WAITING]; // Array to store Process IDs of processes this process is waiting for
  int num_waiting; // Number of processes this process is waiting for
};

void probe(struct Process pList[], struct Process cur, int start, int size) {
  bool foundDeadlock = false;
  // Check all waiting processes
  for (int j = 0; j < cur.num_waiting; j++) {
    for (int i = 0; i < size; i++) {
      if (cur.waiting[j] == pList[i].id) {
        printf("Process-%d sends message (%d,%d,%d)\n", cur.id, start, cur.id, pList[i].id);
        if (pList[i].id == start) {
          printf("Deadlock detected\n");
          foundDeadlock = true;
          return;
        } else {
          probe(pList, pList[i], start, size);
        }
      }
    }
  }
}

int main() {
  int process, initiator;
  printf("Enter number of processes: ");
  scanf("%d", &process);

  struct Process *pList = malloc(process * sizeof(struct Process)); // Corrected memory allocation

  for (int i = 0; i < process; i++) {
    printf("Which processes is process-%d holding? (Enter -1 if none): ", i + 1);
    scanf("%d", &pList[i].holding);
    printf("How many processes is process-%d waiting for? : ", i + 1);
    scanf("%d", &pList[i].num_waiting);
    printf("Enter the IDs of the processes process-%d is waiting for: ", i + 1);
    for (int j = 0; j < pList[i].num_waiting; j++) {
      scanf("%d", &pList[i].waiting[j]);
    }
    pList[i].id = i + 1;
  }

  printf("Process id that initiates probe : ");
  scanf("%d", &initiator);
  struct Process cur;
  for (int i = 0; i < process; i++) {
    if (pList[i].id == initiator) {
      cur = pList[i];
      break;
    }
  }
  probe(pList, cur, cur.id, process);
  free(pList); // Corrected memory deallocation
  return 0;
}
