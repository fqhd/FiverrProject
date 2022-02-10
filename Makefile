CXX = g++
GLUT = -I/usr/include/GL -lglut
GL = $$(pkg-config --libs gl) $$(pkg-config --cflags gl)

out: main.cpp
	${CXX} main.cpp ${GLUT} ${GL} -o out
