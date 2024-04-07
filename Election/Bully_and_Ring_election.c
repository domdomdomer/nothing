#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define MAX 20
int list[MAX],n,cdr;
void display()
{
    int i;
    printf("\nProcesses-->");
    for(i=1;i<=n;i++)
        printf("\t %d",i);
    printf("\nAlive-->");
    for(i=1;i<=n;i++)
        printf("\t %d",list[i]);
    printf("\ncoordinator is::%d",cdr);
}
void ring()
{
    int msg[20],ring_n,k,i;
    int ch,crash,activate,gid,flag,subcdr;
    do
    {
            printf("\n1.Crash\n2.Activate\n3.Display\n4.Exit\nEnter You choice::");
            scanf("%d",&ch);
            switch(ch)
            {
                case 1:
                    printf("\nEnter Process no. to Crash::");
                    scanf("%d",&crash);
                    if(list[crash])
                        list[crash]=0;
                    else
                    {
                        printf("\nProcess is alreaady dead!!");
                        break;
                    }
                    do
                    {
                        printf("\nEnter election generator id::");
                        scanf("%d",&gid);
                        if(gid==cdr)
                        {
                            printf("\nenter a valid generator id::");
                        }
                    }while(gid==crash);
                    flag=0;
                    k=1;
                    if(crash==cdr)
                    {
                        msg[k++]=gid;
                        for(i=(gid+1)%n;i!=gid;i=(i+1)%n)
                        {
                            if(list[i])
                            {
                                printf("\nmessage is sent to %d k =%d",i,k);
                                msg[k++]=i;                          
                            }
                        }
                        subcdr=0;
                        for(i=1;i<k;i++)
                        {
                            printf("msg::%d\n",msg[i]);
                            if(subcdr<msg[i])
                            {
                                subcdr=msg[i];
                            }
                        }
                        cdr=subcdr;
                    }
                    display();
                    break;
                case 2:
                    //activate
                    printf("\nEnter Process no. to Activated::");
                    scanf("%d",&activate);
                    if(!list[activate])
                        list[activate]=1;
                        else
                    {
                        printf("\nProcess is alreaady alive!!");
                        break;
                    }               
                    
                    if(activate==n)
                    {
                        cdr=n;
                        break;
                    }
                    for(i=activate+1;i<=n;i++)
                    {
                        printf("\nmessage is sent from %d to %d",activate,i);
                        if(list[i])
                        {
                            subcdr=i;
                            printf("Response is sent from %d to %d",i,activate);
                            flag=1;
                        }
                    }
                    if(flag==1)
                    {
                        cdr=subcdr;
                    }
                    else
                    {
                        cdr=activate;
                    }
                
                    
                display();
                break;
            case 3:
                display();
            break;
            case 4:
                break;
            }
    }while(ch!=4);
    
}
void bully()
{
    int ch,crash,activate,i,gid,flag,subcdr;
    do
    {
            printf("\n1.Crash\n2.Activate\n3.Display\n4.Exit\nEnter You choice::");
            scanf("%d",&ch);
            switch(ch)
            {
                case 1:
                    printf("\nEnter Process no. to Crash::");
                    scanf("%d",&crash);
                    if(list[crash])
                        list[crash]=0;
                    else
                    {
                        printf("\nProcess is alreaady dead!!");
                        break;
                    }
                    do
                    {
                        printf("\nEnter election generator id::");
                        scanf("%d",&gid);
                        if(gid==cdr)
                        {
                            printf("\nenter a valid generator id::");
                        }
                    }while(gid==crash);
                    flag=0;
                    if(crash==cdr)
                    {
                        for(i=gid+1;i<=n;i++)
                        {
                            printf("\nmessage is sent from %d to %d",gid,i);
                            if(list[i])
                            {
                                subcdr=i;
                                printf("Response is sent from %d to %d",i,gid);
                                flag=1;
                            }
                        }
                        if(flag==1)
                        {
                            cdr=subcdr;
                        }
                        else
                        {
                            cdr=gid;
                        }
                    }
                    display();
                    break;
                case 2:
                    //activate
                    printf("\nEnter Process no. to Activated::");
                    scanf("%d",&activate);
                    if(!list[activate])
                        list[activate]=1;
                    else
                    {
                        printf("\nProcess is alreaady alive!!");
                        break;
                    }               
                    if(activate==n)
                    {
                        cdr=n;
                        break;
                    }
                    for(i=activate+1;i<=n;i++)
                    {
                        printf("\nmessage is sent from %d to %d",activate,i);
                        if(list[i])
                        {
                            subcdr=i;
                            printf("Response is sent from %d to %d",i,activate);
                            flag=1;
                        }
                    }
                    if(flag==1)
                    {
                        cdr=subcdr;
                    }
                    else
                    {
                        cdr=activate;
                    }
                
                    
                display();
                break;
            case 3:
                display();
            break;
            case 4:
                break;
            }
    }while(ch!=4);
}
int main()
{
    int i,j;
    printf("\nEnter no. of process::");
    scanf("%d",&n);
    for(i=1;i<=n;i++)
    {
        printf("\nEnter Process %d is Alive or not(0/1)::",i);
        scanf("%d",&list[i]);
        if(list[i])
            cdr=i;
    }
    display();
    do
    {
        printf("\n1.BULLY ALGORITHM \n2.RING ALGORITHM\n3.Display\n4.EXIT\nEnter your choice::");
        scanf("%d",&j);
        switch(j)
        {
            case 1:
                bully();
                break;
            case 2:
                ring();
            case 3:
                display();
                break;
            case 4:
                exit(1);
        }
    }while(j!=4);
    return 0;

}