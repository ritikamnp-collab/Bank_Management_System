
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <limits>
using namespace std;

struct Account{
    int accNo;
    string name;
    char type;
    double balance;
};

const string ACCOUNT_FILE="accounts.txt";
const string TRANS_FILE="transactions.txt";

string getTimeStamp(){
    time_t now=time(0);
    string s=ctime(&now);
    if(!s.empty() && s.back()=='\n') s.pop_back();
    return s;
}

void logTransaction(int accNo,string action,double amount){
    ofstream fout(TRANS_FILE,ios::app);
    fout<<accNo<<"|"<<action<<"|"<<amount<<"|"<<getTimeStamp()<<'\n';
}

vector<Account> loadAccounts(){
    vector<Account> accounts;
    ifstream fin(ACCOUNT_FILE);
    string line;

    while(getline(fin,line)){
        if(line.empty()) continue;

        stringstream ss(line);
        Account a;
        string temp;

        getline(ss,temp,'|');
        a.accNo=stoi(temp);

        getline(ss,a.name,'|');

        getline(ss,temp,'|');
        if(temp.empty()) continue;
        a.type=temp[0];

        getline(ss,temp,'|');
        a.balance=stod(temp);

        accounts.push_back(a);
    }
    return accounts;
}

void saveAccounts(const vector<Account>& accounts){
    ofstream fout(ACCOUNT_FILE);
    for(const auto &a:accounts){
        fout<<a.accNo<<"|"<<a.name<<"|"<<a.type<<"|"<<a.balance<<"\n";
    }
}

int getNextAccNo(){
    vector<Account> accounts=loadAccounts();
    int mx=1000;
    for(auto &a:accounts) mx=max(mx,a.accNo);
    return mx+1;
}

Account* findAccount(vector<Account>& accounts,int accNo){
    for(auto &a:accounts)
        if(a.accNo==accNo) return &a;
    return nullptr;
}

void createAccount(){
    vector<Account> accounts=loadAccounts();
    Account a;

    a.accNo=getNextAccNo();

    cin.ignore(numeric_limits<streamsize>::max(),'\n');

    cout<<"\nAccount Number: "<<a.accNo;
    cout<<"\nEnter Name: ";
    getline(cin,a.name);

    do{
        cout<<"Enter Account Type (S/C): ";
        cin>>a.type;
        a.type=toupper(a.type);
    }while(a.type!='S' && a.type!='C');

    cout<<"Enter Initial Balance: ";
    cin>>a.balance;

    if(a.type=='S' && a.balance<500) a.balance=500;
    if(a.type=='C' && a.balance<1000) a.balance=1000;

    accounts.push_back(a);
    saveAccounts(accounts);

    logTransaction(a.accNo,"ACCOUNT_CREATED",a.balance);

    cout<<"\nAccount Created Successfully.\n";
}

void deposit(){
    auto accounts=loadAccounts();
    int accNo; double amt;

    cout<<"Enter Account Number: ";
    cin>>accNo;

    Account* acc=findAccount(accounts,accNo);
    if(!acc){ cout<<"Account Not Found\n"; return; }

    cout<<"Enter Amount: ";
    cin>>amt;

    if(amt<=0){ cout<<"Invalid Amount\n"; return; }

    acc->balance+=amt;
    saveAccounts(accounts);
    logTransaction(accNo,"DEPOSIT",amt);

    cout<<"Deposit Successful\n";
}

void withdrawAmount(){
    auto accounts=loadAccounts();
    int accNo; double amt;

    cout<<"Enter Account Number: ";
    cin>>accNo;

    Account* acc=findAccount(accounts,accNo);
    if(!acc){ cout<<"Account Not Found\n"; return; }

    cout<<"Enter Amount: ";
    cin>>amt;

    if(amt<=0){ cout<<"Invalid Amount\n"; return; }

    double minBal=(acc->type=='S')?500:1000;

    if(acc->balance-amt<minBal){
        cout<<"Minimum Balance Rule Violated\n";
        return;
    }

    acc->balance-=amt;
    saveAccounts(accounts);
    logTransaction(accNo,"WITHDRAW",amt);

    cout<<"Withdrawal Successful\n";
}

void balanceEnquiry(){
    auto accounts=loadAccounts();
    int accNo;

    cout<<"Enter Account Number: ";
    cin>>accNo;

    Account* acc=findAccount(accounts,accNo);
    if(!acc){ cout<<"Account Not Found\n"; return; }

    cout<<"\nAccount No : "<<acc->accNo
        <<"\nName       : "<<acc->name
        <<"\nType       : "<<acc->type
        <<"\nBalance    : Rs "<<fixed<<setprecision(2)<<acc->balance<<"\n";
}

void displayAll(){
    auto accounts=loadAccounts();

    cout<<"\n================================================================\n";
    cout<<left<<setw(10)<<"AccNo"<<setw(25)<<"Name"<<setw(10)<<"Type"<<setw(15)<<"Balance"<<'\n';
    cout<<"================================================================\n";

    for(auto &a:accounts){
        cout<<left<<setw(10)<<a.accNo
            <<setw(25)<<a.name
            <<setw(10)<<a.type
            <<setw(15)<<fixed<<setprecision(2)<<a.balance<<"\n";
    }
}

void modifyAccount(){
    auto accounts=loadAccounts();
    int accNo;

    cout<<"Enter Account Number: ";
    cin>>accNo;

    Account* acc=findAccount(accounts,accNo);
    if(!acc){ cout<<"Account Not Found\n"; return; }

    cin.ignore(numeric_limits<streamsize>::max(),'\n');

    cout<<"Enter New Name: ";
    getline(cin,acc->name);

    char newType;
    do{
        cout<<"Enter New Account Type (S/C): ";
        cin>>newType;
        newType=toupper(newType);
    }while(newType!='S' && newType!='C');

    acc->type=newType;

    double minBal=(newType=='S')?500:1000;
    if(acc->balance<minBal) acc->balance=minBal;

    saveAccounts(accounts);
    logTransaction(accNo,"ACCOUNT_MODIFIED",acc->balance);

    cout<<"Account Updated Successfully\n";
}

void deleteAccount(){
    auto accounts=loadAccounts();
    int accNo;

    cout<<"Enter Account Number: ";
    cin>>accNo;

    auto it=remove_if(accounts.begin(),accounts.end(),
        [accNo](const Account&a){ return a.accNo==accNo; });

    if(it==accounts.end()){
        cout<<"Account Not Found\n";
        return;
    }

    accounts.erase(it,accounts.end());
    saveAccounts(accounts);

    logTransaction(accNo,"ACCOUNT_DELETED",0);

    cout<<"Account Deleted Successfully\n";
}

void searchAccount(){
    auto accounts=loadAccounts();
    int accNo;

    cout<<"Enter Account Number: ";
    cin>>accNo;

    Account* acc=findAccount(accounts,accNo);
    if(!acc){ cout<<"Account Not Found\n"; return; }

    cout<<"\nAccount Number : "<<acc->accNo
        <<"\nName           : "<<acc->name
        <<"\nType           : "<<acc->type
        <<"\nBalance        : Rs "<<fixed<<setprecision(2)<<acc->balance<<"\n";
}

void showTransactionHistory(){
    int accNo;
    cout<<"Enter Account Number: ";
    cin>>accNo;

    ifstream fin(TRANS_FILE);
    string line;
    bool found=false;

    cout<<"\n===== TRANSACTION HISTORY =====\n";

    while(getline(fin,line)){
        stringstream ss(line);
        string acc,action,amount,time;

        getline(ss,acc,'|');
        getline(ss,action,'|');
        getline(ss,amount,'|');
        getline(ss,time);

        if(!acc.empty() && stoi(acc)==accNo){
            found=true;
            cout<<action<<" | Rs "<<amount<<" | "<<time<<"\n";
        }
    }

    if(!found) cout<<"No Transactions Found\n";
}

void exportTransactionsCSV(){
    ifstream fin(TRANS_FILE);
    if(!fin){
        cout<<"No Transaction File Found\n";
        return;
    }

    ofstream csv("transactions_export.csv");
    csv<<"AccountNo,Action,Amount,Timestamp\n";

    string line;
    while(getline(fin,line)){
        replace(line.begin(),line.end(),'|',',');
        csv<<line<<"\n";
    }

    cout<<"CSV Exported Successfully: transactions_export.csv\n";
}

int main(){
    int choice;

    do{
        cout<<"\n\n========== BANK MANAGEMENT SYSTEM ==========\n";
        cout<<"1. Create Account\n";
        cout<<"2. Deposit\n";
        cout<<"3. Withdraw\n";
        cout<<"4. Balance Enquiry\n";
        cout<<"5. Display All Accounts\n";
        cout<<"6. Modify Account\n";
        cout<<"7. Delete Account\n";
        cout<<"8. Transaction History\n";
        cout<<"9. Search Account\n";
        cout<<"10. Export Transactions CSV\n";
        cout<<"11. Exit\n";
        cout<<"Enter Choice: ";
        cin>>choice;

        switch(choice){
            case 1: createAccount(); break;
            case 2: deposit(); break;
            case 3: withdrawAmount(); break;
            case 4: balanceEnquiry(); break;
            case 5: displayAll(); break;
            case 6: modifyAccount(); break;
            case 7: deleteAccount(); break;
            case 8: showTransactionHistory(); break;
            case 9: searchAccount(); break;
            case 10: exportTransactionsCSV(); break;
            case 11: cout<<"Thank You!\n"; break;
            default: cout<<"Invalid Choice\n";
        }
    }while(choice!=11);

    return 0;
}
