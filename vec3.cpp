#include "vec3.h"
#include "math.h"
#include "utils.h"
using namespace std;

vec3::vec3(double x, double y, double z) {
	this->set(x, y, z);
}

vec3::vec3() : vec3(0, 0, 0) {
}

vec3::~vec3() {
}

double vec3::getX() {
	return this->x;
}
double vec3::getY() {
	return this->y;
}
double vec3::getZ() {
	return this->z;
}

void vec3::set(double x, double y, double z) {
	this->setX( x );
	this->setY( y );
	this->setZ( z );
}
void vec3::setX(double x) {
	this->x = x;
}
void vec3::setY(double y) {
	this->y = y;
}
void vec3::setZ(double z) {
	this->z = z;
}

vec3 vec3::addition(vec3 a) {
	return vec3( this->getX() + a.getX(), this->getY() + a.getY(), this->getZ() + a.getZ() );
}
vec3 vec3::negative() {
	return this->multiplication( -1 );
}
vec3 vec3::soustraction(vec3 a) {
	return this->addition( a.negative() );
}
vec3 vec3::multiplication(vec3 a) {
	return vec3( this->getX() * a.getX(), this->getY() * a.getY(), this->getZ() * a.getZ() );
}
vec3 vec3::multiplication(double valeur) {
	return vec3( this->getX() * valeur, this->getY() * valeur, this->getZ() * valeur );
}
vec3 vec3::division(vec3 a){
	return vec3(this->getX() / a.getX(), this->getY() / a.getY(), this->getZ() / a.getZ());
}
vec3 vec3::division( double valeur) {
	return vec3( this->getX() / valeur, this->getY() / valeur, this->getZ() / valeur );
}
double vec3::norme(){
	return sqrt( this->getX()*this->getX() + this->getZ()*this->getZ() + this->getY()*this->getY());
}
double vec3::normeCarre(){
	double norme = this->norme();
	return norme * norme;
}
vec3 vec3::normalized(){
	return this->division( this->norme() );
}
double vec3::produitScalaire( vec3 a ){
	return this->getX() * a.getX() + this->getY() * a.getY() + this->getZ() * a.getZ();
}
vec3 vec3::produitVectoriel(vec3 a){
	return vec3(	this->getY() * a.getZ() - a.getY() * this->getZ(),
                    this->getX() * a.getZ() - a.getX() * this->getZ(),
                    this->getX() * a.getY() - a.getX() * this->getY());
}

vec3 vec3::vectorFrom( vec3 origin )
{
    return this->soustraction( origin );
}

vec3 vec3::normal( double angleDegrees)
{
	return (this->multiplication(cos(angleDegrees * PI / 180))).multiplication(-1.).normalized();
	//return (direction.multiplication(cos(angleDegrees * PI / 180))).normalized();
}

std::string vec3::toString(){
	return "( " + doubleToStr(this->getX()) + " , " + doubleToStr(this->getY()) + " , " + doubleToStr(this->getZ()) + " )";
}
