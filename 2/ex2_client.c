// ex2_client.c

#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>

#define SERVER_INPUT_FILE_NAME "to_srv"

int ascii_to_integer(char* ascii) {

    // define variables
    int size = 0;
    int result = 0;

    // get string length
    for (; ascii[size] != '\0'; size++);

    // calculate result
    for(int multiplier = 1; size != 0; size--, multiplier *= 10) {
        result += (ascii[size-1] - 48) * multiplier;
    }

    return result;
}

void integer_to_ascii(int integer, char* buffer) {

    // define variables
    int size = 0;

    // get integer length
    for(int i = integer; i != 0; i /= 10, size++);

    // fill buffer with chars
    buffer[size] = '\0';
    for(; integer != 0; integer /= 10, size--) {
        buffer[size-1] = (integer % 10) + 48;
    }
}

void response_handler(int sig) {

    // define variables
    int server_response_file_fd;
    char server_response_file_name[10 + 7 + 1] = "to_client_"; // approx. 4 mil maximum PIDs on 64 bit systems
    char *client_pid_postfix = server_response_file_name + 10;
    int answer;

    // combine server response file name
    integer_to_ascii(getpid(), client_pid_postfix);

    // read response and delete file
    server_response_file_fd = open(server_response_file_name, O_RDONLY);
    read(server_response_file_fd, &answer, sizeof(int));
    printf("%d\n", answer);
    close(server_response_file_fd);
    remove(server_response_file_name);
}

void main(int argc, char **argv) {

    // define variables
    int action = (int)(argv[3][0]) - 48;
    int server_input_file_fd;
    int buffer;
    int size;

    // handle response from server
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