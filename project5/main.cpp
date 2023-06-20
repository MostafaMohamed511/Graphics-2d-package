#include <Windows.h>
#include "classes.h"
#include <iostream>
#include <vector>
#include <fstream>

#define RED RGB(255,0,0)
#define GREEN RGB(0,255,0)
#define BLUE RGB(0,0,255)
#define BLACK RGB(0,0,0)
#define WHITE RGB(255,255,255)

#define ID_SAVE 100
#define ID_LOAD 101
#define ID_CLEAR 102
#define ID_COLOR 103

#define ID_FLOOD_FILL_RECURSIVE 1
#define ID_FLOOD_FILL_ITERATIVE 2
#define ID_CLIPPED_POINT_CIRCLE 4
#define ID_CLIPPED_POINT 5

#define ID_LINE_DDA 201
#define ID_LINE_PARAMITRIC 202
#define ID_LINE_MIDPOINT 203
#define ID_CIRCLE_DIRECT 204
#define ID_CIRCLE_POLAR 205
#define ID_CIRCLE_ITERATIVE_POLAR 206
#define ID_CIRCLE_MIDPOINT 207
#define ID_CIRCLE_MODIFIED_MIDPOINT 208
#define ID_CIRCLE_LINES_FIRST 209
#define ID_CIRCLE_LINES_SECOND 210
#define ID_CIRCLE_LINES_THIRD 211
#define ID_CIRCLE_LINES_FORTH 212
#define ID_CIRCLE_FIRST 213
#define ID_CIRCLE_SECOND 214
#define ID_CIRCLE_THIRD 215
#define ID_CIRCLE_FOURTH 216
#define ID_SQUARE_FILLING 217
#define ID_CLIPPED_LINE 218
#define ID_CLIPPED_LINE_CIRCLE 219
#define ID_ELLIPSE_DIRECT 301
#define ID_ELLIPSE_POLAR 302
#define ID_ELLIPSE_MIDPOINT 303
#define ID_RECTANGLE_FILLING 304
#define ID_SPECIAL 400
#define ID_CLIPPED_POLYGON 401
#define ID_SPLINE 402
#define ID_CONVEX_FILLING 1001
#define ID_GENERAL_FILLING 1002

using namespace std ;

vector<drawing> history ;
ofstream f ;
ifstream l ;
HMENU mainMenu ;
int checkedMenuItem , r  , g , b ;
int xleft , xright , ytop , ybottom , xc , yc, radius ;
ShapeDrawer s ;

void addMenu (HWND hWnd ) ;

vector<Point> points ;
void ClearScreen(HWND hWnd,HDC hdc)
{
    // Get the client area dimensions
    RECT rect;
    GetClientRect(hWnd, &rect);

    // Create a brush with the desired background color
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255)); // White color example

    // Fill the client area with the brush
    FillRect(hdc, &rect, hBrush);

    // Clean up resources
    DeleteObject(hBrush);
}
void loadScreen (HDC) ;
void updateShapeDrawer(int ch)
{
    switch(ch)
    {
    case ID_LINE_DDA :
        s.setDrawer(new lineDDA() );
        break;
    case ID_LINE_PARAMITRIC :
        s.setDrawer(new lineParametric());
        break;
    case ID_LINE_MIDPOINT :
        s.setDrawer(new lineMidPoint());
        break;
    case ID_CIRCLE_DIRECT :
        s.setDrawer(new circleDirect());
        break;
    case ID_CIRCLE_POLAR :
        s.setDrawer(new circlePolar());
        break;
    case ID_CIRCLE_ITERATIVE_POLAR :
        s.setDrawer(new circleIterativePolar());
        break;
    case ID_CIRCLE_MIDPOINT :
        s.setDrawer(new circleMidPoint());
        break;
    case ID_CIRCLE_MODIFIED_MIDPOINT :
        s.setDrawer(new circleModifiedMidPoint());
        break;
    case ID_CIRCLE_FIRST :
        s.setDrawer(new circleFilledCircles(1));
        break;
    case ID_CIRCLE_SECOND :
        s.setDrawer(new circleFilledCircles(2));
        break;
    case ID_CIRCLE_THIRD :
        s.setDrawer(new circleFilledCircles(3));
        break;
    case ID_CIRCLE_FOURTH :
        s.setDrawer(new circleFilledCircles(4));
        break;
    case ID_CIRCLE_LINES_FIRST :
        s.setDrawer(new circleFilledLines(1));
        break;
    case ID_CIRCLE_LINES_SECOND :
        s.setDrawer(new circleFilledLines(2));
        break;
    case ID_CIRCLE_LINES_THIRD :
        s.setDrawer(new circleFilledLines(3));
        break;
    case ID_CIRCLE_LINES_FORTH :
        s.setDrawer(new circleFilledLines(4));
        break;
    case ID_ELLIPSE_DIRECT :
        s.setDrawer(new ellipseDirect());
        break;
    case ID_ELLIPSE_POLAR :
        s.setDrawer(new ellipsePolar());
        break;
    case ID_ELLIPSE_MIDPOINT :
        s.setDrawer(new ellipseMidPoint());
        break;
    case ID_CONVEX_FILLING :
        s.setFillP(new convexFiller());
        break;
    case ID_GENERAL_FILLING :
        s.setFillP(new generalPolygonFiller());
        break;
    case ID_FLOOD_FILL_RECURSIVE :
        s.setFillS(new recursiveFill());
        break;
    case ID_FLOOD_FILL_ITERATIVE :
        s.setFillS(new nonRecursiveFill());
        break;
    case ID_RECTANGLE_FILLING:
        s.setDrawer(new rectangleWithBezier());
        break ;
    case ID_SQUARE_FILLING:
        s.setDrawer(new squareWithHermite());
        break ;
    case ID_SPLINE:
        s.setDrawer(new splineDrawer());
        break ;
    case ID_CLIPPED_POINT:
        s.setDrawer(new pointClipper(xleft , xright , ytop , ybottom));
        break ;
    case ID_CLIPPED_LINE:
        s.setDrawer(new lineClipper(xleft , xright , ytop , ybottom));
        break ;
    case ID_CLIPPED_POLYGON:
        s.setDrawer(new polygonClipper(xleft , xright , ytop , ybottom));
        break ;
    case ID_CLIPPED_LINE_CIRCLE:
        s.setDrawer(new circleLineClipper(xc , yc , radius));
        break ;
    case ID_CLIPPED_POINT_CIRCLE:
        s.setDrawer(new circlePointClipper(xc , yc , radius));
        break ;
    case ID_SPECIAL:
        s.setDrawer(new specialTask());
        break ;
    }
}
LRESULT WINAPI WndProc(HWND hWnd, UINT m, WPARAM wp, LPARAM lp)
{
    HDC hdc = GetDC(hWnd);
    int x=LOWORD(lp), y=HIWORD(lp) ;
    lineDDA linedrawer ;
    circleModifiedMidPoint circleDrawer ;
    specialTask obj2 ;

    switch (m)
    {
    case WM_LBUTTONDOWN:
        points.push_back({x,y});
        SetPixel(hdc,x, y, RGB(0,255,255) );
        SetPixel(hdc,x, y+1, RGB(0,255,255) );
        SetPixel(hdc,x, y-1, RGB(0,255,255) );
        SetPixel(hdc,x+1, y, RGB(0,255,255) );
        SetPixel(hdc,x+1, y+1, RGB(0,255,255) );
        SetPixel(hdc,x+1, y-1, RGB(0,255,255) );
        SetPixel(hdc,x-1, y, RGB(0,255,255) );
        SetPixel(hdc,x-1, y+1, RGB(0,255,255) );
        SetPixel(hdc,x-1, y-1, RGB(0,255,255) );
        if(checkedMenuItem < 3  )
        {
            cout << "filling\n" ;
            history.push_back({checkedMenuItem, points, RGB(r,g,b)});
            s.fillS(hdc, x, y, RGB(r,g ,b), BLACK);
            cout << "done\n" ;
            points.clear() ;
        }
        else if (checkedMenuItem > 6 && checkedMenuItem <200)
        {
            cout << "less than 200\n" ;
        }
        else if  ((checkedMenuItem <6) || (checkedMenuItem < 300 && points.size() == 2) || (checkedMenuItem < 400 && points.size() == 3) || (checkedMenuItem == 400 && points.size() == 4) )
        {
            history.push_back({checkedMenuItem, points, RGB(r,g,b)});
            s.draw(hdc,points, RGB(r,g ,b)) ;
            points.clear() ;
        }
        break;
    case WM_RBUTTONDOWN :

        if (checkedMenuItem >1000)
        {
            history.push_back({checkedMenuItem, points, RGB(255,255,0)});
            s.fillP(hdc , points , BLUE) ;
            points.clear() ;
        }
        else if(points.size() > 3)
        {
            history.push_back({checkedMenuItem, points, RGB(255,255,0)});
            s.draw(hdc, points, RGB(r,g ,b) ) ;
            points.clear();
        }

        break ;
    case WM_MBUTTONDOWN :
//        obj2.draw(hdc , points , BLACK) ;
//        points.clear() ;
        if(checkedMenuItem == ID_CLIPPED_LINE_CIRCLE ||checkedMenuItem == ID_CLIPPED_POINT_CIRCLE)
            circleDrawer.draw(hdc , {{xc,yc} , {xc+radius,yc}} ,RGB(255,255,0)) ;
        else
        {
            linedrawer.draw(hdc, {{xleft,ytop},{xright,ytop}},RGB(255,255,0));
            linedrawer.draw(hdc, {{xleft,ytop},{xleft,ybottom}},RGB(255,255,0));
            linedrawer.draw(hdc, {{xright,ybottom},{xleft,ybottom}},RGB(255,255,0));
            linedrawer.draw(hdc, {{xright,ybottom},{xright,ytop}},RGB(255,255,0));
        }
        break ;
    case WM_COMMAND:
        CheckMenuItem(mainMenu, checkedMenuItem, MF_BYCOMMAND | MF_UNCHECKED);
        checkedMenuItem = -1 ;
        switch(wp)
        {
        case ID_CLEAR:
            ClearScreen(hWnd,hdc) ;
            break;
        case ID_SAVE :
            cout << "wait\n" ;
            f.open("f.txt") ;
            for (auto a : history)
            {
                f << a.type << " " << a.points.size() << "\n"  ;
                for (auto i : a.points)
                    f << i.x << ' ' << i.y << " ";
                f <<'\n' <<  a.color <<'\n';
            }
            f.close();
            cout << "done\n"<< endl ;
            f.close();
            break;
        case ID_LOAD :
            loadScreen(hdc) ;
            break ;
        case ID_COLOR :
            cout << "Enter RGB values : " ;
            cin >> r >> g >> b;
            break ;
        default:
            checkedMenuItem = wp ;
            CheckMenuItem(mainMenu, checkedMenuItem, MF_BYCOMMAND | MF_CHECKED);
            updateShapeDrawer(wp) ;
            break;
        }
        break ;
    case WM_CREATE :
        addMenu(hWnd) ;
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
    default:
        return DefWindowProc(hWnd, m, wp, lp);
    }
    return 0;
}
int APIENTRY WinMain(HINSTANCE h, HINSTANCE p, LPSTR cmd, int csh)
{
    WNDCLASS wc;
    wc.lpszClassName = "MyClass";
    wc.lpszMenuName = NULL;
    wc.hCursor = LoadCursor(NULL,IDC_ARROW) ;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hbrBackground =(HBRUSH) CreateSolidBrush(RGB(255, 255, 255));
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = h;
    RegisterClass(&wc);
    HWND hWnd = CreateWindow("MyClass", "Hello", WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, NULL, h, 0);
    cout << "for clipping enter xleft , xright , ytop and ybottom : ";
    cin >> xleft  >> xright >> ytop >> ybottom ;
    cout << "for clipping with circle window enter xcenter , ycenter and radius : ";
    cin >> xc >> yc >> radius ;
    r=g=b=0 ;
    ShowWindow(hWnd, csh);
    UpdateWindow(hWnd);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
void addMenu (HWND hWnd )
{
    mainMenu = CreateMenu() ;
    HMENU shapeMenu = CreateMenu() ;
    HMENU fileMenu = CreateMenu() ;
    HMENU lineMenu = CreateMenu() ;
    HMENU circleMenu = CreateMenu() ;
    HMENU ellipseMenu = CreateMenu() ;
    HMENU circleFilled1 = CreateMenu() ;
    HMENU circleFilled2 = CreateMenu() ;
    HMENU fillingMenu = CreateMenu() ;
    HMENU clippingMenu = CreateMenu() ;

    AppendMenu(lineMenu, MF_STRING, ID_LINE_DDA, "DDA");
    AppendMenu(lineMenu, MF_STRING, ID_LINE_PARAMITRIC, "Parametric");
    AppendMenu(lineMenu, MF_STRING, ID_LINE_MIDPOINT, "Mid point");

    AppendMenu(circleMenu, MF_STRING, ID_CIRCLE_DIRECT, "Dirct");
    AppendMenu(circleMenu, MF_STRING, ID_CIRCLE_POLAR, "Polar");
    AppendMenu(circleMenu, MF_STRING, ID_CIRCLE_ITERATIVE_POLAR, "iterative polar");
    AppendMenu(circleMenu, MF_STRING, ID_CIRCLE_MIDPOINT, "mid point");
    AppendMenu(circleMenu, MF_STRING, ID_CIRCLE_MODIFIED_MIDPOINT, "modified mid point");

    AppendMenu(ellipseMenu, MF_STRING, ID_ELLIPSE_DIRECT, "Direct");
    AppendMenu(ellipseMenu, MF_STRING, ID_ELLIPSE_POLAR, "Polar");
    AppendMenu(ellipseMenu, MF_STRING, ID_ELLIPSE_MIDPOINT, "mid Point");

    AppendMenu(circleFilled1, MF_STRING, ID_CIRCLE_FIRST, "first quarter");
    AppendMenu(circleFilled1, MF_STRING, ID_CIRCLE_SECOND, "second quarter");
    AppendMenu(circleFilled1, MF_STRING, ID_CIRCLE_THIRD, "third quarter");
    AppendMenu(circleFilled1, MF_STRING, ID_CIRCLE_FOURTH, "forth quarter");

    AppendMenu(circleFilled2, MF_STRING, ID_CIRCLE_LINES_FIRST, "first quarter");
    AppendMenu(circleFilled2, MF_STRING, ID_CIRCLE_LINES_SECOND, "second quarter");
    AppendMenu(circleFilled2, MF_STRING, ID_CIRCLE_LINES_THIRD, "third quarter");
    AppendMenu(circleFilled2, MF_STRING, ID_CIRCLE_LINES_FORTH, "forth quarter");

    AppendMenu(fillingMenu, MF_STRING, ID_CONVEX_FILLING, "convex");
    AppendMenu(fillingMenu, MF_STRING, ID_GENERAL_FILLING, "non-convex");
    AppendMenu(fillingMenu, MF_STRING, ID_FLOOD_FILL_RECURSIVE, "recursive flood fill");
    AppendMenu(fillingMenu, MF_STRING, ID_FLOOD_FILL_ITERATIVE, "iterative flood fill");

    AppendMenu(fileMenu, MF_STRING, ID_SAVE, "save");
    AppendMenu(fileMenu, MF_STRING, ID_LOAD, "load");
    AppendMenu(fileMenu, MF_STRING, ID_CLEAR, "clear");
    AppendMenu(fileMenu, MF_STRING, ID_COLOR, "change color");

    AppendMenu(shapeMenu, MF_POPUP, (UINT_PTR)lineMenu, "Line");
    AppendMenu(shapeMenu, MF_POPUP, (UINT_PTR)circleMenu, "Circle");
    AppendMenu(shapeMenu, MF_POPUP, (UINT_PTR)ellipseMenu, "Ellipse");
    AppendMenu(shapeMenu, MF_POPUP, (UINT_PTR)circleFilled1, "Circle filled with circles");
    AppendMenu(shapeMenu, MF_POPUP, (UINT_PTR)circleFilled2, "Circle filled with lines");
    AppendMenu(shapeMenu, MF_STRING, ID_RECTANGLE_FILLING, "rectangle");
    AppendMenu(shapeMenu, MF_STRING, ID_SQUARE_FILLING, "square");
    AppendMenu(shapeMenu, MF_STRING, ID_SPLINE, "Spline");
    AppendMenu(shapeMenu, MF_STRING, ID_SPECIAL, "Special task");

    AppendMenu(clippingMenu, MF_STRING, ID_CLIPPED_POINT, "point");
    AppendMenu(clippingMenu, MF_STRING, ID_CLIPPED_LINE, "line");
    AppendMenu(clippingMenu, MF_STRING, ID_CLIPPED_POLYGON, "polygon");
    AppendMenu(clippingMenu, MF_STRING, ID_CLIPPED_POINT_CIRCLE, "point circle");
    AppendMenu(clippingMenu, MF_STRING, ID_CLIPPED_LINE_CIRCLE, "line circle");

    AppendMenu(mainMenu, MF_POPUP, (UINT_PTR)shapeMenu, "shapes");
    AppendMenu(mainMenu, MF_POPUP, (UINT_PTR)clippingMenu, "clipping");
    AppendMenu(mainMenu, MF_POPUP, (UINT_PTR)fillingMenu, "filling");
    AppendMenu(mainMenu, MF_POPUP, (UINT_PTR)fileMenu, "file");

    CheckMenuItem(lineMenu, ID_LINE_DDA, MF_BYCOMMAND | MF_CHECKED);
    checkedMenuItem = ID_LINE_DDA ;
    SetMenu(hWnd, mainMenu) ;
}
void loadScreen (HDC hdc)
{
    l.open("f.txt") ;
    int type ;
    vector<Point> pts ;
    while (l >> type)
    {
        updateShapeDrawer(type) ;
        double x, y ;

        int n ;
        l>> n;
        while (n--)
        {
            l >> x >> y ;
            Point p(x,y) ;
            pts.push_back(p);
        }
        l >> n ;
        int rr = n%255 ; n/= 255 ;
        int gg = n%255 ; n/= 255 ;
        int bb = n%255 ;
        if(type < 3  )
        {
            cout << "filling\n" ;
            s.fillS(hdc, x, y, RGB(rr,gg,bb), BLACK);
            cout << "done\n" ;
        }
        else if(type >1000  )
        {
            s.fillP(hdc , pts , RGB(rr,gg,bb)) ;
        }
        else
        {
            s.draw(hdc, pts, RGB(rr,gg,bb) ) ;
        }
//        if (type == 1005 && pts.size() == 3)
//        {
//            s.draw(hdc,pts,RGB(255,0,0)) ;
////            points.clear() ;
//        }
//        if ((type <50  || type == 1006) && pts.size() == 2)
//        {
//            s.draw(hdc,pts,RGB(255,0,0)) ;
////            points.clear() ;
//        }
////        else if(type == 1003 ||type == 1004  )
////        {
////            cout << "else\n" ;
////            s.fillS(hdc, x, y, RGB(255,255,0), RGB(0,0,0));
////            cout << "done\n" ;
////            points.clear() ;
////        }
//        else if (type >50  && type <55 && points.size()==3)
//        {
//            s.draw(hdc,pts, RGB(255,0,0)) ;
////            points.clear() ;
//        }
//        cout << "here\n" ;
//        cout << type << " " << pts.size() << endl;
        pts.clear() ;
    }
    l.close();
}
