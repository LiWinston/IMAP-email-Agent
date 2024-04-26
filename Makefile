EXE=fetchmail

$(EXE): main.c
	cc -Wall -o $(EXE) $<
run: $(EXE)
	./$(EXE)
memcheck: $(EXE)
	valgrind ./$(EXE)
clean:
	rm -f $(EXE)
format:
	clang-format -style=file -i *.c
