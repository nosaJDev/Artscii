#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "canvas.hpp"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif


int main(){

    printf("Hello World!\n\n\n\n");

    // Create a canvas and a color
    Canvas * mycanvas = new Canvas(80,40);
    Color * white = new Color(1,1,1);
    Color * grey = new Color(0.5,0,0);
    Color * black = new Color(0,0,0);
    
    // Paint the canvas using the color
    mycanvas->draw_point(2,3,white);
    mycanvas->draw_point(12,6,grey);
    mycanvas->draw_point(5,9,white);
    
    for(int i = 0; i < 200; i++){
        mycanvas->draw_circle(19,10,(i%200)/2.0,white);
        mycanvas->draw_circle(19,10,((i+75)%200)/2.0,white);
        mycanvas->draw_circle(19,10,((i+135)%200)/2.0,white);
        mycanvas->render();
        mycanvas->flood(black);
        usleep(50000);
    }

    



}