#include <iostream>
#include <mpir.h>
#include <mpirxx.h>

#include "../PiChecker.h"
#include "PiOutputter.h"

using namespace std;

mpz_class A = 13591409;
mpz_class B = 545140134;
mpz_class C = 640320;
mpz_class D = 426880;
mpz_class E = 10005;
mpz_class C3_24 = C * C * C / 24;

const double DIGITS_PER_ITERATION = 14.1816474627254776555;
int targetDigits = 1000000;

struct PQT {
	mpz_class P, Q, T;
};

PQT operator+(const PQT &a, const PQT &b) {
	PQT result;
	result.P = a.P * b.P;
	result.Q = a.Q * b.Q;
	result.T = b.Q * a.T + a.P * b.T;
	return result;
}

PQT binaryChudnovsky(int a, int b) {
	PQT result;
	if (b == a + 1) {
		result.P = (6 * b - 5);
		result.P *= (2 * b - 1);
		result.P *= (6 * b - 1);
		result.Q = C3_24*b*b*b;
		result.T = result.P * (A + B * b);
		if (b & 1) {
			result.T = -result.T;
		}
	}
	else {
		int m = (a + b) / 2;
		result = binaryChudnovsky(a, m) + binaryChudnovsky(m, b);
	}
	return result;
}

int main() {
	int iterationsNeeded = ((double)targetDigits / DIGITS_PER_ITERATION)+1;
	PQT result = binaryChudnovsky(0, iterationsNeeded);
	
	auto prec = getPrecisionNeeded(targetDigits);
	mpf_class pi(0, prec);
	pi = D * sqrt((mpf_class)E) * result.Q;
	pi /= (result.Q * A + result.T);
	int digits = countDigits(pi);
	cout << digits << endl;
	system("pause");
}