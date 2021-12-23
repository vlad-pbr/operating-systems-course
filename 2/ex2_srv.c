// ex2_srv.c

#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#define SERVER_INPUT_FILE_NAME "to_srv"

// 1 - plus, 2 - minus, 3 - multiply, 4 - divide
#define SOLVE(x, y, action) ( ( action == 1 ? (x + y) : ( action == 2 ? (x - y): ( action == 3 ? (x * y) : (x / y) ) ) ) )

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

void exit_with_error() {
    write(1, "ERROR_FROM_EX2\n", 16);
    _exit(1);
}

void timeout_handler(int sig) {
    write(1, "The server was closed because no service request was received for the last 60 seconds\n", 87);
    _exit(0);
}

void request_handler(int sig) {

    // reset alarm
    alarm(60);

    // child
    if ( (fork()) == 0 ) {

        // define variables
        int server_input_file_fd;
        int client_pid;
        int first_number;
        int action;
        int second_number;
        int client_output_file_fd;
        int size;
        char client_output_file_name[10 + 7 + 1] = "to_client_"; // approx. 4 mil maximum PIDs on 64 bit systems
        char *client_pid_postfix = client_output_file_name + 10;
        int answer;

        // open input file
        if ( (server_input_file_fd = open(SERVER_INPUT_FILE_NAME, O_RDONLY)) == -1 ) {
            exit_with_error();
        };

        // read from input file
        read(server_input_file_fd, &client_pid, sizeof(int));
        read(server_input_file_fd, &first_number, sizeof(int));
        read(server_input_file_fd, &action, sizeof(int));
        read(server_input_file_fd, &second_number, sizeof(int));

        // delete input file
        if ( close(server_input_file_fd) == -1 || unlink(SERVER_INPUT_FILE_NAME) == -1) {
            exit_with_error();
        }

        write(1, "end of stage 4\n", 16);

        // combine client output file name
        integer_to_ascii(client_pid, client_pid_postfix);

        // open answer file
        answer = SOLVE(first_number, second_number, action);
        client_output_file_fd = open(client_output_file_name, O_WRONLY | O_CREAT, 0644);
        
        write(1, "end of stage 5\n", 16);

        // write to answer file
        write(client_output_file_fd, (const void*)(&answer), sizeof(int));
        if ( close(client_output_file_fd) == -1 ) {
            exit_with_error();
        }

        // signal client
        if ( kill(client_pid, SIGUSR1) == -1 ) {
            exit_with_error();
        }

        write(1, "end of stage 6\n", 16);

        _exit(0);
    }

    write(1, "end of stage 3\n", 16);

}

void main() {

    // define variables
    int server_input_file_fd;

    // remove input file if present
    if ( (server_input_file_fd = open(SERVER_INPUT_FILE_NAME, O_RDONLY)) != -1 ) {
        if ( close(server_input_file_fd) == -1 || unlink(SERVER_INPUT_FILE_NAME) == -1 ) {
            exit_with_error();
        }
    }

    // define signal handlers
    signal(SIGUSR1, request_handler);
    signal(SIGALRM, timeout_handler);

    // ignore SIGCHLD so children are silently reaped
    signal(SIGCHLD, SIG_IGN);

    write(1, "end of stage 1\n", 16);

    // set timeout
    alarm(60);

    while(1);
}