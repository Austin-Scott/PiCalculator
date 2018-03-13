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

template<class T> void printStatus(int numOfDigits, T elapsedTime) {
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

int getDigitsPerShape() {
	int result = 0;
	for (char c : piShape) {
		if (c == '*') result++;
	}
	return result;
}

const int shapesToPrint = 20;
const int statusDelayMilli = 1000;

//prints out pi as it converges from the Chudnovsky algorithm with nice formatting and status updates
int piPrintChudnovsky(const string &subStrPi, int startPos, long durationMilli) {
	static auto timeAtStart = chrono::system_clock::now();
	static int digitsPerShape = getDigitsPerShape();
	static int printIndex = 0;
	static int shapesBeforeStatus = shapesToPrint;

	if (startPos > targetDigits) return targetDigits;

	if (subStrPi.size() == 0) {
		this_thread::sleep_for(chrono::milliseconds(1000));
		return startPos;
	}

	int digitsToPrint = subStrPi.size();
	int digitsPerStatus = digitsPerShape*shapesToPrint;
	int statusesToPrint = digitsToPrint / digitsPerStatus;
	int durationLeft = durationMilli - (statusesToPrint*statusDelayMilli);
	if (durationLeft < 0)
		durationLeft = 0;
	int printDelay = durationLeft / digitsToPrint;
	int statusDelay = statusDelayMilli;
	if (printDelay == 0 && statusesToPrint != 0) {
		statusDelay = durationMilli/statusesToPrint;
	}


	bool printDecimal = false;
	for (int i = 0; i < digitsToPrint; i++) {
		while (printIndex < piShape.size() && piShape[printIndex] != '*') {
			cout << piShape[printIndex];
			printIndex++;
		}
		if (printIndex != piShape.size()) {
			if (startPos==0 && i == 1 && !printDecimal) {
				cout << '.';
				printDecimal = true;
				printIndex++;
				i--;
				continue;
			}
			cout << subStrPi[i];
			this_thread::sleep_for(chrono::milliseconds(printDelay));

			printIndex++;
		}
		else {
			shapesBeforeStatus--;
			if (shapesBeforeStatus <= 0) {
				auto elapsed = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - timeAtStart);
				printStatus(startPos + i, elapsed);
				this_thread::sleep_for(chrono::milliseconds(statusDelay));
				shapesBeforeStatus = shapesToPrint;
			}
			i--;
			printIndex = 0;
			continue;
		}
	}

	int endPos = startPos + digitsToPrint;

	if (endPos == targetDigits) {
		cout << endl;
		auto elapsed = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - timeAtStart);
		printStatus(targetDigits, elapsed);
		return endPos;
	}

	return endPos;
}

const string endBanner =
" _    _                                 _       _             _ _ _ \n"
"| |  | |                               (_)     | |           | | | |\n"
"| |__| | __ _ _ __  _ __  _   _   _ __  _    __| | __ _ _   _| | | |\n"
"|  __  |/ _` | '_ \\| '_ \\| | | | | '_ \\| |  / _` |/ _` | | | | | | |\n"
"| |  | | (_| | |_) | |_) | |_| | | |_) | | | (_| | (_| | |_| |_|_|_|\n"
"|_|  |_|\\__,_| .__/| .__/ \\__, | | .__/|_|  \\__,_|\\__,_|\\__, (_|_|_)\n"
"             | |   | |     __/ | | |                     __/ |      \n"
"             |_|   |_|    |___/  |_|                    |___/       ";

void printConclusion() {
	cout << endl << "Congratulations! You made it to the end... well... not of pi but the end of this program :)" << endl
		<< "Go have yourself a delicious slice of pie because you have earned it!" << endl
		<< "Hopefully now you have a higher level of respect for this wonderfully irrational number!" << endl;
	printBox(endBanner);
}