#include <iostream>
#include <string>
using namespace std;

class Account {
private:
    int accountNumber;
    string name;
    string password;
    double balance;
    bool locked;
    int failedAttempts;
public:
    Account() : accountNumber(0), balance(0.0), locked(false), failedAttempts(0) {}
    Account(int accNum, string n, string pass, double bal)
            : accountNumber(accNum), name(n), password(pass), balance(bal), locked(false), failedAttempts(0) {}
    // Getters
    int getAccountNumber() const { return accountNumber; }
    string getName() const { return name; }
    string getPassword() const { return password; }
    double getBalance() const { return balance; }
    bool isLocked() const { return locked; }
    int getFailedAttempts() const { return failedAttempts; }

    // Setters
    void setAccountNumber(int accNum) { accountNumber = accNum; }
    void setName(string n) { name = n; }
    void setPassword(string pass) { password = pass; }
    void setBalance(double bal) { balance = bal; }
    void setLocked(bool lock) { locked = lock; }

    void incrementFailedAttempts() {
            failedAttempts++;
            if (failedAttempts >= 3) 
            locked = true;
    }

    void resetFailedAttempts() { failedAttempts = 0; }
    bool deposit(double amount) { if (amount <= 0) return false; balance += amount; return true; }
    bool withdraw(double amount) { if (amount <= 0 || balance < amount) return false; balance -= amount; return true; }
};

class BankingSystem {
public:
    void createAccount() {
        string name, password;
        cout << "Enter your name: ";
        getline(cin, name);
        cout << "Set password: ";
    }

    void run() {
        while (true) {
            cout << "\n🏦 Virtual Banking System 🏦\n";
            cout << "1. Create Account\n2. Login\n3. Admin Panel\n4. Exit\n";
            cout<<"Enter choice: ";
            string choice; getline(cin, choice);
        }
    }
};

int main() {
    system("chcp 65001 > nul");
    BankingSystem bank;
    bank.run();
    return 0;
}