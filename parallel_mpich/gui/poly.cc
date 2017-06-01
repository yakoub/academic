#include "main.h"

inline void push_backXY(poly &p,double x,double y){
	p.X.push_back(x); p.Y.push_back(y);
}

inline void push_backXY(poly &p,double x,double y,int group){
	p.X.push_back(x); p.Y.push_back(y);
	p.groups.push_back(group);
}
inline void pop_backXY(poly &p){
	p.X.pop_back(); p.Y.pop_back();
}
void poly_add(poly &P, double x, double y){
	
	if( !P.Xb.empty() ) poly_end_motion(P);
	else 
	       push_backXY(P,x,y,0);
}
/***************************************************/

static bool lastPoint=false;
void poly_end_motion(poly &P){
	if( !P.Xb.empty() ){
		for(int i=0;i<P.Xb.size();i++)
			push_backXY(P,P.Xb[i],P.Yb[i]);
		P.Xb.clear();
		P.Yb.clear();
	}
	lastPoint=false;
}

void poly_motion_mouse(poly &P, double x, double y){
	if( P.Xb.empty() && !lastPoint ){
		int i=0;
		double xcmp,ycmp;
		for( i=0;i<P.X.size();i++)
		{
			xcmp=P.X[i]; ycmp=P.Y[i];
			if(xcmp-0.2<x && x<xcmp+0.2 &&
	   		   	ycmp-0.2<y && y<ycmp+0.2  )
	   	   		break;
		}
		if( i==P.X.size() ) return ;
		if( i==P.X.size()-1 ) lastPoint=true;
		else	lastPoint=false;
		vector<double> tmpX,tmpY;
		for( int j=0; j < i+1 ; j++){
			tmpX.push_back(P.X[j]);
			tmpY.push_back(P.Y[j]);
		}
		for(i++;i<P.X.size();i++){
			P.Xb.push_back(P.X[i]);
			P.Yb.push_back(P.Y[i]);
		}
		P.X=tmpX; P.Y=tmpY;
	}
	if( !P.X.empty() ){ 
		pop_backXY(P);
	}
	push_backXY(P,x,y);
}
/***************************************************/
void poly::Reset(){
	if(!X.empty() ){
		X.clear();Y.clear();
		groups.clear();
	}
	if(!Xb.empty() ){
		Xb.clear();Yb.clear();
	}
}
void poly_read(poly &P,ifstream &fin){
	P.Reset();
	int size;
	fin>>size;
	double x,y;
	for(int i=0;i<size;i++){
		fin>>x>>y;
		if( fin.eof() ) break;
		push_backXY(P,x,y,0);
	}
}
void poly_write(poly &P,ofstream &fout){
	fout<<(int)P.X.size()<<std::endl;
	for(int i=0;i<P.X.size();i++)
		fout<<P.X[i]<<" "<<P.Y[i]<<std::endl;
	fout.flush();
}

void poly_read_groups(poly &P,ifstream &fin){
	P.Reset();
	int size,group;
	fin>>size>>group;
	P.group_size=group;
	double x,y;
	for(int i=0;i<size;i++){
		fin>>x>>y>>group;
		if( fin.eof() ) break;
		push_backXY(P,x,y,group);
	}
}

/***************************************************/
void stalkers_read(Paths &stalkers,poly &targets,ifstream &fin){
     stalkers.Reset();
     poly path;
     int target;
     char c;

     fin>>target>>c;
     if(c=='-'){
	  targets.groups.clear();
	  while(c!='#'){
	       targets.groups.push_back(target);
	       fin>>target>>c;
	  }
	  targets.groups.push_back(target);
	  fin>>target>>c;
     }
     while( !fin.eof() ){
	  path.Reset();
	  while(c!='#'){
	       if(target > targets.X.size() ){
		    cout<<"fail"<<target<<" "<<targets.X.size()<<std::endl;
		    exit(1);
	       }
	       push_backXY(path,targets.X[target],targets.Y[target]);
	       fin>>target>>c;
	  }
	  stalkers.arr.push_back(path);
	  c=',';
	  fin>>target>>c;
     }
}

void Paths::Reset(){
	if(!arr.empty() ){
		arr.clear();
	}
	index=0;
}
