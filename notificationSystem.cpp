#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

// Enum to represent the different notification channels
enum class NotificationType {
    SMS,
    EMAIL,
    PUSH
};

// A simple User class to hold user data and contact info
class User {
private:
    string name;
    // Maps a notification type to the user's contact detail for that type
    map<NotificationType, string> contactInfo; 

public:
    User(const string& name) : name(name) {}

    void setContactInfo(NotificationType type, const string& detail) {
        contactInfo[type] = detail;
    }

    string getContactInfo(NotificationType type) const {
        if (contactInfo.count(type)) {
            return contactInfo.at(type);
        }
        return "";
    }
    
    string getName() const {
        return name;
    }
};

// ## 1. Strategy Pattern ##
// The interface for our sending strategies.
class INotificationSender {
public:
    virtual void send(const string& recipient, const string& message) = 0;
    virtual ~INotificationSender() {} // Important: Virtual destructor
};

// Concrete implementation for sending SMS
class SmsSender : public INotificationSender {
public:
    void send(const string& phoneNumber, const string& message) override {
        cout << "📱 Sending SMS to " << phoneNumber << ": '" << message << "'" << endl;
        // In a real system, you would integrate with an SMS gateway API here.
    }
};

// Concrete implementation for sending Email
class EmailSender : public INotificationSender {
public:
    void send(const string& emailAddress, const string& message) override {
        cout << "📧 Sending Email to " << emailAddress << ": '" << message << "'" << endl;
        // In a real system, you would use an SMTP library here.
    }
};

// Concrete implementation for sending Push Notifications
class PushSender : public INotificationSender {
public:
    void send(const string& deviceToken, const string& message) override {
        cout << "🔔 Sending Push Notification to device " << deviceToken << ": '" << message << "'" << endl;
        // In a real system, you would integrate with APNS/FCM here.
    }
};

// ## 2. Factory Pattern ##
// Creates the appropriate sender object based on the notification type.
class NotificationSenderFactory {
public:
    // Factory method
    static INotificationSender* createSender(NotificationType type) {
        switch (type) {
            case NotificationType::SMS:
                return new SmsSender();
            case NotificationType::EMAIL:
                return new EmailSender();
            case NotificationType::PUSH:
                return new PushSender();
            default:
                return nullptr;
        }
    }
};

// ## 3. Singleton Pattern ##
// The main service that clients will interact with.
class NotificationService {
private:
    // Private constructor to prevent instantiation
    NotificationService() {}
    
    // Static instance
    static NotificationService* instance;

    // Prevent copying
    NotificationService(const NotificationService&) = delete;
    NotificationService& operator=(const NotificationService&) = delete;

public:
    // Public method to get the single instance
    static NotificationService* getInstance() {
        if (instance == nullptr) {
            instance = new NotificationService();
        }
        return instance;
    }

    void sendNotification(User* user, const string& message, NotificationType type) {
        if (user == nullptr) {
            cerr << "Error: User is null." << endl;
            return;
        }

        string recipient = user->getContactInfo(type);
        if (recipient.empty()) {
            cerr << "Error: No contact info found for user '" << user->getName() 
                 << "' for channel type." << endl;
            return;
        }
        
        // Use the factory to get the sender
        INotificationSender* sender = NotificationSenderFactory::createSender(type);
        
        if (sender != nullptr) {
            cout << "---" << endl;
            cout << "Processing notification for user: " << user->getName() << endl;
            // Use the strategy to send
            sender->send(recipient, message);
            
            // Clean up the dynamically allocated sender object
            delete sender; 
        } else {
            cerr << "Error: Unsupported notification type." << endl;
        }
    }
};

// Initialize the static instance to null
NotificationService* NotificationService::instance = nullptr;

// Main function to demonstrate the system
int main() {
    // Create a user and set their contact details
    User* user1 = new User("Alice");
    user1->setContactInfo(NotificationType::EMAIL, "alice@example.com");
    user1->setContactInfo(NotificationType::SMS, "+1-123-456-7890");
    user1->setContactInfo(NotificationType::PUSH, "alice_device_token_xyz");

    // Get the singleton instance of the NotificationService
    NotificationService* notificationSvc = NotificationService::getInstance();

    // Send different types of notifications
    notificationSvc->sendNotification(user1, "Welcome! Your account is ready.", NotificationType::EMAIL);
    notificationSvc->sendNotification(user1, "Your OTP is 123456.", NotificationType::SMS);
    notificationSvc->sendNotification(user1, "You have a new friend request!", NotificationType::PUSH);

    // Clean up allocated user
    delete user1;

    // The singleton instance should ideally be cleaned up at the end of the program's lifecycle,
    // but for simplicity in this example, we omit that step.

    return 0;
}
