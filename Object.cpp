#include "Object.h"
#include <cmath>
#include <string>

//////////////////////////////////////////////////////////////////////////////////

Vector3D::Vector3D(){}

Vector3D::Vector3D(double X, double Y, double Z){
    x = X;
    y = Y;
    z = Z;
}

Vector3D& Vector3D::operator=(const Vector3D& ob2) {
    x = ob2.x;
    y = ob2.y;
    z = ob2.z;
    return *this;
}

Vector3D Vector3D::operator+(const Vector3D& ob2) {
    Vector3D sum;
    sum.x = x + ob2.x;
    sum.y = y + ob2.y;
    sum.z = z + ob2.z;
    return sum;
}


bool Vector3D::isNull(){
    if(x == 0 &&  y==0 && z == 0) return true;
    return false;
}

float Vector3D::dotProduct(Vector3D a, Vector3D b) const{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vector3D Vector3D::crossProduct(Vector3D a, Vector3D b) const{
    Vector3D cross;
    cross.x = a.y*b.z - a.z*b.y;
    cross.y = a.x*b.z - a.z*b.x;
    cross.z = a.x*b.y - a.y*b.x;
    return cross;
}

float Vector3D::length(){
    return sqrtf(dotProduct(*this,*this));
}

void Vector3D::normalize(){
    if(isNull()) return;
    float l = length();
    x /= l;
    y /= l;
    z /= l;
}

std::string Vector3D::toString(){
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ") ";
}

//////////////////////////////////////////////////////////////////////////////////

Vector3D& Triangle::getPoint(int index){
    return points[index];
}

void Triangle::setPoint(Vector3D point, int index){
    points[index] = point;
}

uint32_t Triangle::getColor(){
    return color;
}

Triangle::Triangle(Vector3D *inPoints, uint32_t inColor, int index){
    inPoints += index;
    for(int i = 0; i < 3; i++){
        points[i] = *inPoints;
        inPoints++;
    }
    color = inColor;
}

Triangle& Triangle::operator=(Triangle& ob2) {
    points[0] = ob2.getPoint(0);
    points[1] = ob2.getPoint(1);
    points[2] = ob2.getPoint(2);
    color = ob2.getColor();
    return *this;
}

Vector3D& Triangle::getPoints(){
    return *points;
}

//////////////////////////////////////////////////////////////////////////////////

Mesh::Mesh(){}

Mesh::Mesh(Vector3D* points, uint32_t* colors, int size){
    for(int i = 0; i < size - 2; i++){
        faces.push_back(Triangle(points, colors[i], i));
    }
}

Mesh::Mesh(std::vector<Triangle> inFaces){
    faces.assign(inFaces.begin(), inFaces.end());
}

Mesh& Mesh::operator=(Mesh& ob2) {
    faces.assign(ob2.faces.begin(), ob2.faces.end());
    return *this;
}

std::vector<Triangle> Mesh::getFaces(){
    return faces;
}

//////////////////////////////////////////////////////////////////////////////////

Object::Object(Mesh inBasis, Vector3D inPosition, Vector3D inDirection, Vector3D inRotationAxis, double inRotationalSpeed){
    basis = inBasis;
    currentLocation = inBasis;
    position = inPosition;
    direction = inDirection;
    rotationAxis = inRotationAxis;
    rotationalSpeed = inRotationalSpeed;
    setQuaternions();
}

Mesh Object::returnMesh(){
    return currentLocation;
}

void Object::rotateToPoint(Vector3D inRotationAxis, double degree){
    double q1,q2,q3,q4;
    double quaternions[9];
    if(degree == 0) return;

    double a = cos(degree/2);
    double b = position.x * sin(degree/2);
    double c = position.y * sin(degree/2);
    double d = position.z * sin(degree/2);
    double s = 2/(a*a + b*b + c*c + d*d);
    double bs = b*s;
    double cs = c*s;
    double ds = d*s;
    double ab = a*bs;
    double bb = b*bs;
    double cc = c*cs;
    double ac = a*cs;
    double bc = b*cs;
    double cd = c*ds;
    double ad = a*ds;
    double bd = b*ds;
    double dd = d*ds;

    quaternions[0] = 1 - cc - dd;
    quaternions[1] = bc - ad;
    quaternions[2] = bd + ac;
    quaternions[3] = bc + ad;
    quaternions[4] = 1 - bb - dd;
    quaternions[5] = cd - ab;
    quaternions[6] = bd - ac;
    quaternions[7] = cd + ab;
    quaternions[8] = 1 - bb - cc;

    Vector3D point;
    for(Triangle T:currentLocation.getFaces()){
        for(int i = 0; i < 3; i++){
            point = T.getPoint(i);
            
            point.x = quaternions[0]*position.x + quaternions[1]*position.y + quaternions[2]*position.z;
            point.y = quaternions[3]*position.x + quaternions[4]*position.y + quaternions[5]*position.z;
            point.z = quaternions[6]*position.x + quaternions[7]*position.y + quaternions[8]*position.z;
            T.getPoint(i) = point;
           
        }
    }
}

void Object::newRotation(Vector3D inRotationAxis, double inRotationalSpeed){
    rotationAxis = inRotationAxis;
    if(rotationAxis.isNull()) rotationalSpeed = 0;
    else rotationalSpeed = inRotationalSpeed;
    setQuaternions();
}

void Object::setQuaternions(){
    double q1,q2,q3,q4;
    if(rotationalSpeed == 0) return;
    rotationAxis.normalize();

    double a = cos(rotationalSpeed/2);
    double b = rotationAxis.x * sin(rotationalSpeed/2);
    double c = rotationAxis.y * sin(rotationalSpeed/2);
    double d = rotationAxis.z * sin(rotationalSpeed/2);

    double mag = std::sqrt(a * a + b * b + c * c + d * d);
    a /= mag;
    b /= mag;
    c /= mag;
    d /= mag;

    quaternion[0] = 1 - 2 * (c * c + d * d);
    quaternion[1] = 2 * (b * c - a * d);
    quaternion[2] = 2 * (a * c + b * d);
    quaternion[3] = 2 * (b * c + a * d);
    quaternion[4] = 1 - 2 * (b * b + d * d);
    quaternion[5] = 2 * (c * d - a * b);
    quaternion[6] = 2 * (b * d - a * c);
    quaternion[7] = 2 * (a * b + c * d);
    quaternion[8] = 1 - 2 * (b * b + c * c);
    for(int i = 0; i < 9; i++){
        //std::cout << "\n" << quaternion[i];
    }
}

void Object::rotate(){
    if(rotationalSpeed == 0) return;
    Vector3D point;

    for(int k = 0; k < currentLocation.getFaces().size(); k++){
        for(int i = 0; i < 3; i++){
            Vector3D point = currentLocation.getFaces()[k].getPoint(i);  // Use a reference here
            Vector3D holder;

            holder.x = quaternion[0] * point.x + quaternion[1] * point.y + quaternion[2] * point.z;
            holder.y = quaternion[3] * point.x + quaternion[4] * point.y + quaternion[5] * point.z;
            holder.z = quaternion[6] * point.x + quaternion[7] * point.y + quaternion[8] * point.z;

            // Update the point coordinates in-place
             currentLocation.faces[k].points[i] = holder;
        }
    }
    
}

void Object::move(){
    position.x += direction.x;
    position.y += direction.y;
    position.z += direction.z;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

uint32_t makeColor(int red, int green, int blue){
    uint32_t color = (red << 16) + (green << 8) + blue;
    return color;
}

Vector3D TPPoints[6] = {{10,-10,-10},{-10,-10,-10},{0,-10, 17.3},{0,20,0},{10,-10,-10},{-10,-10,-10}};
uint32_t TPColors[4] = {10005000,20050000,30050000,4005000};
Mesh TriangularPyramid(TPPoints, TPColors, 6);

uint32_t pink = makeColor(215,113,134);
uint32_t donutColor = makeColor(223,183,126);

Vector3D DPoint[55] = {{1,1,3},{2,1,6},{3,1,1},{6,1,2},{3,1,-1},{6,1,-2},{1,1,-3},{2,1,-6},{-1,1,-3},{-2,1,-6},{-3,1,-1},
{-6,1,-2},{-3,1,1},{-6,1,2},{-1,1,3},{-2,1,6},{1,1,3},{2,1,6},{2,1,6},{2,-1,6},{6,1,2},{6,-1,2},{6,1,-2},{6,-1,-2},{2,1,-6},
{2,-1,-6},{-2,1,-6},{-2,-1,-6},{-6,1,-2},{-6,-1,-2},{-6,1,2},{-6,-1,2},{-2,1,6},{-2,-1,6},{2,1,6},{2,-1,6},{2,-1,6},{1,-1,3},
{2,-1,6},{3,-1,1},{6,-1,2},{3,-1,-1},{6,-1,-2},{1,-1,-3},{2,-1,-6},{-1,-1,-3},{-2,-1,-6},{-3,-1,-1},
{-6,-1,-2},{-3,-1,1},{-6,-1,2},{-1,-1,3},{-2,-1,6},{1,-1,3},{2,-1,6}};

uint32_t DColors[53] = {pink,pink,pink,pink,pink,pink,pink,23,pink,pink,pink,pink,pink,pink,pink,pink,donutColor,
donutColor,donutColor,donutColor,donutColor,donutColor,donutColor,donutColor,donutColor,donutColor,donutColor,donutColor,donutColor,donutColor,donutColor,donutColor,donutColor
,donutColor,donutColor,donutColor,donutColor,donutColor,donutColor,donutColor,donutColor,donutColor,donutColor,donutColor,donutColor,donutColor,donutColor,donutColor,donutColor,
donutColor,donutColor,donutColor,donutColor};

 Mesh Donut(DPoint, DColors, 55);
