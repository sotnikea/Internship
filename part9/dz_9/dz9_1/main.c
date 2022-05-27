#include "sendfile.h"
#include <stdio.h>

int main(int argc, char **argv){

    //check, if source and destination file taked as argument
    if (argc == 3){
        CopyFile(argv[1], argv[2]);
    }
    //make possible set files name
    else{
        char file1[256], file2[256];
        printf("From file: ");
        scanf("%s",file1);
        printf("To file: ");
        scanf("%s",file2);
        CopyFile(file1, file2);
    }

    return 0;
}