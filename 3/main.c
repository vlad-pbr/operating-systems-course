// main.c

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

struct Process {
    int arrivalTime;
    int computationTime;
};

int ascii_to_integer(char* ascii) {

    // define variables
    int size = 0;
    int result = 0;
    int neg_multiplier = 1;
    char *ascii_pointer = ascii;

    // account for 0
    if (ascii_pointer[0] == '0' && ascii_pointer[1] == '\0') {
        return 0;
    }

    // account for negative integer
    if (ascii_pointer[0] == '-') {
        ascii_pointer++;
        neg_multiplier = -1;
    }

    // get string length
    for (; ascii_pointer[size] != '\0'; size++);

    // calculate result
    for(int multiplier = 1; size != 0; size--, multiplier *= 10) {
        result += (ascii_pointer[size-1] - 48) * multiplier;
    }

    return result * neg_multiplier;
}

int read_int(int fd) {

    // define variables
    char byte[1] = { 1 };
    char buffer[10 + 1]; // max 10 decimals in 32 bit int + zero termination

    // read until separator
    for (int i = 0; byte[0] != '\n' && byte[0] != '\r' && byte[0] != ',' && byte[0] != '\0'; i++ ) {
        read(fd, byte, 1);
        if (byte[0] != '\n' && byte[0] != '\r' && byte[0] != ',' && byte[0] != '\0') {
            buffer[i] = byte[0];
            buffer[i+1] = '\0';
        }
    }

    // read until newline if carriage return was found
    if (byte[0] == '\r') {
        read(fd, byte, 1);
    }

    return ascii_to_integer(buffer);
}

float FCFS_NP_TA(struct Process *processes, int size) {

    // define variables
    int tick = 0;
    float avg_turnaround = 0;
    int current_process;
    int *completed_processes;

    // init dynamic completed processes array
    completed_processes = malloc( size * sizeof(int) );
    for (int i = 0; i < size; i++) {
        completed_processes[i] = 0;
    }

    // scheduler loop
    for (int completed_amt = 0; completed_amt < size; ) {

        current_process = -1;

        // queue next process
        for (int i = 0; i < size; i++) {

            // ignore processes that have not yet arrived or that have been dealt with
            if (processes[i].arrivalTime <= tick && completed_processes[i] == 0 ) {

                // if no process queued yet, select current
                if (current_process == -1) {
                    current_process = i;
                }

                // otherwise queue current if it did arrive before
                else if ( processes[i].arrivalTime < processes[current_process].arrivalTime) {
                    current_process = i;
                }

            }

        }

        // advance clock if no process queued
        if (current_process == -1) {
            tick++;

        // compute queued process
        } else {

            tick += processes[current_process].computationTime;
            completed_processes[current_process] = 1;
            completed_amt++;
            avg_turnaround += tick - processes[current_process].arrivalTime;

        }
    }

    // compute average
    return avg_turnaround / size;
}

float LCFS_NP_TA(struct Process *processes, int size) {
    
    // define variables
    int tick = 0;
    float avg_turnaround = 0;
    int current_process;
    int *completed_processes;

    // init dynamic completed processes array
    completed_processes = malloc( size * sizeof(int) );
    for (int i = 0; i < size; i++) {
        completed_processes[i] = 0;
    }

    // scheduler loop
    for (int completed_amt = 0; completed_amt < size; ) {

        current_process = -1;

        // queue next process
        for (int i = 0; i < size; i++) {

            // ignore processes that have not yet arrived or that have been dealt with
            if (processes[i].arrivalTime <= tick && completed_processes[i] == 0 ) {

                // if no process queued yet, select current
                if (current_process == -1) {
                    current_process = i;
                }

                // otherwise queue current if it did arrive after
                else if ( processes[i].arrivalTime > processes[current_process].arrivalTime) {
                    current_process = i;
                }

            }

        }

        // advance clock if no process queued
        if (current_process == -1) {
            tick++;

        // compute queued process
        } else {

            tick += processes[current_process].computationTime;
            completed_processes[current_process] = 1;
            completed_amt++;
            avg_turnaround += tick - processes[current_process].arrivalTime;

        }
    }

    // compute average
    return avg_turnaround / size;
}

float LCFS_P_TA(struct Process *processes, int size) {
    
    // define variables
    int tick = 0;
    float avg_turnaround = 0;
    int current_process;
    int *processes_completion;

    // init dynamic processes completion array
    processes_completion = malloc( size * sizeof(int) );
    for (int i = 0; i < size; i++) {
        processes_completion[i] = 0;
    }

    // scheduler loop
    for (int completed_amt = 0; completed_amt < size; ) {

        current_process = -1;

        // queue next process
        for (int i = 0; i < size; i++) {

            // ignore processes that have not yet arrived or that have been dealt with
            if (processes[i].arrivalTime <= tick && processes_completion[i] < processes[i].computationTime + 1 ) {

                // if no process queued yet, select current
                if (current_process == -1) {
                    current_process = i;
                }

                // otherwise queue current if it did arrive after
                else if ( processes[i].arrivalTime > processes[current_process].arrivalTime) {
                    current_process = i;
                }

            }

        }

        // advance clock if computational time is required for current process
        if (current_process == -1 || (current_process != -1 && processes[current_process].computationTime != 0) ) {
            tick++;
        }

        // compute current process for 1 tick if queued
        if (current_process != -1) {
            processes_completion[current_process]++;
            
            // if computed - mark as completed
            if (processes_completion[current_process] >= processes[current_process].computationTime) {
                processes_completion[current_process]++;
                completed_amt++;
                avg_turnaround += tick - processes[current_process].arrivalTime;
            }

        }
    }

    // compute average
    return avg_turnaround / size;
}

float RR_TA(struct Process *processes, int size, int time_quantum) {
    
    // define variables
    int tick = 0;
    float avg_turnaround = 0;
    int *processes_completion;
    int *ready_queue;
    int ready_queue_size = size * 2;
    int ready_queue_head = 0;
    int ready_queue_tail = 0;
    int round = 0;

    // init process completion array
    processes_completion = malloc( size * sizeof(int) );
    for (int i = 0; i < size; i++) {
        processes_completion[i] = 0;
    }

    // init ready queue
    ready_queue = malloc( ready_queue_size * sizeof(int) );
    for (int i = 0; i < ready_queue_size; i++) {
        ready_queue[i] = -1;
    }

    // scheduler loop
    for (int completed_amt = 0; completed_amt < size; ) {

        // check for newly arrived processes
        for (int i = 0; i < size; i++) {

            // add process that arrived on current tick to ready queue
            if (processes[i].arrivalTime == tick && processes_completion[i] < processes[i].computationTime + 1) {
                ready_queue[ready_queue_tail] = i;
                ready_queue_tail = ++ready_queue_tail % ready_queue_size;
            }

        }

        // advance clock
        if (ready_queue[ready_queue_head] == -1 || 
           (ready_queue[ready_queue_head] != -1 && processes[ready_queue[ready_queue_head]].computationTime != 0)) {
               tick++;
               round = ++round % time_quantum;
               if (ready_queue[ready_queue_head] != -1 && processes[ready_queue[ready_queue_head] == -1].computationTime != 0) {
                   round = 0;
               }
           }

        // if a process is currently queued - compute for 1 tick
        if (ready_queue[ready_queue_head] != -1) {
            processes_completion[ready_queue[ready_queue_head]]++;
            
            // if process was completed - dequeue and mark
            if (processes[ready_queue[ready_queue_head]].computationTime <= processes_completion[ready_queue[ready_queue_head]]) {
                processes_completion[ready_queue[ready_queue_head]]++;
                completed_amt++;
                avg_turnaround += tick - processes[ready_queue[ready_queue_head]].arrivalTime;
                round = 0;
                ready_queue[ready_queue_head] = -1;
                ready_queue_head = ++ready_queue_head % ready_queue_size;
            }

            // if q was completed for current process - requeue and switch
            else if (round == 0) {
                ready_queue[ready_queue_tail] = ready_queue[ready_queue_head];
                ready_queue_tail = ++ready_queue_tail % ready_queue_size;
                ready_queue[ready_queue_head] = -1;
                ready_queue_head = ++ready_queue_head % ready_queue_size;
            }
        }

        // no process queued - no round
        else {
            round = 0;
        }
    }

    // compute average
    return avg_turnaround / size;
}

float SJF_P_TA(struct Process *processes, int size) {
    
    // define variables
    int tick = 0;
    float avg_turnaround = 0;
    int *processes_completion;
    int *ready_queue;
    int ready_queue_pos = -1;
    int compare;

    // init process completion and ready queue arrays
    processes_completion = malloc( size * sizeof(int) );
    ready_queue = malloc( size * sizeof(int) );
    for (int i = 0; i < size; i++) {
        processes_completion[i] = 0;
        ready_queue[i] = -1;
    }

    // scheduler loop
    for (int completed_amt = 0; completed_amt < size; ) {

        compare = 0;

        // check for newly arrived processes
        for (int i = 0; i < size; i++) {

            // add process that arrived on current tick to ready queue
            if (processes[i].arrivalTime == tick && processes_completion[i] < processes[i].computationTime + 1) {
                ready_queue[i] = 1;
                compare = 1;
            }

        }

        // decide on process to compute
        if (compare == 1 || ready_queue_pos == -1) {

            for (int i = 0; i < size; i++) {

                // ignore non queued processes
                if (ready_queue[i] != -1) {

                    // if no process is currently selected or the current process is shorter - select
                    if (ready_queue_pos == -1 || 
                        (processes[i].computationTime - processes_completion[i]) < 
                        (processes[ready_queue_pos].computationTime - processes_completion[ready_queue_pos])) {
                        ready_queue_pos = i;
                    }

                }

            }

        }

        // advance clock if compute time is not zero
        if (ready_queue_pos == -1 || (ready_queue_pos != -1 && processes[ready_queue_pos].computationTime != 0 ) ) {
            tick++;
        }

        // compute if selected
        if (ready_queue_pos != -1) {
            processes_completion[ready_queue_pos]++;

            // if finished computing process - mark
            if (processes_completion[ready_queue_pos] >= processes[ready_queue_pos].computationTime) {
                processes_completion[ready_queue_pos]++;
                completed_amt++;
                avg_turnaround += tick - processes[ready_queue_pos].arrivalTime;
                ready_queue[ready_queue_pos] = -1;
                ready_queue_pos = -1;
            }
        }
    }

    // compute average
    return avg_turnaround / size;
}

void main(int argc, char **argv) {

    // define variables
    int input_file_fd;
    struct Process *processes;
    int processes_amount;
    int time;
    char prompts[5][31] = {
        "FCFS: mean turnaround = \0",
        "LCFS (NP): mean turnaround = \0",
        "LCFS (P): mean turnaround = \0",
        "RR: mean turnaround = \0",
        "SJF: mean turnaround = \0",
    };

    // make sure input is passed
    if (argc < 2) {
        _exit(1);
    }

    // open input file
    if ( (input_file_fd = open(argv[1], O_RDONLY)) == -1 ) {
        _exit(1);
    }

    // read processes amount
    processes_amount = read_int(input_file_fd);

    // allocate memory for processes to be read
    processes = malloc( processes_amount * sizeof(struct Process) );

    // read times for each process
    for (int i = 0; i < processes_amount; i++) {
        processes[i].arrivalTime = read_int(input_file_fd);
        processes[i].computationTime = read_int(input_file_fd);
    }

    // calculate turnaround times
    printf("%s%g\n%s%g\n%s%g\n%s%g\n%s%g\n",
            prompts[0], FCFS_NP_TA(processes, processes_amount),
            prompts[1], LCFS_NP_TA(processes, processes_amount),
            prompts[2], LCFS_P_TA(processes, processes_amount),
            prompts[3], RR_TA(processes, processes_amount, 2),
            prompts[4], SJF_P_TA(processes, processes_amount));
}