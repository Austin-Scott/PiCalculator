#pragma once
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#include <chrono>

#include "../PiChecker.h"

using namespace std;

void printBox(string value) {
	const char topLeft = 201;
	const char topRight = 187;
	const char bottomLeft = 200;
	const char bottomRight = 188;
	const char topBottom = 205;
	const char leftRight = 186;

	stringstream buffer;
	buffer << value;
	vector<string> lines;
	while (!buffer.eof()) {
		string line = "";
		getline(buffer, line);
		lines.push_back(line);
	}

	int maxLength = 0;
	for (string s : lines) {
		if (s.length() > maxLength) maxLength = s.length();
	}

	string floor = "";
	floor.assign(maxLength, topBottom);

	string top = "";
	top.push_back(topLeft);
	top += floor;
	top.push_back(topRight);

	cout << top << endl;

	for (string s : lines) {
		cout << leftRight << setw(maxLength) << left << s << leftRight << endl;
	}

	string bottom = "";
	bottom.push_back(bottomLeft);
	bottom += floor;
	bottom.push_back(bottomRight);

	cout << bottom << endl;

}

string formatElapsedTime(long seconds) {
	if (seconds < 0) seconds = 0;

	auto minutes = seconds / 60;
	seconds %= 60;
	auto hours = minutes / 60;
	minutes %= 60;
	auto days = hours / 24;
	hours %= 24;

	string result;
	if (days > 0) result += to_string(days) + " days, ";
	if (hours > 0) result += to_string(hours) + " hrs, ";
	if (minutes > 0) result += to_string(minutes) + " min, ";
	result += to_string(seconds) + " sec";

	return result;
}

string formatNumber(int num) {
	string result;
	string temp = to_string(num);
	for (int i = 0; i < temp.size(); i++) {
		result.push_back(temp[i]);
		if (i!=temp.size()-1 && (temp.size() - (i+1)) % 3 == 0) {
			result.push_back(',');
		}
	}
	return result;
}

template<class T> string formatElapsedTime(T time) {
	long seconds = time.count();
	return formatElapsedTime(seconds);
}

template<class T> void printStatus(int numOfDigits, T elapsedTime, int targetDigits) {
	static int lastNumOfDigits = numOfDigits;
	int deltaDigits = numOfDigits - lastNumOfDigits;
	lastNumOfDigits = numOfDigits;

	int digitsToGo = targetDigits - numOfDigits;
	
	static auto startTime = chrono::system_clock::now();
	auto elapsed = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - startTime);
	startTime = chrono::system_clock::now();

	double digitsPerMilli = (double)deltaDigits / (double)elapsed.count();
	long milliToGo = digitsToGo / digitsPerMilli;
	long secToGo = milliToGo / 1000;

	string result;
	result += "Number of digits: " + formatNumber(numOfDigits);
	result += " | Elapsed time: " + formatElapsedTime(elapsedTime);
	result += " | Time remaining: " + formatElapsedTime(secToGo);
	printBox(result);
	cout << endl;
}

const string piShape =
"***********************************************************************************************************************\n"
"***********************************************************************************************************************\n"
"***********************************************************************************************************************\n"
"***********************************************************************************************************************\n"
"**********************                                ***************                                ******************\n"
"********************                                  *************                                  ******************\n"
"*******************                                   ************                                   ******************\n"
"*******************    ****     *********    *********************    ****     *********    ***************************\n"
"******************   ******     ********     ********************   ******     ********     ***************************\n"
"*****************  ********     ********     *******************  ********     ********     ***************************\n"
"*****************  ********     ********    ********************  ********     ********    ****************************\n"
"***************************    *********    ******************************    *********    ****************************\n"
"***************************    *********    ******************************    *********    ****************************\n"
"***************************    *********    ******************************    *********    ****************************\n"
"**************************     ********     *****************************     ********     ****************************\n"
"**************************     ********     *****************************     ********     ****************************\n"
"**************************    *********     *****************************    *********     ****************************\n"
"**************************    *********     *****************************    *********     ****************************\n"
"*************************     *********     ****************************     *********     ****************************\n"
"************************      *********     ***************************      *********     ****************************\n"
"************************     **********     ********  *****************     **********     ********  ******************\n"
"***********************       *********      *******  ****************       *********      *******  ******************\n"
"**********************       ***********              ***************       ***********              ******************\n"
"*********************       ************             ***************       ************             *******************\n"
"*********************       *************           ****************       *************           ********************\n"
"*********************      ***************         *****************      ***************         *********************\n"
"***********************************************************************************************************************\n"
"***********************************************************************************************************************\n"
"***********************************************************************************************************************\n"
"***********************************************************************************************************************\n"
"\n";

//prints out pi as it converges from the Chudnovsky algorithm with nice formatting and status updates
int piPrintChudnovsky(const string &pi, int iterations, int startPos, int targetDigits) {
	static auto startTime = chrono::system_clock::now();

	int initialStartPos = startPos;
	startPos = 14.1816474627254776555 * (double)iterations;
	if (startPos > targetDigits) startPos = targetDigits;

	if (pi.size() == 0) {
		this_thread::sleep_for(chrono::milliseconds(1000));
		return 0;
	}

	static int printIndex = 0;
	static int shapesBeforeStatus = 20;
	bool printDecimal = false;
	for (int i = initialStartPos; i < startPos; i++) {
		while (printIndex < piShape.size() && piShape[printIndex] != '*') {
			cout << piShape[printIndex];
			printIndex++;
		}
		if (printIndex != piShape.size()) {
			if (i == 1 && !printDecimal) {
				cout << '.';
				printDecimal = true;
				printIndex++;
				i--;
				continue;
			}
			cout << pi[i];

			printIndex++;
		}
		else {
			shapesBeforeStatus--;
			if (shapesBeforeStatus <= 0) {
				auto elapsed = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - startTime);
				printStatus(initialStartPos + (i - initialStartPos), elapsed, targetDigits);
				this_thread::sleep_for(chrono::milliseconds(1000));
				shapesBeforeStatus = 20;
			}
			i--;
			printIndex = 0;
			continue;
		}
	}

	if (startPos == targetDigits) {
		cout << endl;
		auto elapsed = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - startTime);
		printStatus(targetDigits, elapsed, targetDigits);
	}

	return startPos;
}