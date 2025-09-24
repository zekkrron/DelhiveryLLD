#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

// Use standard namespace for simplicity as requested
using namespace std;

// Forward declarations to resolve circular dependencies if any
class BankAccount;
class Bank;

// Represents a user's bank card
class Card {
private:
    string cardNumber;
    string customerName;

public:
    Card(const string& num, const string& name) : cardNumber(num), customerName(name) {}

    string getCardNumber() const {
        return cardNumber;
    }
};

// Represents a user's bank account
class BankAccount {
private:
    string accountNumber;
    double balance;

public:
    BankAccount(const string& accNum, double initialBalance) : accountNumber(accNum), balance(initialBalance) {}

    double getBalance() const {
        return balance;
    }

    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            cout << "Deposit successful. New balance: $" << balance << endl;
        }
    }

    bool withdraw(double amount) {
        if (amount > 0 && balance >= amount) {
            balance -= amount;
            cout << "Withdrawal successful. New balance: $" << balance << endl;
            return true;
        }
        cout << "Withdrawal failed. Insufficient funds or invalid amount." << endl;
        return false;
    }
};

// Singleton Bank class - the central authority
class Bank {
private:
    // A map to link card numbers to bank accounts
    map<string, BankAccount*> accounts;
    // A map to store card number and PIN for authentication
    map<string, string> cardPinMap;

    // Private constructor for Singleton
    Bank() {}

    // Disallow copy and assignment
    Bank(const Bank&) = delete;
    Bank& operator=(const Bank&) = delete;

public:
    // Static method to get the single instance
    static Bank& getInstance() {
        static Bank instance; // Created only once
        return instance;
    }

    // Add account and card details for simulation
    void addAccount(BankAccount* account, Card* card, const string& pin) {
        accounts[card->getCardNumber()] = account;
        cardPinMap[card->getCardNumber()] = pin;
    }
    
    bool authenticateUser(const string& cardNumber, const string& pin) {
        if (cardPinMap.count(cardNumber) && cardPinMap[cardNumber] == pin) {
            cout << "Authentication successful." << endl;
            return true;
        }
        cout << "Authentication failed: Invalid card number or PIN." << endl;
        return false;
    }

    BankAccount* getAccount(const string& cardNumber) {
        if (accounts.count(cardNumber)) {
            return accounts[cardNumber];
        }
        return nullptr;
    }
};


// --- Strategy Pattern for Transactions ---

// Abstract base class for all transactions
class Transaction {
public:
    virtual void execute(BankAccount* account) = 0;
    virtual ~Transaction() {} // Virtual destructor for proper cleanup
};

// Concrete Strategy: Balance Inquiry
class BalanceInquiry : public Transaction {
public:
    void execute(BankAccount* account) override {
        cout << "--- Balance Inquiry ---" << endl;
        if (account) {
            cout << "Current Balance: $" << account->getBalance() << endl;
        }
    }
};

// Concrete Strategy: Withdraw Transaction
class WithdrawTransaction : public Transaction {
private:
    double amount;

public:
    WithdrawTransaction(double amt) : amount(amt) {}

    void execute(BankAccount* account) override {
        cout << "--- Withdrawal ---" << endl;
        if (account) {
            account->withdraw(amount);
        }
    }
};

// Concrete Strategy: Deposit Transaction
class DepositTransaction : public Transaction {
private:
    double amount;

public:
    DepositTransaction(double amt) : amount(amt) {}

    void execute(BankAccount* account) override {
        cout << "--- Deposit ---" << endl;
        if (account) {
            account->deposit(amount);
        }
    }
};


// The main ATM machine class
class ATM {
private:
    enum class ATMState { IDLE, CARD_INSERTED, AUTHENTICATED };
    
    ATMState currentState;
    Card* currentCard;
    Bank& bank; // Reference to the singleton bank

public:
    ATM() : currentState(ATMState::IDLE), currentCard(nullptr), bank(Bank::getInstance()) {
        cout << "ATM is now online. State: IDLE" << endl;
    }

    void insertCard(Card* card) {
        if (currentState == ATMState::IDLE) {
            currentCard = card;
            currentState = ATMState::CARD_INSERTED;
            cout << "Card inserted. Please enter your PIN. State: CARD_INSERTED" << endl;
        } else {
            cout << "Error: Another session is active." << endl;
        }
    }

    void enterPin(const string& pin) {
        if (currentState == ATMState::CARD_INSERTED && currentCard != nullptr) {
            if (bank.authenticateUser(currentCard->getCardNumber(), pin)) {
                currentState = ATMState::AUTHENTICATED;
                cout << "PIN accepted. Please select an operation. State: AUTHENTICATED" << endl;
            } else {
                cout << "Incorrect PIN. Please try again." << endl;
            }
        } else {
            cout << "Error: Please insert a card first." << endl;
        }
    }

    void selectOperation() {
        if (currentState != ATMState::AUTHENTICATED) {
            cout << "Error: Please authenticate first." << endl;
            return;
        }

        cout << "\nSelect Operation:" << endl;
        cout << "1. Check Balance" << endl;
        cout << "2. Withdraw" << endl;
        cout << "3. Deposit" << endl;
        cout << "4. Exit" << endl;
        
        int choice;
        cin >> choice;
        
        Transaction* transaction = nullptr;
        BankAccount* account = bank.getAccount(currentCard->getCardNumber());
        double amount;

        switch (choice) {
            case 1:
                transaction = new BalanceInquiry();
                break;
            case 2:
                cout << "Enter amount to withdraw: ";
                cin >> amount;
                transaction = new WithdrawTransaction(amount);
                break;
            case 3:
                cout << "Enter amount to deposit: ";
                cin >> amount;
                transaction = new DepositTransaction(amount);
                break;
            case 4:
                cout << "Exiting..." << endl;
                ejectCard();
                return;
            default:
                cout << "Invalid choice." << endl;
                return;
        }

        if (transaction != nullptr) {
            transaction->execute(account);
            delete transaction; // Clean up the created transaction object
        }
        
        // After one transaction, we could prompt for another or just end the session.
        // For simplicity, let's end the session here.
        ejectCard();
    }

    void ejectCard() {
        cout << "Card ejected. Thank you for using our ATM! State: IDLE" << endl;
        currentCard = nullptr;
        currentState = ATMState::IDLE;
    }
};


// --- Main function to simulate the ATM usage ---
int main() {
    // 1. Setup Bank and Accounts (this would be done by the bank admin)
    Bank& centralBank = Bank::getInstance();
    
    Card card1("1111-2222-3333-4444", "John Doe");
    BankAccount account1("ACC001", 1500.00);
    centralBank.addAccount(&account1, &card1, "1234");

    Card card2("5555-6666-7777-8888", "Jane Smith");
    BankAccount account2("ACC002", 500.00);
    centralBank.addAccount(&account2, &card2, "9876");

    // 2. Initialize the ATM
    ATM myATM;

    // 3. Simulate a user session
    cout << "\n--- JOHN DOE's SESSION ---" << endl;
    myATM.insertCard(&card1);
    myATM.enterPin("1234"); // Correct PIN
    myATM.selectOperation(); // User will choose to withdraw

    cout << "\n--- JANE SMITH's SESSION ---" << endl;
    myATM.insertCard(&card2);
    myATM.enterPin("0000"); // Incorrect PIN
    myATM.enterPin("9876"); // Correct PIN
    myATM.selectOperation(); // User will choose to check balance

    return 0;
}
