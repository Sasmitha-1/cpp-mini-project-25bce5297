// ╔══════════════════════════════════════════════════════════════════╗
// ║  Velora Bank — Personal Savings Account System                   ║
// ║  Student  : Sasmitha S        Reg No : 25BCE5297                 ║
// ║  Subject  : C++ OOP DA2       VIT                                ║
// ╚══════════════════════════════════════════════════════════════════╝
 
#include <emscripten/emscripten.h>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
 
using namespace std;
 
// ═══════════════════════════════════════════════════════════════════
// CLASS: Transaction
// ═══════════════════════════════════════════════════════════════════
class Transaction {
private:
    int    txnNo;
    int    accNo;
    string type;
    double amount;
    double balanceAfter;
 
public:
    Transaction(int no, int acc, const string& t, double amt, double bal)
        : txnNo(no), accNo(acc), type(t), amount(amt), balanceAfter(bal) {}
 
    int    getTxnNo()        const { return txnNo; }
    int    getAccNo()        const { return accNo; }
    string getType()         const { return type; }
    double getAmount()       const { return amount; }
    double getBalanceAfter() const { return balanceAfter; }
};
 
// ═══════════════════════════════════════════════════════════════════
// CLASS: Account
// ═══════════════════════════════════════════════════════════════════
class Account {
private:
    int    accNo;
    string name;
    double balance;
    bool   active;
 
    static const double MIN_BALANCE;
    static const double DAILY_LIMIT;
 
public:
    Account(int no, const string& n, double bal)
        : accNo(no), name(n), balance(bal), active(true) {}
 
    int    getAccNo()   const { return accNo; }
    string getName()    const { return name; }
    double getBalance() const { return balance; }
    bool   isActive()   const { return active; }
 
    bool deposit(double amount, string& msg) {
        if (amount <= 0)       { msg = "Amount must be positive."; return false; }
        if (amount > 10000000) { msg = "Single deposit limit exceeded."; return false; }
        balance += amount;
        msg = "ok";
        return true;
    }
 
    bool withdraw(double amount, string& msg) {
        if (amount <= 0) { msg = "Amount must be positive."; return false; }
        if (balance - amount < MIN_BALANCE) {
            msg = "Insufficient balance. Minimum Rs.500 must be maintained.";
            return false;
        }
        if (amount > DAILY_LIMIT) {
            msg = "Exceeds daily withdrawal limit of Rs.50,000.";
            return false;
        }
        balance -= amount;
        msg = "ok";
        return true;
    }
};
 
const double Account::MIN_BALANCE = 500.0;
const double Account::DAILY_LIMIT = 50000.0;
 
// ═══════════════════════════════════════════════════════════════════
// CLASS: BankSystem
// ═══════════════════════════════════════════════════════════════════
class BankSystem {
private:
    vector<Account>     accounts;
    vector<Transaction> transactions;
    int                 nextTxnNo;
 
    string fmt(double v) const {
        ostringstream o;
        o << fixed << setprecision(2) << v;
        return o.str();
    }
 
    string trim(const string& s) const {
        size_t a = s.find_first_not_of(" \t\n\r");
        if (a == string::npos) return "";
        size_t b = s.find_last_not_of(" \t\n\r");
        return s.substr(a, b - a + 1);
    }
 
    int findIdx(int accNo) const {
        for (int i = 0; i < (int)accounts.size(); i++)
            if (accounts[i].getAccNo() == accNo) return i;
        return -1;
    }
 
public:
    BankSystem() : nextTxnNo(1) {}
 
    string createAccount(int accNo, const char* rawName, double balance) {
        string name = trim(string(rawName ? rawName : ""));
        if (accNo <= 0)           return "ERROR: Invalid account number.";
        if (findIdx(accNo) != -1) return "ERROR: Account number already exists.";
        if (name.empty())         return "ERROR: Name cannot be empty.";
        if (balance < 500)        return "ERROR: Minimum opening balance is Rs.500.";
 
        accounts.emplace_back(accNo, name, balance);
 
        ostringstream o;
        o << "SUCCESS\n"
          << "Account created successfully.\n"
          << "Account No   : " << accNo << "\n"
          << "Client Name  : " << name << "\n"
          << "Opening Bal  : Rs." << fmt(balance) << "\n"
          << "Total Accounts: " << accounts.size();
        return o.str();
    }
 
    string depositMoney(int accNo, double amount) {
        if (accNo <= 0) return "ERROR: Invalid account number.";
        int i = findIdx(accNo);
        if (i == -1) return "ERROR: Account not found.";
 
        string msg;
        if (!accounts[i].deposit(amount, msg)) return "ERROR: " + msg;
 
        transactions.emplace_back(nextTxnNo++, accNo, "Deposit", amount, accounts[i].getBalance());
 
        ostringstream o;
        o << "SUCCESS\n"
          << "Deposit successful.\n"
          << "Account No   : " << accNo << "\n"
          << "Deposited    : Rs." << fmt(amount) << "\n"
          << "New Balance  : Rs." << fmt(accounts[i].getBalance());
        return o.str();
    }
 
    string withdrawMoney(int accNo, double amount) {
        if (accNo <= 0) return "ERROR: Invalid account number.";
        int i = findIdx(accNo);
        if (i == -1) return "ERROR: Account not found.";
 
        string msg;
        if (!accounts[i].withdraw(amount, msg)) return "ERROR: " + msg;
 
        transactions.emplace_back(nextTxnNo++, accNo, "Withdraw", amount, accounts[i].getBalance());
 
        ostringstream o;
        o << "SUCCESS\n"
          << "Withdrawal successful.\n"
          << "Account No   : " << accNo << "\n"
          << "Withdrawn    : Rs." << fmt(amount) << "\n"
          << "New Balance  : Rs." << fmt(accounts[i].getBalance());
        return o.str();
    }
 
    string balanceEnquiry(int accNo) const {
        if (accNo <= 0) return "ERROR: Invalid account number.";
        int i = findIdx(accNo);
        if (i == -1) return "ERROR: Account not found.";
 
        ostringstream o;
        o << "SUCCESS\n"
          << "Account Details\n"
          << "Account No   : " << accounts[i].getAccNo() << "\n"
          << "Client Name  : " << accounts[i].getName() << "\n"
          << "Balance      : Rs." << fmt(accounts[i].getBalance()) << "\n"
          << "Status       : " << (accounts[i].isActive() ? "Active" : "Inactive");
        return o.str();
    }
 
    string totalMoneyReport() const {
        double total = 0;
        for (auto& a : accounts) total += a.getBalance();
 
        vector<pair<double, string>> top;
        for (auto& a : accounts)
            top.push_back({a.getBalance(), a.getName() + " (#" + to_string(a.getAccNo()) + ")"});
        sort(top.begin(), top.end(), [](auto& a, auto& b){ return a.first > b.first; });
 
        ostringstream o;
        o << "SUCCESS\n"
          << "Total Money Report\n"
          << "Total Accounts : " << accounts.size() << "\n"
          << "Total Txns     : " << transactions.size() << "\n"
          << "Total Funds    : Rs." << fmt(total);
 
        int show = min((int)top.size(), 3);
        if (show > 0) {
            o << "\n\nTop " << show << " by Balance:";
            for (int i = 0; i < show; i++)
                o << "\n  #" << (i+1) << " " << top[i].second << "  Rs." << fmt(top[i].first);
        }
        return o.str();
    }
 
    string lowBalanceReport(double threshold) const {
        if (threshold < 0) return "ERROR: Invalid threshold.";
 
        vector<Account> low;
        for (auto& a : accounts)
            if (a.getBalance() < threshold) low.push_back(a);
 
        if (low.empty())
            return "SUCCESS\nNo accounts found below Rs." + fmt(threshold) + ".";
 
        ostringstream o;
        o << "SUCCESS\n"
          << "Low Balance Report\n"
          << "Threshold: Rs." << fmt(threshold) << "\n"
          << "Accounts : " << low.size() << "\n";
        for (auto& a : low)
            o << "  Acc #" << a.getAccNo() << " | " << a.getName()
              << " | Rs." << fmt(a.getBalance()) << "\n";
        return o.str();
    }
 
    string viewAllAccounts() const {
        if (accounts.empty()) return "EMPTY";
        ostringstream o;
        for (auto& a : accounts)
            o << a.getAccNo() << "|" << a.getName() << "|"
              << fmt(a.getBalance()) << "|" << (a.isActive() ? "1" : "0") << "\n";
        return o.str();
    }
 
    string viewTransactions() const {
        if (transactions.empty()) return "EMPTY";
        ostringstream o;
        for (auto& t : transactions)
            o << t.getTxnNo() << "|" << t.getAccNo() << "|"
              << t.getType() << "|" << fmt(t.getAmount()) << "|"
              << fmt(t.getBalanceAfter()) << "\n";
        return o.str();
    }
 
    string getStats(double threshold) const {
        double total = 0;
        for (auto& a : accounts) total += a.getBalance();
        int low = 0;
        for (auto& a : accounts) if (a.getBalance() < threshold) low++;
        ostringstream o;
        o << accounts.size() << "|" << transactions.size() << "|"
          << fmt(total) << "|" << low;
        return o.str();
    }
};
 
// ── Singleton + result buffer ──────────────────────────────────────
static BankSystem bank;
static string buf;
 
extern "C" {
 
EMSCRIPTEN_KEEPALIVE
const char* create_account(int accNo, const char* name, double balance) {
    buf = bank.createAccount(accNo, name, balance);
    return buf.c_str();
}
 
EMSCRIPTEN_KEEPALIVE
const char* deposit_money(int accNo, double amount) {
    buf = bank.depositMoney(accNo, amount);
    return buf.c_str();
}
 
EMSCRIPTEN_KEEPALIVE
const char* withdraw_money(int accNo, double amount) {
    buf = bank.withdrawMoney(accNo, amount);
    return buf.c_str();
}
 
EMSCRIPTEN_KEEPALIVE
const char* balance_enquiry(int accNo) {
    buf = bank.balanceEnquiry(accNo);
    return buf.c_str();
}
 
EMSCRIPTEN_KEEPALIVE
const char* total_money_report() {
    buf = bank.totalMoneyReport();
    return buf.c_str();
}
 
EMSCRIPTEN_KEEPALIVE
const char* low_balance_report(double threshold) {
    buf = bank.lowBalanceReport(threshold);
    return buf.c_str();
}
 
EMSCRIPTEN_KEEPALIVE
const char* view_all_accounts() {
    buf = bank.viewAllAccounts();
    return buf.c_str();
}
 
EMSCRIPTEN_KEEPALIVE
const char* view_transactions() {
    buf = bank.viewTransactions();
    return buf.c_str();
}
 
EMSCRIPTEN_KEEPALIVE
const char* get_stats(double threshold) {
    buf = bank.getStats(threshold);
    return buf.c_str();
}
 
} // extern "C"
 
int main() { return 0; }
