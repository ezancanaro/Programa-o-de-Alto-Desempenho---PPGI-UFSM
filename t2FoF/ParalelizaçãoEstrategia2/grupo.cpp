#include "grupo.h"
#include <vector>
#include <stdio.h>

using namespace std;

grupo::grupo(int i, float x, float y, float z) {
    id=i;
    //lista = new vector<int>();
    xmax=xmin= x;
    ymax=ymin= y;
    zmax=zmin= z;
    bounds[0]=x;
    bounds[1]=x;
    bounds[2]=y;
    bounds[3]=y;
    bounds[4]=z;
    bounds[5]=z;
    grouped = false;
}

void grupo::addParticle(int p){
    lista.push_back(p);
    return;
}

void grupo::mergeBounds(float *NewBounds){
	if(bounds[0] < NewBounds[0]){
		//xmax = x;
		bounds[0] = NewBounds[0];
	}
	else if(bounds[1] > NewBounds[1]){
		//xmin = x;
		bounds[1]= NewBounds[1];
	}

	if(bounds[2] < NewBounds[2]){
	//	ymax = y;
		bounds[2] = NewBounds[2];
	}
	else if(bounds[3] > NewBounds[3]){
		//ymin = y;
		bounds[3] = NewBounds[3];
	}
	if(bounds[4] < NewBounds[4]){
	//	zmax = z;
		bounds[4] = NewBounds[4];
	}
	else if(bounds[5] > NewBounds[5]){
	//	zmin = z;
		bounds[5] = NewBounds[5];
	}
	return;

}

void grupo::changeBounds(float x, float y, float z){
	if(bounds[0] < x){
	//	xmax = x;
		bounds[0] = x;
	}
	else if(bounds[1] > x){
	//	xmin = x;
		bounds[1]= x;
	}

	if(bounds[2] < y){
	//	ymax = y;
		bounds[2] = y;
	}
	else if(bounds[3] > y){
		//ymin = y;
		bounds[3] = y;
	}

	if(bounds[4] < z){
		//zmax = z;
		bounds[4]=z;
	}
	else if(bounds[5] > z){
		//zmin = z;
		bounds[5]=z;
	}
	return;

}
float* grupo::getBounds(){

	return bounds;
};

void grupo::printGrupo(){

	printf("ID = %d \n", id);
	printf("X = %f \n", bounds[0]);
	printf("x = %f \n", bounds[1]);
	printf("Y = %f \n", bounds[2]);
	printf("y = %f \n", bounds[3]);
	printf("Z = %f \n ", bounds[4]);
	printf("z = %f \n ", bounds[5]);
	printf("\n");
}

grupo::grupo(const grupo& orig) {
}

grupo::~grupo() {
}