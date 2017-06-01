#ifndef CP
#define CP
#include <vector>
#include <GL/glut.h>
#include <iostream>
#include <fstream>

using std::vector;
using std::cin;
using std::cout;
using std::ifstream;
using std::ofstream;

class poly{
public:
	vector<double> X;
	vector<double> Y; 
	vector<double> Xb; //Xb,Yb are used for partitioning  
	vector<double> Yb; //vertices when a vertex is moved
	vector<int> groups;
	int group_size;
	poly():group_size(1){} 
	void Reset();
};

/*adds vertex (x,y)*/
void poly_add(poly &P, double x, double y);
/*motion of mouse is treated as an attempt 
  to move previously marked vertices */
/*moving of vertices is handled by making 
  two segments of vertices*/
void poly_motion_mouse(poly &P, double x, double y);
/*rejoins the two segments back to one*/
void poly_end_motion(poly &P);
void poly_Reset(poly &P);
/* saves polygon to file */
void poly_write(poly &P,ofstream &fout);
void poly_read(poly &P,ifstream &fin);
void poly_read_groups(poly &P,ifstream &fin);
/**************************************************/
class Paths{
public:
     vector<poly> arr;
     int index;
     void Reset();
};
void stalkers_read(Paths &stalkers,poly &targets,ifstream &fin);
/**************************************************/

class grid{
public:
	int WIDTH; //viewport values
	int HIGHT;
	grid():WIDTH(800),HIGHT(600){}
}; 

void grid_drawPoly( poly &P);
void grid_drawStalker( Paths &stalkers);
void grid_drawGrid( grid &G);

#endif
