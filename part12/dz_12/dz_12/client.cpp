//blocking_tcp_echo_client
#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <sstream>
#include <Windows.h>

using boost::asio::ip::tcp;
const int max_length = 65536;   //copy bufer size

int main(int argc, char* argv[])
{ 
    //for cyrillic
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    std::string ip; //ip adress of server
    std::string port;   //port for connection with server
    std::string filePath{};
    try
    {
        if (argc < 2)
        {
            throw std::exception("Need path to file in command line arguments\n");
        }    
        //For situation with spaces in path
        else{
            for (int i = 1; i < argc; i++) {                
                filePath += argv[i];
                filePath += " ";                
            }                
        }

        std::cout << "Enter server IP: ";
        std::getline(std::cin, ip);

        //check that ip is not empty
        if (ip.empty())
        {
            throw std::runtime_error("IP cannot be empty!");
        }

        std::cout << "Enter port that server listen for: ";
        std::getline(std::cin, port);

        //check that port is not empty
        if (port.empty())
        {
            throw std::runtime_error("Port cannot be empty!");
        }

        //Boost.Asio uses io_service to talk to the operating system's I/O service
        boost::asio::io_context io_context_info;    //for information about file
        boost::asio::io_context io_context_data;    //for copy data


        //create sockets
        tcp::socket s_info(io_context_info);    //socket for information about file
        tcp::socket s_data(io_context_data);    //socket for copy data


        //create resolvers
        tcp::resolver resolver_info(io_context_info);   //resolver for information about file
        tcp::resolver resolver_data(io_context_data);   //resolver for copy data

        //connection based on the entered host and port
        boost::asio::connect(s_info, resolver_info.resolve(ip, port));
        boost::asio::connect(s_data, resolver_data.resolve(ip, port));

        std::cout << "Connection with server detected...\n";

        //send file name and extension
        //parse path to file for sending
        char* ch_filePath = new char[strlen(filePath.c_str())];
        strcpy(ch_filePath, filePath.c_str());
        char *f_name = strrchr(ch_filePath, '\\');
        if (f_name == NULL) {
            f_name = argv[1];
        }
        else {
            f_name++;
        }

        std::cout << "Sending file info: " << f_name<<"\n";
        
        //send information about file
        boost::asio::write(s_info, boost::asio::buffer(f_name, sizeof(char)*strlen(f_name)));

        //try open file for reading
        FILE* file = fopen(filePath.c_str(), "rb");
        if (!file) {
            throw std::exception("Cannot open file for reading\n");
        }
        
        char data[max_length];  //copy bufer
        double fileSize = 0;

        std::cout << "Data transfer started...\n";
        while(!feof(file)) {              
            //read from file
            int realsize = fread((char*)data, sizeof(char), max_length, file);
            //send to server
            boost::asio::write(s_data, boost::asio::buffer(data, realsize));
            //calculate total file size
            fileSize += realsize / 1048576.0;
        }        

        std::cout << "Data transfer successfully end...\n";
        if (fileSize >= 1.0) {
            std::cout << "Sended: " << fileSize << " Mb\n";
        }
        else {
            fileSize *= 1024;
            if (fileSize >= 1.0) {
                std::cout << "Sended: " << fileSize << " Kb\n";
            }
            else {
                fileSize *= 1024;
                std::cout << "Sended: " << fileSize << " bytes\n";
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