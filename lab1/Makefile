CC = g++
files = backtrack.cc
bin = backtrack
flags = -lm -lraylib

run: clean default
	./$(bin)
clean:
	rm -rf $(bin)
default:
	$(CC) $(files) -o $(bin) $(flags)

