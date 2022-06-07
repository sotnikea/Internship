#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char* argv[]) {
    int port;   //number of port that server will listen for

    if (argc == 2){
        port = atoi(argv[1]);   //port from argument
    }
    else{
        port = 8888;    //default port
    }

    //check if in argument wrong value for port number
    //if set some symbols port will equal 0
    if (port == 0){
        printf("Wrong argument for port number\n");
        exit(EXIT_FAILURE);
    }

    int server_fd;  //server file descriptor
    int client_fd;  //client filedescriptor

    struct sockaddr_in server_address;  //struct of server socket
    struct sockaddr_in client_address;  //struct of client socket

    int server_len; //length of server socket struct
    int client_len; //length of client socket struct

    int result; //result of function --select-- call, check if some descriptor 
                //ready to read or write 

    fd_set readfds; //set for reading, one of argument for function --select--
    fd_set testfds; 

    //create socket for server
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        printf("Cann't connect with socket\n");
        exit(EXIT_FAILURE);
    }

    //use internet address
    server_address.sin_family = AF_INET;

    //connection to any IP address to accept any incoming messages
    server_address.sin_addr.s_addr = INADDR_ANY;

    //Port, that server listen
    //check for big endian
    server_address.sin_port = htons(port);

    //size of server_address struct
    server_len = sizeof(server_address);

    //create connection between socket and address
    //return 0 if ok, -1 if error
    if(bind(server_fd, 
            (struct sockaddr *)&server_address, 
            server_len) != 0){
        printf("Cann't bind address with socket\n");
        exit(EXIT_FAILURE);
    }

    //Create a queue of connection requests and initialize a set of readfds 
    //to process input from the server_sockfd socket

    //run listen for connection on a socket
    //First param - socket for listener
    //Second param - amount of clients in queue
    if(listen(server_fd, 5) < 0){
        printf("Cann't run listen for connection on socket\n");
        exit(EXIT_FAILURE);
    }

    //initialize the fd_set with an empty set
    FD_ZERO(&readfds);

    //specify the elements of the set, using given server descriptor
    FD_SET(server_fd, &readfds);

    //waiting client request
    while(1) {
        int fd;     //for iteration throw client descriptor
        int nread;  //for function --ioctl-- result

        //copy information about enable descriptor in server_fd in temporary variable
        //changed after select call, so need to be copied
        testfds = readfds;  

        printf("Server start listening on port: %d...\n",port);

        //select allows to check if there at least one of the many file descriptors 
        //is ready for reading or writing
        //or is pending due to an error condition and might block until one of the handles is ready        
        result = select(FD_SETSIZE,             //Maximum number of file descriptors in fd_set structure
                        &testfds,               //This set is used to keep track of the appearance of data available for reading from any file descriptor. After select() returns, only those file descriptors remain in readfds that can be read immediately.
                        (fd_set *)0,            //null pointer
                        (fd_set *)0,            //null pointer
                        (struct timeval *)0);   //null pointer

        if (result < 1) {
            printf("Error with select\n");
            exit(EXIT_FAILURE);
        }

        //if activity detected need to find out which descriptor is active
        //checking each one in turn with the FD_ISSET macro
        for (fd = 0; fd < FD_SETSIZE; fd++) {
            if (FD_ISSET(fd, &testfds)) {
                //if there is activity on server_fd, it may be a request for a new connection
                //add it to the set of descriptors of the client_fd
                if (fd == server_fd) {
                    //make connect server with client
                    client_len = sizeof(client_address);
                    if((client_fd = accept(server_fd,
                                            (struct sockaddr*)&client_address, 
                                            &client_len)) < 0){
                        printf("Connection with client failed\n");
                        exit(EXIT_FAILURE);
                    };

                    ////specify the elements of the set, using given client descriptor
                    FD_SET(client_fd, &readfds);

                    printf("Adding client on fd %d\n", client_fd);
                }
                //if the activity is not connected to the server, then the client is active
                else {
                    //check current fd and set information about it condition to nread
                    //FIONREAD - how many bytes there are readable at fd socket
                    ioctl(fd, FIONREAD, &nread);

                    //if -close- is received, the client disappears and can be removed from the handle set
                    if (nread == 0) {
                        //close client be descriptor
                        close(fd);

                        //clear information about current client in data set
                        FD_CLR(fd, &readfds);

                        printf("removing client on fd %d\n\n", fd);
                    } 
                    //in other way, some client try communicate with server
                    else {
                        printf("Client connected on fd %d\n",fd);

                        //sleep(5);	//for testing with few clients

                        //Read information from client
                        char buf[1024] = {0};
                        int read_size = read(fd, buf, sizeof(buf));

                        printf("Get message from fd: %d - %s\n", fd, buf);

                        //Respond message from server
                        if (!strcmp(buf,"hello")){
                            send(fd, "world", sizeof("world"), 0);  //return message "world" for "hello"
                        }
                        else{
                            send(fd, buf, read_size, 0);  //return mirror message
                        }

                        printf("Sent a response to client\n");    
                    }
                }
            }
        }
    }
}