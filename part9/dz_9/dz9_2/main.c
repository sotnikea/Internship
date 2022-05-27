#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



int main(void)
{
    //create pipe
    int pipefds[2];
    pipe(pipefds);

    //clone created process
    pid_t pid = fork();

    //if it parent process
    if (pid > 0) {
        int len;
        char buf[1024];
        close(pipefds[1]);  //close pipe for writing        
        while ((len = read(pipefds[0], buf, 1024)));
        strcat(buf," Hi!\00");
        close(pipefds[0]);  //close pipe for reading

        //Open file for writing, create it if doesn't exist
        int dest = open("readme.txt", O_WRONLY|O_CREAT);

		//Write to file
        write(dest, buf, strlen(buf));
        close(dest);

        printf("Operation succesfull. Read readme in the folder with project\n");
    }
    //if it child process
    else{
        const char *str = "Hi daddy, I'm your child!";
        close(pipefds[0]);  //close pipe for reading
        write(pipefds[1], str, strlen(str) + 1);    //write message to pipe
        close(pipefds[1]);  //close pipe for writing
    } 
}