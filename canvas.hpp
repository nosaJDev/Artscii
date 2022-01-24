#include <cstdio>
#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))
#include <cmath>
#include <algorithm>
#include "space.hpp"

class Color{
    // This class will represent color in various color schemes.
    // It will be stored internally in the rgb format
    // Later, matrices will be used for everything, but not for now

    private:

        double * rgb;

    public:

        Color(double r, double g, double b){
            
            // Create the array and store the rgb values
            rgb = new double[3];
            rgb[0] = (r>1)?1:r;
            rgb[1] = (g>1)?1:g;
            rgb[2] = (b>1)?1:b;

        }

        ~Color(){

            // Delete the array alloted for the values
            delete[] rgb;

        }

        double * getRGB(){
            // Returns the rgb array
            return rgb;

        }

        // Those return basic colors individually
        double getRed(){
            return rgb[0];
        }
        double getGreen(){
            return rgb[1];
        }
        double getBlue(){
            return rgb[2];
        }

        // Overload the operator for easier access
        double operator[](int index){
            return rgb[index];
        }

        // Those return the HSV values for the specific color
        double getValue(){
            // Value is derived by the mean of colors
            return (rgb[0]+rgb[1]+rgb[2])/3.0;
        }

        // This is for ascii matching
        char getLetter(){

            int pallete_no = 4;
            char pallete[pallete_no];
            pallete[0] = ' ';
            pallete[1] = 'o';
            pallete[2] = '0';
            pallete[3] = '@';

            // Pick different letter according to value
            double v = getValue();
            v*= pallete_no-1;
            return pallete[(int)v];
            

        }

        // Function for copying another color exactly
        void paste(Color * other){
            for(int i = 0; i < 3; i++)
                rgb[i] = other->rgb[i];
        }

        //Function to determine if two colors are the same
        bool equals(Color * other){
            return (rgb[0] == other->rgb[0] || rgb[1] == other->rgb[1] || rgb[2] == other->rgb[2]);
        }

};

class Pixel{
    // This class will hold information on a specific pixel.
    // That is color information, depth information, and various things that 
    // have not yet been implemented.

    private:

        Color * color;
        double z; // Used for z-buffering

    public:

        Pixel(){
            // Creates an empty black pixel
            color = new Color(0,0,0);

        }

        ~Pixel(){
            // Delete the color before you go
            delete color;
        }

        // Getter for the color
        Color * getColor(){
            return color;
        }

        // Getters/setter for z value
        double getZ(){
            return z;
        }

        void setZ(double new_z){
            z = new_z;
        }
    

};

// Drawing helper functions
int * bresenham(int lengthx,int heighty,int xstart = 0, int ystart = 0, int order = 0){
    // This will perform bresenham and return the points on the line [x1, y1, x2, y2, ...]
    // Assumes that abs(lengthx) is greater than abs(lengthy)
    
    // Find the signs for correct iteration
    int signx = 1-2*(lengthx<0);
    int signy = 1-2*(heighty<0);
    lengthx *= signx;
    heighty *= signy;

    int * res = new int[2*(lengthx+1)];
    int e = -(lengthx>>1);
    int xx = xstart, yy = ystart;
    for(int i = 0; i <= lengthx; i++){
        res[2*i+order] = xx;
        res[2*i+1-order] = yy;
        xx+=signx;
        e += heighty;
        if (e >= 0){
            yy+=signy;
            e -= lengthx;
        }
    }

    return res;

}

int * bresenham_line(int x1, int y1, int x2, int y2, int &length){
    // This specific function will produce all the points on the
    // specified line using the bresenham method.
    // The points are then saved on an array in the form [x1,y1,x2,y2,x3,y3 ...]
    // This function is very useful not only for drawing lines, but figuring out points
    // for more complex shapes (e.g. triangles)

    // Calculate the initial variables to find which case applies
    int dx = x2-x1, dy = y2-y1;

    // Check which of the two you need to iterate
    if (abs(dx) >= abs(dy)){
        // Iterating x
        length = abs(dx)+1;
        return bresenham(dx,dy,x1,y1);
    }else{
        // Iterating y
        length = abs(dy)+1;
        return bresenham(dy,dx,y1,x1,1);
    }

}


class Canvas{
    // This class introduces a drawable canvas that can display the image with
    // ascii pixel graphics. Lot's of drawing methods for various shapes
    // Canvas will be a 2d array of pixels, coordinates start bottom left

    private:

        int width, height;
        Pixel *** surf; // Stands for surface
        bool ** buffer; // Buffer to check which pixels have been drawn
        bool framerendered = false;

        // Variables for antialaising
        int aa_factor = 1;
        Color * tempcolor;
        static Color * drawcolor;


        // Private drawing functions to help with the public ones
        void draw_point_8(int xx, int yy, int xo, int yo, Color * c){
            // Draws around (xo, yo) with 8-symmetry
            draw_point_aa(xo+xx,yo+yy,c);
            draw_point_aa(xo-xx,yo+yy,c);
            draw_point_aa(xo+xx,yo-yy,c);
            draw_point_aa(xo-xx,yo-yy,c);
            draw_point_aa(xo+yy,yo+xx,c);
            draw_point_aa(xo-yy,yo+xx,c);
            draw_point_aa(xo+yy,yo-xx,c);
            draw_point_aa(xo-yy,yo-xx,c);
        }

        // Draws a point with subvalues for the color on the sides to achieve anti-alising
        // This is only used by the draw methods and not by users
        void draw_point_aa(int x, int y, Color * c){

            // New draw point aa draws a whole pixel, offseted
            for(int i = 0; i < aa_factor; i++){
                for(int j = 0; j < aa_factor; j++){
                    draw_point(x+i,y+j,c);
                }
            }

            /*
            for(int i = -aa_factor/2; i < aa_factor/2+1; i++){
                for(int j = -aa_factor/2; j < aa_factor/2+1; j++){
                    delete tempcolor;
                    double factor = aa_factor-abs(i)+abs(j);
                    tempcolor = new Color(c->getRed()*factor,c->getGreen()*factor,c->getBlue()*factor);
                    draw_point(x+i,y+j,tempcolor);
                }
            }*/
        }

        // This function will draw a subpixel on the surface (subpixels are the same as pixels when aa_factor is set to 1)
        void draw_point(int x, int y,Color * c){
            // Checks that the point is within the rectangle before drawing
            if(x < 0 || x >= width*aa_factor || y < 0 || y >= height*aa_factor)
                return;

            // Draw the point in the specific color
            Color * old = surf[x][y]->getColor();
            if(!old->equals(c)){
                old->paste(c);
                buffer[x/aa_factor][y/aa_factor] = false;
            }
        }


    public:

        Canvas(int w, int h){
            // Creates a new canvas of set dimensions

            // Set the variables for width and height
            width = w;
            height = h;

            // Create the surface array and fill it with pixels
            // Note that the surface array should be size * aa_factor, to achieve the supersampling
            surf = new Pixel**[w*aa_factor];
            for(int i = 0; i < w*aa_factor; i++){
                surf[i] = new Pixel*[h*aa_factor];
                for(int j = 0; j < h*aa_factor; j++)
                    surf[i][j] = new Pixel();
            }

            // Do the same for the buffer array
            // The buffer array does not need to be larger than the canvas
            buffer = new bool*[w];
            for(int i = 0; i < w; i++){
                buffer[i] = new bool[h];
                for(int j = 0; j < h; j++)
                    buffer[i][j] = false;
            }

            // Create tempcolor for calculations and the drawing color
            tempcolor = new Color(0,0,0);
            if(!drawcolor) drawcolor = new Color(1,1,1);

        }

        ~Canvas(){

            // Delete the surface
            for(int i = 0; i < width; i++){
                for(int j = 0; j < height; j++){
                    delete surf[i][j];
                }
                delete[] surf[i];
            }
            delete[] surf;

            // Delete the buffer array
            for(int i = 0; i < width; i++){
                delete[] buffer[i];
            }
            delete[] buffer;

            // Delete the temporary color
            delete tempcolor;

        }

        // Getters for pixels
        Color * getPixelColor(int x, int y){

            // Reset the temp color
            double rgb[3] = {0.0,0.0,0.0};

            for(int i = 0; i < aa_factor; i++){
                for(int j = 0; j < aa_factor; j++){

                    Pixel * p = surf[aa_factor*x+i][aa_factor*y+j];
                    double * prgb = p->getColor()->getRGB();
                    for(int rgb_i = 0; rgb_i < 3; rgb_i++){
                        rgb[rgb_i] += prgb[rgb_i];
                    }
                }
            }

            // Divide by the pixel number
            int aa_square = aa_factor*aa_factor;
            delete tempcolor;
            tempcolor = new Color(rgb[0]/aa_square,rgb[1]/aa_square,rgb[2]/aa_square);

            return tempcolor;
        }


        

        // Functions for rendering on the screen
        void render(){
            // This will render everything on the surface to the screen.
            // Takes no parameters at the moment, but this will change pretty soon.

            // Do that for all pixels
            for(int x = 0; x <= width+1; x++){
                for(int y = 0; y <= height+1; y++){

                    // Draw the border around the view
                    if(x == 0 || y == 0 || x == width+1 || y == height+1){
                        if (framerendered) continue;
                        gotoxy(1+2*x,height-y+2);
                        printf("##");
                        continue;
                    }

                    // Check if you need to draw the specific pixel
                    else if(!buffer[x-1][y-1]){
                        gotoxy(1+2*x,height-y+2);
                        char c = getPixelColor(x-1,y-1)->getLetter();
                        printf("%c%c",c,c);
                        buffer[x-1][y-1] = true;
                    }
                }
            }

            gotoxy(0,height+3);
            framerendered = true;

            // Flush the output
            fflush(stdout);

        }

        // Clean out the canvas with one color only
        void draw_clear(Color * c = drawcolor){

            for(int i = 0; i < width*aa_factor; i++){
                for(int j = 0; j < height*aa_factor; j++)
                    draw_point(i,j,c);
            }
        }

        // This function is different from draw_point because it will fill a single pixel on any aa_factor
        void draw_pixel(int x, int y, Color * c = drawcolor){
            
            // Scale the coords according to aa
            x *= aa_factor;
            y *= aa_factor;

            // Draw all the subpixels of the pixel
            for(int i = 0; i < aa_factor; i++)
                for(int j = 0; j < aa_factor; j++)
                    draw_point(x+i,y+j,c);

        }

        void draw_line(int x1,int y1, int x2, int y2,bool use_aa = false, Color * c = drawcolor){
            // This is done using the bresenham line method, see above

            // Perform the antialaising correction
            if(use_aa){
                x1 *= aa_factor;
                x2 *= aa_factor;
                y1 *= aa_factor;
                y2 *= aa_factor;
            }

            // Find the length of the result
            int res_len;
            
            // Call bresenham line for the points
            int * res = bresenham_line(x1,y1,x2,y2,res_len);

            // Draw all the points of the line
            if(use_aa){
                for(int i = 0; i < res_len; i++){
                    draw_point_aa(res[2*i],res[2*i+1],c);
                }
            }else{
                for(int i = 0; i < res_len; i++){
                    draw_pixel(res[2*i],res[2*i+1],c);
                }
            }

            // Delete the result after drawing
            delete[] res;

        }

        void draw_circle(double xc,double yc, double r,Color * c = drawcolor){

            xc *= aa_factor;
            yc *= aa_factor;
            r *= aa_factor;

            // Draws a circle around (xc,yc) with radius = r
            // This is using a modified bresenham
            int xx = 0, yy = r, e = -r;
            while(xx <= yy){
                draw_point_8(xx,yy,xc,yc,c);
                e += 2*xx+1;
                xx++;
                if (e >= 0){
                    e -= 2*yy-2;
                    yy--;
                }
            }

        }

        void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, Color * c = drawcolor){
            // Draws the triangle using bresenham (fast and reliable)

            //Pypass to test things
            draw_line(x1,y1,x2,y2,c);
            draw_line(x2,y2,x3,y3,c);
            draw_line(x3,y3,x1,y1,c);
            return;


            // Scale the triangle for aa
            x1 *= aa_factor;
            y1 *= aa_factor;
            x2 *= aa_factor;
            y2 *= aa_factor;
            x3 *= aa_factor;
            y3 *= aa_factor;


            //Check which point is in the middle (height wise)
            int miny = std::min(std::min(y1,y2),y3), maxy = std::max(std::max(y1,y2),y3);
            int ex1, ey1, ex2, ey2; // Edge points
            int mx, my; // Middle point
            if(y2 == miny && y3 == maxy || y3 == miny && y2 == maxy){
                // point 1 in the middle
                mx = x1; my = y1;
                ex1 = x2; ey1 = y2; ex2 = x3; ey2 = y3;
            }else if(y1 == miny && y3 == maxy || y3 == miny && y1 == maxy){
                // point2 in the middle
                mx = x2; my = y2;
                ex1 = x1; ey1 = y1; ex2 = x3; ey2 = y3;
            }else{
                //Point three in the middle
                mx = x3; my = y3;
                ex1 = x1; ey1 = y1; ex2 = x2; ey2 = y2;
            }

            // Now that we have the middle points, we can bresenham the two distances
            int rb_len, r1_len, r2_len;
            int * resbig = bresenham_line(ex1,ey1,ex2,ey2,rb_len);
            int * res1small = bresenham_line(ex1,ey1,mx,my,r1_len);
            int * res2small = bresenham_line(mx,my,ex2,ey2,r2_len);
            int * ressmall = res1small;
            int rs_len = r1_len;
            int i = 0, j = 0;
            int lasty = -10;
            bool changed = false;

            for(int i = 0; i < rb_len; i++){
                
                //Find the y on the big line
                int by = resbig[2*i+1];
                int bx = resbig[2*i];

                //If it is the same as last y then just draw the extra point
                if(by == lasty){
                    draw_point(bx,by,c);
                    continue;
                }
                lasty = by;

                // Then find the coords on the small line
                int sx = ressmall[2*j];
                int sy = ressmall[2*j+1];

                // Draw the inbetween points
                for(int xx = std::min(sx,bx); xx <= std::max(sx,bx); xx++)
                    draw_point(xx,by,c);

                // Finally, iterate the small line until a change in y (draw the points as you go)
                while(sy == lasty){
                    if(j == rs_len-1){
                        //Change the line
                        if(changed) break;
                        ressmall = res2small;
                        rs_len = r2_len;
                        j = 0;
                        changed = true;
                    }
                    j++;
                    sx = ressmall[2*j];
                    sy = ressmall[2*j+1];
                    draw_point(sx,sy,c);
                }
            }

        }

        void draw_triangle(Triangle *tri, Color * c){
            // Draws a triangle set by the space file. Rounds coordinates to the best approximate pixel
            
            // Get each coordinate from the points of the Triangle
            double x1 = tri->getPoint(0)->getX(), y1 = tri->getPoint(0)->getY();
            double x2 = tri->getPoint(1)->getX(), y2 = tri->getPoint(1)->getY();
            double x3 = tri->getPoint(2)->getX(), y3 = tri->getPoint(2)->getY();

            // Call the function above
            draw_triangle((int)x1,(int)y1,(int)x2,(int)y2,(int)x3,(int)y3,c);

        }

        void draw_cube(Cube * cube, Color * c){

            // Draw all the triangles of the cube
            for(int i = 0; i < 12; i++)
                draw_triangle(cube->getTriangle(i),c);

        }


};

