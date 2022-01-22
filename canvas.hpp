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
            rgb[0] = r;
            rgb[1] = g;
            rgb[2] = b;

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

            // Pick different letter according to value
            double v = getValue();
            if(v == 0) return ' ';
            if(v < .25) return '.';
            if(v < .5) return 'i';
            if(v < .75) return 'I';
            return '8';

        }

        // Function for copying another color exactly
        void paste(Color * other){
            for(int i = 0; i < 3; i++)
                rgb[i] = other->rgb[i];
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


class Canvas{
    // This class introduces a drawable canvas that can display the image with
    // ascii pixel graphics. Lot's of drawing methods for various shapes
    // Canvas will be a 2d array of pixels, coordinates start bottom left

    private:

        int width, height;
        Pixel *** surf; // Stands for surface
        bool ** buffer; // Buffer to check which pixels have been drawn


    public:

        Canvas(int w, int h){
            // Creates a new canvas of set dimensions

            // Create the array and fill it with pixels
            surf = new Pixel**[width];
            buffer = new bool*[width];
            for(int i = 0; i < width; i++){
                surf[i] = new Pixel*[height];
                buffer[i] = new bool[height];
                for(int j = 0; j < height; j++){
                    surf[i][j] = new Pixel();
                    buffer[i][j] = false;
                }
            }
        }

        ~Canvas(){

            // Delete all the allocated memory
            for(int i = 0; i < width; i++){
                for(int j = 0; j < height; j++)
                    delete surf[i][j];
                delete[] surf[i];
                delete[] buffer[i];
            }
            delete[] surf;
            delete[] buffer;

        }

        // Getters for pixels
        Pixel * getPixel(int x, int y){
            return surf[x][y];
        }


        // Functions for rendering on the screen
        void render(){
            // This will render everything on the surface to the screen.
            // Takes no parameters at the moment, but this will change pretty soon.

            // Do that for all pixels
            for(int x = 0; x < width; x++){
                for(int y = 0; y < height; y++){
                    // Check if you need to draw the specific pixel
                    if(!buffer[x][y]){
                        gotoxy(x,height-y-1);
                        printf("%c",surf[x][y]->getColor()->getLetter());
                        buffer[x][y] = true;
                    }
                }
            }


        }


        // Functions for drawing on the pixel
        void draw_point(int x, int y,Color * c){

            // Checks that the point is within the rectangle before drawing
            if(x < 0 || x > width || y < 0 || y > height)
                return;

            // Draw the point in the specific color
            surf[x][y]->getColor()->paste(c);
            buffer[x][y] = false;

        }

};