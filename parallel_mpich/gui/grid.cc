#include "main.h"


/*******************************************/
void grid_drawPoints(poly &P);
void grid_drawLines(poly &P);
void grid_drawPoly( poly &P){
	if( P.X.empty() ) return ;
	//grid_drawLines(P);
	grid_drawPoints(P);
}
/*******************************************/
void grid_drawGrid( grid &G){
	glLineWidth(1);
	glColor3f(0.0,1.0,0.0);
	glBegin(GL_LINES);
		for(int i=0;i<50;i++){
			glVertex2i(i,0);
			glVertex2i(i,50);
		}
	glEnd();
	glBegin(GL_LINES);
		for(int i=0;i<50;i++){
			glVertex2i(0,i);
			glVertex2i(50,i);
		}
	glEnd();
}

/*******************************************/
void grid_drawPoints(poly &P){
     float c=0.0,d=1/(float)P.X.size();
     glPointSize(8);
     char t_id[5]; 
     for( int i=0 ; i<P.X.size() ; i++ ){
	  if(!P.groups.empty() ){
	       c= (P.groups[i]) / (float)P.group_size ;
	      /* printf("%d %d %.2f\n",i,P.groups[i],c);*/
	  }else{
	       c+=d;
	  }
	  if(P.groups[i] %2 ==0) glColor3f(1-c,1-c,c); 
	  else glColor3f(c,1-c,c); 
	  
	  glBegin(GL_POINTS);
	       glVertex2d( P.X[i] , P.Y[i] );
	  glEnd();
	  glColor3f(0.0,0.5,0.5);
	  glRasterPos2f(P.X[i] , P.Y[i] );
	  sprintf(t_id,"%d%c",P.groups[i],'\0'); 
	  for(int j=0;j<5 && t_id[j]!='\0';j++)
	       glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18 ,t_id[j] );
     }
      c=0.0;
      for( int i=0 ; i<P.Xb.size() ; i++ ){
	      glColor3f(1-c,c,0.0); c+=d;
	      glBegin(GL_POINTS);
		      glVertex2d( P.Xb[i] , P.Yb[i] );
	      glEnd();
      }
}
/*******************************************/
void grid_drawStalker( Paths &stalkers){
if(stalkers.arr.empty() ) return ;
     poly &P=stalkers.arr[stalkers.index];
     float c=0.0,d=1/(float)P.X.size();
     glColor3f(0.0,c,c);
     glPointSize(8);
     glBegin(GL_POINTS);
	  glVertex2d( P.X[0] , P.Y[0] );
     glEnd();
     glLineWidth(2);
     int i; 
     for( i=0 ; i<P.X.size()-1 ; i++ ){
	  glColor3f(0.0,c,c); c+=d;
	  glBegin(GL_LINES);
	       glVertex2d( P.X[i] , P.Y[i] );
	       glVertex2d( P.X[i+1] , P.Y[i+1] );
	  glEnd();
	  if(P.X[i]>50 || P.Y[i] >50 )
	       printf("i %d %.2f %.2f\n",P.X[i],P.Y[i]);
  
     }
     glColor3f(0.0,c,c); 
     glBegin(GL_POINTS);
	  glVertex2d( P.X[i] , P.Y[i] );
     glEnd();

     char t_id[16]; 
     glColor3f(0.0,0.0,1.0);
     glRasterPos2f(0.0,0.0 );
     sprintf(t_id,"stalker %d%c",stalkers.index,'\0'); 
     for(int j=0;j<15 && t_id[j]!='\0';j++)
	  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18 ,t_id[j] );
}
