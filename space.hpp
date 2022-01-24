#include <cstdio>
#include <cmath>

#ifndef _spacee
#define _spacee

class Matrix{

    // This is a representation of a mathematical 2d matrix 
    // It is filled with doubles

    private:

        int rows, columns; // Rows and columns of the matrix
        double ** array; // The actual values

        void deleteArray(){
            // This function will delete the array containing the values

            for(int i = 0; i < rows; i++)
                delete[] array[i];
            delete[] array;


        }

    public:

        Matrix(int r, int c){
            // This will create a zero matrix given the rows and columns

            // Set the rows and columns
            rows = r;
            columns = c;

            // Create the values
            array = new double *[r];
            for(int i = 0; i < r; i++){
                array[i] = new double[c];
                for(int j = 0; j < c; j++)
                    array[i][j] = 0.0;
            }


        }

        ~Matrix(){

            // Delete the array of the values
            deleteArray();

        }

        // Getters/setters for element
        double get(int r, int c){
            return array[r][c];
        }

        void set(int r, int c, double value){
            array[r][c] = value;
        }

        // Useful operations
        void paste(Matrix * other){
            // This function will paste a matrix to this one, will also change rows and columns to match

            // Delete the old array
            deleteArray();

            // Copy the other data over
            rows = other->rows;
            columns = other->columns;
            array = new double *[rows];
            for(int i = 0; i < rows; i++){
                array[i] = new double[columns];
                for(int j = 0; j < columns; j++)
                    array[i][j] = other->array[i][j];
            }

        }


        // Mathematical operations
        void add(Matrix * other, double scale = 1.0){
            // This will add another matrix to this one
            // Scale will be applied to the added matrix before addition (it allows for subtraction)

            // Check same dimensions
            if(rows != other->rows || columns != other->columns){
                printf("Error, tried to add matrices of different dimensions");
            }

            // Perform the addition
            for(int i = 0; i < rows; i++){
                for(int j = 0; j < columns; j++)
                    array[i][j] += scale*other->array[i][j];
            }

        }

        void mul(Matrix * other){
            // This will perform matrix multiplication and save the result to this matrix
            // It is assumed that the other matrix is on the left of the multiplication

            // Check that the coordinates are correct
            if(other->columns != rows){
                printf("Tried to multiply matrices with wrong dimensions\n");
                return;
            }

            // Create a new temporary matrix to store the values
            Matrix * temp = new Matrix(other->rows,columns);
            for(int i = 0; i < other->rows; i++){
                for(int j = 0; j < columns; j++){

                    // Each element is the dot product of the raw of other and column of this
                    double result = 0.0;
                    for(int k = 0; k < rows; k++){
                        result += other->array[i][k]*array[k][j];
                    }

                    temp->array[i][j] = result;

                }
            }

            // Finally copy the matrix over and delete it
            paste(temp);
            delete temp;

        }

        // For debugging
        void print(){
            // Prints the matrix values
            printf("[ ");
            for(int i = 0; i < rows; i++){
                for(int j = 0; j < columns; j++)
                    printf("%.2lf ",array[i][j]);
                printf((i != rows-1)?"\n":" ]\n");
            }
        }

};

//Various functions that produce the right matrices
Matrix * matrix_id(int dimension){
    // Creates an identity matrix of the right dimensions
    
    // Create it and return it
    Matrix * mat = new Matrix(dimension,dimension);
    for(int i = 0; i < dimension; i++)
        mat->set(i,i,1.0);
    return mat;

}

//The rest of the functions assume that you want to create transformations of homogenous 3d coordinates
Matrix * matrix_scale(double width, double height, double depth){
    // This will create a matrix that will scale a point according to the parameters
    
    Matrix * mat = matrix_id(4);
    mat->set(0,0,width);
    mat->set(1,1,height);
    mat->set(2,2,depth);
    return mat;

}

Matrix * matrix_translate(double x, double y, double z){
    // This is a matrix that will move the object at the specified direction

    Matrix * mat = matrix_id(4);
    mat->set(0,3,x);
    mat->set(1,3,y);
    mat->set(2,3,z);
    return mat;

}

Matrix * matrix_rot_axis(double theta, int axis){
    // This will rotate the object around an axis
    // Axis goes like 0 = x, 1 = y, 2 = z

    Matrix * mat = matrix_id(4);
    double cth = cos(theta);
    double sth = sin(theta);
    int a1 = (1+axis)%3;
    int a2 = (2+axis)%3;
    mat->set(a1,a1,cth);
    mat->set(a1,a2,-sth);
    mat->set(a2,a1,sth);
    mat->set(a2,a2,cth);
    return mat;

}

Matrix * matrix_per(double d){
    // Performs projection assuming camera is at (0,0) looking at -z
    // and d is the z of the projection surface that is parallel to xy
    Matrix * mat = matrix_scale(d,d,d);
    mat->set(3,2,1.0);
    mat->set(3,3,0.0);
    return mat;

}

class Transform{
    // This class is a multiplication of matrices, while keeping the original ones
    // It's helpful for memory keeping and easier syntax

    private:

        int mat_no; // How many matrices are used in the transformation
        int mat_max; // How many matrices does the array have space for
        Matrix ** mats; // The matrices used in the transformation
        Matrix * final; // The final matrix of the transformation

    public:

        Transform(){
            // The constructor will initiate the transformation with an identity matrix

            // Create the array of matrices
            mat_max = 8;
            mats = new Matrix*[mat_max];
            for(int i = 0; i < mat_max; i++)
                mats[i] = nullptr;
            mats[0] = matrix_id(4);
            mat_no = 1;

            // Create the final matrix
            final = matrix_id(4);


        }

        ~Transform(){

            // Delete all the matrices of the array
            for(int i = 0; i < mat_no; i++)
                delete mats[i];
            delete[] mats;

            // Delete the final matrix
            delete final;

        }

        void add(Matrix * matrix){
            //Adds a new matrix to the transformation

            // Check if you need to expand the array
            if(mat_no == mat_max){
                //Expand
                Matrix ** newmat = new Matrix*[mat_max*2];
                for(int i = 0; i < mat_max*2; i++)
                    newmat[i] = (i < mat_max)?mats[i]:nullptr;
                delete[] mats;

                // Replace
                mats = newmat;
                mat_max *= 2;

            }

            // Add the new matrix to the array
            mats[mat_no++] = matrix;

            // Multiply the final so you get the correct result
            final->mul(matrix);

        }

        Matrix * getMiniMatrix(int i){
            return mats[i];
        }

        Matrix * getMatrix(){
            // Returns the final matrix used for the calculation
            return final;
        }

        //For debugging
        void print(){
            // Print all the matrices and the final one
            for(int i = 0; i < mat_no; i++){
                mats[i]->print();
            }
            final->print();


        }


};

// Then we create a point of homogenous coordinates

class Point{

    private:

        Matrix * coords; // Matrix that saves the coords

    public:

        Point(double x = 0, double y = 0, double z = 0){
            // Creates the point with the given coordinates

            // Construct the matrix
            coords = new Matrix(4,1);
            coords->set(0,0,x);
            coords->set(1,0,y);
            coords->set(2,0,z);
            coords->set(3,0,1.0);

        }

        ~Point(){

            // Delete the coords matrix
            delete coords;

        }

        Matrix * getMatrix(){
            return coords;
        }

        // Getters for the actual coords
        double get(int i){
            return coords->get(i,0)/coords->get(3,0);
        }

        double getX(){
            return get(0);
        }

        double getY(){
            return get(1);
        }

        double getZ(){
            return get(2);
        }

        double getLength(){
            // Returns the length of the point/vector
            double x = getX();
            double y = getY();
            double z = getZ();
            return sqrt(x*x+y*y+z*z);
        }

        // Method for applying transformations
        void transform(Transform * trans){
            
            // Multiply the coords with the transformation matrix
            transform_matrix(trans->getMatrix());

        }

        void transform_matrix(Matrix * mat){
            //Performs the transformation with only one matrix

            // Multiply the coords with the transformation matrix
            coords->mul(mat);

            // Then divide with the homogenous coord
            for(int i = 0; i < 3; i++)
                coords->set(i,0,coords->get(i,0)/coords->get(3,0));
            coords->set(3,0,1.0);

        }

        // This resets the length to be 1
        void normalize(){
            
            // Check that the length is not zero
            double len = getLength();
            if(len == 0) return;

            // Divide the coordinates by the length
            for(int i = 0; i < 3; i++)
                coords->set(i,i,coords->get(i,i)/len);

        }

        // Spherical angles
        double getTheta(){
            // Perform the calculation formula
            return acos(getZ()/getLength());

        }

        double getPhi(){
            //Perform the calculation formula
            double x = getX();
            double y = getY();
            if(x == 0) return M_PI/2.0;
            return (x>0)?(atan(y/x)):(atan(y/x)+M_PI);
        }


};

class Triangle{

    // This is the backbone of all the models. It consists of three points
    
    private:

        Point * points[3];

    public:

        Triangle(Point * p1, Point *p2, Point *p3){
            // This will create a triange based off the given points

            // Add the points to the array
            points[0] = p1;
            points[1] = p2;
            points[2] = p3;

        }

        // Getter for the points
        Point * getPoint(int index){
            return points[index];
        }

        // Function for applying transformations
        void transform(Transform * trans){
            
            // Apply the transformation to all the points
            for(int i = 0; i < 3; i++){
                points[i]->transform(trans);
            }

        }

        void transform_debug(Transform * trans){

            //Apply transformations showing the points as you go
            for(int i = 0; i < 6; i++){
                Matrix * tr = trans->getMiniMatrix(i);
                if(i == 0)
                for(int j = 0; j < 3; j++)
                    points[j]->getMatrix()->print();
                tr->print();
                for(int j = 0; j < 3; j++){
                    points[j]->transform_matrix(tr);
                    points[j]->getMatrix()->print();
                }
            }
        }


};

// Function for complex transforms
Transform * transform_view_per(Point * viewpoint, Point * viewdirection, double d){
    // This function will create a perspective view transformation
    // viewpoint is the point the camera resides in
    // viewdirection is the directional vector that the camera looks by
    // d is the distance of the projection surface from the camera.

    //Initiate the transformation
    Transform * trans = new Transform();

    // First move the viewpoint to (0,0)
    double vx = viewpoint->getX();
    double vy = viewpoint->getY();
    double vz = viewpoint->getZ();
    trans->add(matrix_translate(-vx,-vy,-vz));

    // Then set the direction to be looking at -z
    double theta = viewdirection->getTheta();
    double phi = viewdirection->getPhi();
    //printf("phi = %lf\n",phi);
    trans->add(matrix_rot_axis(-phi,2));
    trans->add(matrix_rot_axis(M_PI-theta,1));
    trans->add(matrix_rot_axis(-M_PI/2.0,2));

    // Finally apply the perspective projection matrix
    trans->add(matrix_per(d));
    return trans;

}


//Some other shapes for testing
class Cube{
    // This is a cube defined by two points. It consists of triangles.

    private:

        Triangle * triangles[12];

    public:

        Cube(Point * p1, Point * p2){

            // Find all the given coordinates
            double x1 = p1->getX();
            double y1 = p1->getY();
            double z1 = p1->getZ();
            double x2 = p2->getX();
            double y2 = p2->getY();
            double z2 = p2->getZ();

            // Set all the triangles according to the points
            triangles[0] = new Triangle(new Point(x1,y1,z1),new Point(x1,y1,z2),new Point(x2,y1,z1));
            triangles[1] = new Triangle(new Point(x1,y1,z2),new Point(x2,y1,z2),new Point(x2,y1,z1));
            triangles[2] = new Triangle(new Point(x1,y2,z1),new Point(x1,y2,z2),new Point(x2,y2,z1));
            triangles[3] = new Triangle(new Point(x1,y2,z2),new Point(x2,y2,z2),new Point(x2,y2,z1));

            triangles[4] = new Triangle(new Point(x1,y1,z1),new Point(x1,y1,z2),new Point(x1,y2,z1));
            triangles[5] = new Triangle(new Point(x1,y1,z2),new Point(x1,y2,z2),new Point(x1,y2,z1));
            triangles[6] = new Triangle(new Point(x2,y1,z1),new Point(x2,y1,z2),new Point(x2,y2,z1));
            triangles[7] = new Triangle(new Point(x2,y1,z2),new Point(x2,y2,z2),new Point(x2,y2,z1));

            triangles[8] = new Triangle(new Point(x1,y1,z2),new Point(x1,y2,z2),new Point(x2,y2,z2));
            triangles[9] = new Triangle(new Point(x1,y1,z2),new Point(x2,y2,z2),new Point(x1,y2,z2));
            triangles[10] = new Triangle(new Point(x1,y1,z1),new Point(x1,y2,z1),new Point(x2,y2,z1));
            triangles[11] = new Triangle(new Point(x1,y1,z1),new Point(x2,y2,z1),new Point(x1,y2,z1));


        }

        ~Cube(){

            // Delete all the triangles
            for(int i = 0; i < 12; i++)
                delete triangles[i];

        }

        // Get a specific triangle
        Triangle * getTriangle(int i){
            return triangles[i];
        }

        // Transform the cube
        void transform(Transform * trans){

            // Apply the transform to the triangles
            for(int i = 0; i < 12; i++)
                triangles[i]->transform(trans);

        }



};



#endif