#ifndef GEOMAP_H
#define GEOMAP_H
#include <vector>
#include "geoarc.h"
#include "geonode.h"
#include <QFileDialog>
#include <QTextStream>
#include "floyd.h"
#include "ogrsf_frmts.h"
#include "geopoint2.h"


class GeoMap
{
public:
    GeoMap();
    ~GeoMap();
    void drawMap();
    int task=-1;
    double left,top,right,bottom;
    //task1
    std::vector<GeoArc*> arcs;
    std::vector<GeoNode*> nodes;
    void addLine(ifstream &fs);
    void addNode(GeoArc* arc);
    void loadE00File();
    void generateRoute(int start,int end);
    Graph_DG *graph;
    //task 2
    void loadDemandShp();
    void loadSupplyShp();
    void calculateAccessibility();
    std::vector<GeoPoint2> demandPts;
    std::vector<GeoPoint2> supplyPts;
    float maxAccess=0.0;
    float minAccess=0.0;
    int accessCalculated=0;
    //task 3
    void loadPointShp();
    void calculateKde();
    std::vector<GeoPoint2> kdePts;
    int kdeCalculated=0;
    float bandWidth;
    float **loc;
    float maxLoc;
    float minLoc;
};

#endif // GEOMAP_H
