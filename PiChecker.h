#pragma once
#include <string>

#include "PiMillion.h"

using namespace std;

extern const unsigned char PI_MILLION[500000];

#ifdef __GMP_H__
#ifdef __GMP_PLUSPLUS__

string to_string(mpf_class _Val) {
	mp_exp_t exp;
	return _Val.get_str(exp, 10);
}

mp_bitcnt_t getPrecisionNeeded(int digits) {
	return ((double)digits / (log(2)/log(10))) + 1;
}

#endif
#endif

//Get digit of pi as ascii char. Number can be 0-999999 inclusively. Does not include decimal point and starts at '3'.
//Returns null byte if number passed to it is out of its range.
char getDigit(int number) {
	if (number >= 0 && number <= 999999) {
		int index = number / 2;
		unsigned char byte = PI_MILLION[index];
		char result = 0;
		if (number % 2 == 0) { //Digit stored in first half of byte
			byte /= 16;
		}
		else { //Digit stored in second half of byte
			byte %= 16;
		}
		result = '0' + byte;
		return (char)result;
	}
	else return '\0';
}

//Return the number of digits input has correctly of Pi. Stops counting at 1 million digits of Pi.
template<class T> int countDigits(const T &input, int startPos = 0, int backCheck=10) {
	if (startPos < 0) startPos = 0;
	string temp = to_string(input);
	static int skipCount = 0;
	if (startPos == 0) skipCount = 0;
	int index = startPos+skipCount;

	int checkEnd = index - backCheck;
	int digit = startPos - 1;
	if (checkEnd < 0) checkEnd = 0;
	for (int i = index - 1; i >= checkEnd; i--) {
		if (temp[i]<'0' || temp[i]>'9') {
			continue;
		}
		if (temp[i] != getDigit(digit)) return startPos;
		digit--;
	}

	while (startPos < 1000000) {
		if (index >= temp.size()) return startPos;
		while (temp[index]<'0' || temp[index]>'9') {
			index++; //Skip not digit characters such as the decimal point
			skipCount++;
		}
		if (temp[index] != getDigit(startPos)) return startPos;
		index++;
		startPos++;
	}
	return 1000000;
}

//Return the number of digits input has correctly of Pi. Stops counting at 1 million digits of Pi.
//Prints out digits as they are verified correct.
template<class T> int printCountDigits(const T &input, int startPos = 0, int backCheck=10) {
	if (startPos < 0) startPos = 0;
	string temp = to_string(input);
	static int skipCount = 0;
	if (startPos == 0) skipCount = 0;
	int index = startPos+skipCount;

	int checkEnd = index - backCheck;
	int digit = startPos - 1;
	if (checkEnd < 0) checkEnd = 0;
	for (int i = index - 1; i >= checkEnd; i--) {
		if (temp[i]<'0' || temp[i]>'9') {
			continue;
		}
		if (temp[i] != getDigit(digit)) return startPos;
		digit--;
	}

	while (startPos < 1000000) {
		if (index >= temp.size()) return startPos;
		while (temp[index]<'0' || temp[index]>'9') {
			index++; //Skip not digit characters such as the decimal point
			skipCount++;
		}
		char c = getDigit(startPos);
		if (temp[index] != c) 
			return startPos;
		
		if (startPos == 1) cout << '.';
		cout << c;

		index++;
		startPos++;
	}
	return 1000000;
}