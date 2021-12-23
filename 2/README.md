# EX 2 - System calls, signals

Client - server achitecture using signals

__Server__: Calculator that handles SIGUSR1 and reads from "to_srv" file in order to calculate the output
__Client__: Writes the "to_srv" file based on given args and signals SIGUSR1 to server, prints response

```bash

# compiles client and server, runs server on background, runs client with basic 2 + 2 calculation
make run_all

# pkill -9 ex2_ || true
# gcc ex2_srv.c -o ex2_srv.out
# ./ex2_srv.out &
# end of stage 1
# gcc ex2_client.c -o ex2_client.out
# ./ex2_client.out 141277 2 1 2
# end of stage 2
# end of stage 3
# end of stage 4
# end of stage 5
# end of stage 6
# 4
# end of stage 7

# kills any client/server processes still around
make kill
```