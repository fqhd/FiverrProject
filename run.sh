g++ main.cpp $(pkg-config --libs glut) $(pkg-config --cflags glut) $(pkg-config --libs gl) $(pkg-config --cflags gl) && ./a.out