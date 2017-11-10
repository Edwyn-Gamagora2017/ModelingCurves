/**
 *	Tableau des points permettant de gérer les points de controles
 * On sélectionne le point avec un chiffre 0 pour P0, 1 pour P1, ...
 * On sélectionne ensuite si on veut faire monter, descendre amener vers la gauche ou la droite le point.
 *   d : translation à droite
 *   q : à gauche
 *   z : en haut
 *   s : en bas
 *
 */

 #include <windows.h>

#include <GL/glut.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <deque>
#include "struct.h"
#include "vec3.h"
#include "utils.h"

/* au cas ou M_PI ne soit defini */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define ESC 27

float tx=0.0;
float ty=0.0;

vec3 p0( -2,-2,0 );
vec3 p1( -1,1,0 );
vec3 p2( 1,1,0 );
vec3 p3( 2,-2,0 );
vec3 v1( 1,5,0 );
vec3 v2( 1,-5,0 );
std::deque<vec3> hermiteVertices;
std::deque<vec3> bernsteinVertices;
std::deque<vec3> bernsteinControlVertices;
int maxFactorial = 100;
double * factorial;

// calculate the position on the curve between P1 and P2 (and the tangent on these points) related to the factor u [0,1]
vec3 hermite( double u, vec3 p1, vec3 p2, vec3 v1, vec3 v2 ){
    // Factor that represents the importance of each point and tangent to the result
    double importP1 = 2*pow(u,3) -3*pow(u,2) +1;
    double importP2 = -2*pow(u,3) +3*pow(u,2);
    double importV1 = pow(u,3) -2*pow(u,2) + u;
    double importV2 = pow(u,3) -pow(u,2);

    // position = iP1*P1 +  iP2*P2 +  iV1*V1 +  iV2*V2
    return p1.multiplication( importP1 ).addition(
        p2.multiplication( importP2 ).addition(
        v1.multiplication( importV1 ).addition(
        v2.multiplication( importV2 )
        )));
}

// calculate the curve between P1 and P2 (and the tangent on these points). amountSamples defines the amount of samples in the curve
std::deque<vec3> hermite( vec3 p1, vec3 p2, vec3 v1, vec3 v2, int amountSamples ){
    int amount = amountSamples+2;   // at least 2 samples will be created
    std::deque<vec3> result;
    for( int i=0; i<amount; i++ ){
        result.push_back( hermite( i/((double)amount-1), p1, p2, v1, v2 ) );
    }
    return result;
}

// obtains the factorial in the matrix. if it does not exist, create it
double getFactorial( int n ){
    if( factorial[n] != 0 ){
        return factorial[n];
    }
    else{
        factorial[n] = n*getFactorial( n-1 );
        return factorial[n];
    }
}

// obtains the bernsteinB
double getBernsteinB( int n, int i, double t ){
    return (getFactorial( n )/(getFactorial( i )*getFactorial( n-i )))*pow(t,i) * pow(1-t,n-i);
}

// calculate the position on the Bernstein curve related to the factor u [0,1]
vec3 bernstein( double u, std::deque<vec3> controlPoints ){
    // initializing the result with the first point
    vec3 result = controlPoints[0].multiplication( getBernsteinB( controlPoints.size()-1, 0, u ) );
    for( int i = 1; i<controlPoints.size(); i++ ){
        result = result.addition( controlPoints[i].multiplication( getBernsteinB( controlPoints.size()-1, i, u ) ) );
    }
    return result;
}

// calculate the Bernstein curve. amountSamples defines the amount of samples in the curve
std::deque<vec3> bernstein( std::deque<vec3> controlPoints, int amountSamples ){
    int amount = amountSamples+2;   // at least 2 samples will be created
    std::deque<vec3> result;
    for( int i=0; i<amount; i++ ){
        result.push_back( bernstein( i/((double)amount-1), controlPoints ) );
    }
    return result;
}

/* initialisation d'OpenGL*/
static void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	// cleaning factorial matrix
	factorial = new double[maxFactorial];
	FOR(i,maxFactorial){
        factorial[i] = 0;
	}
	factorial[0] = 1;

	// calculate hermite curve
	hermiteVertices = hermite( p0, p3, v1, v2, 10 );
	// setting bernstein control vertices
	bernsteinControlVertices.push_back( p0 );
	bernsteinControlVertices.push_back( p1 );
	bernsteinControlVertices.push_back( p2 );
	bernsteinControlVertices.push_back( p3 );
	// calculate bernstein curve
	bernsteinVertices = bernstein( bernsteinControlVertices, 10 );
}

/* Dessine de la courbe */
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Print Hermite Curve
	glBegin(GL_LINE_STRIP);
	glColor3f(1.,1.,1.);
	for( int i = 0; i < hermiteVertices.size(); i++ ){
        glVertex3f( hermiteVertices[i].getX(), hermiteVertices[i].getY(), hermiteVertices[i].getZ() );
	}
	glEnd();

	// Print Bernstein Control
	glBegin(GL_LINE_STRIP);
	//glBegin(GL_POLYGON);
	glColor3f(1.,0.,0.);
	for( int i = 0; i < bernsteinControlVertices.size(); i++ ){
        glVertex3f( bernsteinControlVertices[i].getX(), bernsteinControlVertices[i].getY(), bernsteinControlVertices[i].getZ() );
	}
	glEnd();

	// Print Bernstein Curve
	glBegin(GL_LINE_STRIP);
	glColor3f(0.,1.,0.);
	for( int i = 0; i < bernsteinVertices.size(); i++ ){
        glVertex3f( bernsteinVertices[i].getX(), bernsteinVertices[i].getY(), bernsteinVertices[i].getZ() );
	}
	glEnd();

	glFlush();
}

/* Au cas ou la fenetre est modifiee ou deplacee */
void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-4, 4, -4, 4, -1, 1);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {


   case 'd':
         tx=0.1;
		 ty=0;
      break;
   case 'q':
         tx=-0.1;
		 ty=0;
      break;
   case 'z':
         ty=0.1;
		 tx=0;
      break;
   case 's':
         ty=-0.1;
		 tx=0;
      break;
   case ESC:
      exit(0);
      break;
   default :
	   tx=0;
	   ty=0;
   }
   glutPostRedisplay();
}

int main(int argc, char **argv)
{
   glutInitWindowSize(400, 400);
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
   glutCreateWindow("Courbe de Bézier");
   init();
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutDisplayFunc(display);
   glutMainLoop();
   return 0;
}
