// comp.c

#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 50

int main() {
    
    // define vars
    char buffer[BUFFER_SIZE+1];
    int nbytes;
    int fds[2];
    int fd_buffers[2][1];
    int fd_eof[2];
    int rc = 2;

    // open files
    for (int i = 0; i < 2; i++) {
        
        // read path from stdin
        // assuming path is valid
        nbytes = read(0, buffer, BUFFER_SIZE);
        buffer[nbytes-1] = '\0';

        // open file for reading
        // assuming permission is granted
        fds[i] = open(buffer, O_RDONLY);
    }

    // iterate bytes
    while (!fd_eof[0] && !fd_eof[1] && rc == 2) {

        // read byte from each file and check for eof
        for (int i = 0; i < 2; i++) {
            if (!read(fds[i], fd_buffers[i], 1)) {
                fd_eof[i] = 1;
            }
        }

        // reached eof of one but not the other OR bytes do not match
        if (fd_eof[0] ^ fd_eof[1] || fd_buffers[0][0] != fd_buffers[1][0]) {
            rc = 1;
        }

    }

    // close files
    close(fds[0]);
    close(fds[1]);

    return rc;
}