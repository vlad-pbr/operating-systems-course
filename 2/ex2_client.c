// ex2_client.c

#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/random.h>

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

void exit_with_error() {
    write(1, "ERROR_FROM_EX2\n", 16);
    _exit(1);
}

void timeout_handler(int sig) {
    write(1, "Client closed because no response was received from the server for 30 seconds\n", 79);
    _exit(1);
}

void response_handler(int sig) {

    // define variables
    int server_response_file_fd;
    char server_response_file_name[10 + 7 + 1] = "to_client_"; // approx. 4 mil maximum PIDs on 64 bit systems
    char *client_pid_postfix = server_response_file_name + 10;
    int answer;
    char answer_string[10 + 1]; // max 10 decimals in 32 bit int
    int size;

    // cancel timeout
    alarm(0);

    // combine server response file name
    integer_to_ascii(getpid(), client_pid_postfix);

    // read response and delete file
    server_response_file_fd = open(server_response_file_name, O_RDONLY);
    read(server_response_file_fd, &answer, sizeof(int));

    // get answer in ascii
    integer_to_ascii(answer, answer_string);

    // write to stdout
    for (size = 0; answer_string[size] != '\0'; size++);
    write(1, answer_string, size);
    write(1, "\n", 1);

    // close and delete
    close(server_response_file_fd);
    unlink(server_response_file_name);

    _exit(0);
}

void main(int argc, char **argv) {

    // define variables
    int action = (int)(argv[3][0]) - 48;
    int server_input_file_fd;
    int random;
    int buffer;
    int size;

    // handle response from server and set timeout
    signal(SIGUSR1, response_handler);
    signal(SIGALRM, timeout_handler);

    // try to create server input file 10 times
    for(int i = 0; i < 10; i++) {

        // sleep if could not create
        if ( (server_input_file_fd = open(SERVER_INPUT_FILE_NAME, O_WRONLY | O_CREAT | O_EXCL, 0644)) == -1 ) {

            // sleep for 1 to 5 seconds
            getrandom(&random, sizeof(int), GRND_RANDOM);
            sleep(( (random * (random < 0 ? -1 : 1) ) % 5 ) + 1);

        } else {
            i = 10;
        }

    }

    // make sure file was created
    if (server_input_file_fd == -1) {
        exit_with_error();
    }

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

    // set timeout
    alarm(30);

    while(1);
}