# File: Makefile
# Author: Keith Schwarz (htiek@cs.stanford.edu)
#
# A Makefile for building the CS106L GraphViz program.

# Disable optimization; turn on debugging.  Feel free to change this to
#
# CCFLAGS = -03
#
# If you want to turn on optimization once things get working.
CCFLAGS = -g -O0

# Builds the main program with the necessary libraries.
graphviz: GraphVisualizer.o main.o
	g++ GraphVisualizer.o main.o -o graphviz -framework OpenGL -framework GLUT $(CCFLAGS)

# Build object files from sources.
%.o: %.cpp
	g++ $^ -c -o $@ $(CCFLAGS)

# Cleans the project by nuking emacs temporary files (*~), object files (*.o),
# and the resulting executable.
clean:
	rm -rf *~ *.o graphviz