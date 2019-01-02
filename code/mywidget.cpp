#include "mywidget.h"

MyWidget::MyWidget()
{
    map = nullptr;
}

MyWidget::~MyWidget()
{
    delete map;
}

void MyWidget::initializeGL(){
    glClearColor(0.16f,0.16f,0.16f,0.0f);
}

void MyWidget::resizeGL(int width,int height){
    glViewport(0,0,width,height);
}

void MyWidget::paintGL(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(offsetX,offsetY,0);
    glScalef(scale,scale,0);
    //task 1
    if(this->map!=nullptr && this->map->task==1)
    {
        if(this->map!=nullptr)
        {
           for(int i=0;i<map->arcs.size();i++)
           {
               glLineWidth(1.0);
                glBegin(GL_LINE_STRIP);
                for(int j=0;j<map->arcs[i]->pts.size();j++)
                {
                    glColor3d(0,0.7,0.9);
                    glVertex2f(xy2screen(map->arcs[i]->pts[j]).x,xy2screen(map->arcs[i]->pts[j]).y);
                }
                glEnd();
            }
        }
        if(this->map!=nullptr&&this->map->graph!=nullptr)
        {
          for(int i=0;i<map->graph->routeId.size()-1;i++)
           {
                for(int j=0;j<map->arcs.size();j++)
                {
                     if((map->arcs[j]->fNodeId==map->graph->routeId[i]&&map->arcs[j]->tNodeId==map->graph->routeId[i+1])||
                             (map->arcs[j]->tNodeId==map->graph->routeId[i]&&map->arcs[j]->fNodeId==map->graph->routeId[i+1]))
                     {
                          //draw
                          glLineWidth(2.0);
                          glBegin(GL_LINE_STRIP);
                          for(int k=0;k<map->arcs[j]->pts.size();k++)
                          {
                              glColor3d(1,1,0);
                              glVertex2f(xy2screen(map->arcs[j]->pts[k]).x,xy2screen(map->arcs[j]->pts[k]).y);
                          }
                          glEnd();
                     }
                }
            }
        }
    }
    //task 2
    if(this->map!=nullptr && this->map->task==2)
    {
        if(this->map->accessCalculated==0)
        {
            glPointSize(5.0f);
            glBegin(GL_POINTS);
            for(int i=0;i<map->demandPts.size();i++)
            {

                 double color =map->demandPts[i].amount/5.0 ;
                 glColor3d(0,0,color);
                 glVertex2f(xy2screen2(map->demandPts[i]).x,xy2screen2(map->demandPts[i]).y);
             }
             glEnd();

             glPointSize(5.0f);
             glBegin(GL_POINTS);
             for(int i=0;i<map->supplyPts.size();i++)
             {

                  double color =map->supplyPts[i].amount/5.0 ;
                  glColor3d(color,color,0);
                  glVertex2f(xy2screen2(map->supplyPts[i]).x,xy2screen2(map->supplyPts[i]).y);
              }
              glEnd();
        }
        if(this->map->accessCalculated==1)
        {

            float scale =(this->map->right-this->map->left)*0.008;
            for(int i=0;i<this->map->demandPts.size();i++)
              {
                  glBegin(GL_POLYGON);
                  double color = (this->map->demandPts[i].k-this->map->minAccess)/(this->map->maxAccess-this->map->minAccess);
                  glColor3f(0.0,color+0.25,0.0);
                  for(int j=0;j<3600;j++)
                  {
                     GeoPoint2 pt;
                     pt.x=this->map->demandPts[i].x + scale*cos(2*3.14*j/3600);
                     pt.y=this->map->demandPts[i].y + scale*sin(2*3.14*j/3600);
                     glVertex2f(xy2screen2(pt).x,xy2screen2(pt).y);
                  }
                  glEnd();
              }
        }
    }
    //task 3
    if(this->map!=nullptr && this->map->task==3)
    {
        if(this->map->kdeCalculated==0)
        {
            glPointSize(5.0f);
            glBegin(GL_POINTS);
            for(int i=0;i<map->kdePts.size();i++)
            {
                 glColor3d(0,0,0.8);
                 glVertex2f(xy2screen2(map->kdePts[i]).x,xy2screen2(map->kdePts[i]).y);
             }
             glEnd();
        }
        if(this->map->kdeCalculated==1)
        {
              for(int i=0;i<1000;i++)
              {
                  for(int j=0;j<1000;j++)
                  {
                      float a=(this->map->loc[i][j]-this->map->minLoc)/(this->map->maxLoc-this->map->minLoc);
                      glColor3f(0.16f,0.16f,0.16f+a);
                      float x,y;
                      glBegin(GL_POLYGON);
                      x=-1.0+0.002*i;
                      y=-1.0+0.002*j;
                      glVertex2f(x,y);
                      glVertex2f(x+0.002,y);
                      glVertex2f(x+0.002,y+0.002);
                      glVertex2f(x,y+0.002);
                      glEnd();
                  }
              }
        }
    }
    glFlush();
}

GeoPoint MyWidget::xy2screen(GeoPoint pt){
    // transfer point to screen coordinate
    double width=this->map->right-this->map->left;
    double height=this->map->top-this->map->bottom;
    GeoPoint screen;
    screen.x=(pt.x-this->map->left)/width*2-1;
    screen.y=(pt.y-this->map->bottom)/height*2-1;
    return screen;
}

GeoPoint2 MyWidget::xy2screen2(GeoPoint2 pt){
    // transfer point to screen coordinate
    double width=this->map->right-this->map->left;
    double height=this->map->top-this->map->bottom;
    GeoPoint2 screen;
    screen.x=(pt.x-this->map->left)/width*2-1;
    screen.y=(pt.y-this->map->bottom)/height*2-1;
    return screen;
}


void MyWidget::mousePressEvent(QMouseEvent *event){
    if (event->buttons() == Qt::LeftButton)
    {
        // 切换光标样式
        setCursor(Qt::OpenHandCursor);
        mouseX = event->x();
        mouseY = event->y();
    }
    if (event->buttons() == Qt::RightButton)
    {
        offsetX = 0;
        offsetY = 0;
        update();
    }
}

void MyWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        // 计算图像偏移量
        newOffsetX = (event->x() - mouseX) / this->width();
        newOffsetY = (mouseY - event->y()) / this->height();
        offsetX += newOffsetX;
        offsetY += newOffsetY;
        update();
    }
}

void MyWidget::mouseReleaseEvent(QMouseEvent *event)
{
    unsetCursor();
    offsetX += newOffsetX;
    offsetY += newOffsetY;
    newOffsetX = 0;
    newOffsetY = 0;
}

void MyWidget::wheelEvent(QWheelEvent *event)
{
    scale -= 0.01 * event->delta();
    // 限制缩小倍数
    if (scale < 0.9f)
    {
        scale = 0.9f;
    }
    update();
}

