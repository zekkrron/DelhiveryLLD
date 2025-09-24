#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <algorithm>
#include <sstream>

// Use standard namespace for simplicity
using namespace std;

// Forward declarations to resolve circular dependencies
class User;
class Email;
class GmailServer;

// A utility function to generate a simple unique ID for emails
string generateUniqueId() {
    time_t now = time(0);
    stringstream ss;
    ss << now << "_" << rand();
    return ss.str();
}

//-------------------------------------------------
// 1. Email Class: A simple data object
//-------------------------------------------------
class Email {
private:
    string id;
    string from;
    vector<string> to;
    string subject;
    string body;
    time_t timestamp;

public:
    Email(const string& from, const vector<string>& to, const string& subject, const string& body)
        : from(from), to(to), subject(subject), body(body) {
        this->id = generateUniqueId();
        this->timestamp = time(0);
    }

    string getFrom() const { return from; }
    const vector<string>& getTo() const { return to; }
    string getSubject() const { return subject; }
    string getBody() const { return body; }

    void display() const {
        cout << "--------------------------------" << endl;
        cout << "From: " << from << endl;
        cout << "Subject: " << subject << endl;
        cout << "Body: " << body.substr(0, 50) << "..." << endl;
        cout << "--------------------------------" << endl;
    }
};

//-------------------------------------------------
// 2. Search Strategy Pattern: For flexible searching
//-------------------------------------------------
// Abstract Strategy Interface
class SearchStrategy {
public:
    virtual ~SearchStrategy() {}
    virtual vector<Email*> search(const vector<Email*>& emails, const string& query) const = 0;
};

// Concrete Strategy for searching by keyword in subject and body
class SearchByKeyword : public SearchStrategy {
public:
    vector<Email*> search(const vector<Email*>& emails, const string& query) const override {
        vector<Email*> results;
        for (Email* email : emails) {
            if (email->getSubject().find(query) != string::npos || email->getBody().find(query) != string::npos) {
                results.push_back(email);
            }
        }
        return results;
    }
};

//-------------------------------------------------
// 3. GmailServer Singleton: The central orchestrator
//-------------------------------------------------
class GmailServer {
private:
    static GmailServer* instance;
    map<string, User*> users; // Map email address to User object
    vector<Email*> allEmails; // To manage memory of all created emails

    // Private constructor for Singleton
    GmailServer() {}
    
    // Disable copy and assignment
    GmailServer(const GmailServer&) = delete;
    GmailServer& operator=(const GmailServer&) = delete;

public:
    // **FIX**: Destructor is only DECLARED here. Definition is moved later.
    ~GmailServer();

    static GmailServer* getInstance() {
        if (instance == nullptr) {
            instance = new GmailServer();
        }
        return instance;
    }

    // **FIX**: Method declarations only. Definitions are moved after User class is defined.
    User* registerUser(const string& emailAddress, const string& name);
    void sendEmail(Email* email);
};

// Initialize static instance
GmailServer* GmailServer::instance = nullptr;

//-------------------------------------------------
// 4. User Class: Represents an email user
//-------------------------------------------------
class User {
private:
    string emailAddress;
    string name;
    vector<Email*> inbox;
    vector<Email*> sent;

public:
    User(const string& emailAddress, const string& name)
        : emailAddress(emailAddress), name(name) {}

    string getEmailAddress() const { return emailAddress; }

    void receiveEmail(Email* email) {
        inbox.push_back(email);
        cout << "Notification for " << emailAddress << ": You've got mail from " << email->getFrom() << "!" << endl;
    }

    void composeAndSendEmail(const vector<string>& to, const string& subject, const string& body) {
        Email* newEmail = new Email(this->emailAddress, to, subject, body);
        sent.push_back(newEmail);
        GmailServer::getInstance()->sendEmail(newEmail);
        cout << this->emailAddress << " sent an email to " << to[0] << "." << endl;
    }
    
    void viewInbox() const {
        cout << "\n--- " << emailAddress << "'s Inbox ---" << endl;
        if (inbox.empty()) {
            cout << "Inbox is empty." << endl;
            return;
        }
        for (const auto& email : inbox) {
            email->display();
        }
    }

    void viewSent() const {
        cout << "\n--- " << emailAddress << "'s Sent Items ---" << endl;
        if (sent.empty()) {
            cout << "Sent folder is empty." << endl;
            return;
        }
        for (const auto& email : sent) {
            email->display();
        }
    }
    
    vector<Email*> searchEmails(const string& query, SearchStrategy* strategy) const {
        vector<Email*> allUserEmails;
        allUserEmails.insert(allUserEmails.end(), inbox.begin(), inbox.end());
        allUserEmails.insert(allUserEmails.end(), sent.begin(), sent.end());
        return strategy->search(allUserEmails, query);
    }
};

//-----------------------------------------------------------------
// **FIX**: GmailServer method definitions are now placed here,
// after the User class is fully known to the compiler.
//-----------------------------------------------------------------
GmailServer::~GmailServer() {
    // Clean up allocated memory
    for (auto const& [key, val] : users) {
        delete val;
    }
    for (Email* email : allEmails) {
        delete email;
    }
}

void GmailServer::sendEmail(Email* email) {
    allEmails.push_back(email); // Keep track for memory management
    const vector<string>& recipients = email->getTo();
    for (const string& recipientAddress : recipients) {
        if (users.count(recipientAddress)) {
            // Now the compiler knows about User::receiveEmail
            users[recipientAddress]->receiveEmail(email);
        } else {
            cout << "System Notice: Delivery failed. User not found: " << recipientAddress << endl;
        }
    }
}

User* GmailServer::registerUser(const string& emailAddress, const string& name) {
    if (users.find(emailAddress) == users.end()) {
        User* newUser = new User(emailAddress, name);
        users[emailAddress] = newUser;
        cout << "User " << name << " registered successfully with address " << emailAddress << "." << endl;
        return newUser;
    }
    cout << "Registration failed: Email address " << emailAddress << " is already taken." << endl;
    return nullptr;
}


//-------------------------------------------------
// 5. Main Driver Function (no changes needed)
//-------------------------------------------------
int main() {
    // Get the single instance of our email server
    GmailServer* server = GmailServer::getInstance();

    // Register some users
    User* alice = server->registerUser("alice@gmail.com", "Alice");
    User* bob = server->registerUser("bob@gmail.com", "Bob");
    User* charlie = server->registerUser("charlie@gmail.com", "Charlie");
    
    cout << "\n--- Emailing ---" << endl;
    // Alice sends an email to Bob and Charlie
    alice->composeAndSendEmail({"bob@gmail.com", "charlie@gmail.com"}, "Project Update", "Hey team, the latest project documents are now available.");
    
    // Bob sends an email to Alice
    bob->composeAndSendEmail({"alice@gmail.com"}, "Lunch Plans", "Hi Alice, are we still on for lunch tomorrow? The documents look great.");
    
    // View mailboxes
    alice->viewInbox();
    bob->viewInbox();
    alice->viewSent();
    
    cout << "\n--- Searching ---" << endl;
    // Alice wants to search for an email with the keyword "documents"
    SearchStrategy* keywordSearch = new SearchByKeyword();
    vector<Email*> results = alice->searchEmails("documents", keywordSearch);

    cout << "Alice searched for 'documents' and found " << results.size() << " email(s):" << endl;
    for (const auto& email : results) {
        email->display();
    }
    
    // Clean up allocated memory
    delete keywordSearch;
    delete server; // This will delete all users and emails
    
    return 0;
}
