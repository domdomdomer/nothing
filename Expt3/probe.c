#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#define MSG_CONFIRM 0
#define TRUE 1
#define FALSE 0
typedef struct list{
	int probe[3];	
}list;


int connect_to_port (int connect_to)
{
  int sock_id;
  int opt = 1;
  struct sockaddr_in server;
  if ((sock_id = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
    {
      perror ("unable to create a socket");
      exit (EXIT_FAILURE);
    }
  setsockopt (sock_id, SOL_SOCKET, SO_REUSEADDR, (const void
						  *) &opt, sizeof (int));
  memset (&server, 0, sizeof (server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons (connect_to);
  if (bind (sock_id, (const struct sockaddr *) &server, sizeof (server)) < 0)
    {
      perror ("unable to bind to port");
      exit (EXIT_FAILURE);
    }
  return sock_id;
}


void send_to_id (int to, int id, list llist)
{
  struct sockaddr_in cl;
  memset (&cl, 0, sizeof (cl));
  cl.sin_family = AF_INET;
  cl.sin_addr.s_addr = INADDR_ANY;
  cl.sin_port = htons (to);
  sendto (id, &llist, sizeof(list), MSG_CONFIRM, (const struct sockaddr *) &cl, sizeof (cl));
}


void send_probes (int id, int self,int *procs,int n_edges,list l)
{
  	
  	for (int i = 0; i < n_edges; i++)
	{
		l.probe[2]=procs[i];
		printf ("Sending Probe to: %d\n", procs[i]);
		send_to_id (procs[i], id, l);
	}
	 
}

void print(list l)
{
	for(int i=0;i<3;i++)
	{
		printf("%d ",l.probe[i]);
	}
}

int main (int argc, char *argv[])
{

	int self = atoi (argv[1]);
	int sock_id, bully_id;
	int itr, len, n, start_at;  
	int n_edges = atoi (argv[3]);

	int procs[10];

	struct sockaddr_in from;

	for (itr = 0; itr < n_edges; itr += 1)
	procs[itr] = atoi (argv[4 + itr]);

	start_at = atoi (argv[2]) == 1 ? TRUE : FALSE;

	printf ("creating a node at %d %d \n\n", self, start_at);
	sock_id = connect_to_port (self);
		
	list l;
		
	if(start_at == TRUE)
	{
		list l;
	  	l.probe[0]=self;
	  	l.probe[1]=self;
		send_probes(sock_id,self,procs,n_edges,l);
	}

	while (TRUE)
	{
		memset (&from, 0, sizeof (from));
		n = recvfrom (sock_id, &l, sizeof(list), MSG_WAITALL,(struct sockaddr *) &from, &len);
		printf("Recieved probe: ");
		print(l);
		printf("\n");
		if(l.probe[0]==-1)
		{
			printf("Deadlock has happened");
			send_probes(sock_id,self,procs,n_edges,l);
			break;
		}
		if(l.probe[0]==self)
		{
			printf("Deadlock is detected");
			l.probe[0]=-1;
			send_probes(sock_id,self,procs,n_edges,l);
			break;
		}
		
		else{
			l.probe[1]=self;
			
			send_probes(sock_id,self,procs,n_edges,l);
			
		}
	  
  	}
  
}

