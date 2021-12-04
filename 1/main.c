// main.c

#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include<sys/wait.h>

#include <stdio.h>

#define BUFFER_LEN 50

int main(int argc, char *argv[]) {
    
    // define vars
    char byte[1];
    int byte_index;
    int config_fd;
    char config_lines[3][BUFFER_LEN+1];
    int nbytes;
    DIR* students_dir;
    struct dirent* entry;
    char *student_file_names[3] = { "main.c", "main", "output.txt" };
    char student_path[BUFFER_LEN];
    char student_files[3][BUFFER_LEN];
    int input_fd;
    int output_fd;
    int rc;
    pid_t pid;
    
    // open config file
    config_fd = open(argv[1], O_RDONLY);

    // read each line
    for (int i = 0; i < 3; i++) {

        for(byte_index = 0, byte[0] = 0; byte[0] != '\n' && byte_index < BUFFER_LEN; byte_index++) {
            nbytes = read(config_fd, byte, 1);
            if(nbytes && byte[0] != '\n') {
                config_lines[i][byte_index] = byte[0];
            }
        }

        config_lines[i][byte_index-1] = '\0';
    }

    // iterate students
    students_dir = opendir(config_lines[0]);
    for (entry = readdir(students_dir); entry != NULL; entry = readdir(students_dir)) {

        // ignore hidden files
        if (entry->d_name[0] != '.') {

            // append students dir path
            int j = 0;
            for (; config_lines[0][j] != '\0'; j++) {
                student_path[j] = config_lines[0][j];
            }
            student_path[j++] = '/';

            // append student name
            for (int k = 0; entry->d_name[k] != '\0'; k++, j++) {
                student_path[j] = entry->d_name[k];
            }
            student_path[j++] = '/';

            // append file names
            for (int f = 0, z = j; f < 3; f++, z = j) {

                // copy path to file
                for (int k = 0; k < j; k++) {
                    student_files[f][k] = student_path[k];
                }

                // copy file name to each student file
                for (int k = 0; student_file_names[f][k] != '\0'; k++, z++) {
                    student_files[f][z] = student_file_names[f][k];
                }
                student_files[f][z] = '\0';
            }

            // child
            if (fork() == 0) {

                // compile student's program
                execlp("gcc", "gcc", student_files[0], "-o", student_files[1], (char *)NULL);
                return 0;

            }

            // wait on child to compile
            wait(NULL);

            // child
            if (fork() == 0) {

                // set stdin as input from config
                input_fd = open(config_lines[1], O_RDONLY);
                dup2(input_fd, 0);
                close(input_fd);

                // set stdout to the output file
                output_fd = open(student_files[2], O_WRONLY | O_CREAT, 0644 );
                dup2(output_fd, 1);
                close(output_fd);

                // run student's program
                execlp(student_files[1], student_files[1], (char *)NULL);

                return 0;
            }

            // wait on child to run student's program
            wait(NULL);

            // TODO compare answers

        }

    }

    // close students dir
    closedir(students_dir);
}