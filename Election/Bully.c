#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#define MAX 10

int list[MAX], n, c;
void display() {
  int i;
  printf("\nProcesses-->");
  for (i = 1; i <= n; i++)
    printf("\t %d", i);
  printf("\nAlive-->");
  for (i = 1; i <= n; i++)
    printf("\t %d", list[i]);
  printf("\ncoordinator is::%d", c);
}
void bully() {
  int ch, crash, activate, i, gid, flag, subcdr;
  do {
    printf("\n1.Crash\n2.Activate\n3.Display\n4.Exit\nEnter You choice::");
    scanf("%d", & ch);
    switch (ch) {
    case 1:
      printf("\nEnter Process no. to Crash::");
      scanf("%d", & crash);
      if (list[crash])
        list[crash] = 0;
      else {
        printf("\nProcess is alreaady dead!!");
        break;
      }
      do {
        printf("\nEnter election generator id::");
        scanf("%d", & gid);
        if (gid == c) {
          printf("\nenter a valid generator id::");
        }
      } while (gid == crash);
      flag = 0;
      if (crash == c) {
        for (i = gid + 1; i <= n; i++) {
          printf("\nmessage is sent from %d to %d", gid, i);
          if (list[i]) {
            subcdr = i;
            printf("Response is sent from %d to %d", i, gid);
            flag = 1;
          }
        }
        if (flag == 1) {
          c = subcdr;
        } else {
          c = gid;
        }
      }

      display();
      break;
    case 2:
      //activate
      printf("\nEnter Process no. to Activated::");
      scanf("%d", & activate);
      if (!list[activate])
        list[activate] = 1;
      else {
        printf("\nProcess is alreaady alive!!");
        break;
      }
      if (activate == n) {
        c = n;
        break;
      }
      for (i = activate + 1; i <= n; i++) {
        printf("\nmessage is sent from %d to %d", activate, i);
        if (list[i]) {
          subcdr = i;
          printf("Response is sent from %d to %d", i, activate);
          flag = 1;
        }
      }
      if (flag == 1) {
        c = subcdr;
      } else {
        c = activate;
      }

      display();
      break;
    case 3:
      display();
      break;
    case 4:
      break;
    }
  } while (ch != 4);
}
int main() {
  int i, j;
  printf("\nEnter no. of process::");
  scanf("%d", & n);
  for (i = 1; i <= n; i++) {
    printf("\nEnter Process %d is Alive or not(0/1)::", i);
    scanf("%d", & list[i]);
    if (list[i])
      c = i;
  }
  display();
  printf("\nBULLY ALGORITHM\n");
  bully();
  return 0;
}