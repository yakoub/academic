run:main.cc main.h grid.o poly.o
	g++ -o vw main.cc grid.o poly.o -lGL -L/usr/X11/lib/ -lglut
grid.o:grid.cc main.h
	g++ -c grid.cc
poly.o:poly.cc main.h
	g++ -c poly.cc
debug:
	g++ -g -o vw main.cc grid.cc poly.cc -lGL -L/usr/X11/lib/ -lglut
