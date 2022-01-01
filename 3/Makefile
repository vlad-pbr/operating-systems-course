
run: compile
	for INPUT in $(shell ls ./examples); do \
		echo "\n$$INPUT"; \
		./main.out examples/$$INPUT; \
	done

compile:
	gcc main.c -o main.out