// ex2_client.c

#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>

#define SERVER_INPUT_FILE_NAME "to_srv"

#define ACTION_TO_SIGNAL (int[]) { SIGUSR1, SIGUSR2, SIGSTKFLT, 4 }

int ascii_to_integer(char* ascii) {

    // define variables
    int size;
    int result = 0;

    // get string length
    for (size = 0; ascii[size] != '\0'; size++);

    // calculate result
    for(int multiplier = 1; size != 0; size--, multiplier *= 10) {
        result += (ascii[size-1] - 48) * multiplier;
    }

    return result;
}

void response_handler(int sig) {
    printf("received response\n");
}

void main(int argc, char **argv) {

    // define variables
    int action = (int)(argv[3][0]) - 48;
    int server_input_file_fd;
    int buffer;
    int size;

    // handle respose from client
    signal(SIGUSR1, response_handler);

    // create server input file
    server_input_file_fd = open(SERVER_INPUT_FILE_NAME, O_WRONLY | O_CREAT, 0644);

    // write client pid
    buffer = getpid();
    write(server_input_file_fd, (const void*)(&buffer), sizeof(int));

    // write first number
    buffer = ascii_to_integer(argv[2]);
    write(server_input_file_fd, (const void*)(&buffer), sizeof(int));

    // write action
    buffer = ascii_to_integer(argv[3]);
    write(server_input_file_fd, (const void*)(&buffer), sizeof(int));

    // write second number
    buffer = ascii_to_integer(argv[4]);
    write(server_input_file_fd, (const void*)(&buffer), sizeof(int));

    // close server input file
    close(server_input_file_fd);

    // send signal to server
    kill(ascii_to_integer(argv[1]), SIGUSR1);

    while(1);
}