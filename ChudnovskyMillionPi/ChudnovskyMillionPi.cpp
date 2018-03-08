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

//SNode
struct SNode {
	mpq_class S;
	SNode* next;
};
SNode* SHead = nullptr;
SNode* STail = nullptr;
mutex StailLock;
mutex SheadLock;
atomic<int> SnodeCount = 0;
void addS(const mpq_class &value) {
	static SNode* setHead = nullptr;
	SNode* node = new SNode();
	node->next = nullptr;
	node->S = value;
	StailLock.lock();
	if (STail == nullptr) {
		SheadLock.lock();
		SHead = node;
		setHead = node;
		SheadLock.unlock();
		STail = node;
	}
	else {
		bool isSame = STail == setHead;
		if (isSame)
			SheadLock.lock();
		STail->next = node;
		STail = node;
		if (isSame)
			SheadLock.unlock();
	}
	SnodeCount++;
	StailLock.unlock();
}
mpq_class getDeltaS() {
	SheadLock.lock();
	while (SHead == nullptr || SHead->next == nullptr) {
		SheadLock.unlock();
		this_thread::sleep_for(chrono::milliseconds(10));
		SheadLock.lock();
	}
	mpq_class result = SHead->S;
	SNode* toDelete = SHead;
	SHead = SHead->next;
	delete toDelete;
	SnodeCount--;
	if (SHead == nullptr) {
		StailLock.lock();
		STail = nullptr;
		StailLock.unlock();
	}
	SheadLock.unlock();
	return result;
}
//SNode

//Prevent multiple threads from writting and reading from mpq_class S at the same time
mutex sumGuard;

//Super-Fast calculation for pi using the Chudnovsky algorithm: https://en.wikipedia.org/wiki/Chudnovsky_algorithm
int main() {
	cout << "Chudnovsky algorithm for pi to one million digits:" << endl;

	atomic<int> piCount = 0;

	mpq_class S = 13591409;

	//Compute M values and Partial Sum deltas
	thread t0([&]() {
		mpz_class k = 1;
		mpz_class K = 6;
		mpz_class L = 13591409;
		mpz_class M1;
		mpz_class M2;
		mpq_class M=1;
		while (piCount < 1000000) {
			while (!(SnodeCount < 3)) this_thread::sleep_for(chrono::milliseconds(1));
			M1 = pow(K, (mpz_class)3);
			M2 = pow(k, (mpz_class)3);
			M=(mpq_class)(M1 - (K << 4))*(M / M2);

			L += 545140134;
			addS(M*L);

			K += 12;
			k++;
		}
	});

	//Compute Sum values from M values and partial Sum deltas
	thread t1([&]() {
		mpz_class k = 1;
		mpz_class X = 1;
		while (piCount < 1000000) {

			X *= -262537412640768000;
			
			k++;

			sumGuard.lock();
			S += getDeltaS() / X;
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


	}
	t0.join();
	t1.join();

	cout << endl << "One million digits of pi have been calculated! Happy pi day! :)" << endl;
	system("pause");
}