#include <iostream>
#include <mpir.h>
#include <mpirxx.h>

#include "../PiChecker.h"

using namespace std;

//Uses the Leibniz formula for pi: https://en.wikipedia.org/wiki/Leibniz_formula_for_%CF%80

int main() {

	cout << "Leibniz formula for pi:" << endl;

	mpz_class k = 0;
	mpq_class sum = 0;

	int piCount = 0;

	while (true) {
		for (int i = 0; i < 5; i++) {
			mpq_class term = 1;
			term.get_den() = (k * 2) + 1;
			if (k % 2 == 0) {
				sum += term;
			}
			else {
				sum -= term;
			}
			k++;
		}
		mpf_class pi(sum * 4, getPrecisionNeeded(piCount+100));
		piCount = printCountDigits(pi, piCount);
	}
}