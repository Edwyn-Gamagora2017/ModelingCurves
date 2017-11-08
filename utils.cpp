#include "utils.h"

// Comparaison entre deux valeurs du type double (precision de 4 chifres)
bool equalDouble(double value1, double value2) {
	return ((value1 - value2 < 0.0001) && (value2 - value1 < 0.0001));
}

std::string doubleToStr( double value ){
    std::ostringstream o;
    o << value;
    return o.str();
}

double clamp(double valeur, double min, double max) {
	if (valeur < min) {
		return min;
	}
	else{
		if (valeur > max) {
			return max;
		}
		else
		{
			return valeur;
		}
	}
}

/*vec3 clamp(vec3 valeur, double min, double max)
{
	return vec3(clamp(valeur.getX(), min, max), clamp(valeur.getY(), min, max), clamp(valeur.getZ(), min, max));
}*/
