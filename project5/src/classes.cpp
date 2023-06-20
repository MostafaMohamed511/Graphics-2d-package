#include "classes.h"

typedef bool (*IsInFunc) (Point& v,int edge);
typedef Point (*IntersectFunc) (Point& v1,Point& v2,int edge);

bool InLeft(Point& v,int edge)
{
    return v.x>=edge;
}
bool InRight(Point& v,int edge)
{
    return v.x<=edge;
}
bool InTop(Point& v,int edge)
{
    return v.y>=edge;
}
bool InBottom(Point& v,int edge)
{
    return v.y<=edge;
}
Point VIntersect(Point& v1,Point& v2,int xedge)
{
    Point res;
    res.x=xedge;
    res.y=v1.y+(xedge-v1.x)*(v2.y-v1.y)/(v2.x-v1.x);
    return res;
}
Point HIntersect(Point& v1,Point& v2,int yedge)
{
    Point res;
    res.y=yedge;
    res.x=v1.x+(yedge-v1.y)*(v2.x-v1.x)/(v2.y-v1.y);
    return res;
}


void polygonFiller :: drawHorizontalLine(HDC hdc,int xs,int xe,int y,COLORREF c)
{
    while(xs <= xe+1 )
        SetPixel(hdc, xs++, y,c );
}
int polygonFiller :: Round(double x)
{
    return (int)(x+0.5);
}


void convexFiller :: fillPolygon(HDC hdc,vector<Point> p,COLORREF c)
{
        cout << "convex filling algorithm\n";

    if (p.size() < 3)
        return ;
    vector<pair<int, int> >table (1000) ;
    doublePoint v1(p.back()),v2 ;
    for (auto &i : table)
        i.first = 10000, i.second=-10000 ;
    for (auto u : p)
    {
        v2 = u ;
        if (v1.y == v2.y)
            continue;
        if (v1.y > v2.y)
            swap(v1,v2) ;
        double inv  = 1.0*(v1.x-v2.x)/(v1.y - v2.y) ;
        while (v1.y <= v2.y)
        {
            table[v1.y].first = min(table[v1.y].first, Round(v1.x) ) ;
            table[v1.y].second= max(table[v1.y].second, Round(v1.x) ) ;
            v1.y++;
            v1.x += inv ;
        }
        v1=u ;
    }
    for (int i=0 ; i< 1000 ; i++)
        if (table[i].first < table[i].second)
            drawHorizontalLine(hdc,table[i].first, table[i].second, i, c) ;
}



void generalPolygonFiller :: fillPolygon(HDC hdc,vector<Point> p,COLORREF c)
{
    cout << "general filling algorithm\n";
    int sz = (int)p.size() ;
    if (sz < 3)
        return ;
    vector< vector< edgeRecord > > table(1000) ;
    // init table
    Point v1,v2 ;
    v1 = p.back() ;
    for (auto u : p)
    {
        v2 = u ;
//            draw(hdc, v1.x, v1.y, v2.x, v2.y, RGB(0,255,0)) ;
        if(v1.y==v2.y)
        {
            v1=v2;
            continue;
        }
        else if (v1.y > v2.y)
            swap(v1,v2) ;
        double inv  = 1.0*(v1.x-v2.x)/(v1.y - v2.y) ;
        table[(int)v1.y].push_back({(double)v1.x, inv,(int) v2.y }) ;
        v1=u;
    }
    vector< edgeRecord > active ;
    int y =0 ;
    while (y < 1000 && table[y].empty())   y++;
    active = table[y] ;
    while (!active.empty())
    {
        if (active.size()&1)
        {
            cout << "error\n" ;
            return ;
        }
        sort(active.begin(), active.end() ) ;
        for (unsigned int i =1 ; i< active.size() ; i+=2)
        {
            drawHorizontalLine(hdc, active[i-1].x, active[i].x, y, c  );
            active[i].x += active[i].mInv ;
            active[i-1].x += active[i-1].mInv ;
        }
        y++ ;
        auto it = active.begin() ;
        while (it != active.end())
            if (it->ymax == y)
                active.erase(it) ;
            else
                it++ ;
        if (y<1000 && !table[y].empty() )
        {
            cout << active.size() << " + " << table[y].size()  << "  " << y << endl;
            for (auto q : table[y])
                active.push_back(q) ;
        }

    }
}

void recursiveFill ::fillShape(HDC hdc,int x,int y,COLORREF Cb,COLORREF Cf)

{
    COLORREF C=GetPixel(hdc,x,y);
    if(C==Cb || C==Cf)return;
    SetPixel(hdc,x,y,Cf);
    fillShape(hdc,x+1,y,Cb,Cf);
    fillShape(hdc,x-1,y,Cb,Cf);
    fillShape(hdc,x,y+1,Cb,Cf);
    fillShape(hdc,x,y-1,Cb,Cf);
}

void nonRecursiveFill :: fillShape(HDC hdc,int x,int y,COLORREF Cb,COLORREF Cf)
{
    stack<Vertex> S;
    S.push(Vertex(x,y));
    while(!S.empty())
    {
        Vertex v=S.top();
        S.pop();
        COLORREF c=GetPixel(hdc, v.x,v.y);
        if(c==Cb || c==Cf)continue;
        SetPixel(hdc,v.x,v.y,Cf);
        S.push(Vertex(v.x+1,v.y));
        S.push(Vertex(v.x-1,v.y));
        S.push(Vertex(v.x,v.y+1));
        S.push(Vertex(v.x,v.y-1));
    }
}


int Drawer :: round(double x)
{
    return (int)(x+0.5);
}
int circularDrawer :: getDistance(Point p1, Point p2)
{
    int xdif = (p1.x-p2.x) ;
    int ydif = (p1.y-p2.y) ;

    return round( sqrt(xdif*xdif + ydif*ydif) );
}

void ellipseDrawer :: DrawPoints(HDC hdc,int xc,int yc, int a, int b,COLORREF color)
{
    SetPixel(hdc, xc+a, yc+b, color);
    SetPixel(hdc, xc-a, yc+b, color);
    SetPixel(hdc, xc-a, yc-b, color);
    SetPixel(hdc, xc+a, yc-b, color);
}
void circleDrawer ::  DrawPoints(HDC hdc,int xc,int yc, int a, int b,COLORREF color)
{
    SetPixel(hdc, xc+a, yc+b, color);
    SetPixel(hdc, xc-a, yc+b, color);
    SetPixel(hdc, xc-a, yc-b, color);
    SetPixel(hdc, xc+a, yc-b, color);
    SetPixel(hdc, xc+b, yc+a, color);
    SetPixel(hdc, xc-b, yc+a, color);
    SetPixel(hdc, xc-b, yc-a, color);
    SetPixel(hdc, xc+b, yc-a, color);

}


void ellipseDirect :: draw(HDC hdc,vector<Point> p,COLORREF c)
{
    int A = getDistance(p[0], p[1]);
    int B = getDistance(p[0], p[2]);
    if (A < B) swap(A,B) ;
    int A2 = A*A;
    int B2 = B*B;
    int xc = p[0].x, yc = p[0].y ;
    int x = 0 ;
    double y = B ;
    while (x*B2/A2<= y)
    {
        y =sqrt(B2 - x*x*B2/A2) ;
        DrawPoints(hdc,xc,yc,x,round(y),c);
        x++ ;
    }
    int yy = round(y) ;
    double x2 ;
    while (yy>=0)
    {
        x2= sqrt(A2-yy*yy*A2/B2);
        DrawPoints(hdc,xc,yc,round(x2),yy,c);
        yy-- ;
    }
}

void ellipsePolar :: draw(HDC hdc,vector<Point> p,COLORREF c)
{
    cout << "ellipse\n" ;
    int A = getDistance(p[0], p[1]);
    int B = getDistance(p[0], p[2]);
    int xc = p[0].x, yc = p[0].y ;
    if (A < B) swap(A,B) ;
    cout << xc << " " << yc << " " << A << " " << B << endl;
    double deltaTheta = 1.0 / A ;
    for (double theta=0 ; theta <= 1.572 ; theta += deltaTheta)
    {
        double x =  A*cos(theta);
        double y =  B*sin(theta);
        DrawPoints(hdc,xc,yc,round(x),round(y),c);
        cout << x << " " << y << endl;
    }
}
void ellipseMidPoint :: draw(HDC hdc,vector<Point> p,COLORREF c)
{
    cout << "midpoint Ellipse\n" ;
    int A = getDistance(p[0], p[1]);
    int B = getDistance(p[0], p[2]);
    int xc = p[0].x, yc = p[0].y ;

    float dx, dy, d1, d2, x, y;
    x = 0;
    y = B;

    // Initial decision parameter of region 1
    d1 = (B * B) - (A * A * B) + (0.25 * A * A);
    dx = 2 * B * B * x;
    dy = 2 * A * A * y;

    // For region 1
    while (dx < dy)
    {

        DrawPoints(hdc, xc,yc,round(x),round(y),c);
        if (d1 < 0)
        {
            x++;
            dx += (2 * B*B);
            d1 += dx + (B*B);
        }
        else
        {
            x++;
            y--;
            dx += (2 * B*B);
            dy -= (2 * A*A);
            d1 += dx - dy + (B * B);
        }
    }

    // Decision parameter of region 2
    d2 = ((B*B) * ((x + 0.5) * (x + 0.5))) +
         ((A* A) * ((y - 1) * (y - 1))) -
         (A * A * B * B);

    // Plotting points of region 2
    while (y >= 0)
    {

        DrawPoints(hdc, xc,yc,round(x),round(y),c);
        if (d2 > 0)
        {
            y--;
            dy -= (2 * A*A );
            d2 += (A * A) - dy;
        }
        else
        {
            y--;
            x++;
            dx +=  (2 * B*B);
            dy -=  (2 * A*A);
            d2 +=  dx - dy + (A*A);
        }
    }
}

void circleDirect :: draw(HDC hdc,vector<Point> p,COLORREF c)
{
    cout << "drawing direct circle from\n" ;
    int R = getDistance(p[0],p[1]);
    int xs = p[0].x, ys= p[0].y ;
    int x=0,y=R;
    int R2=R*R;
    DrawPoints(hdc,xs,ys,x,y,c);
    while(x<y)
    {
        x++;
        y=round(sqrt((double)(R2-x*x)));
        DrawPoints(hdc,xs,ys,x,y,c);
    }
}
void circlePolar  :: draw(HDC hdc,vector<Point> p,COLORREF c)
{
    cout << "drawing polar circle from\n" ;
    int R = getDistance(p[0],p[1]);
    int xs = p[0].x, ys= p[0].y ;
    int x=R,y=0;
    double theta=0,dtheta=1.0/R;
    DrawPoints(hdc,xs,ys,x,y,c);
    while(x>y)
    {
        theta+=dtheta;
        x=round(R*cos(theta));
        y=round(R*sin(theta));
        DrawPoints(hdc,xs,ys,x,y,c);
    }
}
void circleIterativePolar  :: draw(HDC hdc,vector<Point> p,COLORREF c)
{
    cout << "drawing iterative polar circle from\n" ;
    int R = getDistance(p[0],p[1]);
    int xs = p[0].x, ys= p[0].y ;
    double x=R,y=0;
    double dtheta=1.0/R;
    double cdtheta=cos(dtheta),sdtheta=sin(dtheta);
    DrawPoints(hdc,xs,ys,R,0,c);
    while(x>y)
    {
        double x1=x*cdtheta-y*sdtheta;
        y=x*sdtheta+y*cdtheta;
        x=x1;
        DrawPoints(hdc,xs,ys,round(x),round(y),c);
    }
}
void circleMidPoint  :: draw(HDC hdc,vector<Point> p,COLORREF c)
{
    cout << "drawing mid point circle from\n" ;
    int R = getDistance(p[0],p[1]);
    int xs = p[0].x, ys= p[0].y ;
    int x=0,y=R;
    int d=1-R;
    DrawPoints(hdc,xs,ys,x,y,c);
    while(x<y)
    {
        if(d<0)
            d+=2*x+2;
        else
        {

            d+=2*(x-y)+5;
            y--;
        }
        x++;
        DrawPoints(hdc,xs,ys,x,y,c);
    }
}
void circleModifiedMidPoint  :: draw(HDC hdc,vector<Point> p,COLORREF c)
{
    cout << "drawing modified mid point circle from\n" ;
    int R = getDistance(p[0],p[1]);
    int xs = p[0].x, ys= p[0].y ;

    int x=0,y=R;
    int d=1-R;
    int c1=3, c2=5-2*R;
    DrawPoints(hdc,xs,ys,x,y,c);
    while(x<y)
    {
        if(d<0)
        {
            d+=c1;
            c2+=2;
        }
        else
        {

            d+=c2;
            c2+=4;
            y--;
        }
        c1+=2;
        x++;
        DrawPoints(hdc,xs,ys,x,y,c);
    }
}

void circleFilledCircles :: draw(HDC hdc,vector<Point> p,COLORREF c)
{
    int R = getDistance(p[0],p[1]);
    int xs = p[0].x, ys= p[0].y ;
    for (int i=1 ; i<R ; i++)
        drawQuarter(hdc, xs, ys, i, c );

}
void circleFilledCircles :: drawQuarter (HDC hdc,int xc,int yc,int R,COLORREF c)
{
    int x=0,y=R;
    int d=1-R;
    int c1=3, c2=5-2*R;
    while(x<y)
    {
        if(d<0)
        {
            d+=c1;
            c2+=2;
        }
        else
        {

            d+=c2;
            c2+=4;
            y--;
        }
        c1+=2;
        x++;
        DrawPoints(hdc,xc,yc,x,y,c);
    }
}
void circleFilledCircles :: DrawPoints(HDC hdc,int xc,int yc, int a, int b,COLORREF color)
{
    switch(quarter)
    {
    case 1 :
        SetPixel(hdc, xc+a, yc-b, color);
        SetPixel(hdc, xc+b, yc-a, color);
        break ;
    case 2 :
        SetPixel(hdc, xc-a, yc-b, color);
        SetPixel(hdc, xc-b, yc-a, color);
        break ;
    case 3 :
        SetPixel(hdc, xc-a, yc+b, color);
        SetPixel(hdc, xc-b, yc+a, color);
        break ;
    case 4 :
        SetPixel(hdc, xc+a, yc+b, color);
        SetPixel(hdc, xc+b, yc+a, color);
        break ;
    }
}
void circleFilledLines :: draw(HDC hdc,vector<Point> p,COLORREF c)
{
    int R = getDistance(p[0],p[1]);
    int xs = p[0].x, ys= p[0].y ;
    int x=0,y=R;
    int d=1-R;
    int c1=3, c2=5-2*R;
    while(x<y)
    {
        if(d<0)
        {
            d+=c1;
            c2+=2;
        }
        else
        {

            d+=c2;
            c2+=4;
            y--;
        }
        c1+=2;
        x++;
        DrawPoints(hdc,xs,ys,x,y,c);
    }
}
void circleFilledLines :: DrawPoints(HDC hdc,int xc,int yc, int a, int b,COLORREF color)
{
    vector<Point> p(2) ;
    p[0] = {xc, yc};
    switch(quarter)
    {
    case 1 :
        p[1] = {xc+a, yc-b} ;
        lineDrawer.draw(hdc,p, color ) ;
        p[1] = {xc+b, yc-a} ;
        lineDrawer.draw(hdc,p, color ) ;
        break ;
    case 2 :
        p[1] = {xc-a, yc-b} ;
        lineDrawer.draw(hdc,p, color ) ;
        p[1] = {xc-b, yc-a} ;
        lineDrawer.draw(hdc,p, color ) ;
        break ;
    case 3 :
        p[1] = {xc-a, yc+b} ;
        lineDrawer.draw(hdc,p, color ) ;
        p[1] = {xc-b, yc+a} ;
        lineDrawer.draw(hdc,p, color ) ;
        break ;
    case 4 :
        p[1] = {xc+a, yc+b} ;
        lineDrawer.draw(hdc,p, color ) ;
        p[1] = {xc+b, yc+a} ;
        lineDrawer.draw(hdc,p, color ) ;
        break ;
    }
}
void hermiteUser :: hermite (HDC hdc, Point p0, Point t0, Point p1, Point t1, COLORREF c )
{
    int a0 = p0.x, a1=t0.x ;
    int a2 = -3*a0 -2*a1 + 3* p1.x - t1.x, a3=2*a0 + a1 -2* p1.x + t1.x ;
    int b0 = p0.y, b1=t0.y ;
    int b2 = -3*b0 -2*b1 + 3* p1.y - t1.y, b3=2*b0 + b1 -2*p1.y + t1.y ;
    for (double t=0 ; t<=1 ; t+=0.001)
    {
        double x = a3*t*t*t + a2*t*t + a1*t+ a0 ;
        double y = b3*t*t*t + b2*t*t + b1*t+ b0 ;
        SetPixel(hdc, round(x), round(y), c) ;
    }
}
void splineDrawer :: draw (HDC hdc, vector<Point> p, COLORREF c)
{
    int sz = p.size() ;
    if (sz > 2)
    {
        vector<Point>t(sz) ;
        t[0] = p[1]-p[0] ;
        t[sz-1] = p[sz-1]-p[sz-2] ;
        for (int i=1 ; i< sz-1 ; i++)
            t[i] = p[i+1] - p[i-1] ;
        for (int i=0 ; i< sz-1 ; i++)
           hermite(hdc, p[i],t[i],p[i+1], t[i+1], c );
    }
}
void squareWithHermite :: draw(HDC hdc,vector<Point> points,COLORREF c)
{
    Point p = points [0] ;
    int len = getDistance(points[0], points[1]) ;
    xleft = p.x ;
    xright = xleft + len ;
    int y = p.y, y2= y+len, a = y+len/3, b = y2-len/3 ;
    for (int x =p.x ; x<p.x+150 ; x++)
        hermite(hdc, {x,y}, {x-50, a}, {x,y2}, {x+50,b}, c);
}
void rectangleWithBezier :: bezier(HDC hdc, vector <Point> p, COLORREF c)
{
    Point t0 = {p[1].x-p[0].x, p[1].y-p[0].y};
    Point t1 = {p[3].x-p[2].x, p[3].y-p[2].y};
    hermite(hdc, p[0], t0,p[2], t1, c) ;
}
void rectangleWithBezier :: draw(HDC hdc,vector<Point> points,COLORREF c)
{
    Point p = points [0] ;
    int len = getDistance(points[0], points[1]) ;
    int width = getDistance(points[0], points[2]) ;
    ytop = p.y ;
    ybottom = ytop + len ;
    int x = p.x, x2= x+len, a = x+len/3, b =  x2 - len/3  ;
    for (int y =p.y ; y<p.y+width ; y++)
        bezier(hdc, {{x,y}, {a, y-20}, {x2,y}, {b,y+20}}, c);
}

void lineDDA::draw(HDC hdc, vector<Point> p,COLORREF c)
{
    int dx = p[1].x - p[0].x;
    int dy = p[1].y - p[0].y;
    if (abs(dy) <= abs(dx)) /// slope < 1
    {
        if (p[0].x > p[1].x)
        {
            swap(p[0], p[1]);
        }
        int x = p[0].x;
        double y = p[0].y;
        double m = (double)dy / (double)dx;
        SetPixel(hdc, x, round(y), c);
        while (x < p[1].x)
        {
            x++;
            y += m;
            SetPixel(hdc, x, round(y), c);
        }
    }
    else /// slope > 1
    {
        if (p[0].y > p[1].y)
        {
            swap(p[0], p[1]);
        }
        double x = p[0].x;
        int y = p[0].y;
        double minV = (double)dx / (double)dy;
        SetPixel(hdc, round(x), y, c);
        while (y < p[1].y)
        {
            y++;
            x += minV;
            SetPixel(hdc, round(x), y, c);
        }
    }
}

void lineParametric::draw(HDC hdc, vector<Point> p,COLORREF c)
{
    double x, y;
    int x1 = p[0].x ;
    int y1 = p[0].y ;
    int dx = p[1].x - x1;
    int dy = p[1].y - y1 ;
//    double dt = 1.0/((abs(dy) > abs(dx)) ? dy : dx);
    double dt = 0.001 ;
    for (double t = 0; t < 1; t += dt)
    {
        x = x1 + t * (dx);
        y = y1 + t * (dy);
        SetPixel(hdc, round(x), round(y), c);
    }
}
void lineMidPoint::draw(HDC hdc, vector<Point> p,COLORREF c)
{
    Point p1 = p[0], p2=p[1] ;
    cout << " Midpoint Line with X1 = " << p1.x << " Y1 = " << p1.y << " X2 = " << p2.x << " Y2 = " << p2.y << endl;
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    if ((dx >= 0 && 0 <= dy && dy <= dx) || (dx < 0 && 0 >= dy && dy >= dx)) /// 0 < slope < 1
    {
        if (p1.x > p2.x)
        {
            swap(p1, p2);
            dx = p2.x - p1.x;
            dy = p2.y - p1.y;
        }
        int x = p1.x, y = p1.y;
        int d = dx - 2 * dy;
        int d1 = 2 * (dx - dy);
        int d2 = -2 * dy;
        SetPixel(hdc, x, y, c);
        while (x < p2.x)
        {
            if (d <= 0)
            {
                d += d1;
                y++;
            }
            else
            {
                d += d2;
            }
            x++;
            SetPixel(hdc, x, y, c);
        }
    }
    else if ((dx >= 0 && dy > dx) || (dx < 0 && dy < dx)) /// slope > 1
    {
        if (p1.y > p2.y)
        {
            swap(p1, p2);
            dx = p2.x - p1.x;
            dy = p2.y - p1.y;
        }
        int x = p1.x, y = p1.y;
        int d = 2 * dx - dy;
        int d1 = 2 * dx;
        int d2 = 2 * dx - 2 * dy;
        SetPixel(hdc, x, y, c);

        while (y < p2.y)
        {
            if (d <= 0)
            {
                d += d1;
            }
            else
            {
                d += d2;
                x++;
            }
            y++;

            SetPixel(hdc, x, y, c);
        }
    }
    else if ((dx >= 0 && dy < -dx) || (dx < 0 && dy > -dx)) /// slope < -1
    {
        if (p1.y > p2.y)
        {
            swap(p1, p2);
            dx = p2.x - p1.x;
            dy = p2.y - p1.y;
        }
        int x = p1.x, y = p1.y;
        int d = 2 * dx + dy;
        int d1 = 2 * (dx + dy);
        int d2 = 2 * dx;
        SetPixel(hdc, x, y, c);

        while (y < p2.y)
        {
            if (d <= 0)
            {
                d += d1;
                x--;
            }
            else
            {
                d += d2;
            }
            y++;

            SetPixel(hdc, x, y, c);
        }
    }
    else
    {
        if (p1.x > p2.x)
        {
            swap(p1, p2);
            dx = p2.x - p1.x;
            dy = p2.y - p1.y;
        }
        int x = p1.x, y = p1.y;
        int d = -dx - 2 * dy;
        int d1 = -2 * dy;
        int d2 = 2 * (-dx - dy);
        SetPixel(hdc, x, y, c);
        while (x < p2.x)
        {
            if (d <= 0)
            {
                d += d1;
            }
            else
            {
                d += d2;
                y--;
            }
            x++;
            SetPixel(hdc, x, y, c);
        }
    }
}
void polygonClipper::draw (HDC hdc,vector<Point>p,COLORREF color)
    {
        vector<Point> vlist = p ;

        vlist=ClipWithEdge(vlist,xleft,InLeft,VIntersect);
        if (!vlist.empty ())
            vlist=ClipWithEdge(vlist,top,InTop,HIntersect);
        if (!vlist.empty ())
            vlist=ClipWithEdge(vlist,xright,InRight,VIntersect);
        if (!vlist.empty ())
            vlist=ClipWithEdge(vlist,bottom,InBottom,HIntersect);
        if (!vlist.empty ())
        {
            Point v1=vlist.back();
            for(int i=0; i<(int)vlist.size(); i++)
            {
                Point v2=vlist[i];
                MoveToEx(hdc,round(v1.x),round(v1.y),NULL);
                LineTo(hdc,round(v2.x),round(v2.y));
                v1=v2;
            }
        }
    }
int specialTask::len (Point p1 , Point p2)
{
    int xdif = p1.x - p2.x ;
    int ydif = p1.y - p2.y ;
    return sqrt(xdif*xdif + ydif * ydif) ;
}
void specialTask::draw (HDC hdc,vector<Point>p,COLORREF color)
{
    circleModifiedMidPoint c ;
    c.draw(hdc , {p[0] , p[1]} , color);
    c.draw(hdc , {p[2] , p[3]} , color);
    int r1 = len(p[0] , p[1]) ;
    int r2 = len(p[2] , p[3]) ;
    int d = len(p[0] , p[2]) ;
    if (d > r1 + r2)
        cout << "Two circles apart\n" ;
    else if (d == r1 + r2)
        cout << "Two contiguous apart\n" ;
    else
    {
        nonRecursiveFill obj ;
        int dx = (p[2].x - p[0].x);
        int dy = (p[2].y - p[0].y);
        double rat = d- r1 + (r1+r2-d)/2 ;
        rat /= d ;
        cout << p[0].x << " " << p[0].y << " "  ;
        cout << p[2].x << " " << p[2].y << "\n" ;
        cout << dx << " " << dy;
        cout << rat << endl;
        int x = p[0].x + rat * dx ;
        int y = p[0].y + rat * dy ;
        SetPixel(hdc,x, y, RGB(0,255,255) );
        SetPixel(hdc,x, y+1, RGB(0,255,255) );
        SetPixel(hdc,x, y-1, RGB(0,255,255) );
        SetPixel(hdc,x+1, y, RGB(0,255,255) );
        SetPixel(hdc,x+1, y+1, RGB(0,255,255) );
        SetPixel(hdc,x+1, y-1, RGB(0,255,255) );
        SetPixel(hdc,x-1, y, RGB(0,255,255) );
        SetPixel(hdc,x-1, y+1, RGB(0,255,255) );
        SetPixel(hdc,x-1, y-1, RGB(0,255,255) );
        obj.fillShape(hdc ,x ,y , color , color) ;
    }

}
