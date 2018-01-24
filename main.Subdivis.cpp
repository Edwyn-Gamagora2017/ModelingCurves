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

int selectedCurve = 0;
int selectedControlPoint = 0;
double selectedControlPointSquareSize = 0.2;
double selectedControlPointMoveStep = 0.2;

vec3 p0( -2,0,0 );
vec3 p1( 0,3,0 );
vec3 p2( 3,3,0 );
vec3 p3( 1,0,0 );
vec3 p4( 3,-3,0 );
vec3 p5( 0,-3,0 );
std::deque< std::deque<vec3> > generalControlVertices;

vec3 chaikinPoint( vec3 p1, vec3 p2 ){
    return p1.multiplication( 3/4. ).addition( p2.multiplication( 1/4. ) );
}

std::deque<vec3> chaikin( std::deque<vec3> controlPoints, int level, int maxLevel ){
    if( level == maxLevel ){
        return controlPoints;
    }
    else{
        std::deque<vec3> result;
        for( int i=0; i<controlPoints.size(); i++ ){
            result.push_back( chaikinPoint( controlPoints[i], controlPoints[(i+1)%controlPoints.size()] ) );
            result.push_back( chaikinPoint( controlPoints[(i+1)%controlPoints.size()], controlPoints[i] ) );
        }
        return chaikin( result, level+1, maxLevel );
    }
}

/* initialisation d'OpenGL*/
static void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	// setting control vertices
    std::deque<vec3> controlPoints;

    controlPoints.push_back(p0);
    controlPoints.push_back(p1);
    controlPoints.push_back(p2);
    controlPoints.push_back(p3);
    controlPoints.push_back(p4);
    controlPoints.push_back(p5);
    //controlPoints.push_back(p0);

    generalControlVertices.push_back( controlPoints );
}

void drawCurve(std::deque<vec3> vertices, std::deque<vec3> controlPoints, bool isSelected){
	// Print Control Box
	glBegin(GL_LINE_LOOP);
	//glBegin(GL_POLYGON);
	glColor3f(1.,0.,0.);
	for( int i = 0; i < controlPoints.size(); i++ ){
        glVertex3f( controlPoints[i].getX(), controlPoints[i].getY(), controlPoints[i].getZ() );
	}
	glEnd();

	// Print Curve
	glBegin(GL_LINE_STRIP);
	glColor3f(0.,1.,0.);
	for( int i = 0; i < vertices.size(); i++ ){
        glVertex3f( vertices[i].getX(), vertices[i].getY(), vertices[i].getZ() );
	}
	glEnd();

	// Draw a square that identifies the selected Control Point
    if( isSelected ){
        glBegin(GL_LINE_LOOP);
        glColor3f(0.,0.,1.);
            glVertex3f( controlPoints[ selectedControlPoint ].getX() - selectedControlPointSquareSize/2., controlPoints[ selectedControlPoint ].getY() - selectedControlPointSquareSize/2., controlPoints[ selectedControlPoint ].getZ() );
            glVertex3f( controlPoints[ selectedControlPoint ].getX() - selectedControlPointSquareSize/2., controlPoints[ selectedControlPoint ].getY() + selectedControlPointSquareSize/2., controlPoints[ selectedControlPoint ].getZ() );
            glVertex3f( controlPoints[ selectedControlPoint ].getX() + selectedControlPointSquareSize/2., controlPoints[ selectedControlPoint ].getY() + selectedControlPointSquareSize/2., controlPoints[ selectedControlPoint ].getZ() );
            glVertex3f( controlPoints[ selectedControlPoint ].getX() + selectedControlPointSquareSize/2., controlPoints[ selectedControlPoint ].getY() - selectedControlPointSquareSize/2., controlPoints[ selectedControlPoint ].getZ() );
        glEnd();
    }
}

/* Dessine de la courbe */
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	FOR(i,generalControlVertices.size()){
        std::deque<vec3> curveVertices = chaikin( generalControlVertices[i], 0, 5 );

        drawCurve( curveVertices, generalControlVertices[i], selectedCurve == i );
	}

	glFlush();
}

/* Au cas ou la fenetre est modifiee ou deplacee */
void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-5, 5, -5, 5, -1, 1);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
       // Selecting Control Point
    case '0': case '1': case '2': case '3': case '4': case '5':
        selectedControlPoint = key - '0';
        break;
        // Selecting Curve
    case '7': case '8': case '9':
        if( generalControlVertices.size() > key - '7' ){
            selectedCurve = key - '7';
        }
        break;

    // Moving Control Point
    case 'd':    // move right
       generalControlVertices[selectedCurve][selectedControlPoint].setX( generalControlVertices[selectedCurve][selectedControlPoint].getX()+selectedControlPointMoveStep );
      break;
    case 'q':    // move left
       generalControlVertices[selectedCurve][selectedControlPoint].setX( generalControlVertices[selectedCurve][selectedControlPoint].getX()-selectedControlPointMoveStep );
      break;
    case 'z':    // move up
       generalControlVertices[selectedCurve][selectedControlPoint].setY( generalControlVertices[selectedCurve][selectedControlPoint].getY()+selectedControlPointMoveStep );
      break;
    case 's':    // move down
       generalControlVertices[selectedCurve][selectedControlPoint].setY( generalControlVertices[selectedCurve][selectedControlPoint].getY()-selectedControlPointMoveStep );
      break;

   case ESC:
      exit(0);
      break;
   default :
       break;
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
