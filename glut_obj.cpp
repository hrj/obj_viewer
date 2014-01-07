/*
 *
 * Demonstrates how to load and display an Wavefront OBJ file.
 * Using triangles and normals as static object. No texture mapping.
 *
 * OBJ files must be triangulated!!!
 * Non triangulated objects wont work!
 * You can use Blender to triangulate
 *
 */

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

#define KEY_ESCAPE 27
#define KEY_PLUS   43
#define KEY_MINUS  45
#define KEY_W      119
#define KEY_S      115
#define KEY_A      97
#define KEY_D      100

using namespace std;

/* Window */

typedef struct {
  int width;
  int height;
  const char* title;

  float field_of_view_angle;
  float z_near;
  float z_far;
} glutWindow;


/* OBJ Loading */

class Model_OBJ
{
  private:
    int id;

  public:
    Model_OBJ();
    float* calculateNormal(float* coord1,float* coord2,float* coord3 );
    int Load(char *filename); // Loads the model
    void Draw();          // Draws the model on the screen
    void Release();       // Release the model

    float* normals;             // Stores the normals
    float* Faces_Triangles;         // Stores the triangles
    float* vertexBuffer;          // Stores the points which make the object
    long TotalConnectedPoints;        // Stores the total number of connected verteces
    long TotalConnectedTriangles;     // Stores the total number of connected triangles

};


#define POINTS_PER_VERTEX 3
#define TOTAL_FLOATS_IN_TRIANGLE 9
using namespace std;

static int countObjs = 0;

Model_OBJ::Model_OBJ()
{
  this->TotalConnectedTriangles = 0;
  this->TotalConnectedPoints = 0;
  this->id = countObjs++;
}

float* Model_OBJ::calculateNormal( float *coord1, float *coord2, float *coord3 )
{
  /* calculate Vector1 and Vector2 */
  float va[3], vb[3], vr[3], val;
  va[0] = coord1[0] - coord2[0];
  va[1] = coord1[1] - coord2[1];
  va[2] = coord1[2] - coord2[2];

  vb[0] = coord1[0] - coord3[0];
  vb[1] = coord1[1] - coord3[1];
  vb[2] = coord1[2] - coord3[2];

  /* cross product */
  vr[0] = va[1] * vb[2] - vb[1] * va[2];
  vr[1] = vb[0] * va[2] - va[0] * vb[2];
  vr[2] = va[0] * vb[1] - vb[0] * va[1];

  /* normalization factor */
  val = sqrt( vr[0]*vr[0] + vr[1]*vr[1] + vr[2]*vr[2] );

  float *norm = new float[3];
  norm[0] = vr[0]/val;
  norm[1] = vr[1]/val;
  norm[2] = vr[2]/val;


  return norm;
}

struct FileInfo {
  int fileSize;
  int vertexCount;
  int faceCount;
};


FileInfo getFileInfo(ifstream &objFile) {
  FileInfo fileInfo;
  fileInfo.vertexCount = 0;
  fileInfo.faceCount = 0;

  string line;

  objFile.seekg (0, ios::beg);                    // Go to end of the file,

  while (! objFile.eof() )                      // Start reading file data
  {
    getline (objFile,line);                     // Get line from file
    const char firstChar = line.c_str()[0];

    switch (firstChar) {
      case 'v':
        fileInfo.vertexCount++;
        break;
      case 'f':
        fileInfo.faceCount++;
        break;
      default:
        break;
    }
  }


  printf("Vertex Count: %d\n", fileInfo.vertexCount);
  printf("Face Count: %d\n", fileInfo.faceCount);

  objFile.clear();
  objFile.seekg (0, ios::beg);
  return fileInfo;
}

int Model_OBJ::Load(char* filename)
{
  string line;
  ifstream objFile (filename);
  if (objFile.is_open())                          // If obj file is open, continue
  {
    FileInfo fileInfo = getFileInfo(objFile);

    vertexBuffer = (float*) malloc (fileInfo.vertexCount * sizeof(float) * POINTS_PER_VERTEX);              // Allocate memory for the verteces
    Faces_Triangles = (float*) malloc(fileInfo.faceCount*sizeof(float)*9*POINTS_PER_VERTEX);      // Allocate memory for the triangles
    normals  = (float*) malloc(fileInfo.faceCount*sizeof(float)*9*POINTS_PER_VERTEX);         // Allocate memory for the normals

    int triangle_index = 0;                       // Set triangle index to zero
    int normal_index = 0;                       // Set normal index to zero

    double tempX, tempY, tempZ;
    while (! objFile.eof() )                      // Start reading file data
    {
      getline (objFile,line);                     // Get line from file
      const char *lineStrCpy = line.c_str();
      char *lineStr = (char *) malloc(strlen(lineStrCpy) + 1);
      strcpy(lineStr, lineStrCpy);

      if (lineStr[0] == 'v') {                   // The first character is a v: on this line is a vertex stored.
        lineStr[0] = ' ';                        // Set first character to 0. This will allow us to use sscanf

        sscanf(lineStr,"%lf %lf %lf ",              // Read floats from the line: v X Y Z
            &tempX,
            &tempY,
            &tempZ);
        // printf("%lf, %lf, %lf\n", tempX, tempY, tempZ);

        vertexBuffer[TotalConnectedPoints] = tempX,
        vertexBuffer[TotalConnectedPoints+1] = tempY;
        vertexBuffer[TotalConnectedPoints+2] = tempZ;

        TotalConnectedPoints += POINTS_PER_VERTEX;          // Add 3 to the total connected points
      } else if (lineStr[0] == 'f') {                 // The first character is an 'f': on this line is a point stored
        lineStr[0] = ' ';                        // Set first character to 0. This will allow us to use sscanf

        int vertexNumber[4] = { 0, 0, 0 };
        sscanf(lineStr,"%i%i%i",               // Read integers from the line:  f 1 2 3
            &vertexNumber[0],                   // First point of our triangle. This is an
            &vertexNumber[1],                   // pointer to our vertexBuffer list
            &vertexNumber[2] );                   // each point represents an X,Y,Z.

        vertexNumber[0] -= 1;                   // OBJ file starts counting from 1
        vertexNumber[1] -= 1;                   // OBJ file starts counting from 1
        vertexNumber[2] -= 1;                   // OBJ file starts counting from 1


        /********************************************************************
         * Create triangles (f 1 2 3) from points: (v X Y Z) (v X Y Z) (v X Y Z).
         * The vertexBuffer contains all verteces
         * The triangles will be created using the verteces we read previously
         */

        int tCounter = 0;
        for (int i = 0; i < POINTS_PER_VERTEX; i++)
        {
          Faces_Triangles[triangle_index + tCounter   ] = vertexBuffer[3*vertexNumber[i] ];
          Faces_Triangles[triangle_index + tCounter +1 ] = vertexBuffer[3*vertexNumber[i]+1 ];
          Faces_Triangles[triangle_index + tCounter +2 ] = vertexBuffer[3*vertexNumber[i]+2 ];
          tCounter += POINTS_PER_VERTEX;
        }

        /*********************************************************************
         * Calculate all normals, used for lighting
         */
        float coord1[3] = { Faces_Triangles[triangle_index], Faces_Triangles[triangle_index+1],Faces_Triangles[triangle_index+2]};
        float coord2[3] = {Faces_Triangles[triangle_index+3],Faces_Triangles[triangle_index+4],Faces_Triangles[triangle_index+5]};
        float coord3[3] = {Faces_Triangles[triangle_index+6],Faces_Triangles[triangle_index+7],Faces_Triangles[triangle_index+8]};
        float *norm = this->calculateNormal( coord1, coord2, coord3 );

        // printf("norm: %f, %f, %f\n", norm[0], norm[1], norm[2]);
        tCounter = 0;
        for (int i = 0; i < POINTS_PER_VERTEX; i++)
        {
          normals[normal_index + tCounter ] = norm[0];
          normals[normal_index + tCounter +1] = norm[1];
          normals[normal_index + tCounter +2] = norm[2];
          tCounter += POINTS_PER_VERTEX;
        }
        free(norm);

        // printf("norm index= %d  /  %d\n", normal_index, malloc_usable_size(normals)/ sizeof(float));
        triangle_index += TOTAL_FLOATS_IN_TRIANGLE;
        normal_index += TOTAL_FLOATS_IN_TRIANGLE;
        TotalConnectedTriangles += TOTAL_FLOATS_IN_TRIANGLE;
      }
    }
    objFile.close();                            // Close OBJ file
  }
  else
  {
    cout << "Unable to open file" << endl;
  }
  return 0;
}

void Model_OBJ::Release()
{
  free(this->Faces_Triangles);
  free(this->normals);
  free(this->vertexBuffer);
}

static int lastDrawnId = -1;

void Model_OBJ::Draw()
{
  if (lastDrawnId != id) {
    glEnableClientState(GL_VERTEX_ARRAY);           // Enable vertex arrays
    glEnableClientState(GL_NORMAL_ARRAY);           // Enable normal arrays
    glVertexPointer(3,GL_FLOAT, 0,Faces_Triangles);       // Vertex Pointer to triangle array
    glNormalPointer(GL_FLOAT, 0, normals);            // Normal pointer to normal array
  }

  glDrawArrays(GL_TRIANGLES, 0, TotalConnectedTriangles);   // Draw the triangles

  // glDisableClientState(GL_VERTEX_ARRAY);            // Disable vertex arrays
  // glDisableClientState(GL_NORMAL_ARRAY);            // Disable normal arrays
  lastDrawnId = id;
}

/***************************************************************************
 * Program code
 ***************************************************************************/

Model_OBJ obj;
static float g_rotation = 0.2;
static float g_scale = 1.0;
static float g_deltaY = 0.0;
static float g_deltaX = 0.0;
glutWindow win;

void printError(const char *msg) {
  int err = glGetError();
  if (err != GL_NO_ERROR) {
    printf("%s: %d\n", msg, err);
  }
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();
  gluLookAt( 0,1,4 - g_scale, 0,0,0, 0,1,0);
  glTranslatef(g_deltaX, g_deltaY, 0);
  glRotatef(g_rotation,0,1,0);
  g_rotation += 0.1;
  obj.Draw();

  glutSwapBuffers();
}


void initialize ()
{
  glViewport(0, 0, win.width, win.height);
  glMatrixMode(GL_PROJECTION);
  GLfloat aspect = (GLfloat) win.width / win.height;
  glLoadIdentity();
  gluPerspective(win.field_of_view_angle, aspect, win.z_near, win.z_far);

  glEnable(GL_LIGHTING);
  GLfloat amb_light[] = { 0.01, 0.01, 0.02, 1.0 };
  glLightModelfv( GL_LIGHT_MODEL_AMBIENT, amb_light );

  glEnable(GL_LIGHT0);
  GLfloat ambient[] = { 0.01, 0.01, 0.01, 1.0 };
  GLfloat diffuse[] = { 0.9, 0.9, 0.9, 1 };
  GLfloat specular[] = { 0.1, 0.0, 0.0, 1 };
  GLfloat position[] = { 4.0, 0.0, 0, 1.0 };
  glLightfv( GL_LIGHT0, GL_AMBIENT, ambient );
  glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
  glLightfv( GL_LIGHT0, GL_SPECULAR, specular );
  glLightfv( GL_LIGHT0, GL_POSITION, position );

  glShadeModel( GL_SMOOTH );
  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
  glClearDepth( 1.0f );
  glEnable( GL_DEPTH_TEST );
  glDepthFunc( GL_LEQUAL );
  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

  glEnable( GL_COLOR_MATERIAL );
  glShadeModel( GL_SMOOTH );
  // glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

  glDepthFunc( GL_LEQUAL );
  glEnable( GL_DEPTH_TEST );
  glEnable(GL_CULL_FACE);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt( 0,1,4, 0,0,0, 0,1,0);

  printError("after init");
}

void keyboard ( unsigned char key, int x, int y )
{
  switch ( key ) {
    case KEY_ESCAPE:
      exit ( 0 );
      break;
    case KEY_PLUS:
      g_scale += 0.1;
      break;
    case KEY_MINUS:
      g_scale -= 0.1;
      break;
    case KEY_W:
      g_deltaY -= 0.01;
      break;
    case KEY_S:
      g_deltaY += 0.01;
      break;
    case KEY_D:
      g_deltaX -= 0.01;
      break;
    case KEY_A:
      g_deltaX += 0.01;
      break;
    default:
      printf("Key: %d\n", key);
      break;
  }
}

int main(int argc, char **argv)
{
  // set window values
  win.width = 800;
  win.height = 800;
  win.title = "OpenGL/GLUT OBJ Loader.";
  win.field_of_view_angle = 45;
  win.z_near = 0.01f;
  win.z_far = 500.0f;

  obj.Load(argv[1]);

  // initialize and run program
  glutInit(&argc, argv);                                      // GLUT initialization
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );  // Display Mode
  glutInitWindowSize(win.width,win.height);         // set window size
  glutCreateWindow(win.title);                // create Window
  glutDisplayFunc(display);                 // register Display Function
  glutIdleFunc( display );                  // register Idle Function
  glutKeyboardFunc( keyboard );               // register Keyboard Handler
  initialize();
  glutMainLoop();                       // run GLUT mainloop
  return 0;
}