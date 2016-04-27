/* **************************************************************************************** */
/* main.cpp -- Projet [Kro]bot 2015/2016                                                    */
/*                                                 -- By Valentin Leboeuf & Olivier Leveque */
/* **************************************************************************************** */

#include "detectSimpleShapes.h"

// Global variables
SimpleShapes shapes;
int edgeThresh = 1;
int low_Threshold = 50;
const int max_lowThreshold = 100;

/**
 * Main
 */
int main() {
    
    /*****
    //load a picture
    src = imread("/examples_img/shapes.png");
    if (src.empty()) { // Verification
        cout << "Error : Image cannot be loaded" <<endl;
        return -1;
    }
    *****/
    
    //windows
    namedWindow("Image Traitee", CV_WINDOW_AUTOSIZE);
    namedWindow("Image Canny", CV_WINDOW_AUTOSIZE);
    
    //trackbar "RATIO" in "bw"
    createTrackbar("Min Threshold:", "Image Canny", &low_Threshold, max_lowThreshold);
    
    while(true){
        //Extration of simple shape (triangles/rectangles/circles) from the frame
        shapes = detectShapes(0, low_Threshold);
        
        //viewing
        imshow("Image Traitee", shapes.img_dst);
        imshow("Image Canny", shapes.img_bw);
        
        //ESC -- exit the loop
        if (waitKey(30) == 27) {
            destroyWindow("Image Traitee");
            destroyWindow("Image Canny");
            destroyAllWindows();
            cout << "esc key is pressed by user" << endl;
            return 0;
        }
    }
}
