#pragma once
#include <string>

using namespace std;

static int GetNumberOfDigits(int i)
{
	return i > 0 ? (int)log10((double)i) + 1 : 1;
}