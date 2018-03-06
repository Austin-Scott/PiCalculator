#include <iostream>
#include <mpir.h>
#include <mpirxx.h>

#include "../PiChecker.h"
#include "PiOutputter.h"

using namespace std;

//Power function using exponentiation by squaring: https://en.wikipedia.org/wiki/Exponentiation_by_squaring
mpz_class pow(mpz_class a, mpz_class b) {
	mpz_class result = 1;
	while (b > 0) {
		if (b % 2 == 1) {
			result *= a;
		}
		b >>= 1;
		a *= a;
	}
	return result;
}

//Super-Fast calculation for pi using the Chudnovsky algorithm: https://en.wikipedia.org/wiki/Chudnovsky_algorithm
int main() {
	cout << "Chudnovsky algorithm for pi to one million digits:" << endl;

	int piCount = 0;

	mpz_class K = 6;
	mpq_class M = 1;
	mpz_class L = 13591409;
	mpz_class X = 1;
	mpq_class S = 13591409;

	mpz_class k = 1;

	while (piCount<1000000) {
		M = (mpq_class)(pow(K, (mpz_class)3) - (K << 4))*M / pow(k, (mpz_class)3);
		L += 545140134;
		X *= -262537412640768000;
		S += (M*L) / X;
		K += 12;
		k++;

		auto prec = getPrecisionNeeded(piCount + 100);
		mpf_class Q(10005, prec);
		Q = sqrt(Q) * 426880;
		mpf_class pi(Q / S, prec);
		piCount = piPrintCount(pi, piCount);

	}

	cout << endl << "One million digits of pi have been calculated! Happy pi day! :)" << endl;
	system("pause");
}