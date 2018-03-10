#include <iostream>
#include <mpir.h>
#include <mpirxx.h>
#include <thread>
#include <atomic>
#include <mutex>

#include "../PiChecker.h"
#include "PiOutputter.h"

using namespace std;

//Super-Fast calculation for pi using the Chudnovsky algorithm: https://en.wikipedia.org/wiki/Chudnovsky_algorithm
int main() {
	cout << "Chudnovsky algorithm for pi to one million digits:" << endl;

	atomic<int> piCount = 0;

	mpq_class EM = 83160;
	atomic<int> Ek = 2;

	mpq_class OM = 1;
	atomic<int> Ok = 1;

	mutex mGuard;

	mpq_class S = 13591409;
	mutex sumGuard;

	//Odd k values
	thread t1([&]() {
		int K = 6;
		mpz_class M1 = 0;
		mpz_class M2 = 0;
		mpz_class L = 13591409;
		mpz_class X = 1;

		while (piCount<1000000) {
			mpz_ui_pow_ui(M1.get_mpz_t(), K, 3);
			mpz_ui_pow_ui(M2.get_mpz_t(), Ok, 3);
			while (Ek != Ok + 1) this_thread::sleep_for(chrono::microseconds(1));
			mGuard.lock();
			OM = (mpq_class)(M1 - ((mpz_class)K << 4))*EM / M2;
			mGuard.unlock();
			Ok += 2;
			L += 1090280268;
			X *= mpz_class("68925893036108889235415629824000000");
			sumGuard.lock();
			S += (OM*L) / X;
			sumGuard.unlock();
			K += 24;
		}
	});

	//Even k values and printing
	int K = 18;
	mpz_class M1 = 0;
	mpz_class M2 = 0;
	mpz_class L = 558731543;
	mpz_class X = -262537412640768000;
	
	while (piCount<1000000) {
		mpz_ui_pow_ui(M1.get_mpz_t(), K, 3);
		mpz_ui_pow_ui(M2.get_mpz_t(), Ek, 3);
		while (Ok != Ek + 1) this_thread::sleep_for(chrono::microseconds(1));
		mGuard.lock();
		EM = (mpq_class)(M1 - ((mpz_class)K << 4))*OM / M2;
		mGuard.unlock();
		Ek += 2;
		L += 1090280268;
		X *= mpz_class("68925893036108889235415629824000000");
		sumGuard.lock();
		S += (EM*L) / X;
		sumGuard.unlock();
		K += 24;
		
		piCount = piPrintChudnovsky(Ek, piCount);
	}

	cout << endl << "One million digits of pi have been calculated! Happy pi day! :)" << endl;
	system("pause");
}