//blocking_tcp_echo_server
#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <iostream>
#include <thread>
#include <utility>
#include <boost/asio.hpp>
#include <Windows.h>

using boost::asio::ip::tcp;
const int max_length = 65536;   //copy bufer size


int main(int argc, char* argv[])
{
    //for cyrillic
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    unsigned short port;
    //check, if port set as command line argument 
    if (argc != 2) {
        port = 8888; //default server working port
    }
    else {
        port = std::atoi(argv[1]);
    }

    try
    {
        //Boost.Asio uses io_service to talk to the operating system's I/O service
        boost::asio::io_context io_context;

        //tcp::v4() - localhost (create a server on the local network)
        //port() - port number to listen on
        //create a receiver, an object that handles client connections
        tcp::acceptor a(io_context, tcp::endpoint(tcp::v4(), port));

        std::cout << "Server start..." << std::endl;

        tcp::socket sockInfo = a.accept();  //socket for information about file
        tcp::socket sockData = a.accept();  //socket to copy data
        std::cout << "Connection detected...\n";

        char data[max_length];  //copy buffer

        //to determine when the data transfer ended or the connection was closed for other reasons
        boost::system::error_code error;       

        char fileName[256]; //for the name and extension of the received file

        //read name and extension of the received file and check it size
        size_t length = sockInfo.read_some(boost::asio::buffer(fileName), error);
        fileName[length] = '\0';    //set end of file name/extension
        std::cout << "File name received...\n";
       
        //create file with the received name on binary record
        FILE* file = fopen(fileName, "wb");
        if (!file) {
            throw std::exception("Can't open/create file for writing\n");            
        }

        std::cout << "Create file for writing: "<< fileName <<"\n";
        std::cout << "Data transfer started...\n";

        double fileSize = 0;

        //copy data from socket to file
        for (;;) {
            //try read from socket
            size_t length = sockData.read_some(boost::asio::buffer(data), error);

            if (error == boost::asio::error::eof)
                break; //if end of data and connection close
            else if (error)
                throw boost::system::system_error(error); // some other error.

            //write data to file
            fwrite(data, sizeof(char), length, file);

            //calculate total file size
            fileSize += length / 1048576.0;
        }
        
        std::cout << "Data transfer successfully end...\n";  

        if (fileSize >= 1.0) {
            std::cout << "Received: " << fileSize << " Mb\n";
        }
        else {
            fileSize *= 1024;
            if (fileSize >= 1.0) {
                std::cout << "Received: " << fileSize << " Kb\n";
            }
            else {
                fileSize *= 1024;
                std::cout << "Received: " << fileSize << " bytes\n";
            }
        }
        

        fclose(file);
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    catch (...) {
        std::cerr << "Unknown error\n";
    }

    return 0;
}