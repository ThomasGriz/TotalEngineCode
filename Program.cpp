
#include <iostream>
#include "Window.cpp"
#include "Windows.h"
#include "Object.cpp"
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib,"Gdiplus.lib");


const int GAMESPEED = 100; //How fast objects move

#if RAND_MAX == 32767
#define Rand32() ((rand() << 16) + (rand() << 1) + (rand() & 1))
#else
#define Rand32() rand()
#endif

class Camera{
    Vector3D position;
    Vector3D lookDir;
    float fFovDegrees, fAspectRatio, fNear, fFar;
    double M[4][4]; //Projection Matrix

    public:
        Camera(Vector3D inPosition, Vector3D inLookDir, float inFOV, float inAR, float inNear, float inFar){
            position = inPosition;
            lookDir = inLookDir;
            fFovDegrees = inFOV;
            fAspectRatio = inAR;
            fNear = inNear;
            fFar = inFar;
            for(int i = 0; i < 4; i++) for(int j = 0; j < 4; j++) M[i][j] = 0;
            setMatrixProjection();
        }
        void setAspectRatio(float AR){
            fAspectRatio = AR;
            
        }
        void setMatrixProjection(){
            float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);

            M[0][0] = fAspectRatio * fFovRad;
		    M[1][1] = fFovRad;
		    M[2][2] = fFar / (fFar - fNear);
		    M[3][2] = (-fFar * fNear) / (fFar - fNear);
		    M[2][3] = 1.0f;
		    M[3][3] = 0.0f;
        }
        Vector3D transformPoint(Vector3D inPoint){
            
            Vector3D point;
            point.x = inPoint.x*M[0][0];
            point.y = inPoint.y*M[1][1];
            point.z = (inPoint.z*M[2][2] + M[3][2]);
            if(point.z != 0.0){
                point.x /= inPoint.z;
                point.y /= inPoint.z;
                point.z /= inPoint.z;
            }
            return point;
            
        }
};

class screenTriangle : public Triangle{
    public:
        void screenScale(){
            points[0].x++; points[1].x++; points[2].x++;
            points[0].y++; points[1].y++; points[2].y++;
            points[0].x *= 0.5f * frame.width;
            points[1].x *= 0.5f * frame.width;
            points[2].x *= 0.5f * frame.width;
            points[0].y *= 0.5f * frame.height;
            points[1].y *= 0.5f * frame.height;
            points[2].y *= 0.5f * frame.height;

        }
};

void drawTriangle(screenTriangle triangleShape);
void drawTriangleold(screenTriangle triangleShape);
std::vector<screenTriangle> generateScreenTriangles(std::vector<Object> Entities, Camera Perspective);

int main(){
    
    std::cout << "Creating Window \n";

    Window* pWindow = new Window();
    std::cout << "Window Made \n";

    bool running = true;
    
    /*
    uint32_t color;
    screenTriangle triangles[1000];
    for(int i = 0; i < 1000; i++){
            for(int j = 0; j < 3; j++){
                triangles[i].x[j] = (int)(rand()%frame.width);
                triangles[i].y[j] = (int)(rand()%frame.height);
                triangles[i].z[j] = (int)(rand()%400) + 500;
            }
            triangles[i].color = Rand32();
        }
        for(int i = 0; i < 1000; i++){
            for(int j = 0; j < 3; j++){
                triangles[i].x[j] += 1;
                if(triangles[i].x[j] >= frame.width)
                    triangles[i].x[j] = 1;
                triangles[i].y[j] += 0;
                if(triangles[i].y[j] >= frame.height)
                    triangles[i].y[j] = 1;
                triangles[i].z[j] += 2;
                if(triangles[i].y[j] >= 1000)
                    triangles[i].y[j] = 50;
            }
        }
        triangles[0].z[0] = 1;
        triangles[0].z[1] = 1;
        triangles[0].z[2] = 3;
        triangles[1].z[0] = 1;
        triangles[1].z[1] = 2;
        triangles[1].z[2] = 1;
        triangles[2].z[0] = 29;
        triangles[2].z[1] = 30;
        triangles[2].z[2] = 30000;
        int x[3] = {50,70,400};
        int y[3] = {50,150,100};
        int z[3] = {20,20,0};

        screenTriangle tri1(x, y, z, (uint32_t)0);
        
        
        for(int i = 0; i < 1000; i++){
            for(int j = 0; j < 3; j++){
                triangles[i].x[j] += 1;
                if(triangles[i].x[j] >= frame.width)
                    triangles[i].x[j] = 1;
                triangles[i].y[j] += 0;
                if(triangles[i].y[j] >= frame.height)
                    triangles[i].y[j] = 1;
                triangles[i].z[j] = rand()%500;
                if(triangles[i].y[j] >= 1000)
                    triangles[i].y[j] = 50;
            }
        } 
        static unsigned int p = 0;
        for(int i = 0; i < 256*256; i++)
            frame.pixels[(p++)%(frame.width*frame.height)] = p;
        
       frame.pixels[Rand32()%(frame.width*frame.height)] = 0;
       
    */
    //Vector3D inPosition, Vector3D inLookDir, float inFOV, float inAR, float inNear, float inFar
    Vector3D tempPos = {0,0,0};
    Vector3D tempView = {0,0,1};
    Camera player(tempPos, tempView, 90, ((float)frame.height)/frame.width, 0.1, 300);

    std::vector<Object> allObjects;
    Vector3D randomPos;
    Vector3D spin;

    for(int i = 0; i < 1; i++){
        randomPos.x = rand()%90-45;
        randomPos.y = rand()%45-22;
        randomPos.z = rand()%2 +35;
        spin.x = rand()%4 - 2;
        spin.y = rand()%4 - 2;
        spin.z = rand()%4 - 2;

        allObjects.push_back(Object(Donut, randomPos, {0,0,0}, spin, (rand()%30)/100.0));
    }

    for(int i = 0; i < 1; i++){
        randomPos.x = rand()%80-40;
        randomPos.y = rand()%55-27;
        randomPos.z = rand()%50 +80;
        spin.x = rand()%4 - 2;
        spin.y = rand()%4 - 2;
        spin.z = rand()%4 - 2;

        allObjects.push_back(Object(TriangularPyramid, randomPos, {0,0,0}, spin,  (rand()%3)/100.0));
    }


    while(running){

        

        
        
       
        
        
        //tri1.color = Rand32();
        for(int i = 0; i < frame.height; i++){
            for(int j = 0; j < frame.width; j++){
                frame.pixels[i*frame.width + j] = makeColor(i*255/frame.height,i*245/frame.height,i*51/frame.height);
            }
        }
        //zBuffer resetter
        //int renderDistance = 4294967295;
        int pixelCount = frame.height * frame.width;
        for(int i = 0; i < pixelCount; i++){
            frame.zBuffer[i] = 4294967295;
        }
        for(int i = 0; i < allObjects.size(); i++){
            allObjects[i].rotate();
        }
        std::vector<screenTriangle> everything = generateScreenTriangles(allObjects, player);
        for(screenTriangle ST: everything){
            drawTriangle(ST);
        }
        //drawTriangle(tri1);
        
        //render everything
        if(!pWindow->ProcessMessages()){
            std::cout << "Closing Window\n";
            running = false;
        }
       
        
    }

    delete pWindow;

    

    return 0;

}

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////

void drawFlatLine(int x1, int y, int x2, float zStart, float flatLineCurve, uint32_t color){
    int height = y*frame.width;
    double zPos;
    
    for(int i = std::max(std::min(x1,x2), 0); i < std::min(frame.width,std::max(x2,x1)); i++){
        zPos = (zStart + i*flatLineCurve)*512;
        if( (uint32_t)(zPos) < frame.zBuffer[height+i]){
            frame.zBuffer[height + i] = (uint32_t)(zPos);
            frame.pixels[height + i] = color;
            //std::cout << " - " << zPos << "\n";
           
            //std::cout << frame.zBuffer[height + i] << " - ";
        }else{
            //frame.pixels[height + i] = -1;
        }
        
    }
}

//////////////////////////////////////////////////////////////////////////////////////////

void fillBottomFlatTriangle(float flatLineCurve, float yUpCurve, Vector3D *points, uint32_t color)
{ 
    float invslope1 = (std::min(x2,x3) - x1) / ((y2 - y1 == 0)? 0.001: y2-y1);
    float invslope2 = (std::max(x3, x2) - x1) / ((y3 - y1 == 0)? 0.001: y3-y1);

    
    float curx1 = x1;
    float curx2 = x1;

    float zStart = z1 - (x1*flatLineCurve);
    float zPos = zStart + (y3-y1)*yUpCurve;
     //From where the z buffer calculations are based off of
    
    //float slope = (y1-y2) / (x1-x2); 
    
    for (int scanlineY = y1; scanlineY > y3; scanlineY--)
    {
        
        zPos -= yUpCurve;
        if(scanlineY > 0 && scanlineY < frame.height) //clipping
            drawFlatLine((int)curx1, scanlineY, (int)curx2, zPos, flatLineCurve, color);
        
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}
//------------------------------------------------------------------------------------------
void fillTopFlatTriangle(float flatLineCurve, float yUpCurve, Vector3D *points, uint32_t color)
{
    float invslope1 = (x3 - std::min(x1, x2)) / ((y3 - y1 == 0)? 0.001: y3-y1);
    float invslope2 = (x3 - std::max(x1,x2)) / ((y3 - y2 == 0)? 0.001: y3-y1);

    float curx1 = x3;
    float curx2 = x3;

    float zStart = z1 - (x1*flatLineCurve);
    float zPos = zStart + (y3-y1)*yUpCurve; //From where the z buffer calculations are based off of

    for (int scanlineY = y3; scanlineY <= y1; scanlineY++)
    {
        
        zPos += yUpCurve;
        if(scanlineY > 0 && scanlineY < frame.height) //clipping
            drawFlatLine((int)curx1, scanlineY, (int)curx2, zPos, flatLineCurve, color);
        curx1 += invslope1;
        curx2 += invslope2;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////

void drawTriangle(screenTriangle tri){
    
    Vector3D sortedPoints[3];
    double yP[3] = {tri.points[0].y, tri.points[1].y, tri.points[2].y};

    if(yP[0] > yP[1] && yP[0] > yP[2]){ //if [0] is greatest
        sortedPoints[0] = tri.points[0];
        if(yP[1] > yP[2]){
            sortedPoints[1] = tri.points[1];
            sortedPoints[2] = tri.points[2];
        }else{
            sortedPoints[1] = tri.points[2];
            sortedPoints[2] = tri.points[1];
        }

    }else if(yP[1] > yP[2]){ //if [1] is greatest
        sortedPoints[0] = tri.points[1];
        if(yP[0] > yP[2]){
            sortedPoints[1] = tri.points[0];
            sortedPoints[2] = tri.points[2];
        }else{
            sortedPoints[1] = tri.points[2];
            sortedPoints[2] = tri.points[0];
        }

    }else{ //if [2] is greatest
        sortedPoints[0] = tri.points[2];
        if(yP[0] > yP[1]){
            sortedPoints[1] = tri.points[0];
            sortedPoints[2] = tri.points[1];
        }else{
            sortedPoints[1] = tri.points[1];
            sortedPoints[2] = tri.points[0];
        }
    }
    
    //Normalizing vectors

    int vec12x = xP[1]-xP[0];
    int vec12y = yP[1]-yP[0];
    float vec12z = zP[1]-zP[0]; // Because z needs to be flipped for anything to work

    int vec13x = xP[1]-xP[2];
    int vec13y = yP[1]-yP[2];
    float vec13z = zP[1]-zP[2]; // same as ^

    float crossX = vec12y*vec13z - vec12z*vec13y;
    float crossY = vec12x*vec13z - vec12z*vec13x;
    float crossZ = vec12x*vec13y - vec12y*vec13x;



    float flatLineCurve =  crossX/crossZ;
    float yUpCurve = -crossY/crossZ;
   
    

    if(yP[1] == yP[2]){
        fillBottomFlatTriangle(flatLineCurve, yUpCurve, sortedPoints, tri.color);
    }else if(yP[0] == yP[1]){
        fillTopFlatTriangle(flatLineCurve, yUpCurve, sortedPoints, tri.color);

    }else{
        int vX =  (int)(xP[0]+((float)(yP[1]-yP[0]) / (float)(yP[2]-yP[0]+0.001)) * (xP[2]-xP[0]));
        int vY = yP[1];
        float vZ = (zP[0]+((float)(yP[1]-yP[0]) / (float)(yP[2]-yP[0]+0.001)) * (zP[2]-zP[0]));
        Vector3D midpoint(vX,vY,vZ)

        fillBottomFlatTriangle(flatLineCurve, yUpCurve, xP[0],yP[0],zP[0], xP[1],yP[1],zP[1], vX,vY,vZ, tri.color);
        fillTopFlatTriangle(flatLineCurve, yUpCurve, xP[1],yP[1],zP[1], vX,vY,vZ, xP[2],yP[2],zP[2], tri.color);
    }

}


std::vector<screenTriangle> generateScreenTriangles(std::vector<Object> Entities, Camera Perspective){
    std::vector<screenTriangle> renderedTriangles;
    std::vector<Triangle> preTransformedTriangles;
    int index = 0;
    
    for(Object O:Entities){
        preTransformedTriangles = O.returnMesh().getFaces();
        for(Triangle T:preTransformedTriangles){
            renderedTriangles.push_back(screenTriangle());
            renderedTriangles[index].setPoint(Perspective.transformPoint(T.getPoint(0)+O.position), 0);
            renderedTriangles[index].setPoint(Perspective.transformPoint(T.getPoint(1)+O.position), 1);
            renderedTriangles[index].setPoint(Perspective.transformPoint(T.getPoint(2)+O.position), 2);

            renderedTriangles[index].color = T.getColor();
            
            renderedTriangles[index].screenScale(); //Takes them from (-1 , 1) to the size of the screen
            index++;
        }
    }
    return renderedTriangles;
}

/*
int crossProduct(int x1, int y1, int x2, int y2){
    return x1 * y2 - x2 * y1;
}

void drawTriangleold(screenTriangle tri){
    int maxX = std::max(tri.x[1], std::max(tri.x[2], tri.x[3]));
    int minX = std::min(tri.x[1], std::min(tri.x[2], tri.x[3]));
    int maxY = std::max(tri.y[1], std::max(tri.y[2], tri.y[3]));
    int minY = std::min(tri.y[1], std::min(tri.y[2], tri.y[3]));
    int x1 = tri.x[2]-tri.x[1];
    int y1 = tri.y[2]-tri.y[1];
    int x2 = tri.x[3]-tri.x[1];
    int y2 = tri.y[3]-tri.y[1];
    int qX;
    int qY;
    for(int x = minX; x <= maxX; x++){
        for(int y = minY; y <= maxY; y++){
            qX = x - tri.x[1];
            qY = y - tri.y[1];

            float s = (float)(qX * y2 - x2 * qY) / (x1 * y2 - x2 * y1);
            float t = (float)(x1 * qY - y1 * qX) / (x1 * y2 - x2 * y1);
            if ( (s >= 0) && (t >= 0) && (s + t <= 1) )
            { //inside triangle 
                frame.pixels[y * frame.width + x] = tri.color;
                
            }

        }
    }
*/


// 216 - 46 - 24 - 47 - 3