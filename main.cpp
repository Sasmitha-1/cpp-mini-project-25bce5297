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
#include <ctime>

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
// CLASS: Cheque
// ═══════════════════════════════════════════════════════════════════
class Cheque {
private:
    int    chequeNo;
    int    accNo;
    string payee;
    double amount;
    string status; // ISSUED | CLEARED | CANCELLED
    string issuedDate;
public:
    Cheque(int no, int acc, const string& p, double amt, const string& date)
        : chequeNo(no), accNo(acc), payee(p), amount(amt),
          status("ISSUED"), issuedDate(date) {}
    int    getChequeNo()  const { return chequeNo; }
    int    getAccNo()     const { return accNo; }
    string getPayee()     const { return payee; }
    double getAmount()    const { return amount; }
    string getStatus()    const { return status; }
    string getIssuedDate()const { return issuedDate; }
    void   cancel()             { status = "CANCELLED"; }
    void   clear()              { status = "CLEARED"; }
};

// ═══════════════════════════════════════════════════════════════════
// CLASS: FixedDeposit
// ═══════════════════════════════════════════════════════════════════
class FixedDeposit {
private:
    int    fdNo;
    int    accNo;
    double principal;
    int    tenureMonths;
    double ratePercent;
    string startDate;
    string status; // ACTIVE | CLOSED
public:
    FixedDeposit(int no, int acc, double p, int t, double r, const string& d)
        : fdNo(no), accNo(acc), principal(p), tenureMonths(t),
          ratePercent(r), startDate(d), status("ACTIVE") {}
    int    getFdNo()       const { return fdNo; }
    int    getAccNo()      const { return accNo; }
    double getPrincipal()  const { return principal; }
    int    getTenure()     const { return tenureMonths; }
    double getRate()       const { return ratePercent; }
    string getStartDate()  const { return startDate; }
    string getStatus()     const { return status; }
    double getMaturityAmt()const {
        return principal * (1 + (ratePercent / 100.0) * (tenureMonths / 12.0));
    }
    void close() { status = "CLOSED"; }
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
        balance += amount; msg = "ok"; return true;
    }
    bool withdraw(double amount, string& msg) {
        if (amount <= 0) { msg = "Amount must be positive."; return false; }
        if (balance - amount < MIN_BALANCE) {
            msg = "Insufficient balance. Minimum Rs.500 must be maintained."; return false;
        }
        if (amount > DAILY_LIMIT) {
            msg = "Exceeds daily withdrawal limit of Rs.50,000."; return false;
        }
        balance -= amount; msg = "ok"; return true;
    }
};
const double Account::MIN_BALANCE = 500.0;
const double Account::DAILY_LIMIT = 50000.0;

// ═══════════════════════════════════════════════════════════════════
// CLASS: BankSystem
// ═══════════════════════════════════════════════════════════════════
class BankSystem {
private:
    vector<Account>      accounts;
    vector<Transaction>  transactions;
    vector<Cheque>       cheques;
    vector<FixedDeposit> fds;
    int nextTxnNo, nextChequeNo, nextFdNo;

    string fmt(double v) const {
        ostringstream o; o << fixed << setprecision(2) << v; return o.str();
    }
    string trim(const string& s) const {
        size_t a = s.find_first_not_of(" \t\n\r");
        if (a == string::npos) return "";
        size_t b = s.find_last_not_of(" \t\n\r");
        return s.substr(a, b - a + 1);
    }
    int findAccIdx(int accNo) const {
        for (int i = 0; i < (int)accounts.size(); i++)
            if (accounts[i].getAccNo() == accNo) return i;
        return -1;
    }
    int findChequeIdx(int chequeNo) const {
        for (int i = 0; i < (int)cheques.size(); i++)
            if (cheques[i].getChequeNo() == chequeNo) return i;
        return -1;
    }
    int findFdIdx(int fdNo) const {
        for (int i = 0; i < (int)fds.size(); i++)
            if (fds[i].getFdNo() == fdNo) return i;
        return -1;
    }
    string todayStr() const {
        time_t t = time(nullptr);
        char buf[16]; strftime(buf, sizeof(buf), "%d %b %Y", localtime(&t));
        return string(buf);
    }

public:
    BankSystem() : nextTxnNo(1), nextChequeNo(1001), nextFdNo(5001) {}

    // ── Create Account ─────────────────────────────────────────────
    string createAccount(int accNo, const char* rawName, double balance) {
        string name = trim(string(rawName ? rawName : ""));
        if (accNo <= 0)           return "ERROR: Invalid account number.";
        if (findAccIdx(accNo)!=-1)return "ERROR: Account number already exists.";
        if (name.empty())         return "ERROR: Name cannot be empty.";
        if (balance < 500)        return "ERROR: Minimum opening balance is Rs.500.";
        accounts.emplace_back(accNo, name, balance);
        ostringstream o;
        o<<"SUCCESS\nAccount created successfully.\nAccount No   : "<<accNo
         <<"\nClient Name  : "<<name<<"\nOpening Bal  : Rs."<<fmt(balance)
         <<"\nTotal Accounts: "<<accounts.size();
        return o.str();
    }

    // ── Deposit ────────────────────────────────────────────────────
    string depositMoney(int accNo, double amount) {
        if (accNo<=0) return "ERROR: Invalid account number.";
        int i = findAccIdx(accNo);
        if (i==-1) return "ERROR: Account not found.";
        string msg;
        if (!accounts[i].deposit(amount, msg)) return "ERROR: "+msg;
        transactions.emplace_back(nextTxnNo++, accNo, "Deposit", amount, accounts[i].getBalance());
        ostringstream o;
        o<<"SUCCESS\nDeposit successful.\nAccount No   : "<<accNo
         <<"\nDeposited    : Rs."<<fmt(amount)<<"\nNew Balance  : Rs."<<fmt(accounts[i].getBalance());
        return o.str();
    }

    // ── Withdraw ───────────────────────────────────────────────────
    string withdrawMoney(int accNo, double amount) {
        if (accNo<=0) return "ERROR: Invalid account number.";
        int i = findAccIdx(accNo);
        if (i==-1) return "ERROR: Account not found.";
        string msg;
        if (!accounts[i].withdraw(amount, msg)) return "ERROR: "+msg;
        transactions.emplace_back(nextTxnNo++, accNo, "Withdraw", amount, accounts[i].getBalance());
        ostringstream o;
        o<<"SUCCESS\nWithdrawal successful.\nAccount No   : "<<accNo
         <<"\nWithdrawn    : Rs."<<fmt(amount)<<"\nNew Balance  : Rs."<<fmt(accounts[i].getBalance());
        return o.str();
    }

    // ── Balance Enquiry ────────────────────────────────────────────
    string balanceEnquiry(int accNo) const {
        if (accNo<=0) return "ERROR: Invalid account number.";
        int i = findAccIdx(accNo);
        if (i==-1) return "ERROR: Account not found.";
        ostringstream o;
        o<<"SUCCESS\nAccount Details\nAccount No   : "<<accounts[i].getAccNo()
         <<"\nClient Name  : "<<accounts[i].getName()
         <<"\nBalance      : Rs."<<fmt(accounts[i].getBalance())
         <<"\nStatus       : "<<(accounts[i].isActive()?"Active":"Inactive");
        return o.str();
    }

    // ── Total Money Report ─────────────────────────────────────────
    string totalMoneyReport() const {
        double total=0;
        for (auto& a:accounts) total+=a.getBalance();
        vector<pair<double,string>> top;
        for (auto& a:accounts)
            top.push_back({a.getBalance(), a.getName()+" (#"+to_string(a.getAccNo())+")"});
        sort(top.begin(),top.end(),[](auto& a,auto& b){return a.first>b.first;});
        ostringstream o;
        o<<"SUCCESS\nTotal Money Report\nTotal Accounts : "<<accounts.size()
         <<"\nTotal Txns     : "<<transactions.size()
         <<"\nTotal Funds    : Rs."<<fmt(total);
        int show=min((int)top.size(),3);
        if(show>0){
            o<<"\n\nTop "<<show<<" by Balance:";
            for(int i=0;i<show;i++)
                o<<"\n  #"<<(i+1)<<" "<<top[i].second<<"  Rs."<<fmt(top[i].first);
        }
        return o.str();
    }

    // ── Low Balance Report ─────────────────────────────────────────
    string lowBalanceReport(double threshold) const {
        if (threshold<0) return "ERROR: Invalid threshold.";
        vector<Account> low;
        for (auto& a:accounts) if(a.getBalance()<threshold) low.push_back(a);
        if (low.empty()) return "SUCCESS\nNo accounts found below Rs."+fmt(threshold)+".";
        ostringstream o;
        o<<"SUCCESS\nLow Balance Report\nThreshold: Rs."<<fmt(threshold)<<"\nAccounts : "<<low.size()<<"\n";
        for (auto& a:low)
            o<<"  Acc #"<<a.getAccNo()<<" | "<<a.getName()<<" | Rs."<<fmt(a.getBalance())<<"\n";
        return o.str();
    }

    // ── View All Accounts ──────────────────────────────────────────
    string viewAllAccounts() const {
        if (accounts.empty()) return "EMPTY";
        ostringstream o;
        for (auto& a:accounts)
            o<<a.getAccNo()<<"|"<<a.getName()<<"|"<<fmt(a.getBalance())<<"|"<<(a.isActive()?"1":"0")<<"\n";
        return o.str();
    }

    // ── View Transactions ──────────────────────────────────────────
    string viewTransactions() const {
        if (transactions.empty()) return "EMPTY";
        ostringstream o;
        for (auto& t:transactions)
            o<<t.getTxnNo()<<"|"<<t.getAccNo()<<"|"<<t.getType()<<"|"
             <<fmt(t.getAmount())<<"|"<<fmt(t.getBalanceAfter())<<"\n";
        return o.str();
    }

    // ── Get Stats ──────────────────────────────────────────────────
    string getStats(double threshold) const {
        double total=0;
        for (auto& a:accounts) total+=a.getBalance();
        int low=0;
        for (auto& a:accounts) if(a.getBalance()<threshold) low++;
        ostringstream o;
        o<<accounts.size()<<"|"<<transactions.size()<<"|"<<fmt(total)<<"|"<<low;
        return o.str();
    }

    // ── Issue Cheque ───────────────────────────────────────────────
    string issueCheque(int accNo, const char* rawPayee, double amount) {
        string payee = trim(string(rawPayee ? rawPayee : ""));
        if (accNo<=0) return "ERROR: Invalid account number.";
        int i = findAccIdx(accNo);
        if (i==-1) return "ERROR: Account not found.";
        if (payee.empty()) return "ERROR: Payee name cannot be empty.";
        if (amount<=0) return "ERROR: Amount must be positive.";
        if (accounts[i].getBalance()<amount)
            return "ERROR: Insufficient balance to issue cheque.";

        int cNo = nextChequeNo++;
        cheques.emplace_back(cNo, accNo, payee, amount, todayStr());

        ostringstream o;
        o<<"SUCCESS\nCheque issued successfully.\nCheque No  : "<<cNo
         <<"\nAccount No : "<<accNo
         <<"\nPay To     : "<<payee
         <<"\nAmount     : Rs."<<fmt(amount)
         <<"\nDate       : "<<todayStr()
         <<"\nStatus     : ISSUED";
        return o.str();
    }

    // ── Cancel Cheque ──────────────────────────────────────────────
    string cancelCheque(int chequeNo) {
        int i = findChequeIdx(chequeNo);
        if (i==-1) return "ERROR: Cheque not found.";
        if (cheques[i].getStatus()!="ISSUED")
            return "ERROR: Only ISSUED cheques can be cancelled. Status: "+cheques[i].getStatus();
        cheques[i].cancel();
        ostringstream o;
        o<<"SUCCESS\nCheque #"<<chequeNo<<" cancelled successfully.\nStatus: CANCELLED";
        return o.str();
    }

    // ── Clear Cheque (deducts balance) ─────────────────────────────
    string clearCheque(int chequeNo) {
        int ci = findChequeIdx(chequeNo);
        if (ci==-1) return "ERROR: Cheque not found.";
        if (cheques[ci].getStatus()!="ISSUED")
            return "ERROR: Only ISSUED cheques can be cleared. Status: "+cheques[ci].getStatus();
        int ai = findAccIdx(cheques[ci].getAccNo());
        if (ai==-1) return "ERROR: Account not found.";
        string msg;
        if (!accounts[ai].withdraw(cheques[ci].getAmount(), msg)) return "ERROR: "+msg;
        cheques[ci].clear();
        transactions.emplace_back(nextTxnNo++, cheques[ci].getAccNo(),
            "Cheque", cheques[ci].getAmount(), accounts[ai].getBalance());
        ostringstream o;
        o<<"SUCCESS\nCheque #"<<chequeNo<<" cleared.\nDeducted : Rs."<<fmt(cheques[ci].getAmount())
         <<"\nBalance  : Rs."<<fmt(accounts[ai].getBalance());
        return o.str();
    }

    // ── View Cheques ───────────────────────────────────────────────
    string viewCheques() const {
        if (cheques.empty()) return "EMPTY";
        ostringstream o;
        for (auto& c:cheques)
            o<<c.getChequeNo()<<"|"<<c.getAccNo()<<"|"<<c.getPayee()<<"|"
             <<fmt(c.getAmount())<<"|"<<c.getStatus()<<"|"<<c.getIssuedDate()<<"\n";
        return o.str();
    }

    // ── Create FD ──────────────────────────────────────────────────
    string createFD(int accNo, double principal, int tenureMonths) {
        if (accNo<=0) return "ERROR: Invalid account number.";
        int i = findAccIdx(accNo);
        if (i==-1) return "ERROR: Account not found.";
        if (principal<1000) return "ERROR: Minimum FD amount is Rs.1,000.";
        if (tenureMonths<1||tenureMonths>120) return "ERROR: Tenure must be 1-120 months.";
        string msg;
        if (!accounts[i].withdraw(principal, msg)) return "ERROR: "+msg;

        // Rate based on tenure
        double rate = tenureMonths<=12 ? 6.5 : tenureMonths<=36 ? 7.0 : 7.5;
        int fdNo = nextFdNo++;
        fds.emplace_back(fdNo, accNo, principal, tenureMonths, rate, todayStr());
        transactions.emplace_back(nextTxnNo++, accNo, "FD Created", principal, accounts[i].getBalance());

        double maturity = fds.back().getMaturityAmt();
        ostringstream o;
        o<<"SUCCESS\nFixed Deposit created.\nFD No      : "<<fdNo
         <<"\nAccount No : "<<accNo
         <<"\nPrincipal  : Rs."<<fmt(principal)
         <<"\nTenure     : "<<tenureMonths<<" months"
         <<"\nRate       : "<<rate<<"% p.a."
         <<"\nMaturity   : Rs."<<fmt(maturity)
         <<"\nStart Date : "<<todayStr();
        return o.str();
    }

    // ── View FDs ───────────────────────────────────────────────────
    string viewFDs() const {
        if (fds.empty()) return "EMPTY";
        ostringstream o;
        for (auto& f:fds)
            o<<f.getFdNo()<<"|"<<f.getAccNo()<<"|"<<fmt(f.getPrincipal())<<"|"
             <<f.getTenure()<<"|"<<f.getRate()<<"|"<<fmt(f.getMaturityAmt())<<"|"
             <<f.getStatus()<<"|"<<f.getStartDate()<<"\n";
        return o.str();
    }

    // ── Close FD ───────────────────────────────────────────────────
    string closeFD(int fdNo) {
        int fi = findFdIdx(fdNo);
        if (fi==-1) return "ERROR: FD not found.";
        if (fds[fi].getStatus()!="ACTIVE") return "ERROR: FD is already closed.";
        int ai = findAccIdx(fds[fi].getAccNo());
        if (ai==-1) return "ERROR: Account not found.";
        double maturity = fds[fi].getMaturityAmt();
        string msg;
        accounts[ai].deposit(maturity, msg);
        fds[fi].close();
        transactions.emplace_back(nextTxnNo++, fds[fi].getAccNo(),
            "FD Matured", maturity, accounts[ai].getBalance());
        ostringstream o;
        o<<"SUCCESS\nFD #"<<fdNo<<" closed.\nMaturity Amount : Rs."<<fmt(maturity)
         <<"\nCredited to Acc : "<<fds[fi].getAccNo()
         <<"\nNew Balance     : Rs."<<fmt(accounts[ai].getBalance());
        return o.str();
    }

    // ── Account Statement ──────────────────────────────────────────
    string getStatement(int accNo) const {
        if (accNo<=0) return "ERROR: Invalid account number.";
        int ai = findAccIdx(accNo);
        if (ai==-1) return "ERROR: Account not found.";
        ostringstream o;
        o<<"SUCCESS\n"
         <<"VELORA BANK — ACCOUNT STATEMENT\n"
         <<"Account No   : "<<accounts[ai].getAccNo()<<"\n"
         <<"Client Name  : "<<accounts[ai].getName()<<"\n"
         <<"Balance      : Rs."<<fmt(accounts[ai].getBalance())<<"\n"
         <<"---\n";
        bool found=false;
        for (auto& t:transactions) {
            if (t.getAccNo()==accNo) {
                o<<t.getTxnNo()<<"|"<<t.getType()<<"|"<<fmt(t.getAmount())
                 <<"|"<<fmt(t.getBalanceAfter())<<"\n";
                found=true;
            }
        }
        if (!found) o<<"No transactions on this account.\n";
        return o.str();
    }
};

// ── Singleton ──────────────────────────────────────────────────────
static BankSystem bank;
static string buf;

extern "C" {

EMSCRIPTEN_KEEPALIVE
const char* create_account(int accNo, const char* name, double balance) {
    buf = bank.createAccount(accNo, name, balance); return buf.c_str();
}
EMSCRIPTEN_KEEPALIVE
const char* deposit_money(int accNo, double amount) {
    buf = bank.depositMoney(accNo, amount); return buf.c_str();
}
EMSCRIPTEN_KEEPALIVE
const char* withdraw_money(int accNo, double amount) {
    buf = bank.withdrawMoney(accNo, amount); return buf.c_str();
}
EMSCRIPTEN_KEEPALIVE
const char* balance_enquiry(int accNo) {
    buf = bank.balanceEnquiry(accNo); return buf.c_str();
}
EMSCRIPTEN_KEEPALIVE
const char* total_money_report() {
    buf = bank.totalMoneyReport(); return buf.c_str();
}
EMSCRIPTEN_KEEPALIVE
const char* low_balance_report(double threshold) {
    buf = bank.lowBalanceReport(threshold); return buf.c_str();
}
EMSCRIPTEN_KEEPALIVE
const char* view_all_accounts() {
    buf = bank.viewAllAccounts(); return buf.c_str();
}
EMSCRIPTEN_KEEPALIVE
const char* view_transactions() {
    buf = bank.viewTransactions(); return buf.c_str();
}
EMSCRIPTEN_KEEPALIVE
const char* get_stats(double threshold) {
    buf = bank.getStats(threshold); return buf.c_str();
}
EMSCRIPTEN_KEEPALIVE
const char* issue_cheque(int accNo, const char* payee, double amount) {
    buf = bank.issueCheque(accNo, payee, amount); return buf.c_str();
}
EMSCRIPTEN_KEEPALIVE
const char* cancel_cheque(int chequeNo) {
    buf = bank.cancelCheque(chequeNo); return buf.c_str();
}
EMSCRIPTEN_KEEPALIVE
const char* clear_cheque(int chequeNo) {
    buf = bank.clearCheque(chequeNo); return buf.c_str();
}
EMSCRIPTEN_KEEPALIVE
const char* view_cheques() {
    buf = bank.viewCheques(); return buf.c_str();
}
EMSCRIPTEN_KEEPALIVE
const char* create_fd(int accNo, double principal, int tenureMonths) {
    buf = bank.createFD(accNo, principal, tenureMonths); return buf.c_str();
}
EMSCRIPTEN_KEEPALIVE
const char* view_fds() {
    buf = bank.viewFDs(); return buf.c_str();
}
EMSCRIPTEN_KEEPALIVE
const char* close_fd(int fdNo) {
    buf = bank.closeFD(fdNo); return buf.c_str();
}
EMSCRIPTEN_KEEPALIVE
const char* get_statement(int accNo) {
    buf = bank.getStatement(accNo); return buf.c_str();
}

} // extern "C"

int main() { return 0; }
