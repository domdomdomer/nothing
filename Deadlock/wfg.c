#include <stdio.h>

#include <stdbool.h>

#define MAX_NODES 100
// Function to detect deadlock using the WFG based algorithm
bool detectDeadlock(int graph[][MAX_NODES], int num_processes, int num_resources) {
  bool visited[num_resources];
  bool in_stack[num_resources];
  int i, j;
  // Initialize visited and in_stack arrays
  for (i = 0; i < num_resources; ++i) {
    visited[i] = false;
    in_stack[i] = false;
  }
  // Perform DFS traversal to detect cycles
  for (i = 0; i < num_resources; ++i) {
    if (!visited[i]) {
      int stack[MAX_NODES];
      int top = -1;
      visited[i] = true;
      in_stack[i] = true;
      stack[++top] = i;
      while (top != -1) {
        int node = stack[top];
        bool found = false;
        for (j = 0; j < num_processes; ++j) {
          if (graph[j][node]) {
            if (!visited[j]) {
              visited[j] = true;
              in_stack[j] = true;
              stack[++top] = j;
              found = true;
              break;
            } else if (in_stack[j]) {
              return true; // Cycle detected
            }
          }
        }
        if (!found) {
          in_stack[node] = false;
          --top;
        }
      }
    }
  }
  return false;
}
int main() {
  int num_resources = 0;
  printf("Enter the number of resources: ");
  scanf("%d", & num_resources);
  int wait_for_graph[MAX_NODES][MAX_NODES] = {
    0
  };
  printf("Enter the processes that are allocated resources (Enter -1 to stop):\n");
  int process, resource;
  while (true) {
    printf("Process: ");
    scanf("%d", & process);
    if (process == -1) {
      break;
    }
    printf("Resource: ");
    scanf("%d", & resource);
    wait_for_graph[process][resource] = 1;
  }
  printf("Enter the processes that are waiting for resources (Enter -1 to stop):\n");
  while (true) {
    printf("Process: ");
    scanf("%d", & process);
    if (process == -1) {
      break;
    }
    printf("Resource: ");
    scanf("%d", & resource);
    wait_for_graph[process][resource] = 1;
  }
  int num_processes = MAX_NODES; // Assuming the maximum number of processes
  if (detectDeadlock(wait_for_graph, num_processes, num_resources)) {
    printf("Deadlock detected!\n");
  } else {
    printf("No deadlock detected.\n");
  }
  return 0;
}