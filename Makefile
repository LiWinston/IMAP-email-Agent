EXE=fetchmail

$(EXE): src/main.c
	gcc -Wall -o $(EXE) $<
run: $(EXE)
	./$(EXE)
memcheck: $(EXE)
	valgrind ./$(EXE)
clean:
	rm -f $(EXE)
format:
	clang-format -style=file -i *.c
