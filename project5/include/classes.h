#ifndef CLASSES_H
#define CLASSES_H

#include<iostream>
#include<cmath>
#include<stack>
#include<vector>
#include<algorithm>
#include<windows.h>

using namespace std ;
struct Point
{
    int x ;
    int y ;
    Point(int a=0, int b=0): x(a),y(b)
    {

    }
    Point operator- (const Point & p)
    {
        Point tmp ;
        tmp.x = x-p.x ;
        tmp.y = y-p.y ;
        return tmp ;
    }
};
struct doublePoint
{
    double x ;
    double y ;
    doublePoint(Point p = {0,0})
    {
        x= (double) p.x ;
        y= (double) p.y ;
    }
    doublePoint operator= (const Point &p)
    {
        x= (double) p.x ;
        y= (double) p.y ;
        return *this ;
    }
} ;
struct drawing
{
    int type ;
    vector<Point> points ;
    COLORREF color;
};
struct Vertex
{
    int x,y;
    Vertex(int x,int y):x(x),y(y) { }
};

struct edgeRecord
{
    double x ;
    double mInv;
    int ymax ;
    bool operator < (const edgeRecord &e)
    {
        return (this->x < e.x );
    }
};
typedef bool (*IsInFunc) (Point& v,int edge);
typedef Point (*IntersectFunc) (Point& v1,Point& v2,int edge);

union OutCode
{
    unsigned All:4;
    struct
    {
        unsigned left:1,top:1,right:1,bottom:1;
    };
};


class polygonFiller
{
public :
    void drawHorizontalLine(HDC hdc,int xs,int xe,int y,COLORREF c) ;
    int Round(double x) ;
    virtual void fillPolygon(HDC hdc,vector<Point> p,COLORREF c) = 0 ;
    virtual ~ polygonFiller() {}
};
class convexFiller : public polygonFiller
{
public :
    void fillPolygon(HDC hdc,vector<Point> p,COLORREF c) ;
};
class generalPolygonFiller : public polygonFiller
{
public :
    void fillPolygon(HDC hdc,vector<Point> p,COLORREF c) ;
};

class shapeFiller
{
public :
    virtual void fillShape (HDC,int,int,COLORREF,COLORREF ) = 0 ;
    virtual ~ shapeFiller() {}
};
class recursiveFill : public shapeFiller
{
public :
    void fillShape(HDC hdc,int x,int y,COLORREF Cb,COLORREF Cf) ;
};

class nonRecursiveFill : public shapeFiller
{
public :
    void fillShape(HDC hdc,int x,int y,COLORREF Cb,COLORREF Cf) ;
};
class Drawer
{
protected:
    int round(double x) ;
//    void Draw8Points(HDC hdc,int xc,int yc, int a, int b,COLORREF color, int quarter=-1) ;
public :
    virtual void draw (HDC,vector<Point>,COLORREF) = 0;
    virtual ~Drawer () {}
};
class circularDrawer : public Drawer
{
protected:
    int getDistance(Point p1, Point p2) ;
public:
    void DrawPoints(HDC hdc,int xc,int yc, int a, int b,COLORREF color) ;
};
class ellipseDrawer : public circularDrawer
{
protected:
    void DrawPoints(HDC hdc,int xc,int yc, int a, int b,COLORREF color) ;
};
class circleDrawer : public circularDrawer
{
protected:
    void DrawPoints(HDC hdc,int xc,int yc, int a, int b,COLORREF color) ;
};

class ellipseDirect : public ellipseDrawer
{
public :
    void draw(HDC hdc,vector<Point> p,COLORREF c) ;
};
class ellipsePolar : public ellipseDrawer
{
public :
    void draw(HDC hdc,vector<Point> p,COLORREF c) ;
};
class ellipseMidPoint : public ellipseDrawer
{
public :
    void draw(HDC hdc,vector<Point> p,COLORREF c) ;
};


class circleDirect : public circleDrawer
{
public :
    void draw(HDC hdc,vector<Point> p,COLORREF c) ;
};
class circlePolar : public circleDrawer
{
public :
    void draw(HDC hdc,vector<Point> p,COLORREF c) ;
};
class circleIterativePolar : public circleDrawer
{
public :
    void draw(HDC hdc,vector<Point> p,COLORREF c) ;
};
class circleMidPoint : public circleDrawer
{
public :
    void draw(HDC hdc,vector<Point> p,COLORREF c) ;
};
class circleModifiedMidPoint : public circleDrawer
{
public :
    void draw(HDC hdc,vector<Point> p,COLORREF c) ;
};

class lineDDA : public Drawer
{
public :
    void draw(HDC hdc, vector<Point> p,COLORREF c) ;
};
class lineParametric : public Drawer
{
public :
    void draw(HDC hdc,vector<Point>p,COLORREF c);
};
class lineMidPoint :public Drawer
{
public :
    void draw(HDC hdc,vector<Point> p,COLORREF c) ;
};



class circleFilledCircles : public circularDrawer
{
protected:
    int quarter ;
    void drawQuarter (HDC hdc,int xc,int yc,int R,COLORREF c) ;
    void DrawPoints(HDC hdc,int xc,int yc, int a, int b,COLORREF color) ;

public :
    circleFilledCircles(int x) : quarter(x) { }
    void draw(HDC hdc,vector<Point> p,COLORREF c) ;
};
class circleFilledLines : public circularDrawer
{
private :
    int quarter ;
    lineDDA lineDrawer ;
public :
    circleFilledLines(int x) :quarter(x) { }
    void draw(HDC hdc,vector<Point> p,COLORREF c) ;
    void DrawPoints(HDC hdc,int xc,int yc, int a, int b,COLORREF color) ;
};

class hermiteUser : public Drawer
{
protected:
    void hermite (HDC hdc, Point p0, Point t0, Point p1, Point t1, COLORREF c ) ;
    int  getDistance(Point p1, Point p2)
    {
        int xdif = (p1.x-p2.x) ;
        int ydif = (p1.y-p2.y) ;

        return round( sqrt(xdif*xdif + ydif*ydif) );
    }

};

class splineDrawer : public hermiteUser
{
public :
    void draw (HDC hdc, vector<Point> p, COLORREF c) ;
};

class squareWithHermite : public hermiteUser
{
private :
    int xleft, xright ;
public :
    void draw(HDC hdc,vector<Point> points,COLORREF c) ;
};
class rectangleWithBezier : public hermiteUser
{
private :
    int ytop, ybottom ;
    void bezier(HDC hdc, vector <Point> p, COLORREF c) ;
public :
    void draw(HDC hdc,vector<Point> points,COLORREF c) ;

};
class clipper : public Drawer
{
protected:
    int xleft, xright, top, bottom ;
public:
    clipper(int a,int b, int c,int d): xleft(a), xright(b), top(c), bottom(d)
    {

    }
};
class pointClipper : public clipper
{
public :
    pointClipper(int a,int b, int c,int d): clipper(a,b,c,d)
    {

    }
    virtual void draw (HDC hdc,vector<Point>p,COLORREF color)
    {
        if(p[0].x>=xleft && p[0].x<= xright && p[0].y>=top && p[0].y<=bottom)
            SetPixel(hdc,p[0].x,p[0].y,color);
    }
};
class lineClipper : public clipper
{
protected:
    OutCode GetOutCode(double x,double y)
    {
        OutCode out;
        out.All=0;
        if(x<xleft)out.left=1;
        else if(x>xright)out.right=1;
        if(y<top)out.top=1;
        else if(y>bottom)out.bottom=1;
        return out;
    }
    void VIntersect(double xs,double ys,double xe,double ye,int x,double *xi,double *yi)
    {
        *xi=x;
        *yi=ys+(x-xs)*(ye-ys)/(xe-xs);
    }
    void HIntersect(double xs,double ys,double xe,double ye,int y,double *xi,double *yi)
    {
        *yi=y;
        *xi=xs+(y-ys)*(xe-xs)/(ye-ys);
    }
public :
    lineClipper(int a,int b, int c,int d): clipper(a,b,c,d)
    {

    }

    virtual void draw (HDC hdc,vector<Point> p,COLORREF color)
    {
        double x1=p[0].x,y1=p[0].y,x2=p[1].x,y2=p[1].y;
        OutCode out1=GetOutCode(x1,y1);
        OutCode out2=GetOutCode(x2,y2);
        while( (out1.All || out2.All) && !(out1.All & out2.All))
        {
            double xi,yi;
            if(out1.All)
            {
                if(out1.left)VIntersect(x1,y1,x2,y2,xleft,&xi,&yi);
                else if(out1.top)HIntersect(x1,y1,x2,y2,top,&xi,&yi);
                else if(out1.right)VIntersect(x1,y1,x2,y2,xright,&xi,&yi);
                else HIntersect(x1,y1,x2,y2,bottom,&xi,&yi);
                x1=xi;
                y1=yi;
                out1=GetOutCode(x1,y1);
            }
            else
            {
                if(out2.left)VIntersect(x1,y1,x2,y2,xleft,&xi,&yi);
                else if(out2.top)HIntersect(x1,y1,x2,y2,top,&xi,&yi);
                else if(out2.right)VIntersect(x1,y1,x2,y2,xright,&xi,&yi);
                else HIntersect(x1,y1,x2,y2,bottom,&xi,&yi);
                x2=xi;
                y2=yi;
                out2=GetOutCode(x2,y2);
            }
        }
        if(!out1.All && !out2.All)
        {
            MoveToEx(hdc,round(x1),round(y1),NULL);
            LineTo(hdc,round(x2),round(y2));
        }
    }
};
class polygonClipper: public clipper
{
public:
    polygonClipper(int a,int b, int c,int d): clipper(a,b,c,d)
    {

    }
    vector<Point> ClipWithEdge(vector<Point> p,int edge, IsInFunc In, IntersectFunc Intersect)
    {
        vector<Point> OutList;
        Point v1=p.back();
        bool v1_in=In(v1,edge);
        for(int i=0; i<(int)p.size(); i++)
        {
            Point v2=p[i];
            bool v2_in=In(v2,edge);
            if(!v1_in && v2_in)
            {

                OutList.push_back(Intersect(v1,v2,edge));
                OutList.push_back(v2);
            }
            else if(v1_in && v2_in) OutList.push_back(v2);
            else if(v1_in) OutList.push_back(Intersect(v1,v2,edge));
            v1=v2;
            v1_in=v2_in;
        }
        return OutList;
    }
    void draw (HDC hdc,vector<Point>p,COLORREF color) ;
};
class circleLineClipper : public Drawer
{
protected:
    int R ;
    Point center ;
public :
    circleLineClipper(int x,int y, int r ): R(r)
    {
        center= {x,y};
    }
    virtual void draw (HDC hdc,vector<Point>p,COLORREF color)
    {
        Point v = p[1]-p[0] ; // ( xe-xs , ye-ys )
        // y = m*x + c
        double m = (double) v.y / v.x ;
        double c =  p[0].y - m * p[0].x  ;

        int w = ( center.x * center.x ) + (center.y * center.y) - (R*R)  ;

        double A = m * m + 1 ;
        double B = 2 * m * c - 2*center.x - 2*center.y * m ;
        double C = c*c + w - 2*center.y *c;
        double  D = B*B - 4*A*C  ;
        if (D < 0)
            cout << "no intersection\n" ;
        else if (D == 0)
        {
            cout << "tangent \n" ;
            double x = -B / (2*A) ;
            double y = m * x + c ;
            SetPixel(hdc , round(x) , round(y) , RGB(0,0,0));
        }
        else
        {
            lineDDA obj ;
            double D2 = sqrt(D) ;
            double x1 = (- (B)  + D2  )/ (2*A) ;
            double y1 = m * x1 + c ;
            double x2 =(- (B)  - D2 )/ (2*A) ;
            double y2 = m * x2 +c ;
            obj.draw(hdc, {{x1,y1} , {x2,y2}} , RGB(0,0,0)) ;
        }

    }
};
class circlePointClipper : public Drawer
{
protected:
    int R ;
    Point center ;
public :
    circlePointClipper(int x,int y, int r ): R(r), center({x,y})
    {    }
    virtual void draw (HDC hdc,vector<Point>p,COLORREF color)
    {
        int xdif = p[0].x - center.x ;
        int ydif = p[0].y - center.y ;
        int d = round(sqrt(xdif*xdif + ydif * ydif)) ;
        if (d<= R)
        {
            cout << "point in\n";
            SetPixel(hdc , p[0].x , p[0].y , color);
        }
        else
            cout << "point in\n" ;


    }
};


class ShapeDrawer
{
private:
    Drawer *d ;
    shapeFiller *s ;
    polygonFiller *p ;
public :
    ShapeDrawer()
    {
        d= new lineDDA() ;
        s = new nonRecursiveFill();
        p = new generalPolygonFiller ();
    }
    void fillS(HDC hdc, int x, int y, COLORREF c1, COLORREF c2)
    {
        s->fillShape(hdc, x, y, c1, c2);
    }
    void fillP(HDC hdc, vector<Point> points,COLORREF c)
    {
        p->fillPolygon(hdc, points, c) ;
    }
    void draw(HDC hdc,vector<Point> p,COLORREF c)
    {
        d->draw(hdc,p, c);
    }
    void setDrawer(Drawer *x)
    {
        if (d != NULL)
            delete d ;
        d = x ;
    }
    void setFillS(shapeFiller *x)
    {
        if (s != NULL)
            delete s ;
        s = x ;
    }
    void setFillP(polygonFiller *x)
    {
        if (p != NULL)
            delete p ;
        p = x ;
    }
    ~ShapeDrawer()
    {
        if (d != NULL)
            delete d ;
        if (s!= NULL)
            delete s ;
        if (p!= NULL)
            delete p ;
    }
};
class specialTask : public Drawer
{
public:
    void draw (HDC hdc,vector<Point>p,COLORREF color) ;
    int len (Point p1 , Point p2);
};

#endif // CLASSES_H
