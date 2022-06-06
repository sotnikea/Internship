# Основы сетевого взаимодействия
## Оглавление
1. [Computer Network: ](#r1)
2. [OSI](#r2)
3. [Protocol](#r3)
4. [OSI Layers](#r4)
5. [Sockets](#r5)
6. [Sending TCP packets using sockets](#r6)
7. [Sources](#r7)


## <a name="r1">Computer Network: </a>
An interconnection of multiple devices, also known as hosts, that are connected using multiple paths for the purpose of sending/receiving data or media. Computer networks can also include multiple devices/mediums which help in the communication between two different devices; these are known as Network devices and include things such as routers, switches, hubs, and bridges. 

## <a name="r2">OSI</a>
OSI stands for Open Systems Interconnection. It is a reference model that specifies standards for communications protocols and also the functionalities of each layer. 

## <a name="r3">Protocol</a>
A protocol is the set of rules or algorithms which define the way how two entities can communicate across the network and there exists different protocol defined at each layer of the OSI model. Few of such protocols are TCP, IP, UDP, ARP, DHCP, FTP and so on.   

## <a name="r4">OSI Layers</a>
OSI model has seven layers and each layer is responsible for performing different operations. The OSI model layers are shown below.
1. **APPLICATION LAYER** - this layer is responsible for receiving webpage requests from users or clients using the most widely used protocol namely HTTP protocol and displaying response back in the browser.
2. **PRESENTATION LAYER** - this layer is the middle layer between the Application layer and the Session layer. It receives data from the application layer and encodes it.
3. **SESSION LAYER** - this layer responsible for creating a session between client and server. Session means any activity performed within start time and stop time is called the session.
4. **TRANSPORT LAYER** - this layer uses TCP and UDP protocols. When transport layer uses the TCP protocol, it gives the guarantee of transmission. When transport layer uses the UDP protocol, it does not give the guarantee of transmission
5. **NETWORK LAYER** - this layer converts received segment into packets, adds Source and destination IP address to each packet, and is responsible for routing of packets.
6. **DATA LINK LAYER** - this layer receives the Packets from the Network layer and converts them into frames by padding error correction bits, adds the physical address of the sender in frames, detects and corrects errors.
7. **PHYSICAL LAYER** - this layer is responsible for sending data bits over the communication medium or media.

## <a name="r5">Sockets</a>
A socket can be thought of as an endpoint in a two-way communication channel. Socket routines create the communication channel, and the channel is used to send data between application programs either locally or over networks. Each socket within the network has a unique name associated with it called a socket descriptor—a fullword integer that designates a socket and allows application programs to refer to it when needed.

## <a name="r6">Sending TCP packets using sockets</a>
Server:
~~~C
int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    auto hello = "Hello from server";

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        std::cout << "socket failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        std::cout << "bind failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 1) < 0) {
        std::cout << "listen" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Waiting for a client..." << std::endl;
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        std::cout << "accept" << std::endl;
        exit(EXIT_FAILURE);
    }

    valread = read( new_socket , buffer, 1024);
~~~
Client:

~~~C
int client_fd, read_len;
    struct sockaddr_in address;
    char buffer[1024] = {0};
    auto hello = "Hello from client";

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cout << "socket failed";
        exit(EXIT_FAILURE);
    }

    int addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (connect(client_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        std::cout << "connect failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    send(client_fd, hello, strlen(hello), 0);
~~~

## <a name="r7">Sources</a>
https://www.ibm.com/docs/en/zos/2.3.0?topic=services-what-is-socket
https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-socket-create-socket#socket
https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-bind-bind-name-socket
https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-listen-prepare-server-incoming-client-requests
https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-accept-accept-new-connection-socket
https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-connect-connect-socket#connect
https://en.wikipedia.org/wiki/Network_socket
https://bytesofgigabytes.com/networking/osi-model/
