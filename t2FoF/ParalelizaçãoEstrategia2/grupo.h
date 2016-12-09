#ifndef GRUPO_H
#define	GRUPO_H
#include <vector>

using namespace std;

class grupo {
    int id;


public:
    vector<int> lista;
    bool grouped;
    float xmax,xmin,ymax,ymin,zmax,zmin;
    float bounds[6];
    int getI_gru() {
        return id;
    }
    grupo(int i, float x, float y, float z);
    void addParticle(int p);
    void mergeBounds(float *NewBounds);
    void changeBounds(float x, float y, float z);
    void printGrupo();
    float* getBounds();
    grupo(const grupo& orig);
    virtual ~grupo();
private:
};

#endif	/* GRUPO_H */