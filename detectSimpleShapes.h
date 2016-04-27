/* **************************************************************************************** */
/* detectSimpleShapes.h -- Projet [Kro]bot 2015/2016                                        */
/*                                                 -- By Valentin Leboeuf & Olivier Leveque */
/* **************************************************************************************** */

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>
#include <iostream>

using namespace cv;
using namespace std;

struct Cerc {
    Point2f center;
    float radius;
};

struct SimpleShapes {
    Mat img_src;
    Mat img_dst;
    Mat img_bw;
    vector<RotatedRect> rectangles;
    vector<vector<Point>> triangles;
    vector<Cerc> cercles;
};

SimpleShapes detectShapes(int , int );

double angleCos(Point , Point , Point );

void setLabel(Mat& , const string , vector<Point>& );