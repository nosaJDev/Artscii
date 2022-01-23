#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "canvas.hpp"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif


Color * Canvas::drawcolor = nullptr;


int main(){

    printf("Hello World!\n\n\n\n");

    // Create a canvas and a color
    Canvas * mycanvas = new Canvas(80,40);
    Color * white = new Color(1,1,1);
    Color * grey = new Color(0.5,0,0);
    Color * black = new Color(0,0,0);
    
    // Paint the canvas using the color
    mycanvas->draw_pixel(2,3,white);
    mycanvas->draw_pixel(12,6,grey);
    mycanvas->draw_pixel(5,9,white);
    
    // Create a static triangle of points
    


    

    for(double w = 0; w < 8*2*M_PI; w+=0.01){
        
        // For every frame produce a new view direction and a new transform
        Point * a = new Point(0,0,0), *b = new Point(10,10,0), *c = new Point(10,0,0);
        Triangle * tri = new Triangle(a,b,c);
        Point * viewdir = new Point(-1,-1,-1);
        Point * viewpoint = new Point(20,20,20);
        Transform * camera = transform_view_per(viewpoint,viewdir,10);


        // Transform the triangle
        tri->transform_debug(camera);


        //camera->print();

        // Print the triangle
        mycanvas->draw_triangle(tri,white);
        mycanvas->render();
        mycanvas->draw_clear(black);
        
        // Delete the redundant stuff
        delete tri;
        delete a,b,c;
        delete camera;
        delete viewdir;
        //break;
        usleep(1000);
    }

    



}