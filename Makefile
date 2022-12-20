all:
	mkdir out/
	gcc src/servTCPContTh2.c -l sqlite3 -o out/servTcp
	gcc src/clipTCPIt.c -o out/cliTcp
clean:
	rm -rf out
