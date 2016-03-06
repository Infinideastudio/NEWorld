#pragma once
#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;

enum { MESSAGE_INFO, MESSAGE_WARNING, MESSAGE_ERROR };

void Print(string message, int level = MESSAGE_INFO);

string toString(int i);