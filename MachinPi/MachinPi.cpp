#include <iostream>
#include <mpir.h>
#include <mpirxx.h>

#include "../PiChecker.h"

using namespace std;

//Arctan calculated using an infinite series: https://en.wikipedia.org/wiki/Inverse_trigonometric_functions#Infinite_series
class Arctan {
private:
	mpq_class z;
	mpz_class n;
	mpq_class sum;

	//Power function using exponentiation by squaring: https://en.wikipedia.org/wiki/Exponentiation_by_squaring
	mpq_class pow(mpq_class a, mpz_class b) {
		mpq_class result = 1;
		while (b > 0) {
			if (b % 2 == 1) {
				result *= a;
			}
			b >>= 1;
			a *= a;
		}
		return result;
	}
public:
	Arctan(const char* z) {
		this->z = mpq_class(z);
		n = mpz_class(0);
		sum = mpq_class(0);
	}
	mpq_class operator()(int times=1) {
		for (int i = 0; i < times; i++) {
			mpq_class term = 1;
			term.get_den() = (n * 2) + 1;
			term *= pow(z, (n * 2) + 1);
			if (n % 2 == 0) {
				sum += term;
			}
			else {
				sum -= term;
			}
			n++;
		}
		return sum;
	}
};

//Fast calculation for pi using a Machin-like formula: https://en.wikipedia.org/wiki/Approximations_of_%CF%80#Machin-like_formula
int main() {
	cout << "Machin-like formula for pi:" << endl;

	int piCount = 0;
	Arctan one("1/49");
	Arctan two("1/57");
	Arctan three("1/239");
	Arctan four("1/110443");
	while (true) {
		mpq_class sum = one() * 12;
		sum += two() * 32;
		sum -= three() * 5;
		sum += four() * 12;
		sum *= 4;
		mpf_class pi(sum, getPrecisionNeeded(piCount + 100));
		piCount=printCountDigits(pi, piCount);
	}
}