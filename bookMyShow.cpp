#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

// Forward declarations to handle class dependencies
class Show;
class Screen;
class Theater;

// Represents a single seat in a screen
class Seat {
private:
    int id;
    char row;
    int number;
    bool isBooked;

public:
    Seat(int id, char row, int num) : id(id), row(row), number(num), isBooked(false) {}

    int getId() const { return id; }
    char getRow() const { return row; }
    int getNumber() const { return number; }
    bool isAvailable() const { return !isBooked; }

    void bookSeat() {
        if (!isBooked) {
            isBooked = true;
        }
    }
};

// Represents a movie
class Movie {
private:
    string title;
    int durationMinutes;

public:
    Movie(string title, int duration) : title(title), durationMinutes(duration) {}
    string getTitle() const { return title; }
};

// Represents a specific screening of a movie
class Show {
private:
    Movie* movie;
    Screen* screen;
    string startTime;
    map<int, bool> bookedSeatIds; // Using a map for quick lookups

public:
    Show(Movie* movie, Screen* screen, string time) : movie(movie), screen(screen), startTime(time) {}

    Movie* getMovie() const { return movie; }
    Screen* getScreen() const { return screen; }
    string getStartTime() const { return startTime; }

    bool isSeatAvailable(int seatId) {
        return bookedSeatIds.find(seatId) == bookedSeatIds.end();
    }

    void bookSeats(const vector<int>& seatIds) {
        for (int id : seatIds) {
            if (isSeatAvailable(id)) {
                bookedSeatIds[id] = true;
                cout << "Seat " << id << " booked for show '" << movie->getTitle() << "' at " << startTime << endl;
            } else {
                cout << "Seat " << id << " is already booked!" << endl;
            }
        }
    }
};

// Represents a single screen in a theater
class Screen {
private:
    int id;
    vector<Seat> seats;
    vector<Show*> shows;

public:
    Screen(int id, int numSeats) : id(id) {
        // Simple seat creation (e.g., 10 seats per row)
        for (int i = 0; i < numSeats; ++i) {
            seats.push_back(Seat(i + 1, 'A' + (i / 10), (i % 10) + 1));
        }
    }

    int getId() const { return id; }
    const vector<Seat>& getSeats() const { return seats; }
    const vector<Show*>& getShows() const { return shows; }

    void addShow(Show* show) {
        shows.push_back(show);
    }
};

// Represents a theater complex
class Theater {
private:
    string name;
    string city;
    vector<Screen*> screens;

public:
    Theater(string name, string city) : name(name), city(city) {}

    string getName() const { return name; }
    string getCity() const { return city; }
    const vector<Screen*>& getScreens() const { return screens; }

    void addScreen(Screen* screen) {
        screens.push_back(screen);
    }
};

// Strategy Pattern for Payment
class PaymentStrategy {
public:
    virtual ~PaymentStrategy() {}
    virtual void pay(double amount) = 0;
};

class CreditCardPayment : public PaymentStrategy {
public:
    void pay(double amount) override {
        cout << "Paid " << amount << " using Credit Card." << endl;
    }
};

class UpiPayment : public PaymentStrategy {
public:
    void pay(double amount) override {
        cout << "Paid " << amount << " using UPI." << endl;
    }
};

// Represents a user's booking
class Booking {
private:
    Show* show;
    vector<Seat> bookedSeats;
    double totalCost;
    PaymentStrategy* paymentStrategy;

public:
    Booking(Show* show, const vector<Seat>& seats) : show(show), bookedSeats(seats), paymentStrategy(nullptr) {
        totalCost = seats.size() * 150.0; // Assume a fixed price per seat
    }

    // *** CORRECTED PART 1: Add a destructor for proper memory management (RAII) ***
    ~Booking() {
        delete paymentStrategy; // This will clean up the allocated payment strategy object
        paymentStrategy = nullptr;
    }

    void setPaymentStrategy(PaymentStrategy* strategy) {
        // If a strategy already exists, delete it before assigning the new one
        if(this->paymentStrategy != nullptr) {
            delete this->paymentStrategy;
        }
        this->paymentStrategy = strategy;
    }

    void makePayment() {
        if (paymentStrategy) {
            paymentStrategy->pay(totalCost);
            cout << "Booking successful for '" << show->getMovie()->getTitle() << "'!" << endl;
        } else {
            cout << "No payment method selected." << endl;
        }
    }
};

// Singleton Pattern for the main booking system
class BookingSystem {
private:
    vector<Movie*> movies;
    vector<Theater*> theaters;
    static BookingSystem* instance;

    // Private constructor to prevent instantiation
    BookingSystem() {}

public:
    // Delete copy constructor and assignment operator
    BookingSystem(const BookingSystem&) = delete;
    void operator=(const BookingSystem&) = delete;

    static BookingSystem* getInstance() {
        if (instance == nullptr) {
            instance = new BookingSystem();
        }
        return instance;
    }
    
    // Setup initial data for movies, theaters, screens, and shows
    void setupSystemData() {
        // Add movies
        movies.push_back(new Movie("Inception", 148));
        movies.push_back(new Movie("The Dark Knight", 152));

        // Add theaters and screens
        Theater* pvr = new Theater("PVR Cinemas", "Gurugram");
        Screen* pvr_s1 = new Screen(1, 50); // 50 seats
        Screen* pvr_s2 = new Screen(2, 60); // 60 seats
        pvr->addScreen(pvr_s1);
        pvr->addScreen(pvr_s2);
        theaters.push_back(pvr);
        
        // Add shows
        Show* inception_show1 = new Show(movies[0], pvr_s1, "6:00 PM");
        Show* tdk_show1 = new Show(movies[1], pvr_s1, "9:00 PM");
        Show* tdk_show2 = new Show(movies[1], pvr_s2, "7:00 PM");
        
        pvr_s1->addShow(inception_show1);
        pvr_s1->addShow(tdk_show1);
        pvr_s2->addShow(tdk_show2);
    }
    
    // Core functionalities
    const vector<Movie*>& getMovies() const { return movies; }
    const vector<Theater*>& getTheaters() const { return theaters; }
    
    Booking* createBooking(Show* show, const vector<int>& seatIds) {
        // Check seat availability first
        for (int id : seatIds) {
            if (!show->isSeatAvailable(id)) {
                cout << "Error: Seat " << id << " is not available." << endl;
                return nullptr;
            }
        }
        
        // Collect seat objects
        vector<Seat> selectedSeats;
        for (const Seat& s : show->getScreen()->getSeats()) {
            for (int id : seatIds) {
                if (s.getId() == id) {
                    selectedSeats.push_back(s);
                }
            }
        }
        
        // Book the seats in the show
        show->bookSeats(seatIds);
        
        // Create the booking object
        return new Booking(show, selectedSeats);
    }
    
    // Cleanup memory
    ~BookingSystem() {
        for (auto m : movies) delete m;
        for (auto t : theaters) {
            for (auto s : t->getScreens()) {
                for(auto sh : s->getShows()) delete sh;
                delete s;
            }
            delete t;
        }
    }
};

// Initialize static instance
BookingSystem* BookingSystem::instance = nullptr;


// --- Main function to simulate the user flow ---
int main() {
    // 1. Initialize the system (using Singleton)
    BookingSystem* bookingSystem = BookingSystem::getInstance();
    bookingSystem->setupSystemData();

    cout << "🎬 Welcome to the Movie Ticket Booking System! 🎬" << endl;

    // 2. User selects a movie
    cout << "\nMovies playing in Gurugram:" << endl;
    const auto& movies = bookingSystem->getMovies();
    for (int i = 0; i < movies.size(); ++i) {
        cout << i + 1 << ". " << movies[i]->getTitle() << endl;
    }

    // User selects "The Dark Knight"
    Movie* selectedMovie = movies[1];
    cout << "\nUser selected: " << selectedMovie->getTitle() << endl;

    // 3. User finds the right show
    // In a real app, the user would select a theater, then a showtime.
    // We will simulate this by directly finding the show object.
    Theater* selectedTheater = bookingSystem->getTheaters()[0]; // PVR Cinemas
    Screen* selectedScreen = selectedTheater->getScreens()[0]; // Screen 1
    Show* selectedShow = selectedScreen->getShows()[1]; // The 9:00 PM TDK show

    cout << "\nUser selected show at " << selectedShow->getStartTime() << " in " << selectedTheater->getName() << endl;

    // 4. User selects seats
    cout << "\nPlease select your seats (e.g., 5, 6, 7):" << endl;
    vector<int> desiredSeatIds = {5, 6}; // User wants to book seats with ID 5 and 6
    cout << "User wants to book seats: ";
    for(int id : desiredSeatIds) cout << id << " ";
    cout << endl;

    // 5. Create a booking
    Booking* myBooking = bookingSystem->createBooking(selectedShow, desiredSeatIds);

    if (myBooking) {
        // 6. User chooses a payment method
        myBooking->setPaymentStrategy(new CreditCardPayment());
        
        // 7. Make payment
        myBooking->makePayment();

        // *** CORRECTED PART 2: Clean up the booking object itself. ***
        // The booking's destructor will now handle cleaning up the payment strategy.
        delete myBooking;
    }
    
    // Try to book the same seat again to see if it fails
    cout << "\n--- Another user tries to book the same seat (Seat 5) ---" << endl;
    Booking* anotherBooking = bookingSystem->createBooking(selectedShow, {5});
    if (!anotherBooking) {
        cout << "As expected, booking failed for the already booked seat." << endl;
    }
    // No need to delete anotherBooking as it's nullptr

    // Clean up the singleton instance and its owned data
    delete BookingSystem::getInstance();

    return 0;
}
