PROGRAM = main

$(PROGRAM): main.c
	gcc -o $(PROGRAM)  $^ 

debug: main.c
	gcc -o $(PROGRAM) -g -Wall $^ -D DEBUG

.PHONY: clean

clean:
	-rm -rf $(PROGRAM)
