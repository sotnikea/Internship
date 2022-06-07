#include<sys/socket.h>
#include<unistd.h>
#include<stdio.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char* argv[]){
    int port;   //port for connection with server

    char outMessage[256]={""};  
    if(argc < 2){
        printf("Input message for sending: ");
        scanf("%s", outMessage);
    }
    else{
        for (int i=1; i<argc; i++){            
            if(i>1) strcat(outMessage," ");
            if((strlen(outMessage)+strlen(argv[i])+1) > 256){
                printf("To long message\n");
                exit(EXIT_FAILURE);
            }
            strcat(outMessage,argv[i]);           
        }
    }

    printf("Input port for connection with server: ");
    scanf("%d", &port);

    //check if in argument wrong value for port number
    //if set some symbols port will equal 0
    if (port == 0){
        printf("Wrong argument for port number\n");
        exit(EXIT_FAILURE);
    }

    int client_fd;  //client file descriptor

    //try connect with socket
    if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        printf("Cann't connect with socket\n");
        exit(EXIT_FAILURE);
    }

    //create struct that consist type of connection
    struct sockaddr_in addr;

    //Address to accept any incoming messages (localhost)
    addr.sin_addr.s_addr = INADDR_ANY;

    //Port, for connection with server
    //check for big endian
    addr.sin_port =  htons(port);

    //use internet address
    addr.sin_family = AF_INET;

    //size of created address struct
    int addr_len = sizeof(addr);

    printf("Connect to server on port: %d...\n",port);

    //try connect with server
    if(connect(client_fd, (struct sockaddr *)&addr, addr_len) < 0){
        printf("Connection with server failed\n");
        exit(EXIT_FAILURE);
    }  

    printf("Sending message to server...\n");
    send(client_fd, outMessage, sizeof(outMessage), 0);

    //Read respond from server
    char buf[1024] = {0};
    int read_size = read(client_fd, buf, sizeof(buf));

    printf("Get message from server: %s\n", buf);

    return 0;
}