#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <string>
#include <limits>
#include <conio.h>
using namespace std;

// Windows-specific password reader
class PasswordReader {
    public:
        static string readPassword() {
            string password;
            int ch;
            while ((ch = _getch()) != 13) { // 13 is Enter key in Windows
                if (ch == 8) { // Backspace
                    if (!password.empty()) {
                        password.pop_back();
                        cout << "\b \b"; // Move back, print space, move back again
                    }
                } else {
                    password += static_cast<char>(ch);
                    cout << '*';
                }
            }
            cout << endl;
            return password;
        }
        // Alternative simpler method that doesn't mask input
        static string readPasswordSimple() {
            string password;
            cout << "(Input will be hidden) ";
            getline(cin, password);
            return password;
        }
    };