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

int nCurves = 3;            // Amount of curves
bool isBernstein = true;   // Bernstein or Casteljau

float tx=0.0;
float ty=0.0;
int selectedCurve = 0;
int selectedControlPoint = 0;
double selectedControlPointSquareSize = 0.2;
double selectedControlPointMoveStep = 0.2;

vec3 p0( -2,-2,0 );
vec3 p1( -1,1,0 );
vec3 p2( 1,1,0 );
vec3 p3( 2,-2,0 );
vec3 p4( 0,2,0 );
vec3 v1( 1,5,0 );
vec3 v2( 1,-5,0 );
std::deque< std::deque<vec3> > bernsteinControlVertices;
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

// calculate the position on the Bezier curve (Bernstein) related to the factor u [0,1]
vec3 bernstein( double u, std::deque<vec3> controlPoints ){
    // initializing the result with the first point
    vec3 result = controlPoints[0].multiplication( getBernsteinB( controlPoints.size()-1, 0, u ) );
    for( int i = 1; i<controlPoints.size(); i++ ){
        result = result.addition( controlPoints[i].multiplication( getBernsteinB( controlPoints.size()-1, i, u ) ) );
    }
    return result;
}

// calculate the Bezier curve based on Bernstein algorithm. amountSamples defines the amount of samples in the curve
std::deque<vec3> bernstein( std::deque<vec3> controlPoints, int amountSamples ){
    int amount = amountSamples+2;   // at least 2 samples will be created
    std::deque<vec3> result;
    for( int i=0; i<amount; i++ ){
        result.push_back( bernstein( i/((double)amount-1), controlPoints ) );
    }
    return result;
}

// Set the position of the point 1 to the point 2
void adjustContinuity( vec3 * controlPoint1, vec3 * controlPoint2 ){
    controlPoint2->setX(controlPoint1->getX());
    controlPoint2->setY(controlPoint1->getY());
    controlPoint2->setZ(controlPoint1->getZ());
}

// Set the inverse of the position of the point 1 to the point 2 (relative to the center)
void adjustContinuityTangent( vec3 * centerPoint, vec3 * controlPoint1, vec3 * controlPoint2 ){
    vec3 distance = controlPoint1->soustraction( *centerPoint );

    controlPoint2->setX(centerPoint->getX()-distance.getX());
    controlPoint2->setY(centerPoint->getY()-distance.getY());
    controlPoint2->setZ(centerPoint->getZ()-distance.getZ());
}

// calculate the intermediate k position on the Bezier curve (Casteljau) related to the factor u [0,1]
vec3 casteljauP( double u, int k, int i, std::deque<vec3> controlPoints ){
    if( k == 0 ){
        return controlPoints[i];
    }
    else{
        return casteljauP( u, k-1, i, controlPoints ).multiplication( 1-u ).addition( casteljauP( u, k-1, i+1, controlPoints ).multiplication( u ) );
    }
}

// calculate the Bezier curve based on Casteljau algorithm. amountSamples defines the amount of samples in the curve
std::deque<vec3> casteljau( std::deque<vec3> controlPoints, int amountSamples ){
    int amount = amountSamples+2;   // at least 2 samples will be created
    std::deque<vec3> result;
    for( int i=0; i<amount; i++ ){
        result.push_back( casteljauP( i/((double)amount-1), controlPoints.size()-1, 0, controlPoints ) );
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

	// setting bernstein control vertices
	FOR(i,nCurves)
	{
        std::deque<vec3> controlPoints;

        vec3 translate( (p3.getX()-p0.getX())*i,0,0 );  // translate the curve
        vec3 translateInverse( 0, (p1.getY()-p0.getY())*(i%2==0?0:-2),0 );  // translate the curve

        controlPoints.push_back( p0.addition( translate ) );
        controlPoints.push_back( p1.addition( translate ).addition( translateInverse ) );
        controlPoints.push_back( p2.addition( translate ).addition( translateInverse ) );
        controlPoints.push_back( p3.addition( translate ) );

        bernsteinControlVertices.push_back( controlPoints );

        // adjust continuity
        if( i > 0 ){
            adjustContinuity( &bernsteinControlVertices[i-1][bernsteinControlVertices[i-1].size()-1], &bernsteinControlVertices[i][0] );
            adjustContinuityTangent( &bernsteinControlVertices[i-1][bernsteinControlVertices[i-1].size()-1], &bernsteinControlVertices[i-1][bernsteinControlVertices[i-1].size()-2], &bernsteinControlVertices[i][1] );
        }
	}
}

void drawCurve(std::deque<vec3> bernsteinVertices, std::deque<vec3> controlPoints, bool isSelected){
    // calculate hermite curve
	std::deque<vec3> hermiteVertices = hermite( controlPoints[0],
                                                controlPoints[3],
                                                controlPoints[1].soustraction( controlPoints[0] ),
                                                controlPoints[controlPoints.size()-1].soustraction( controlPoints[controlPoints.size()-2] ),
                                                10
                                                );

	// Print Hermite Curve
	glBegin(GL_LINE_STRIP);
	glColor3f(1.,1.,1.);
	for( int i = 0; i < hermiteVertices.size(); i++ ){
        glVertex3f( hermiteVertices[i].getX(), hermiteVertices[i].getY(), hermiteVertices[i].getZ() );
	}
	glEnd();

	// Print Bernstein Control Box
	glBegin(GL_LINE_LOOP);
	//glBegin(GL_POLYGON);
	glColor3f(1.,0.,0.);
	for( int i = 0; i < controlPoints.size(); i++ ){
        glVertex3f( controlPoints[i].getX(), controlPoints[i].getY(), controlPoints[i].getZ() );
	}
	/*std::deque<vec3> box = boxPoints( bernsteinControlVertices );
	for( int i = 0; i < box.size(); i++ ){
        glVertex3f( box[i].getX(), box[i].getY(), box[i].getZ() );
	}*/
	glEnd();

	// Print Bezier Curve (Bernstein)
	glBegin(GL_LINE_STRIP);
	glColor3f(0.,1.,0.);
	for( int i = 0; i < bernsteinVertices.size(); i++ ){
        glVertex3f( bernsteinVertices[i].getX(), bernsteinVertices[i].getY(), bernsteinVertices[i].getZ() );
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

	FOR(i,bernsteinControlVertices.size()){
        // adjust continuity
        // HERE it blocks modifications on the begin of the curves
        /*if( i > 0 ){
            adjustContinuity( &bernsteinControlVertices[i-1][bernsteinControlVertices[i-1].size()-1], &bernsteinControlVertices[i][0] );
            adjustContinuityTangent( &bernsteinControlVertices[i-1][bernsteinControlVertices[i-1].size()-1], &bernsteinControlVertices[i-1][bernsteinControlVertices[i-1].size()-2], &bernsteinControlVertices[i][1] );
        }*/

        std::deque<vec3> bernsteinVertices;
        if( isBernstein ){
            //calculate bezier curve (bernstein)
            bernsteinVertices = bernstein( bernsteinControlVertices[i], 10 );
        }else{
            // calculate bezier curve (casteljau)
            bernsteinVertices = casteljau( bernsteinControlVertices[i], 10 );
        }

        drawCurve( bernsteinVertices, bernsteinControlVertices[i], selectedCurve == i );
	}

	glFlush();
}

/* Au cas ou la fenetre est modifiee ou deplacee */
void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-3, 11, -7, 7, -1, 1);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
       // Selecting Control Point
    case '0': case '1': case '2': case '3':
        selectedControlPoint = key - '0';
        break;
        // Selecting Curve
    case '7': case '8': case '9':
        if( bernsteinControlVertices.size() > key - '7' ){
            selectedCurve = key - '7';
        }
        break;

    // Moving Control Point
    case 'd':    // move right
       bernsteinControlVertices[selectedCurve][selectedControlPoint].setX( bernsteinControlVertices[selectedCurve][selectedControlPoint].getX()+selectedControlPointMoveStep );
      break;
    case 'q':    // move left
       bernsteinControlVertices[selectedCurve][selectedControlPoint].setX( bernsteinControlVertices[selectedCurve][selectedControlPoint].getX()-selectedControlPointMoveStep );
      break;
    case 'z':    // move up
       bernsteinControlVertices[selectedCurve][selectedControlPoint].setY( bernsteinControlVertices[selectedCurve][selectedControlPoint].getY()+selectedControlPointMoveStep );
      break;
    case 's':    // move down
       bernsteinControlVertices[selectedCurve][selectedControlPoint].setY( bernsteinControlVertices[selectedCurve][selectedControlPoint].getY()-selectedControlPointMoveStep );
      break;

   case ESC:
      exit(0);
      break;
   default :
       break;
   }

    // adjust continuity
    if( selectedCurve < bernsteinControlVertices.size()-1 && selectedControlPoint == bernsteinControlVertices[selectedCurve].size()-1 ){
        adjustContinuity( &bernsteinControlVertices[selectedCurve][selectedControlPoint], &bernsteinControlVertices[selectedCurve+1][0] );
        adjustContinuityTangent( &bernsteinControlVertices[selectedCurve][selectedControlPoint], &bernsteinControlVertices[selectedCurve][selectedControlPoint-1], &bernsteinControlVertices[selectedCurve+1][1] );
    }
    else if( selectedCurve > 0 && selectedControlPoint == 0 ){
        adjustContinuity( &bernsteinControlVertices[selectedCurve][selectedControlPoint], &bernsteinControlVertices[selectedCurve-1][bernsteinControlVertices[selectedCurve-1].size()-1] );
        adjustContinuityTangent( &bernsteinControlVertices[selectedCurve][selectedControlPoint], &bernsteinControlVertices[selectedCurve][selectedControlPoint+1], &bernsteinControlVertices[selectedCurve-1][bernsteinControlVertices[selectedCurve-1].size()-2] );
    }
    else if( selectedCurve < bernsteinControlVertices.size()-1 && selectedControlPoint == bernsteinControlVertices[selectedCurve].size()-2 ){
        adjustContinuityTangent( &bernsteinControlVertices[selectedCurve][selectedControlPoint+1], &bernsteinControlVertices[selectedCurve][selectedControlPoint], &bernsteinControlVertices[selectedCurve+1][1] );
    }
    else if( selectedCurve > 0 && selectedControlPoint == 1 ){
        adjustContinuityTangent( &bernsteinControlVertices[selectedCurve][selectedControlPoint-1], &bernsteinControlVertices[selectedCurve][selectedControlPoint], &bernsteinControlVertices[selectedCurve-1][bernsteinControlVertices[selectedCurve-1].size()-2] );
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
