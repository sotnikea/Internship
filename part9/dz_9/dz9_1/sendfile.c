#include "sendfile.h"


void CopyFile(const char* from, const char* to){
    int src;    //descriptor opened for reading
    int dest;   //descriptor opened for writing
    struct stat stat_buf;  //information about start file
    off_t offset = 0;      //loacation in the file for reading

    //open file for reading
    src = open(from, O_RDONLY);

    //if file doesn't exist, stop operation
    if (src==-1){
        printf("File for reading doesn't exist\n");
        return;
    }
    
    //get information about size of file
    fstat(src, &stat_buf);

    //Open file for writing, create it if doesn't exist
    dest = open(to, O_WRONLY|O_CREAT, stat_buf.st_mode);

    //copy file
    sendfile (dest, src, &offset, stat_buf.st_size);

    printf("Operation succesful\n");

    //close both files
    close(dest);
    close(src);
}