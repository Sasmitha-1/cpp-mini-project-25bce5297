#include <emscripten/emscripten.h>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

class Account {
private:
    int accNo;
    string name;
    double balance;

public:
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
        if (amount > balance) return false;
        balance -= amount;
        return true;
    }
};

class BankSystem {
private:
    vector<Account> accounts;
    vector<string> transactions;

    int findAccountIndex(int accNo) {
        for (int i = 0; i < (int)accounts.size(); i++) {
            if (accounts[i].getAccNo() == accNo) return i;
        }
        return -1;
    }

public:
    string createAccount(int accNo, const string& name, double balance) {
        if (findAccountIndex(accNo) != -1) {
            return "Account number already exists.";
        }
        if (name.empty()) {
            return "Name cannot be empty.";
        }
        if (balance < 0) {
            return "Initial balance cannot be negative.";
        }

        accounts.push_back(Account(accNo, name, balance));

        ostringstream out;
        out << "Account created successfully.\n";
        out << "Account No: " << accNo << "\n";
        out << "Name: " << name << "\n";
        out << "Balance: " << balance;
        return out.str();
    }

    string depositMoney(int accNo, double amount) {
        int index = findAccountIndex(accNo);
        if (index == -1) return "Account not found.";
        if (amount <= 0) return "Invalid deposit amount.";

        accounts[index].deposit(amount);

        ostringstream txn;
        txn << "Deposit | AccNo: " << accNo << " | Amount: " << amount;
        transactions.push_back(txn.str());

        ostringstream out;
        out << "Deposit successful.\n";
        out << "Updated balance: " << accounts[index].getBalance();
        return out.str();
    }

    string withdrawMoney(int accNo, double amount) {
        int index = findAccountIndex(accNo);
        if (index == -1) return "Account not found.";
        if (amount <= 0) return "Invalid withdrawal amount.";
        if (!accounts[index].withdraw(amount)) return "Insufficient balance. Overdraft not allowed.";

        ostringstream txn;
        txn << "Withdraw | AccNo: " << accNo << " | Amount: " << amount;
        transactions.push_back(txn.str());

        ostringstream out;
        out << "Withdrawal successful.\n";
        out << "Updated balance: " << accounts[index].getBalance();
        return out.str();
    }

    string balanceEnquiry(int accNo) {
        int index = findAccountIndex(accNo);
        if (index == -1) return "Account not found.";

        ostringstream out;
        out << "Account Details\n";
        out << "Acc No: " << accounts[index].getAccNo() << "\n";
        out << "Name: " << accounts[index].getName() << "\n";
        out << "Balance: " << accounts[index].getBalance();
        return out.str();
    }

    string lowBalanceReport(double threshold) {
        ostringstream out;
        bool found = false;

        out << "Accounts below threshold " << threshold << ":\n";

        for (int i = 0; i < (int)accounts.size(); i++) {
            if (accounts[i].getBalance() < threshold) {
                out << accounts[i].getAccNo() << " | "
                    << accounts[i].getName() << " | "
                    << accounts[i].getBalance() << "\n";
                found = true;
            }
        }

        if (!found) {
            return "No accounts found below threshold.";
        }

        return out.str();
    }

    string totalMoneyReport() {
        double total = 0;
        for (int i = 0; i < (int)accounts.size(); i++) {
            total += accounts[i].getBalance();
        }

        ostringstream out;
        out << "Total money held across all accounts: " << total;
        return out.str();
    }

    string viewAllAccounts() {
        if (accounts.empty()) return "No accounts available.";

        ostringstream out;
        out << "All Accounts:\n";
        for (int i = 0; i < (int)accounts.size(); i++) {
            out << accounts[i].getAccNo() << " | "
                << accounts[i].getName() << " | "
                << accounts[i].getBalance() << "\n";
        }
        return out.str();
    }

    string viewTransactions() {
        if (transactions.empty()) return "No transactions available.";

        ostringstream out;
        out << "Transaction History:\n";
        for (int i = 0; i < (int)transactions.size(); i++) {
            out << transactions[i] << "\n";
        }
        return out.str();
    }
};

BankSystem bank;
string resultBuffer;

extern "C" {

EMSCRIPTEN_KEEPALIVE
const char* create_account(int accNo, const char* name, double balance) {
    resultBuffer = bank.createAccount(accNo, string(name), balance);
    return resultBuffer.c_str();
}

EMSCRIPTEN_KEEPALIVE
const char* deposit_money(int accNo, double amount) {
    resultBuffer = bank.depositMoney(accNo, amount);
    return resultBuffer.c_str();
}

EMSCRIPTEN_KEEPALIVE
const char* withdraw_money(int accNo, double amount) {
    resultBuffer = bank.withdrawMoney(accNo, amount);
    return resultBuffer.c_str();
}

EMSCRIPTEN_KEEPALIVE
const char* balance_enquiry(int accNo) {
    resultBuffer = bank.balanceEnquiry(accNo);
    return resultBuffer.c_str();
}

EMSCRIPTEN_KEEPALIVE
const char* total_money_report() {
    resultBuffer = bank.totalMoneyReport();
    return resultBuffer.c_str();
}

EMSCRIPTEN_KEEPALIVE
const char* low_balance_report(double threshold) {
    resultBuffer = bank.lowBalanceReport(threshold);
    return resultBuffer.c_str();
}

EMSCRIPTEN_KEEPALIVE
const char* view_all_accounts() {
    resultBuffer = bank.viewAllAccounts();
    return resultBuffer.c_str();
}

EMSCRIPTEN_KEEPALIVE
const char* view_transactions() {
    resultBuffer = bank.viewTransactions();
    return resultBuffer.c_str();
}

}

int main() {
    return 0;
}
