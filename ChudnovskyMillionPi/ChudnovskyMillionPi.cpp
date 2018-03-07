#include <iostream>
#include <mpir.h>
#include <mpirxx.h>
#include <thread>
#include <atomic>
#include <mutex>

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

//Prevent multiple threads from writting and reading from mpq_class S at the same time
mutex sumGuard;

//Super-Fast calculation for pi using the Chudnovsky algorithm: https://en.wikipedia.org/wiki/Chudnovsky_algorithm
int main() {
	cout << "Chudnovsky algorithm for pi to one million digits:" << endl;

	atomic<int> piCount = 0;

	mpz_class K = 6;
	mpq_class M = 1;
	mpz_class M1;
	mpz_class M2;
	mpz_class L = 13591409;
	mpz_class X = 1;
	mpq_class S = 13591409;

	mpz_class k = 1;

	thread t1([&]() {
		while (piCount < 1000000) {
			M1 = pow(K, (mpz_class)3);
			M2 = pow(k, (mpz_class)3);
			M = (mpq_class)(M1 - (K << 4))*M / M2;
			L += 545140134;
			X *= -262537412640768000;
			K += 12;
			k++;

			sumGuard.lock();
			S += (M*L) / X;
			sumGuard.unlock();
		}
	});

	while (piCount<1000000) {
		
		auto prec = getPrecisionNeeded(piCount + 100);
		mpf_class Q(10005, prec);
		Q = sqrt(Q) * 426880;

		sumGuard.lock();
		mpf_class pi(Q / S, prec);
		sumGuard.unlock();

		piCount = piPrintCount(pi, piCount);

		//this_thread::sleep_for(chrono::milliseconds(10));

	}

	cout << endl << "One million digits of pi have been calculated! Happy pi day! :)" << endl;
	system("pause");
}