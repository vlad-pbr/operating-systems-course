
kill:
	pkill -9 ex2_srv.out || true

run_server: kill
	gcc ex2_srv.c -o ex2_srv.out
	./ex2_srv.out &

run_client:
	gcc ex2_client.c -o ex2_client.out
	./ex2_client.out $(shell pgrep ex2_srv.out | sort -n | head -n 1) 2 1 2

run_all: run_server run_client
