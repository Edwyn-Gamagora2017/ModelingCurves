#include <string>

#pragma once
#define PI 3.14159265

class vec3
{
private:
	double x = 0, y = 0, z = 0;	// coordonnees du vec3

public:
	vec3();
	vec3( double x, double y, double z );
	~vec3();

	double getX();
	double getY();
	double getZ();

	void set( double x, double y, double z );
	void setX( double x );
	void setY( double y );
	void setZ( double z );

	vec3 addition( vec3 a );
	vec3 negative();
	vec3 soustraction( vec3 a );
	vec3 multiplication( vec3 a );
	vec3 multiplication( double valeur );
	vec3 division( vec3 a );
	vec3 division( double valeur );
	double norme();
	double normeCarre();
	vec3 normalized();
	double produitScalaire( vec3 a );
	vec3 produitVectoriel( vec3 a );

	vec3 vectorFrom( vec3 origin );

	vec3 normal( double angleDegrees );

	std::string toString();
};

