#include <emscripten/emscripten.h>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

class Account {
private:
    int accNo;
    string name;
    double balance;

public:
    Account(int accNo, const string& name, double balance)
        : accNo(accNo), name(name), balance(balance) {}

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
};

class Transaction {
private:
    int transactionNo;
    int accNo;
    string type;
    double amount;

public:
    Transaction(int transactionNo, int accNo, const string& type, double amount)
        : transactionNo(transactionNo), accNo(accNo), type(type), amount(amount) {}

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
};

class BankSystem {
private:
    vector<Account> accounts;
    vector<Transaction> transactions;
    int nextTransactionNo = 1;

    string formatMoney(double value) const {
        ostringstream out;
        out << fixed << setprecision(2) << value;
        return out.str();
    }

    string trim(const string& s) const {
        size_t start = s.find_first_not_of(" \t\n\r");
        if (start == string::npos) {
            return "";
        }
        size_t end = s.find_last_not_of(" \t\n\r");
        return s.substr(start, end - start + 1);
    }

    int findAccountIndex(int accNo) const {
        for (int i = 0; i < (int)accounts.size(); i++) {
            if (accounts[i].getAccNo() == accNo) {
                return i;
            }
        }
        return -1;
    }

    double totalMoney() const {
        double total = 0.0;
        for (const auto& account : accounts) {
            total += account.getBalance();
        }
        return total;
    }

public:
    string createAccount(int accNo, const string& rawName, double balance) {
        string name = trim(rawName);

        if (accNo <= 0) {
            return "Invalid account number. Please enter a positive number.";
        }

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
        out << "Client Name: " << name << "\n";
        out << "Opening Balance: ₹" << formatMoney(balance) << "\n";
        out << "Total Accounts: " << accounts.size();
        return out.str();
    }

    string depositMoney(int accNo, double amount) {
        if (accNo <= 0) {
            return "Invalid account number.";
        }

        if (amount <= 0) {
            return "Invalid deposit amount.";
        }

        int index = findAccountIndex(accNo);
        if (index == -1) {
            return "Account not found.";
        }

        accounts[index].deposit(amount);
        transactions.push_back(Transaction(nextTransactionNo++, accNo, "Deposit", amount));

        ostringstream out;
        out << "Deposit successful.\n";
        out << "Account No: " << accNo << "\n";
        out << "Deposited Amount: ₹" << formatMoney(amount) << "\n";
        out << "Updated Balance: ₹" << formatMoney(accounts[index].getBalance());
        return out.str();
    }

    string withdrawMoney(int accNo, double amount) {
        if (accNo <= 0) {
            return "Invalid account number.";
        }

        if (amount <= 0) {
            return "Invalid withdrawal amount.";
        }

        int index = findAccountIndex(accNo);
        if (index == -1) {
            return "Account not found.";
        }

        if (!accounts[index].withdraw(amount)) {
            return "Insufficient balance. Overdraft not allowed.";
        }

        transactions.push_back(Transaction(nextTransactionNo++, accNo, "Withdraw", amount));

        ostringstream out;
        out << "Withdrawal successful.\n";
        out << "Account No: " << accNo << "\n";
        out << "Withdrawal Amount: ₹" << formatMoney(amount) << "\n";
        out << "Updated Balance: ₹" << formatMoney(accounts[index].getBalance());
        return out.str();
    }

    string balanceEnquiry(int accNo) const {
        if (accNo <= 0) {
            return "Invalid account number.";
        }

        int index = findAccountIndex(accNo);
        if (index == -1) {
            return "Account not found.";
        }

        ostringstream out;
        out << "Account Details\n";
        out << "-------------------------\n";
        out << "Account No: " << accounts[index].getAccNo() << "\n";
        out << "Client Name: " << accounts[index].getName() << "\n";
        out << "Current Balance: ₹" << formatMoney(accounts[index].getBalance());
        return out.str();
    }

    string totalMoneyReport() const {
        ostringstream out;
        out << "Total Money Report\n";
        out << "-------------------------\n";
        out << "Total Accounts: " << accounts.size() << "\n";
        out << "Total Money Held Across All Accounts: ₹" << formatMoney(totalMoney());
        return out.str();
    }

    string lowBalanceReport(double threshold) const {
        if (threshold < 0) {
            return "Invalid threshold amount.";
        }

        ostringstream out;
        bool found = false;

        out << "Low Balance Report\n";
        out << "Threshold: ₹" << formatMoney(threshold) << "\n";
        out << "-------------------------\n";

        for (const auto& account : accounts) {
            if (account.getBalance() < threshold) {
                out << "Acc No: " << account.getAccNo()
                    << " | Name: " << account.getName()
                    << " | Balance: ₹" << formatMoney(account.getBalance()) << "\n";
                found = true;
            }
        }

        if (!found) {
            return "No accounts found below threshold.";
        }

        return out.str();
    }

    string viewAllAccounts() const {
        if (accounts.empty()) {
            return "No accounts available.";
        }

        ostringstream out;
        out << "All Accounts\n";
        out << "-------------------------\n";

        for (const auto& account : accounts) {
            out << "Acc No: " << account.getAccNo()
                << " | Name: " << account.getName()
                << " | Balance: ₹" << formatMoney(account.getBalance()) << "\n";
        }

        return out.str();
    }

    string viewTransactions() const {
        if (transactions.empty()) {
            return "No transactions available.";
        }

        ostringstream out;
        out << "Transaction History\n";
        out << "-------------------------\n";

        for (const auto& txn : transactions) {
            out << "#" << txn.getTransactionNo()
                << " | " << txn.getType()
                << " | Acc No: " << txn.getAccNo()
                << " | Amount: ₹" << formatMoney(txn.getAmount()) << "\n";
        }

        return out.str();
    }
};

static BankSystem bank;
static string resultBuffer;

extern "C" {

EMSCRIPTEN_KEEPALIVE
const char* create_account(int accNo, const char* name, double balance) {
    resultBuffer = bank.createAccount(accNo, string(name ? name : ""), balance);
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
