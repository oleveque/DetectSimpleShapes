/* **************************************************************************************** */
/* detectSimpleShapes.ccp -- Projet [Kro]bot 2015/2016                                      */
/*                                                 -- By Valentin Leboeuf & Olivier Leveque */
/* **************************************************************************************** */

#include "detectSimpleShapes.h"

/**
 * triangles, rectangles and circles detection
 */
SimpleShapes detectShapes(int num_cam, int lowThreshold) {
    static VideoCapture cap(num_cam); //open the webcam
    
    static Mat img_src, img_dst; //scr: source & dst: destination
    img_src.release(); img_dst.release();
    static Mat img_gray, img_canny;
    img_gray.release(); img_canny.release();
    
    static SimpleShapes shapes;
    static vector<vector<Point>> contours; contours.clear();
    static vector<Point> vertices_contour; vertices_contour.clear();
    static vector<vector<Point>> triangles; triangles.clear();
    static vector<RotatedRect> rectangles; rectangles.clear();
    static vector<Cerc> cercles; cercles.clear();
    
    static int RATIO = 3;
    static int kernel_size = 3;
    
    //checking the webcam
    if(!cap.isOpened()) {
        cout << "Error - camera cannot be openned" << endl;
        exit(-1);
    }
    
    //new frame from the webcam
    cap >> img_src;
    
    //convert to Grayscale
    cvtColor(img_src, img_gray, CV_BGR2GRAY);
    
    //binarisation (Canny filter)
    Canny(img_gray, img_canny, lowThreshold, RATIO*lowThreshold, kernel_size);
    
    //find coutours
    findContours(img_canny.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    img_dst = img_src.clone();
    
    //find shapes
    for (int i = 0; i < contours.size(); i++) {
        //approximate contour with accuracy proportional
        //to the contour perimeter
        approxPolyDP(Mat(contours[i]), vertices_contour, arcLength(Mat(contours[i]), true)*0.02, true);
        
        //skip small or non-convex objects
        if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(vertices_contour)) {
            continue;
        }
        
        if (vertices_contour.size() == 3) {//TRIANGLES
            triangles.push_back(vertices_contour); //add to the set of detected triangles
            
            //drawing
            for (int i = 0; i < 3; i++)
                line(img_dst, vertices_contour[i], vertices_contour[(i+1)%3], Scalar(0,255,0), 3);
            
            //label
            setLabel(img_dst, "TRI", contours[i]);
        }
        else if (vertices_contour.size() == 4) {
            int nb_vertices = 4; //vertices_contour.size();
            
            //calculation of "cos" from all corners
            vector<double> cos;
            for (int j = 2; j < nb_vertices+1; j++) {
                cos.push_back(angleCos(vertices_contour[j%nb_vertices], vertices_contour[j-2], vertices_contour[j-1]));
            }
            
            //storage of "cos" in ascending order
            sort(cos.begin(), cos.end());
            
            //save the min and max values of "cos"
            double mincos = cos.front();
            double maxcos = cos.back();
            
            //check that the corners are right angles
            if (mincos >= -0.3 && maxcos <= 0.3) { //RECTANGLES
                RotatedRect rotRect = minAreaRect(contours[i]);
                rectangles.push_back(rotRect); //add to the set of detected rectangles
                
                //drawing
                Point2f coins_rect[4];
                rotRect.points(coins_rect);
                for (int i = 0; i < 4; i++)
                    line(img_dst, coins_rect[i], coins_rect[(i+1)%4], Scalar(0,255,0), 3);
                
                //label
                stringstream sentence;
                sentence << "RECT rot = " << rotRect.angle;
                string rotLabel = sentence.str();
                setLabel(img_dst, rotLabel, contours[i]);
            }
        }
        else { //CIRCLES
            double area = contourArea(contours[i]);
            Rect r = boundingRect(contours[i]);
            int radius = r.width / 2;

            if (abs(1 - ((double)r.width / r.height)) <= 0.2 && abs(1 - (area / (CV_PI * pow(radius, 2)))) <= 0.2) {
                Cerc cercle;
                minEnclosingCircle(contours[i], cercle.center, cercle.radius);
                cercles.push_back(cercle); //add to the set of detected circles
                
                //drawing
                circle(img_dst, cercle.center, cercle.radius, CV_RGB(255,255,0), 3);
                
                //label
                setLabel(img_dst, "CIR", contours[i]);
            }
        }
    }
    
    //save data
    shapes.img_src = img_src;
    shapes.img_dst = img_dst;
    shapes.img_bw = img_canny;
    shapes.rectangles = rectangles;
    shapes.triangles = triangles;
    shapes.cercles = cercles;
    
    //printf some informations
    if (triangles.size() == 0 && rectangles.size() == 0 && cercles.size() == 0) printf("NOTHING\n");
    else if(triangles.size() == 1 && rectangles.size() == 0 && cercles.size() == 0) printf("TRIANGLE\n");
    else if (triangles.size() == 0 && rectangles.size() == 1 && cercles.size() == 0) printf("RECTANGLE\n");
    else if (triangles.size() == 0 && rectangles.size() == 0 && cercles.size() == 1) printf("CERCLE\n");
    else printf("ERROR\n");
    
    //send data
    return shapes;
}

/**
 * Helper function to find a cosine of angle between vectors
 * from pt0->pt1 and pt0->pt2
 */
double angleCos(Point pt1, Point pt2, Point pt0) {
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2));
    
}

/**
 * Viewing "label" on the contour detected in the picture "img"
 */
void setLabel(Mat& img, const string label, vector<Point>& contour) {
    static int fontface = FONT_HERSHEY_SIMPLEX;
    static double scale = 0.4;
    static int thickness = 1;
    static int baseline = 0;
    
    Size text = getTextSize(label, fontface, scale, thickness, &baseline);
    Rect r = boundingRect(contour); //returns the smallest rectangle containing the "contour"
    
    Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
    rectangle(img, pt + Point(0, baseline), pt + Point(text.width, -text.height), CV_RGB(255,255,255), CV_FILLED);
    putText(img, label, pt, fontface, scale, CV_RGB(0,0,0), thickness, 8);
}
