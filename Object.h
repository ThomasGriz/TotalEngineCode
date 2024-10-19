#define UNICODE
#define _UNICODE
#include <stdint.h>
#include <vector>

class Vector3D{    
    public:
        double x,y,z;
        Vector3D();
        Vector3D(double x, double y, double z);
        Vector3D& operator=(const Vector3D& ob2);
        Vector3D operator+(const Vector3D& ob2);
        bool isNull();
        float dotProduct(Vector3D a, Vector3D b) const;
        Vector3D crossProduct(Vector3D a, Vector3D b) const;
        float length();
        void normalize();
        std::string toString();
};

class Triangle{ 
    public:
        uint32_t color;
        Vector3D points[3];
        Triangle(){};
        Triangle(Vector3D *points);
        Triangle(Vector3D *points, uint32_t inColor, int index = 0);
        Triangle& operator=( Triangle& ob2);
        void setPoint(Vector3D point, int index);
        Vector3D& getPoint(int index);
        uint32_t getColor();
        Vector3D& getPoints();
        
};

class Mesh{
    private:
        
    public:
        std::vector<Triangle> faces;
        Mesh();
        Mesh(Vector3D* points, uint32_t* colors, int size);
        Mesh(std::vector<Triangle> inFaces);
        Mesh& operator=( Mesh& ob2);
        std::vector<Triangle> getFaces();


};

class Object{
    private:
        Vector3D direction; // Where it is heading
        double rotationalSpeed; //How fast it's rotating
        Vector3D rotationAxis; //What it is rotating around
        double quaternion[9];
        Mesh basis; //Coordinates of unrotated mesh around (0,0,0)
        Mesh currentLocation;
    public:
        Vector3D position; //Where it is in space
        Object();
        Object(Mesh inBasis, Vector3D inPosition, Vector3D inDirection, Vector3D inRotationalAxis, double inRotationalSpeed);
        Mesh returnMesh();
        void rotateToPoint(Vector3D inRotationAxis, double degree);
        void newRotation(Vector3D inRotationAxis, double inDegree);
        void setQuaternions();
        void rotate();
        void move();
};