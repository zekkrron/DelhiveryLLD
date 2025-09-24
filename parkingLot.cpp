#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <map>
#include <cmath>

// Using the entire std namespace for simplicity, as requested.
using namespace std;

// Enum for different types of vehicles
enum class VehicleType {
    MOTORCYCLE,
    CAR
};

// Enum for different types of parking spots
enum class SpotType {
    TWO_WHEELER,
    FOUR_WHEELER
};

// A helper enum to represent the day type
enum class DayType {
    WEEKDAY,
    WEEKEND
};

// Forward declarations for classes
class ParkingSpot;
class Vehicle;
class Ticket;

// ==================== Vehicle Classes ====================

class Vehicle {
protected:
    string licenseNumber;
    VehicleType type;

public:
    Vehicle(string license, VehicleType vType) : licenseNumber(license), type(vType) {}
    virtual ~Vehicle() {}

    string getLicenseNumber() const { return licenseNumber; }
    VehicleType getType() const { return type; }
};

class Motorcycle : public Vehicle {
public:
    Motorcycle(string license) : Vehicle(license, VehicleType::MOTORCYCLE) {}
};

class Car : public Vehicle {
public:
    Car(string license) : Vehicle(license, VehicleType::CAR) {}
};

// ==================== Parking Spot ====================

class ParkingSpot {
private:
    int spotId;
    bool isOccupied;
    SpotType spotType;
    Vehicle* vehicle;

public:
    ParkingSpot(int id, SpotType type) : spotId(id), spotType(type), isOccupied(false), vehicle(nullptr) {}

    bool isFree() const { return !isOccupied; }
    SpotType getSpotType() const { return spotType; }
    
    void parkVehicle(Vehicle* v) {
        if (!isOccupied) {
            vehicle = v;
            isOccupied = true;
        }
    }

    void unparkVehicle() {
        vehicle = nullptr;
        isOccupied = false;
    }
};

// ==================== Floor ====================

class Floor {
private:
    int floorId;
    vector<ParkingSpot*> spots;

public:
    Floor(int id, int numTwoWheelerSpots, int numFourWheelerSpots) : floorId(id) {
        for (int i = 0; i < numTwoWheelerSpots; ++i) {
            spots.push_back(new ParkingSpot(i + 1, SpotType::TWO_WHEELER));
        }
        for (int i = 0; i < numFourWheelerSpots; ++i) {
            spots.push_back(new ParkingSpot(numTwoWheelerSpots + i + 1, SpotType::FOUR_WHEELER));
        }
    }

    ~Floor() {
        for (auto spot : spots) {
            delete spot;
        }
    }

    ParkingSpot* findFirstFreeSpot(SpotType requiredType) {
        for (auto spot : spots) {
            if (spot->isFree() && spot->getSpotType() == requiredType) {
                return spot;
            }
        }
        return nullptr;
    }
};

// ==================== Ticket ====================

class Ticket {
private:
    int ticketId;
    time_t entryTime;
    Vehicle* vehicle;
    ParkingSpot* spot;

public:
    Ticket(int id, Vehicle* v, ParkingSpot* s)
        : ticketId(id), vehicle(v), spot(s) {
        entryTime = time(0);
    }

    time_t getEntryTime() const { return entryTime; }
    Vehicle* getVehicle() const { return vehicle; }
    ParkingSpot* getSpot() const { return spot; }
    int getId() const { return ticketId; }
};

// ==================== Fee Calculation (Strategy Pattern) ====================

class FeeCalculationStrategy {
public:
    virtual ~FeeCalculationStrategy() {}
    virtual double calculateFee(Ticket* ticket) = 0;
};

class WeekdayFeeStrategy : public FeeCalculationStrategy {
private:
    map<SpotType, double> hourlyRates;
public:
    WeekdayFeeStrategy() {
        hourlyRates[SpotType::TWO_WHEELER] = 3.0;
        hourlyRates[SpotType::FOUR_WHEELER] = 5.0;
    }
    double calculateFee(Ticket* ticket) override {
        time_t exitTime = time(0);
        long durationInSeconds = exitTime - ticket->getEntryTime();
        SpotType type = ticket->getSpot()->getSpotType();
        double rate = hourlyRates.count(type) ? hourlyRates[type] : 0.0;
        double hours = ceil(static_cast<double>(durationInSeconds) / 3600.0);
        return hours * rate;
    }
};

class WeekendFeeStrategy : public FeeCalculationStrategy {
private:
    map<SpotType, double> hourlyRates;
public:
    WeekendFeeStrategy() {
        hourlyRates[SpotType::TWO_WHEELER] = 4.0;
        hourlyRates[SpotType::FOUR_WHEELER] = 7.0;
    }
    double calculateFee(Ticket* ticket) override {
        time_t exitTime = time(0);
        long durationInSeconds = exitTime - ticket->getEntryTime();
        SpotType type = ticket->getSpot()->getSpotType();
        double rate = hourlyRates.count(type) ? hourlyRates[type] : 0.0;
        double hours = ceil(static_cast<double>(durationInSeconds) / 3600.0);
        return hours * rate;
    }
};

// ==================== Checkout Process ====================

class Checkout {
private:
    FeeCalculationStrategy* strategy;
public:
    Checkout(FeeCalculationStrategy* s) : strategy(s) {}

    double processCheckout(Ticket* ticket) {
        double fee = strategy->calculateFee(ticket);
        cout << "Vehicle " << ticket->getVehicle()->getLicenseNumber() 
             << " (Spot Type: " << (ticket->getSpot()->getSpotType() == SpotType::TWO_WHEELER ? "2-Wheeler" : "4-Wheeler") 
             << ") parked for " << (time(0) - ticket->getEntryTime()) << " seconds." << endl;
        cout << "Fee calculated: $" << fee << endl;
        return fee;
    }
};

// ==================== ParkingLot (Singleton) ====================

class ParkingLot {
private:
    static ParkingLot* instance;
    vector<Floor*> floors;
    map<int, Ticket*> tickets;
    int nextTicketId;

    ParkingLot() : nextTicketId(1) {
        floors.push_back(new Floor(1, 10, 10));
        floors.push_back(new Floor(2, 5, 15));
        cout << "Parking Lot initialized." << endl;
    }

    DayType getCurrentDayType() {
        time_t t = time(0);
        tm* now = localtime(&t);
        // Sunday (0) and Saturday (6) are weekends in tm_wday
        if (now->tm_wday == 0 || now->tm_wday == 6) {
            return DayType::WEEKEND;
        }
        return DayType::WEEKDAY;
    }

public:
    ParkingLot(const ParkingLot&) = delete;
    void operator=(const ParkingLot&) = delete;

    ~ParkingLot() {
        for (auto floor : floors) delete floor;
        for (auto const& [key, val] : tickets) delete val;
        delete instance;
    }
    
    static ParkingLot* getInstance() {
        if (instance == nullptr) {
            instance = new ParkingLot();
        }
        return instance;
    }

    Ticket* parkVehicle(Vehicle* vehicle) {
        SpotType requiredType = (vehicle->getType() == VehicleType::MOTORCYCLE) ? SpotType::TWO_WHEELER : SpotType::FOUR_WHEELER;
        ParkingSpot* spot = nullptr;
        for (auto floor : floors) {
            spot = floor->findFirstFreeSpot(requiredType);
            if (spot != nullptr) break;
        }
        
        if (spot == nullptr) {
            cout << "Sorry, parking lot is full for this vehicle type." << endl;
            return nullptr;
        }
        
        spot->parkVehicle(vehicle);
        Ticket* ticket = new Ticket(nextTicketId++, vehicle, spot);
        tickets[ticket->getId()] = ticket;
        cout << "Vehicle " << vehicle->getLicenseNumber() << " parked successfully. Ticket ID: " << ticket->getId() << endl;
        return ticket;
    }

    void unparkVehicle(Ticket* ticket) {
        if (tickets.find(ticket->getId()) == tickets.end()) {
            cout << "Invalid ticket!" << endl;
            return;
        }

        FeeCalculationStrategy* strategy = nullptr;
        DayType day = getCurrentDayType();

        if (day == DayType::WEEKDAY) {
            cout << "(Applying Weekday Pricing)" << endl;
            strategy = new WeekdayFeeStrategy();
        } else {
            cout << "(Applying Weekend Pricing)" << endl;
            strategy = new WeekendFeeStrategy();
        }

        Checkout checkout(strategy);
        checkout.processCheckout(ticket);
        
        ticket->getSpot()->unparkVehicle();
        cout << "Vehicle " << ticket->getVehicle()->getLicenseNumber() << " has been unparked." << endl;
        
        tickets.erase(ticket->getId());
        delete ticket;
        delete strategy;
    }
};

ParkingLot* ParkingLot::instance = nullptr;

// ==================== Main Function to Demonstrate ====================

int main() {
    ParkingLot* lot = ParkingLot::getInstance();

    Vehicle* car1 = new Car("CAR-123");
    Vehicle* bike1 = new Motorcycle("BIKE-456");
    
    cout << "\n--- Parking Vehicles ---" << endl;
    Ticket* ticket1 = lot->parkVehicle(car1);
    Ticket* ticket2 = lot->parkVehicle(bike1);

    cout << "\n... Vehicles are parked for a while ...\n" << endl;

    cout << "--- Unparking Vehicles ---" << endl;
    // The following will use the correct pricing based on today's date (Sept 24, 2025 is a Wednesday)
    if (ticket1) {
        lot->unparkVehicle(ticket1);
    }
    cout << "--------------------------" << endl;
    if (ticket2) {
        lot->unparkVehicle(ticket2);
    }

    delete car1;
    delete bike1;

    return 0;
}
