#include"struct.h"


/**********************************************************************/
/**********************************************************************/
/*                                                                    */
/*       methodes de la classe point3                                 */
/*                                                                    */
/**********************************************************************/
/**********************************************************************/
point3::point3()
{	x=0; y=0; z=0;	}

point3::point3(const double X,const double Y,const double Z)
{	x=X; y=Y; z=Z;	}

bool
point3::operator==(const point3 &op)const
{	return(x==op.x && y==op.y && z==op.z);	}

point3&
point3::operator=(const point3 &op)
{	x=op.x; y=op.y; z=op.z; return *this;	}



point3
point3::operator+(const point3 &op)const
{	return(point3(x+op.x,y+op.y,z+op.z));	}


point3&
point3::operator*=(const double op)
{	x*=op; y*=op; z*=op; return *this;	}

point3
point3::operator*(const double op)const
{	return(point3(x*op,y*op,z*op));	}

point3&
point3::operator/=(const double op)
{	x/=op; y/=op; z/=op; return *this;	}

point3
point3::operator/(const double op)const
{	return(point3(x/op,y/op,z/op));	}


void
point3 :: Permutation(point3 *A, point3 *B)
{ point3 PEch; 
  PEch = (*A); 
  (*A) = (*B);
  (*B) = PEch;
}

ostream&  operator<<(ostream& p, point3 op)
{
	p << "(" << op.x <<", " << op.y << "," << op.z  << ")";
	return(p);
}

istream&  operator>>(istream& p, point3 &op)
{
	cout << "Entrez x:";
	p >> op.x;
	cout << "Entrez y:";
	p >> op.y;
	cout << "Entrez z:";
	p >> op.z;

	return (p);
}

