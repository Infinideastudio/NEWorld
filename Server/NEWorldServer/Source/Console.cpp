#include "Console.h"
#include <time.h>

void Print(string message, int level) {
    switch (level) {
    case MESSAGE_INFO:
        cout << "[INFO]";
        break;
    case MESSAGE_WARNING:
        cout << "[WARNING]";
        break;
    case MESSAGE_ERROR:
        cout << "[ERROR]";
        break;
    }
    auto t = time(NULL);
    static tm lt;
    localtime_s(&lt, &t);
    cout << "["
        << (lt.tm_hour < 10 ? "0" : "") << lt.tm_hour << ":"
        << (lt.tm_min < 10 ? "0" : "") << lt.tm_min << ":"
        << (lt.tm_sec < 10 ? "0" : "") << lt.tm_sec << "] "
        << message << endl;
}

string toString(int i) {
    char a[12];
    _itoa_s(i, a, 12, 10);
    return string(a);
}
