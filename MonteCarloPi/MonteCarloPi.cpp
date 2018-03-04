#include <iostream>
#include <random>

#include "../PiChecker.h"

using namespace std;

//Calculating pi using the Monte Carlo method: https://en.wikipedia.org/wiki/Monte_Carlo_method

int main() {
	cout << "Monte Carlo pi method:" << endl;

	int hits = 0;
	int misses = 0;

	int piCount = 0;

	default_random_engine e;
	uniform_real_distribution<double> dist;

	while (true) {
		for (int i = 0; i < 100000; i++) {
			double x = dist(e);
			double y = dist(e);
			if (sqrt(pow(x, 2) + pow(y, 2)) <= 1) hits++;
			else misses++;
		}
		double pi = 4.0*((double)hits / (double)(hits + misses));
		piCount = printCountDigits(pi, piCount);
	}
}