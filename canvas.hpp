#include <cstdio>
#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))


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

            char pallete[] = " .:-=+*#%@";

            // Pick different letter according to value
            double v = getValue();
            v*= 9;
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
int * bresenham(int lengthx,int heighty){
    // This returns the offset for every x value in the [0,lengthx] space
    // Setting the arguments accordingly you can get any info you need on drawing lines.
    // Just don't forget to add the offset of your segment each time.
    // The range [0,lengthx] is inclusive so you get lengthx+1 values.
    // This also needs memory cleaning afterward

    int * res = new int[lengthx+1];
    int e = -(lengthx>>1);
    int xx = 0, yy = 0;
    for(int i = 0; i <= lengthx; i++){
        res[i] = yy;
        xx++;
        e += heighty;
        if (e >= 0){
            yy++;
            e -= lengthx;
        }
    }

    return res;


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
        int aa_factor = 3;
        Color * tempcolor;


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


    public:

        Canvas(int w, int h){
            // Creates a new canvas of set dimensions

            // Create the array and fill it with pixels
            width = w;
            height = h;
            surf = new Pixel**[width*aa_factor];
            buffer = new bool*[width];
            for(int i = 0; i < width*aa_factor; i++){
                surf[i] = new Pixel*[height*aa_factor];
                if(i < width)
                    buffer[i] = new bool[height];
                for(int j = 0; j < height*aa_factor; j++){
                    surf[i][j] = new Pixel();
                    if(i < width && j < height)
                        buffer[i][j] = false;
                }
            }

            // Create tempcolor for calculations
            tempcolor = new Color(0,0,0);

        }

        ~Canvas(){

            // Delete all the allocated memory
            for(int i = 0; i < width*aa_factor; i++){
                for(int j = 0; j < height*aa_factor; j++)
                    delete surf[i][j];
                delete[] surf[i];
                if(i < width)
                    delete[] buffer[i];
            }
            delete[] surf;
            delete[] buffer;

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

        // Clean out the canvas with one color only
        void flood(Color * c){

            for(int i = 0; i < width*aa_factor; i++){
                for(int j = 0; j < height*aa_factor; j++)
                    draw_point(i,j,c);
            }

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


        // Functions for drawing on the pixel
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

        void draw_point_aa(int x, int y, Color * c){

            

            for(int i = -aa_factor/2; i < aa_factor/2+1; i++){
                for(int j = -aa_factor/2; j < aa_factor/2+1; j++){
                    delete tempcolor;
                    double factor = aa_factor-abs(i)+abs(j);
                    tempcolor = new Color(c->getRed()*factor,c->getGreen()*factor,c->getBlue()*factor);
                    draw_point(x+i,y+j,tempcolor);
                }
            }


        }

        void draw_line(int x1,int y1, int x2, int y2, Color * c){
            // This will perform all the 8 cases of the line drawing and draw using bresenham's method

            // Perform the antialaising correction
            x1 *= aa_factor;
            x2 *= aa_factor;
            y1 *= aa_factor;
            y2 *= aa_factor;

            // Find the actual differences
            int dx = x2-x1, dy = y2-y1;
            int signx = 1-2*(dx<0), signy = 1-2*(dy<0);
            dx *= signx;
            dy *= signy;

            // Find out which is getting iterated
            bool iteratex = dx >= dy;
            int iter = (iteratex)?dx:dy;
            int signiter = (iteratex)?signx:signy;
            int signnoniter = (iteratex)?signy:signx;
            int iter1 = (iteratex)?x1:y1;
            int noniter1 = (iteratex)?y1:x1;

            // Calcuate the bresenham
            int * res = bresenham(iter,(iteratex)?dy:dx);

            int xx,yy;
            for(int i = 0; i < iter+1; i++){
                
                (iteratex?xx:yy) = iter1+i*signiter;
                (iteratex?yy:xx) = noniter1+signnoniter*res[i];
                draw_point(xx,yy,c);

            }

            delete[] res;

        }

        void draw_circle(double xc,double yc, double r,Color * c){

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

};