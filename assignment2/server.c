// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Declare them to get rid of warnings */
int doWork(int socket_fd); 
int reducePrivs();
int wait4Children();

#define PORT 80
int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    pid_t fpid=-1;
    
    printf("execve=0x%p\n", execve);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    /* Attaching socket to port 80 */
    if (setsockopt(server_fd, 
    		    SOL_SOCKET, 
    		    SO_REUSEADDR | SO_REUSEPORT,
                    &opt, 
                    sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    /* Forcefully attaching socket to the port 80 */
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    if ((new_socket = accept(server_fd, 
    		 	      (struct sockaddr *)&address,
                             (socklen_t*)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    fpid = fork();    

    if (fpid == 0) /* Child process */
    {
    	int status = 0;
    	status = reducePrivs();
    	if(status ==0)
    	  status = doWork(new_socket); 
    	else
    	  printf("Privelege Separation Failure");
    
    }
    else if (fpid > 0) /* Parent process */
    {
        int status = 0;
        status = wait4Children();  
    }
    else /* FORK failed */
    {
    	; /* Cannot do anything here */
    } 

    return 0;
}

int doWork(int socket_fd)
{
    int valread = 0; 
    char buffer[102] = {0};
    char *hello = "Hello from server";
   
    valread = read(socket_fd, buffer, 1024);
    printf("%s\n", buffer);
    send(socket_fd, hello, strlen(hello), 0);
    printf("Hello message sent\n");

    return 0; 
}

int reducePrivs()
{
    int stat = setuid(65534); 
    if(stat == 0)  
    { 
      printf("Priveleges Dropped\n");
      return 0;
    } 
    else 
      return 1; 
}

int wait4Children()
{
    int status = 0; 
    int numchildren = 0; 
    pid_t cpid = wait(&status); 
    
    while (cpid > 0)
    {
    	numchildren++; 
     	/* check if there is another child process to wait on */ 
    	cpid = wait(&status); 
    }
    printf("All child processes have completed execution. Parent can now exit\n"); 
    return 0; 
}
