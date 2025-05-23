#include <iostream>
#include <string>
#include <conio.h>
#include <vector>
#include <fstream>
#include <iomanip>
#include <limits>
#include <direct.h>
using namespace std;

class PasswordReader {
public:
    static string readPassword() {
        string password;
        int ch;
        while ((ch = _getch()) != 13) {
            if (ch == 8) {
                if (!password.empty()) {
                    password.pop_back();
                    cout <<"\b \b";
                }
            } else {
                password += static_cast<char>(ch);
                cout << '*';
            }
        }
        cout <<endl;
        return password;
    }
};

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
            if (failedAttempts >= 3) locked = true;
    }

    void resetFailedAttempts() { failedAttempts = 0; }
    bool deposit(double amount) { if (amount <= 0) return false; balance += amount; return true; }
    bool withdraw(double amount) { if (amount <= 0 || balance < amount) return false; balance -= amount; return true; }
};

class BankingSystem {
private:
    vector<Account>accounts;
    int accountCounter = 2025000201;

    void encryptDecrypt(string& data, char key = 'X') {
        for(char& c : data) c ^= key;
    }

    void logTransaction(int accountNumber, const string& entry) {
        _mkdir("logs");
        ofstream log("logs/account_" + to_string(accountNumber) + ".txt", ios::app);
        log << entry << endl;
        log.close();
    }

public:
    BankingSystem() { loadFromFile(); }
    ~BankingSystem() { saveToFile(); }

    void saveToFile() {
        ofstream file("bank_accounts.txt");
        for (auto& acc : accounts) {
            string encryptedPass = acc.getPassword();
            encryptDecrypt(encryptedPass);
            file << acc.getAccountNumber() << "," << acc.getName() << ","
                 << encryptedPass << "," << acc.getBalance() << ","
                 << (acc.isLocked() ? "1" : "0") << "," << acc.getFailedAttempts() << "\n";
        }
        file.close();
    }

    void loadFromFile() {
        ifstream file("bank_accounts.txt");
        if (!file) return;
        accounts.clear();

        string line;
        while (getline(file, line)) {
            vector<string> tokens;
            size_t pos;
            while ((pos = line.find(',')) != string::npos) {
                tokens.push_back(line.substr(0, pos));
                line.erase(0, pos + 1);
            }
            tokens.push_back(line);
            if (tokens.size() == 6) {
                int num = stoi(tokens[0]);
                string name = tokens[1], pass = tokens[2];
                double bal = stod(tokens[3]);
                bool lock = tokens[4] == "1";
                int fails = stoi(tokens[5]);
                encryptDecrypt(pass);
                Account acc(num, name, pass, bal);
                acc.setLocked(lock);
                for (int i = 0; i < fails; i++) acc.incrementFailedAttempts();
                accounts.push_back(acc);
                if (num >= accountCounter) accountCounter = num + 1;
            }
        }
        file.close();
    }

    void createAccount() {
        string name, password;
        cout << "Enter your name: ";
        getline(cin, name);
        cout << "Set password: ";
        password = PasswordReader::readPassword();
        string encrypted = password; encryptDecrypt(encrypted);
        Account acc(accountCounter++, name, encrypted, 0.0);
        accounts.push_back(acc);
        saveToFile();
        cout << "✅ Account created successfully! Your account number is: " << acc.getAccountNumber() << "\n";
    }

    int login() {
        string numStr, pass;
        cout << "Enter Account Number: ";
        getline(cin, numStr);
        int num = stoi(numStr);

        for (size_t i = 0; i < accounts.size(); ++i) {
            if (accounts[i].getAccountNumber() == num) {
                if (accounts[i].isLocked()) {
                    cout << "❌ Your account is locked. Please contact at Bank.\n";
                    return -1;
                }
                cout << "Enter Password: ";
                pass = PasswordReader::readPassword();
                string stored = accounts[i].getPassword(); encryptDecrypt(stored);
                if (stored == pass) {
                    accounts[i].resetFailedAttempts();
                    saveToFile();
                    return i;
                } else {
                    accounts[i].incrementFailedAttempts();
                    saveToFile();
                    cout << "❌ Invalid password! Attempts remaining: " 
                         << (3 - accounts[i].getFailedAttempts()) << "\n";
                    return -1;
                }
            }
        }
        cout << "❌ Account not found.\n";
        return -1;
    }

    void deposit(int index) {
        string amountStr;
        double amount;
        
        cout << "Enter deposit amount: ";
        getline(cin, amountStr);
        try {
            amount = stod(amountStr);
        } catch (...) {
            cout << "❌ Invalid amount format!\n";
            return;
        }
        if (accounts[index].deposit(amount)) {
            saveToFile();
            logTransaction(accounts[index].getAccountNumber(), "[Deposit] " + amountStr + " tk ");
            cout << "✅ Deposit successful! New balance: " << fixed << setprecision(2) << accounts[index].getBalance() << " tk\n";
        } else {
            cout << "❌ Invalid amount!\n";
        }
    }

    void withdraw(int index) {
        string amountStr;
        double amount;
        
        cout << "Enter withdrawal amount: ";
        getline(cin, amountStr);
        try {
            amount = stod(amountStr);
        } catch (...) {
            cout << "❌ Invalid amount format!\n";
            return;
        }
        if (accounts[index].withdraw(amount)) {
            saveToFile();
            logTransaction(accounts[index].getAccountNumber(), "[Withdraw] " + amountStr + " tk ");
            cout << "✅ Withdrawal successful! New balance: " << fixed << setprecision(2) << accounts[index].getBalance() << " tk\n";
        } else {
            cout << "❌ Insufficient balance or invalid amount!\n";
        }
    }

    void transfer(int index) {
        string targetAccNumStr, amountStr;
        int targetAccNum;
        double amount;
        cout << "Enter target account number: ";
        getline(cin, targetAccNumStr);
        cout << "Enter amount to transfer: ";
        getline(cin, amountStr);
        try {
            targetAccNum = stoi(targetAccNumStr);
            amount = stod(amountStr);
        } catch (...) {
            cout << "❌ Invalid account number format!\n";
            return;
        }

        if (amount <= 0) {
            cout << "❌ Invalid amount!\n";
            return;
        }
        if (accounts[index].getBalance() < amount) {
            cout << "❌ Insufficient balance!\n";
            return;
        }
        int targetIndex = -1;
        for (size_t i = 0; i < accounts.size(); ++i) {
            if (accounts[i].getAccountNumber() == targetAccNum) {
                targetIndex = i;
                break;
            }
        }
        if (targetIndex == -1) {
            cout << "❌ Target account not found!\n";
            return;
        }
        if (accounts[targetIndex].isLocked()) {
            cout << "❌ Target account is locked!\n";
            return;
        }
        
        accounts[index].withdraw(amount);
        accounts[targetIndex].deposit(amount);
        saveToFile();
        logTransaction(accounts[index].getAccountNumber(), "[Transfer] " + amountStr + " tk to " + targetAccNumStr);
        logTransaction(targetAccNum, "[Received] " + amountStr + " tk from " + to_string(accounts[index].getAccountNumber()));
        cout << "✅ Transfer successful! New balance: " << fixed << setprecision(2) << accounts[index].getBalance() << " tk\n";
    }

    void viewAccount(int index) {
        cout << "\n📋 Account Details:\n";
        cout << "Account Number: " << accounts[index].getAccountNumber() << "\n";
        cout << "Name: " << accounts[index].getName() << "\n";
        cout << "Balance: " << fixed << setprecision(2) << accounts[index].getBalance() << " tk\n";
        cout << "Account Status: " << (accounts[index].isLocked() ? "Locked" : "Active") << "\n";
    }

    void resetPassword(int index) {
        string newPass;
        cout << "Enter new password: ";
        newPass = PasswordReader::readPassword();
        string encrypted = newPass; encryptDecrypt(encrypted);
        accounts[index].setPassword(encrypted);
        saveToFile();
        cout << "✅ Password reset successfully.\n";
    }

    void showTransactions(int accountNumber) {
        ifstream log("logs/account_" + to_string(accountNumber) + ".txt");
        if (!log) {
            cout << "No transactions found.\n";
            return;
        }
        cout << "\n📜 Transaction History:\n";
        string line;
        while (getline(log, line)) cout << line << "\n";
    }

    void adminPanel() {
        string user, pass;
        cout << "Admin Username: "; getline(cin, user);
        cout << "Admin Password: "; pass = PasswordReader::readPassword();
        if (user != "admin" || pass != "admin123") {
            cout << "❌ Invalid credentials.\n";
            return;
        }
        cout << "\n🔐 Locked Accounts:\n";
        for (size_t i = 0; i < accounts.size(); ++i) {
            if (accounts[i].isLocked()) {
                cout << "Account " << accounts[i].getAccountNumber() << " - " << accounts[i].getName() << "\n";
            }
        }
        string unlockStr;
        cout << "Enter account number to unlock (or 0 to cancel): ";
        getline(cin, unlockStr);
        int unlockNum = stoi(unlockStr);
        for (auto& acc : accounts) {
            if (acc.getAccountNumber() == unlockNum && acc.isLocked()) {
                acc.setLocked(false);
                acc.resetFailedAttempts();
                saveToFile();
                cout << "✅ Account unlocked.\n";
                return;
            }
        }
        if (unlockNum != 0) cout << "❌ Account not found or not locked.\n";
    }

    void run() {
        while (true) {
            cout << "\n🏦 Virtual Banking Menu:\n";
            cout << "1. Create Account\n2. Login\n3. Admin Panel\n4. Exit\nChoice: ";
            string choice; getline(cin, choice);

            if (choice == "1") createAccount();
            else if (choice == "2") {
                int index = login();
                if (index != -1) {
                    while (true) {
                        cout << "\n🔹 Welcome, " << accounts[index].getName() << " 🔹\n";
                        cout << "1. Deposit\n2. Withdraw\n3. Transfer\n4. View Info\n5. Reset Password\n6. Transaction History\n7. Logout\nChoice: ";
                        string opt; getline(cin, opt);
                        if (opt == "1") deposit(index);
                        else if (opt == "2") withdraw(index);
                        else if (opt == "3") transfer(index);
                        else if (opt == "4") viewAccount(index);
                        else if (opt == "5") resetPassword(index);
                        else if (opt == "6") showTransactions(accounts[index].getAccountNumber());
                        else if (opt == "7") {cout << "🔓 Logging out...\n"; break;}
                        else cout << "❌ Invalid choice.\n";
                    }
                }
            } else if (choice == "3") adminPanel();
            else if (choice == "4") {cout << "🚪 Exiting Banking System...\n"; break;}
            else cout << "❌ Invalid input.\n";
        }
    }
};

int main() {
    system("chcp 65001 > nul");
    BankingSystem bank;
    bank.run();
    return 0;
}