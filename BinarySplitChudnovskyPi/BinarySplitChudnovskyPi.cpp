#include <iostream>
#include <mpir.h>
#include <mpirxx.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <cassert>

#include "../PiChecker.h"

#define DIGITS_PER_ITERATION 14.1816474627254776555
#define targetDigits 1000000000

#include "PiOutputter.h"

using namespace std;

mpz_class A = 13591409;
mpz_class B = 545140134;
mpz_class C = 640320;
mpz_class D = 426880;
mpz_class E = 10005;
mpz_class C3_24 = C * C * C / 24;

struct PQT {
	mpz_class P, Q, T;
	PQT() {
		P = 0;
		Q = 0;
		T = 0;
	}
};

PQT operator+(const PQT &a, const PQT &b) {
	if (a.P == 0&&a.Q == 0&&a.T == 0) return b;
	PQT result;
	result.P = a.P * b.P;
	result.Q = a.Q * b.Q;
	result.T = b.Q * a.T + a.P * b.T;
	return result;
}

//Binary splitting (aka divide and conquer) variant of the Chudnovsky Algorithm for pi: https://en.wikipedia.org/wiki/Chudnovsky_algorithm
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
		//Stores the number of times we will need to iterate before pi is computed to the target number of digits
	int iterationsNeeded = ((double)targetDigits / DIGITS_PER_ITERATION)+1; 
		//Stores number of digits that have been printed out
	atomic<int> piPrintCount = 0; 
		//Stores number of digits that have been calculated and coverted to string
	atomic<int> piCalcCount = 0; 
		//Span size of iterations that the divide and conquer algorithm covers
		//The larger this number is the more memory will be needed but the faster we will converge on pi.
	atomic<int> iterationsPerBlock = 1000000; 
		//Stores number of times Chudnovsky Algorithm has been iterated.
	atomic<int> iterations=0; 
		//Stores the time it took to compute the last Chudnovsky block in milliseconds
	atomic<long> blockIterationTimeMilli = 0;
		//Stores the current PQT sum of all blocks from the main thread.
	PQT result;

	auto prec = getPrecisionNeeded(targetDigits);
	mpf_class pi(0);

	cout << "Computing sqrt(10005) to " << formatNumber(targetDigits) << " decimal digits..." << endl;
	mpf_class sqrtE(E, prec);
	sqrtE = sqrt(sqrtE);
	cout << "Done." << endl;

	system("pause");

	cout << "Behold the power of pi..." << endl;

	mutex piStrGuard;

	string piStr;
	string piSubStr;

	//Worker thread to print to console while main thread continues pi calculation
	thread t0([&]() {
		while (piPrintCount < targetDigits) {
			
			piStrGuard.lock();
			if(piStr.size()!=0)
				piSubStr = piStr.substr(piPrintCount, piCalcCount - piPrintCount);
			piStrGuard.unlock();

			piPrintCount = piPrintChudnovsky(piSubStr,piPrintCount,blockIterationTimeMilli);
		}
	});

	auto lastTime = chrono::system_clock::now();
	while (iterations<iterationsNeeded) {
		result = result + binaryChudnovsky(iterations, iterations + iterationsPerBlock);
		iterations += iterationsPerBlock;

		pi.set_prec(getPrecisionNeeded((int)((double)iterations*DIGITS_PER_ITERATION + 100)));
		pi = D * sqrtE * result.Q;
		pi /= (result.Q * A + result.T);
		
		piStrGuard.lock();
		piStr = to_string(pi);
		piCalcCount = DIGITS_PER_ITERATION*iterations;
		if (piCalcCount > targetDigits)
			piCalcCount = targetDigits;
		piStrGuard.unlock();

		blockIterationTimeMilli = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - lastTime).count();
		lastTime = chrono::system_clock::now();
		
	}

	t0.join();

	printConclusion();

	system("pause");
}