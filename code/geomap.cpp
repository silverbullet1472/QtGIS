#include "geomap.h"
#include <iostream>
#include <fstream>


GeoMap::GeoMap()
{
    this->graph=nullptr;
}

GeoMap::~GeoMap()
{
    if(graph!=nullptr) delete this->graph;
    if(arcs.size()!=0)
    for(auto arc:arcs)
    {
        delete arc;
    }
    if(nodes.size()!=0)
    for(auto node:nodes)
    {
        delete node;
    }
    if(kdeCalculated==1)
    {
        for(int i=0;i<1000;i++)
        {
            delete[] loc[i];
            loc[i]=nullptr;
        }
        delete[] loc;
        loc=nullptr;
    }
}

void GeoMap::loadE00File()
{
    QFileDialog *fileDialog = new QFileDialog(nullptr,"open E00 File!","D:\\MyCplusProject\\TinyGIS\\data","*.e00");
    QString filePath;
    if(fileDialog->exec())
    {
        filePath = fileDialog->selectedFiles()[0];
    }
    string line;
    try
     {
        ifstream fs(filePath.toStdString(),ios::in);
        while(line!="EOS")
        {
            if(line=="ARC  2")
            {
                addLine(fs);
            }
            if(strncmp(line.c_str(),"YMAX",4) == 0)
            {
                 fs >> this->left >> this->bottom >> this->right >>this->top;
            }
            getline(fs, line);
        }
        fs.close();
     }
     catch (exception e)
     {
     }
}

void GeoMap::generateRoute(int startNode,int endNode)
{
    this->graph=new Graph_DG((int)this->nodes.size(),(int)this->arcs.size());
    int start;
    int end;
    double distance;
    int count = 0;
    for(int i=0;i<this->arcs.size();i++)
    {
        start=arcs[i]->fNodeId;
        end=arcs[i]->tNodeId;
        distance=arcs[i]->calculateDistance();
        graph->arc[start - 1][end - 1] = distance;
        graph->arc[end - 1][start - 1] = distance;
        ++count;
    }
    this->graph->Floyd();
    this->graph->printMatrix();
    this->graph->generateRoute(startNode,endNode);
    this->graph->printRoute();
}

void GeoMap::addLine(ifstream &fs)
{
    double x,y;
    int arcId;
    int fNodeId;
    int tNodeId;
    int numOfPoint;
    int go;
    while(true)
    {
        fs>>arcId;
        fs>>go>>fNodeId;
        fs>>tNodeId;
        fs>>go>>go>>numOfPoint;
        if(!numOfPoint)
        {
            break;
        }
        GeoArc *arc=new GeoArc(arcId,numOfPoint,fNodeId,tNodeId);
        for (int i = 0; i < numOfPoint; i++)
        {
            fs >> x >> y;
            GeoPoint pt(x,y);
            arc->pts.push_back(pt);            
        }
        this->arcs.push_back(arc);    
        //处理结点的存储
        addNode(arc);
    }
}

void GeoMap::addNode(GeoArc* arc)
{
    int isfNodeIn=0;//判断结点是否已经在数组中
    int istNodeIn=0;
    for(auto node:nodes)
    {
        if(arc->fNodeId==node->nodeId) isfNodeIn=1;
        if(arc->tNodeId==node->nodeId) istNodeIn=1;
    }
    if(isfNodeIn==0)
    {
        GeoNode *node=new GeoNode(arc->fNodeId,arc->pts[0].x,arc->pts[0].y);
        nodes.push_back(node);
    }
    if(istNodeIn==0)
    {
        GeoNode *node=new GeoNode(arc->tNodeId,arc->pts[arc->ptSum-1].x,arc->pts[arc->ptSum-1].y);
        nodes.push_back(node);
    }
    int fNode;int tNode;
    //取得邻接结点在数组中的索引
    int i=0;
    for(auto node:nodes)
    {
        if(arc->fNodeId==node->nodeId) fNode=i;
        if(arc->tNodeId==node->nodeId) tNode=i;
        i++;
    }
    //判断t结点是否已经与f结点邻接,若无则添加
    int istNodeInfNode=0;
    for(int i=0;i<nodes[fNode]->adjacentNodeId.size();i++)
    {
        if(nodes[fNode]->adjacentNodeId[i]==nodes[tNode]->nodeId) istNodeInfNode=1;
    }
    if(istNodeInfNode==0)
    {
        nodes[fNode]->adjacentNodeId.push_back(nodes[tNode]->nodeId);    
    }
    //判断f结点是否已经与t结点邻接，若无则添加
    int isfNodeIntNode=0;
    for(int i=0;i<nodes[tNode]->adjacentNodeId.size();i++)
    {
        if(nodes[tNode]->adjacentNodeId[i]==nodes[fNode]->nodeId) isfNodeIntNode=1;
    }
    if(isfNodeIntNode==0)
    {
        nodes[tNode]->adjacentNodeId.push_back(nodes[fNode]->nodeId);
    }
}

void GeoMap::loadDemandShp()
{
    GDALAllRegister();
    QFileDialog *fileDialog = new QFileDialog(nullptr,"open Demand ShpFile!","D:\\MyCplusProject\\TinyGIS\\data","*.shp");
    QString filePath;
    if(fileDialog->exec())
    {
        filePath = fileDialog->selectedFiles()[0];
    }
    else return;
    std::string str = filePath.toStdString();
    const char* path = str.c_str();
    GDALDataset *poDS;
    poDS = (GDALDataset*)GDALOpenEx(path, GDAL_OF_VECTOR,nullptr,nullptr,nullptr);
    if (poDS == nullptr) return;
    OGRLayer  *poLayer;
    poLayer = poDS->GetLayerByName("demand");

    OGREnvelope evlop;
    poLayer->GetExtent(&evlop);
    this->bottom=evlop.MinY;
    this->top=evlop.MaxY;
    this->right=evlop.MaxX;
    this->left=evlop.MinX;
    for (auto& poFeature : poLayer)
    {
        GeoPoint2 pt;
        for (auto&& oField : *poFeature)
        {
            switch (oField.GetType())
            {
            case OFTInteger:
                if(strcmp(oField.GetName(),"amount")==0)
                {
                     pt.amount=oField.GetInteger();
                     break;
                }
                else if(strcmp(oField.GetName(),"Id")==0)
                {
                    pt.id=oField.GetInteger();
                    break;
                }
                break;
            }
        }
        const OGRGeometry *poGeometry = poFeature->GetGeometryRef();
        if (poGeometry != nullptr
            && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint)
        {
            const OGRPoint *poPoint = poGeometry->toPoint();
            pt.x=poPoint->getX();
            pt.y=poPoint->getY();
        }
        cout<<"id:"<<pt.id<<"amout:"<<pt.amount<<"x:"<<pt.x<<"y:"<<pt.y<<endl;
        demandPts.push_back(pt);
    }
    cout<<"demandpts read!"<<endl;
}

void GeoMap::loadSupplyShp()
{
    GDALAllRegister();
    QFileDialog *fileDialog = new QFileDialog(nullptr,"open Supply ShpFile!","D:\\MyCplusProject\\TinyGIS\\data","*.shp");
    QString filePath;
    if(fileDialog->exec())
    {
        filePath = fileDialog->selectedFiles()[0];
    }
    else return;
    std::string str = filePath.toStdString();
    const char* path = str.c_str();
    GDALDataset *poDS;
    poDS = (GDALDataset*)GDALOpenEx(path, GDAL_OF_VECTOR,nullptr,nullptr,nullptr);
    if (poDS == nullptr) return;
    OGRLayer  *poLayer;
    poLayer = poDS->GetLayerByName("supply");

    OGREnvelope evlop;
    poLayer->GetExtent(&evlop);
    if(this->bottom>evlop.MinY) this->bottom=evlop.MinY;
    if(this->top<evlop.MaxY) this->top=evlop.MaxY;
    if(this->right<evlop.MaxX) this->right=evlop.MaxX;
    if(this->left>evlop.MinX) this->left=evlop.MinX;
    for (auto& poFeature : poLayer)
    {
        GeoPoint2 pt;
        for (auto&& oField : *poFeature)
        {
            switch (oField.GetType())
            {
            case OFTInteger:
                if(strcmp(oField.GetName(),"amount")==0)
                {
                     pt.amount=oField.GetInteger();
                     break;
                }
                else if(strcmp(oField.GetName(),"Id")==0)
                {
                    pt.id=oField.GetInteger();
                    break;
                }
                break;
            }
        }
        const OGRGeometry *poGeometry = poFeature->GetGeometryRef();
        if (poGeometry != nullptr
            && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint)
        {
            const OGRPoint *poPoint = poGeometry->toPoint();
            pt.x=poPoint->getX();
            pt.y=poPoint->getY();
        }
        cout<<"id:"<<pt.id<<"amout:"<<pt.amount<<"x:"<<pt.x<<"y:"<<pt.y<<endl;
        supplyPts.push_back(pt);
    }
    cout<<"supply read!"<<endl;
}

void GeoMap::calculateAccessibility(){
    //计算服务半径
    float height = this->top-this->bottom;
    float width = this->right-this->left;
    float r = min(height, width)/15;
    //计算供需比
    int sizeS = this->supplyPts.size();
    int sizeD = this->demandPts.size();
    float temp;
    float dis;
    for(int i=0;i<sizeS;i++)
    {
        temp=0;
        for(int j=0;j<sizeD;j++)
        {
            GeoPoint2 pt1=this->supplyPts[i];
            GeoPoint2 pt2=this->demandPts[j];
            dis=sqrt(pow(pt2.x-pt1.x,2)+pow(pt2.y-pt1.y,2));
            if(dis<r)
                temp+=demandPts[j].amount;
        }
        this->supplyPts[i].k = supplyPts[i].amount/temp;
    }
    //计算可达性
    for(int i=0;i<sizeD;i++)
    {
        temp=0;
        for(int j=0;j<sizeS;j++)
        {
            GeoPoint2 pt2=this->demandPts[i];
            GeoPoint2 pt1=this->supplyPts[j];
            dis=sqrt(pow(pt2.x-pt1.x,2)+pow(pt2.y-pt1.y,2));
            if(dis<r)
            {
                temp+=(supplyPts[j].k) * exp(-0.5*pow(dis/r,2))/(1-exp(-0.5));
            }
        }
        if(temp>this->maxAccess)
            maxAccess=temp;
        if(temp<this->minAccess)
            minAccess=temp;
        this->demandPts[i].k = temp;
    }
    this->accessCalculated=1;
    for(int i=0;i<sizeD;i++)
    {
        cout<<"id:"<<demandPts[i].id<<"amout:"<<demandPts[i].amount<<"k:"<<demandPts[i].k<<"x:"<<demandPts[i].x<<"y:"<<demandPts[i].y<<endl;
    }
}

void GeoMap::loadPointShp()
{
    GDALAllRegister();
    QFileDialog *fileDialog = new QFileDialog(nullptr,"open Supply ShpFile!","D:\\MyCplusProject\\TinyGIS\\data","*.shp");
    QString filePath;
    if(fileDialog->exec())
    {
        filePath = fileDialog->selectedFiles()[0];
    }
    else return;
    std::string str = filePath.toStdString();
    const char* path = str.c_str();
    GDALDataset *poDS;
    poDS = (GDALDataset*)GDALOpenEx(path, GDAL_OF_VECTOR,nullptr,nullptr,nullptr);
    if (poDS == nullptr) return;
    OGRLayer  *poLayer;
    poLayer = poDS->GetLayerByName("supply");

    OGREnvelope evlop;
    poLayer->GetExtent(&evlop);
    this->bottom=evlop.MinY;
    this->top=evlop.MaxY;
    this->right=evlop.MaxX;
    this->left=evlop.MinX;
    for (auto& poFeature : poLayer)
    {
        GeoPoint2 pt;
        const OGRGeometry *poGeometry = poFeature->GetGeometryRef();
        if (poGeometry != nullptr
            && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint)
        {
            const OGRPoint *poPoint = poGeometry->toPoint();
            pt.x=poPoint->getX();
            pt.y=poPoint->getY();
        }
        kdePts.push_back(pt);
    }
    cout<<"point read!"<<endl;
}

void GeoMap::calculateKde(){
    //bandWidth
    vector<double> Distance;
    float Avex=0,Avey=0;
    for(int i=0;i<this->kdePts.size();i++)
    {
        Avex+=kdePts[i].x;
        Avey+=kdePts[i].y;
    }
    Avex/=this->kdePts.size();
    Avey/=this->kdePts.size();
    for(int i=0;i<this->kdePts.size();i++)
    {
        float Dis=sqrt(pow(kdePts[i].x-Avex,2)+pow(kdePts[i].y-Avey,2));
        Distance.push_back(Dis);
    }
    sort(Distance.begin(),Distance.end());
    float Dm=Distance.at(Distance.size()/2);
    double SDx=0,SDy=0;
    for(int i=0;i<this->kdePts.size();i++)
    {
        SDx+=pow(kdePts[i].x-Avex,2);
        SDy+=pow(kdePts[i].y-Avey,2);
    }
    double SD=sqrt(SDx/this->kdePts.size()+SDy/this->kdePts.size());
    if(SD>(sqrt(1/log(2))*Dm))
        this->bandWidth=0.9*(sqrt(1/log(2))*Dm)*pow(this->kdePts.size(),-0.2);
    else
        this->bandWidth=0.9*SD*pow(this->kdePts.size(),-0.2);
    //generate raster
    loc=new float*[1000];
    for(int i=0;i<1000;i++)
    {
        loc[i]=new float[1000];
    }
    //calculateKde
    float temp;
    float dis2;
    float width=(this->right-this->left)/1000;
    float height=(this->top-this->bottom)/1000;
    for(int i=0;i<1000;i++)
    {
       float x,y;
       x=this->left+width*i+width/2;
       for(int j=0;j<1000;j++)
       {
           temp=0;
           y=this->bottom+height*j+height/2;
           for(int m=0;m<this->kdePts.size();m++)
           {
               dis2=pow(x-this->kdePts[m].x,2)+pow(y-this->kdePts[m].y,2);
               if(dis2<(this->bandWidth*this->bandWidth))
               {
                   temp+=3/3.1415926*pow((1-dis2/pow(this->bandWidth,2)),2);
               }
           }
           temp=temp/this->kdePts.size()/pow(this->bandWidth,2);
           loc[i][j]=temp;
           if(i==0&&j==0)
           {
               maxLoc=minLoc=temp;
           }
           else if(temp>maxLoc)
               maxLoc=temp;
           else if(temp<minLoc)
               minLoc=temp;
       }
    }
    this->kdeCalculated=1;
}
