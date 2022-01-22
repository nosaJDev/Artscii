#include <cstdio>
#include <cstdlib>
#include "canvas.hpp"

int main(){

    printf("Hello World!\n");

    // Create a canvas and a color
    Canvas * mycanvas = new Canvas(20,20);
    Color * white = new Color(1,1,1);
    
    // Paint the canvas using the color
    mycanvas->draw_point(2,3,white);
    mycanvas->draw_point(12,6,white);
    mycanvas->draw_point(5,9,white);

    // Finally render the canvas
    mycanvas->render();



}