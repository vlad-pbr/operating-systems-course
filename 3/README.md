# EX 3 - Scheduling algorithms

Given a list of processes with their arrival and burst times, calculate the average turnaround time using the following scheduling algorithms:
- FCFS
- LFCF (Non-preemptive)
- LCFS (Preemptive)
- RR (with 2 time quantum)
- SJF (Preemptive)

```bash

# compiles and runs calculation using 5 different input files
make run

# gcc main.c -o main.out
# for INPUT in input1.txt input2.txt input3.txt input4.txt input5.txt; do \
#         echo "\n$INPUT"; \
#         ./main.out examples/$INPUT; \
# done

# input1.txt
# FCFS: mean turnaround = 17.25
# LCFS (NP): mean turnaround = 19.25
# LCFS (P): mean turnaround = 20
# RR: mean turnaround = 23.75
# SJF: mean turnaround = 16.5

# input2.txt
# FCFS: mean turnaround = 28.125
# LCFS (NP): mean turnaround = 30.125
# LCFS (P): mean turnaround = 30.75
# RR: mean turnaround = 37.375
# SJF: mean turnaround = 21.625

# input3.txt
# FCFS: mean turnaround = 11.5
# LCFS (NP): mean turnaround = 11.5
# LCFS (P): mean turnaround = 11.5
# RR: mean turnaround = 11.5
# SJF: mean turnaround = 11.5

# input4.txt
# FCFS: mean turnaround = 14
# LCFS (NP): mean turnaround = 14.75
# LCFS (P): mean turnaround = 17.25
# RR: mean turnaround = 20
# SJF: mean turnaround = 14

# input5.txt
# FCFS: mean turnaround = 6
# LCFS (NP): mean turnaround = 6
# LCFS (P): mean turnaround = 6.5
# RR: mean turnaround = 7.5
# SJF: mean turnaround = 6
```
