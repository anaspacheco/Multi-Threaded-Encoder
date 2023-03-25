#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char* argv[]){
    int output = STDOUT_FILENO;
   
    char last_char = 0;
    int last_count = 0;

    //Looping through all input files (skipping the program)
    for(int i = 1; i < argc; i++){
        int fd = open(argv[i], O_RDONLY);
        if (fd == -1){
            handle_error("fd");
        }

        // Get file size
        struct stat sb;
        if (fstat(fd, &sb) == -1){
            handle_error("sb");
        }

        // Map file into memory
        char *addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (addr == MAP_FAILED){
            handle_error("addr");
        }

        //Checking the # of occurances of each char and writing it to stdout
        int char_count = 1;
        char prev_char = addr[0];

        if(prev_char == last_char && i > 1){
            char_count+=last_count;
        }
        for(int j = 1; j < sb.st_size; j++){
            if(addr[j] != prev_char){
                write(output, &prev_char, 1);
                write(output, &char_count, 1);
                char_count = 1;
                prev_char= addr[j];
            } else{
                char_count++;
            }
        }
        last_char = prev_char; last_count = char_count;
        if(i == argc - 1) {
            write(output, &prev_char, 1);
            write(output, &char_count, 1);
        }
        //Closing fd and freeing memory https://linux.die.net/man/2/munmap 
        close(fd);
        munmap(addr, sb.st_size);
    }
}




