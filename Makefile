CXX = clang++
GLUT = $$(pkg-config --libs glut) $$(pkg-config --cflags glut)
GL = $$(pkg-config --libs gl) $$(pkg-config --cflags gl)

out: main.cpp
	${CXX} main.cpp ${GLUT} ${GL} -o out
