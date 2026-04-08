#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>

using namespace std;

class Account {
private:
    int accNo;
    string name;
    double balance;

public:
    Account() {
        accNo = 0;
        name = "";
        balance = 0.0;
    }

    Account(int accNo, string name, double balance) {
        this->accNo = accNo;
        this->name = name;
        this->balance = balance;
    }

    int getAccNo() const {
        return accNo;
    }

    string getName() const {
        return name;
    }

    double getBalance() const {
        return balance;
    }

    void deposit(double amount) {
        balance += amount;
    }

    bool withdraw(double amount) {
        if (amount > balance) {
            return false;
        }
        balance -= amount;
        return true;
    }

    void display() const {
        cout << left << setw(12) << accNo
             << setw(25) << name
             << fixed << setprecision(2) << balance << endl;
    }

    string toFileString() const {
        return to_string(accNo) + "|" + name + "|" + to_string(balance);
    }
};

class Transaction {
private:
    int transactionNo;
    int accNo;
    string type;
    double amount;

public:
    Transaction() {
        transactionNo = 0;
        accNo = 0;
        type = "";
        amount = 0.0;
    }

    Transaction(int transactionNo, int accNo, string type, double amount) {
        this->transactionNo = transactionNo;
        this->accNo = accNo;
        this->type = type;
        this->amount = amount;
    }

    int getTransactionNo() const {
        return transactionNo;
    }

    int getAccNo() const {
        return accNo;
    }

    string getType() const {
        return type;
    }

    double getAmount() const {
        return amount;
    }

    void display() const {
        cout << left << setw(10) << transactionNo
             << setw(12) << accNo
             << setw(15) << type
             << fixed << setprecision(2) << amount << endl;
    }

    string toFileString() const {
        return to_string(transactionNo) + "|" + to_string(accNo) + "|" + type + "|" + to_string(amount);
    }
};

class BankSystem {
private:
    vector<Account> accounts;
    vector<Transaction> transactions;
    int nextTransactionNo;

    void clearInput() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    int findAccountIndex(int accNo) {
        for (int i = 0; i < (int)accounts.size(); i++) {
            if (accounts[i].getAccNo() == accNo) {
                return i;
            }
        }
        return -1;
    }

public:
    BankSystem() {
        nextTransactionNo = 1;
        loadAccounts();
        loadTransactions();
    }

    void loadAccounts() {
        accounts.clear();
        ifstream file("accounts.txt");

        if (!file.is_open()) {
            return;
        }

        string line;
        while (getline(file, line)) {
            if (line.empty()) {
                continue;
            }

            size_t p1 = line.find('|');
            size_t p2 = line.rfind('|');

            if (p1 == string::npos || p2 == string::npos || p1 == p2) {
                continue;
            }

            int accNo = stoi(line.substr(0, p1));
            string name = line.substr(p1 + 1, p2 - p1 - 1);
            double balance = stod(line.substr(p2 + 1));

            accounts.push_back(Account(accNo, name, balance));
        }

        file.close();
    }

    void saveAccounts() {
        ofstream file("accounts.txt");

        for (int i = 0; i < (int)accounts.size(); i++) {
            file << accounts[i].toFileString() << endl;
        }

        file.close();
    }

    void loadTransactions() {
        transactions.clear();
        ifstream file("transactions.txt");

        if (!file.is_open()) {
            return;
        }

        string line;
        int maxTxn = 0;

        while (getline(file, line)) {
            if (line.empty()) {
                continue;
            }

            size_t p1 = line.find('|');
            size_t p2 = line.find('|', p1 + 1);
            size_t p3 = line.find('|', p2 + 1);

            if (p1 == string::npos || p2 == string::npos || p3 == string::npos) {
                continue;
            }

            int txnNo = stoi(line.substr(0, p1));
            int accNo = stoi(line.substr(p1 + 1, p2 - p1 - 1));
            string type = line.substr(p2 + 1, p3 - p2 - 1);
            double amount = stod(line.substr(p3 + 1));

            transactions.push_back(Transaction(txnNo, accNo, type, amount));

            if (txnNo > maxTxn) {
                maxTxn = txnNo;
            }
        }

        nextTransactionNo = maxTxn + 1;
        file.close();
    }

    void saveTransactions() {
        ofstream file("transactions.txt");

        for (int i = 0; i < (int)transactions.size(); i++) {
            file << transactions[i].toFileString() << endl;
        }

        file.close();
    }

    void createAccount() {
        int accNo;
        string name;
        double initialBalance;

        cout << "\nEnter account number: ";
        if (!(cin >> accNo)) {
            cout << "Invalid account number.\n";
            clearInput();
            return;
        }
        clearInput();

        if (findAccountIndex(accNo) != -1) {
            cout << "Account number already exists.\n";
            return;
        }

        cout << "Enter account holder name: ";
        getline(cin, name);

        if (name.empty()) {
            cout << "Name cannot be empty.\n";
            return;
        }

        cout << "Enter initial balance: ";
        if (!(cin >> initialBalance) || initialBalance < 0) {
            cout << "Invalid balance amount.\n";
            clearInput();
            return;
        }

        accounts.push_back(Account(accNo, name, initialBalance));
        saveAccounts();

        cout << "Account created successfully.\n";
    }

    void depositMoney() {
        int accNo;
        double amount;

        cout << "\nEnter account number: ";
        if (!(cin >> accNo)) {
            cout << "Invalid account number.\n";
            clearInput();
            return;
        }

        int index = findAccountIndex(accNo);
        if (index == -1) {
            cout << "Account not found.\n";
            return;
        }

        cout << "Enter deposit amount: ";
        if (!(cin >> amount) || amount <= 0) {
            cout << "Invalid deposit amount.\n";
            clearInput();
            return;
        }

        accounts[index].deposit(amount);
        transactions.push_back(Transaction(nextTransactionNo, accNo, "Deposit", amount));
        nextTransactionNo++;

        saveAccounts();
        saveTransactions();

        cout << "Deposit successful.\n";
        cout << "Updated balance: " << fixed << setprecision(2) << accounts[index].getBalance() << endl;
    }

    void withdrawMoney() {
        int accNo;
        double amount;

        cout << "\nEnter account number: ";
        if (!(cin >> accNo)) {
            cout << "Invalid account number.\n";
            clearInput();
            return;
        }

        int index = findAccountIndex(accNo);
        if (index == -1) {
            cout << "Account not found.\n";
            return;
        }

        cout << "Enter withdrawal amount: ";
        if (!(cin >> amount) || amount <= 0) {
            cout << "Invalid withdrawal amount.\n";
            clearInput();
            return;
        }

        if (!accounts[index].withdraw(amount)) {
            cout << "Insufficient balance. Overdraft not allowed.\n";
            return;
        }

        transactions.push_back(Transaction(nextTransactionNo, accNo, "Withdraw", amount));
        nextTransactionNo++;

        saveAccounts();
        saveTransactions();

        cout << "Withdrawal successful.\n";
        cout << "Updated balance: " << fixed << setprecision(2) << accounts[index].getBalance() << endl;
    }

    void balanceEnquiry() {
        int accNo;

        cout << "\nEnter account number: ";
        if (!(cin >> accNo)) {
            cout << "Invalid account number.\n";
            clearInput();
            return;
        }

        int index = findAccountIndex(accNo);
        if (index == -1) {
            cout << "Account not found.\n";
            return;
        }

        cout << "\nAccount Details\n";
        cout << "---------------------------------------------\n";
        cout << left << setw(12) << "Acc No"
             << setw(25) << "Name"
             << "Balance" << endl;
        cout << "---------------------------------------------\n";
        accounts[index].display();
    }

    void viewLastFiveTransactions() {
        int accNo;
        int count = 0;

        cout << "\nEnter account number: ";
        if (!(cin >> accNo)) {
            cout << "Invalid account number.\n";
            clearInput();
            return;
        }

        if (findAccountIndex(accNo) == -1) {
            cout << "Account not found.\n";
            return;
        }

        cout << "\nLast 5 Transactions\n";
        cout << "---------------------------------------------\n";
        cout << left << setw(10) << "Txn No"
             << setw(12) << "Acc No"
             << setw(15) << "Type"
             << "Amount" << endl;
        cout << "---------------------------------------------\n";

        for (int i = (int)transactions.size() - 1; i >= 0 && count < 5; i--) {
            if (transactions[i].getAccNo() == accNo) {
                transactions[i].display();
                count++;
            }
        }

        if (count == 0) {
            cout << "No transactions found for this account.\n";
        }
    }

    void totalMoneyReport() {
        double total = 0.0;

        for (int i = 0; i < (int)accounts.size(); i++) {
            total += accounts[i].getBalance();
        }

        cout << "\nTotal money held across all accounts: "
             << fixed << setprecision(2) << total << endl;
    }

    void lowBalanceReport() {
        double threshold;
        bool found = false;

        cout << "\nEnter threshold amount: ";
        if (!(cin >> threshold) || threshold < 0) {
            cout << "Invalid threshold value.\n";
            clearInput();
            return;
        }

        cout << "\nAccounts with balance below " << fixed << setprecision(2) << threshold << endl;
        cout << "---------------------------------------------\n";
        cout << left << setw(12) << "Acc No"
             << setw(25) << "Name"
             << "Balance" << endl;
        cout << "---------------------------------------------\n";

        for (int i = 0; i < (int)accounts.size(); i++) {
            if (accounts[i].getBalance() < threshold) {
                accounts[i].display();
                found = true;
            }
        }

        if (!found) {
            cout << "No accounts found below the threshold.\n";
        }
    }

    void viewAllAccounts() {
        if (accounts.empty()) {
            cout << "\nNo accounts available.\n";
            return;
        }

        cout << "\nAll Account Details\n";
        cout << "---------------------------------------------\n";
        cout << left << setw(12) << "Acc No"
             << setw(25) << "Name"
             << "Balance" << endl;
        cout << "---------------------------------------------\n";

        for (int i = 0; i < (int)accounts.size(); i++) {
            accounts[i].display();
        }
    }

    void menu() {
        int choice;

        do {
            cout << "\n========== PERSONAL SAVINGS ACCOUNT SYSTEM ==========\n";
            cout << "1. Create New Account\n";
            cout << "2. Deposit Money\n";
            cout << "3. Withdraw Money\n";
            cout << "4. Balance Enquiry\n";
            cout << "5. View Last 5 Transactions\n";
            cout << "6. Total Money Report\n";
            cout << "7. Low Balance Report\n";
            cout << "8. View All Accounts\n";
            cout << "9. Exit\n";
            cout << "Enter your choice: ";

            if (!(cin >> choice)) {
                cout << "Invalid choice. Please enter a number.\n";
                clearInput();
                continue;
            }

            switch (choice) {
                case 1:
                    createAccount();
                    break;
                case 2:
                    depositMoney();
                    break;
                case 3:
                    withdrawMoney();
                    break;
                case 4:
                    balanceEnquiry();
                    break;
                case 5:
                    viewLastFiveTransactions();
                    break;
                case 6:
                    totalMoneyReport();
                    break;
                case 7:
                    lowBalanceReport();
                    break;
                case 8:
                    viewAllAccounts();
                    break;
                case 9:
                    cout << "Exiting program.\n";
                    break;
                default:
                    cout << "Invalid choice. Try again.\n";
            }

        } while (choice != 9);
    }
};

int main() {
    BankSystem bank;
    bank.menu();
    return 0;
}
