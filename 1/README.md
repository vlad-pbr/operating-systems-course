# EX 1 - System calls, file descriptors, I/O redirection

```bash
# compile comp.c
gcc -o comp.out comp.c

# identical files comparison
echo -ne "./examples/a.txt\n./examples/a.txt\n" | ./comp.out
echo $?
# 2

# different files comparison
echo -ne "./examples/a.txt\n./examples/b.txt\n" | ./comp.out
echo $?
# 1

# compile main.c
gcc -o main main.c

# run student grading
./main data/config.txt
cat results.csv
# vlad,0
# rafael,100
# or,0
```