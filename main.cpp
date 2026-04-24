#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <ctime>
#include <iomanip>

using namespace std;

string encrypt (string data){
    for (char &c : data)c^='K' ;
    return data;
}

//Multiple Branch Management
struct Branch {
    string code;
    string location;
    double totalDeposits;
};
vector<Branch> branches = {
    {"DBN01", "Durban Central", 0.0},
    {"JHB01", "Johannesburg North", 0.0},
    {"CPT01", "Cape Town Waterfront", 0.0}
};

//ACCOUNT TYPES
class Account {
public:
    virtual double getMinBalance() = 0;
    virtual double getInterestRate() = 0; // For 1.5.1
    virtual ~Account() = default;
};

class SavingsAccount : public Account {
    double getMinBalance() override { return 100.0; }
    double getInterestRate() override { return 0.05; } // 5%
};
class ChequeAccount : public Account {
    double getMinBalance() override { return 500.0; }
    double getInterestRate() override { return 0.02; }
};
class FixedDepositAccount : public Account {
    double getMinBalance() override{return 1000.0;}
    double getInterestRate() override{return 0.08;}
};
class StudentAccount : public Account {
    double getMinBalance() override{return 20.0;}
    double getInterestRate() override{return 0.03;}
};

//DATA STRUCTURES AND ORGANISING
struct Transaction {
    char accNum[30];
    char type[20];
    double amount;
    char date[20];
};

struct Teller {
    char tellerID[10], name[50], password[50], branchCode[10];
};

struct CustomerRecord {
    char accNum[30], name[50], idNumber[15], contact[15], email[50], address[100], dob[15], pin[50];
    int type;
    double balance;
    int loginAttempts = 0;
};

//SYSTEM LOGIC
class BankSystem {
public:
    static int findCustomerIndex(const string& accNo, vector<CustomerRecord>& list) {
        for (int i = 0; i < list.size(); i++) {
            if (string(list[i].accNum) == accNo) return i;
        }
        return -1;
    }

    static void logTransaction(string acc, string type, double amt) {
        Transaction t;
        snprintf(t.accNum, 30, "%s", acc.c_str());
        snprintf(t.type, 20, "%s", type.c_str());
        t.amount = amt;
        time_t now = time(0);
        strftime(t.date, 20, "%Y-%m-%d", localtime(&now));

        ofstream out("transactions.dat", ios::binary | ios::app);
        out.write((char*)&t, sizeof(Transaction));
        out.close();
    }

    //Calculations for Transactions & Interest
    static void performTransaction(CustomerRecord& c, bool isTellerAssisted) {
        if (isTellerAssisted) {
            string pinCheck;
            cout << "Verify Customer PIN: "; cin >> pinCheck;
            if (encrypt(pinCheck) != string(c.pin)) {
                cout << "PIN Verification Failed!\n";
                return;
            }
        }

        int choice;
        cout << "\n1. Deposit\n2. Withdraw\n3. Apply Interest\n4. Statement\nChoice: ";
        cin >> choice;

        try {
            if (choice == 1) {
                double amt; cout << "Amount: "; cin >> amt;
                c.balance += amt;
                logTransaction(c.accNum, "DEPOSIT", amt);
                cout << "Success.\n";
            }
            else if (choice == 2) {
                double amt; cout << "Amount: "; cin >> amt;
                if (amt > c.balance) throw runtime_error("Insufficient Funds!");
                c.balance -= amt;
                logTransaction(c.accNum, "WITHDRAW", amt);
                cout << "Success.\n";
            }
            else if (choice == 3) {//How to calculate interest on balances
                double rate = (c.type == 1) ? 0.05 : (c.type == 2) ? 0.02 : (c.type == 3) ? 0.08 : 0.03;
                double interest = c.balance * rate;
                c.balance += interest;
                logTransaction(c.accNum, "INTEREST", interest);
                cout << "Interest of R" << interest << " applied.\n";
            }
            else if (choice == 4) {//Account Statement
                ifstream in("transactions.dat", ios::binary);
                Transaction t;
                cout << "\n--- Statement for " << c.accNum << " ---\n";
                while (in.read((char*)&t, sizeof(Transaction))) {
                    if (string(t.accNum) == string(c.accNum))
                        cout << t.date << " | " << t.type << " | R" << t.amount << endl;
                }
                in.close();
            }
        } catch (const exception& e) {
            cout << "TRANSACTION ERROR: " << e.what() << endl;
        }
    }

    //Branch Management
    static void branchManagement() {
        cout << "\n--- Branch Comparison ---\n";
        for (auto& b : branches) {
            cout << "Branch: " << b.code << " (" << b.location << ")\n";
        }
    }

    //Reporting
    static void dailyReport() {
        ifstream in("customers.dat", ios::binary);
        CustomerRecord c;
        double totalBankBalance = 0;
        cout << "\n--- Customer Account Summary ---\n";
        while (in.read((char*)&c, sizeof(CustomerRecord))) {
            cout << "Acc: " << c.accNum << " | Holder: " << c.name << " | Bal: R" << c.balance << endl;
            totalBankBalance += c.balance;
        }
        cout << "Total Bank Holdings: R" << totalBankBalance << endl;
        in.close();
    }

    void handleTellerLogin() {
        string id, pass;
        cout << "Teller ID: "; cin >> id;
        cout << "Password: "; cin >> pass;

        ifstream in("tellers.dat", ios::binary);
        Teller t;
        bool found = false;
        while (in.read((char*)&t, sizeof(Teller))) {
            if (string(t.tellerID) == id && encrypt(pass) == string(t.password)) {
                found = true; break;
            }
        }
        in.close();

        if (found) {
            int choice;
            while (true) {
                cout << "\n--- Teller Menu (" << t.branchCode << ") ---\n";
                cout << "1. Register Customer\n2. Customer Transactions\n3. Branch Reports\n4. Logout\nChoice: ";
                cin >> choice;
                if (choice == 1) registerCustomer(t.branchCode);
                else if (choice == 2) {
                    string acc; cout << "Enter Customer Acc: "; cin >> acc;
                    updateCustomerInFile(acc, true);
                }
                else if (choice == 3) {
                    branchManagement();
                    dailyReport();
                }
                else break;
            }
        } else cout << "Login Failed.\n";
    }

    // Helper to find and update customer in file
    static void updateCustomerInFile(string accNum, bool isTeller) {
        fstream file("customers.dat", ios::binary | ios::in | ios::out);
        CustomerRecord c;
        bool found = false;
        while (file.read((char*)&c, sizeof(CustomerRecord))) {
            if (string(c.accNum) == accNum) {
                performTransaction(c, isTeller);
                file.seekp((int)file.tellg() - sizeof(CustomerRecord));
                file.write((char*)&c, sizeof(CustomerRecord));
                found = true;
                break;
            }
        }
        if (!found) cout << "Account not found.\n";
        file.close();
    }

    void registerCustomer(string branch) {
        CustomerRecord c;
        string rawPin = to_string(rand() % 90000 + 10000);
        cout << "Full Name: "; cin.ignore(); cin.getline(c.name, 50);
        cout << "ID (13 digit): "; cin >> c.idNumber;
        cout << "Email: "; cin >> c.email;
        cout << "Type (1-Savings, 2-Cheque, 3-Fixed, 4-Student): "; cin >> c.type;
        cout << "Initial Deposit: "; cin >> c.balance;

        string acc = "ACC-" + branch + "-" + to_string(rand() % 90000 + 10000);
        snprintf(c.accNum, 30, "%s", acc.c_str());
        snprintf(c.pin, 50, "%s", encrypt(rawPin).c_str());

        ofstream out("customers.dat", ios::binary | ios::app);
        out.write((char*)&c, sizeof(CustomerRecord));
        out.close();
        cout << "Registered! PIN: " << rawPin << endl;
    }

    void handleCustomerLogin() {
        string acc, pin;
        cout << "Acc No: "; cin >> acc;
        cout << "PIN: "; cin >> pin;

        fstream file("customers.dat", ios::binary | ios::in | ios::out);
        CustomerRecord c;
        while (file.read((char*)&c, sizeof(CustomerRecord))) {
            if (string(c.accNum) == acc && encrypt(pin) == string(c.pin)) {
                performTransaction(c, false);
                file.seekp((int)file.tellg() - sizeof(CustomerRecord));
                file.write((char*)&c, sizeof(CustomerRecord));
                file.close();
                return;
            }
        }
        cout << "Login Failed.\n";
        file.close();
    }
};

int main() {
    srand(time(0));
    BankSystem sys;

    // Seed Teller if needed
    ifstream f("tellers.dat");
    if(!f) {
        ofstream fo("tellers.dat", ios::binary);
        Teller admin = {"T001", "Admin", "", "DBN01"};
        snprintf(admin.password, 50, "%s", encrypt("1234").c_str());
        fo.write((char*)&admin, sizeof(Teller));
    }

    int choice;
    while (true) {
        cout << "\n--- STANDARD BANK CORE ---\n1. Teller Login\n2. Customer Login\n3. Exit\nChoice: ";
        cin >> choice;
        if (choice == 1) sys.handleTellerLogin();
        else if (choice == 2) sys.handleCustomerLogin();
        else break;
    }
    return 0;
}