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
    Canvas * mycanvas = new Canvas(100,60);
    Color * white = new Color(1,1,1);
    Color * grey = new Color(0.5,0,0);
    Color * black = new Color(0,0,0);
    
    // Paint the canvas using the color
    mycanvas->draw_pixel(2,3,white);
    mycanvas->draw_pixel(12,6,grey);
    mycanvas->draw_pixel(5,9,white);
    
    // Create a static triangle of points
    


    
    while(true)
    for(double w = 0; w < 8*2*M_PI; w+=0.001){
        
        // For every frame produce a new view direction and a new transform
        /*Point * a = new Point(0,0,0), *b = new Point(10,10,0), *c = new Point(10,0,0);
        Triangle * tri = new Triangle(a,b,c);
        Point * viewdir = new Point(-1,-1,-1);
        Point * viewpoint = new Point(20,20,20);
        Transform * camera = transform_view_per(viewpoint,viewdir,10);

        // Transform the triangle
        tri->transform(camera);
        */

        Cube * cube = new Cube(new Point(0,0,0),new Point(10,10,10));
        Cube * cube2 = new Cube(new Point(30,30,0),new Point(35,35,5));
        Cube * cube3 = new Cube(new Point(15,15,10),new Point(25,25,30));

        //Point * a = tri->getPoint(0);
        //a->getMatrix()->print();

        // Create a projection camera transform
        Transform * camera = transform_view_per(
            new Point(80*cos(w),80*sin(w),30),
            new Point(-cos(w),-sin(w),-0.9),
            100.0
        );
        //camera->add(matrix_rot_axis(.7,0));
        //Transform * camera = new Transform();
        //Matrix * m = matrix_per(10.0);
        //camera->add(m);
        camera->add(matrix_translate(50,10,0));

        //m->print();



        /*
        Transform * rot = new Transform();
        rot->add(matrix_translate(-15,-15,0));
        rot->add(matrix_rot_axis(w,0));
        rot->add(matrix_rot_axis(2.12456*w,1));
        rot->add(matrix_rot_axis(1.5678*w,2));
        rot->add(matrix_translate(15,15,0));
        */


        cube->transform(camera);
        cube2->transform(camera);
        cube3->transform(camera);
        //a->transform_matrix(m);

        //camera->print();
        //tri->getPoint(0)->getMatrix()->print();



        // Print the triangle
        mycanvas->draw_cube(cube,white);
        mycanvas->draw_cube(cube2,grey);
        mycanvas->draw_cube(cube3,grey);
        mycanvas->render();
        mycanvas->draw_clear(black);
        
        // Delete the redundant stuff
        delete cube;
        delete camera;
        //delete a,b,c;
        //delete camera;
        //delete viewdir;
        //break;
        usleep(1000);
    }

    



}