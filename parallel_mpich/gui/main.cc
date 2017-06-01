#include "main.h"
#include <string>
using std::string;
using std::cout;

poly targets;  // these classes are 
Paths stalkers;
grid Grid;  // defined in cp.h

/****************************/
/*glut callbacks functions***/

void init();
void display();
void mouse(int buttom , int state , int x , int y);
void motion_mouse(int x, int y);
void reshape(int width,int hight);
void menuOP(int value);
void read_polygon();
void read_polygon_groups();
void write_polygon();
void read_stalkers();
void draw_next();
void draw_prev();
/*******************************************/
int main(int argc,char** argv){
	/*init*/
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(Grid.WIDTH,Grid.HIGHT);
	glutInitWindowPosition(100,100);
	glutCreateWindow("grid");
	init();
	/*callbacks*/
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(motion_mouse);
	/*Menu*/
	glutCreateMenu(menuOP);
	glutAddMenuEntry("Reset",0);
	glutAddMenuEntry("write targets",1);
	glutAddMenuEntry("read targets",2);
	glutAddMenuEntry("read solution",3);
	glutAddMenuEntry("draw next stalker ",4);
	glutAddMenuEntry("draw prev stalker ",5);
	glutAddMenuEntry("Quit",6);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	/*Mainloop*/
	glutMainLoop();
	return 0;
}

/*******************************************/
void init(){
	glClearColor(1.0,1.0,1.0,0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0,50.0,0.0,50.0,-1.0,1.0);
}

/*******************************************/
void display(){
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0,0.0,1.0);

	/*draws 50x50 grid*/
	grid_drawGrid(Grid);
	/*draw polygon edges*/
	grid_drawPoly(targets);
	grid_drawStalker(stalkers);  
	glFlush();
}

/*******************************************/
void mouse(int buttom , int state , int x , int y){
	if(state==GLUT_DOWN) {
		poly_end_motion(targets);	
		return;
	}
	/*transforming coordinates from viewport
	  size to imagenative grid size*/
	double xd=x,yd=Grid.HIGHT-y;
	float xf=xd/Grid.WIDTH,yf=yd/Grid.HIGHT;
	xd=xf*50; yd=yf*50;
	poly_add(targets,xd,yd);	
	glutPostRedisplay();
}

/*******************************************/
void motion_mouse(int x, int y){
	double xd=x,yd=Grid.HIGHT-y;
	float xf=xd/Grid.WIDTH,yf=yd/Grid.HIGHT;
	xd=xf*50; yd=yf*50;
	poly_motion_mouse(targets,xd,yd);
	glutPostRedisplay();
}

/*******************************************/
void reshape(int width,int hight){
	Grid.WIDTH=width; Grid.HIGHT=hight;
	glViewport(0,0,(GLsizei)width,(GLsizei)hight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	/*setting 50.0 value makes it easier
	  to relate imagenative grid using integer
	  valeus ranging in [0,50] 
	  ie: Vertex2i instead of Vertex2d*/
	glOrtho(0.0,50.0,0.0,50.0,-1.0,1.0);
}

/*******************************************/
void menuOP(int value){
	switch(value){
		case 0:/*reset*/
			stalkers.Reset();
			targets.Reset();/*clears all poly feilds*/
			break;
		case 1:/*write polygon*/
			write_polygon();
			break;
		case 2:/*read polygon*/
			read_polygon();
			break;
		case 3:/*read solution*/
			read_stalkers();
			break;
		case 4:/*draw next stalker*/
			 draw_next();
			 break;
		case 5:/*draw prev stalker*/
			 draw_prev();
			 break;
		case 6:/*quit*/
			exit(1);
	}
}
void write_polygon(){
	ofstream fout("targets.txt");
	poly_write(targets,fout);
}
void read_polygon(){
	ifstream fin("targets.txt");
	poly_read(targets,fin);
}
void read_stalkers(){
	ifstream fin("solution.txt");
	stalkers_read(stalkers,targets,fin);
}
void draw_next(){
     if(stalkers.arr.size()>0){
	  stalkers.index++;
	  stalkers.index%=stalkers.arr.size();
     }
     glutPostRedisplay();
}
void draw_prev(){
     if(stalkers.arr.size()>0){
	  stalkers.index--;
	  if(stalkers.index<0) stalkers.index=0;
     }
     glutPostRedisplay();
}
