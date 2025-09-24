#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <algorithm>

// Use the standard namespace as requested
using namespace std;

// Enum for the direction of the elevator
enum class Direction {
    UP,
    DOWN,
    IDLE
};

// --- Observer Pattern: For Displays ---

// The Observer interface
class IDisplay {
public:
    virtual ~IDisplay() {}
    virtual void update(int elevatorId, int floor, Direction dir) = 0;
};

// Concrete Observer: Elevator's internal display
class ElevatorDisplay : public IDisplay {
public:
    void update(int elevatorId, int floor, Direction dir) override {
        cout << "[Elevator " << elevatorId << " Display]: Floor " << floor
             << ", Direction: " << (dir == Direction::IDLE ? "Idle" : (dir == Direction::UP ? "Up" : "Down")) << endl;
    }
};

// Concrete Observer: Floor display
class FloorDisplay : public IDisplay {
private:
    int floor;
public:
    FloorDisplay(int floorNum) : floor(floorNum) {}
    void update(int elevatorId, int floor, Direction dir) override {
        // A floor display only shows info if the elevator is at its floor
        if (this->floor == floor) {
             cout << "[Floor " << this->floor << " Display]: Elevator " << elevatorId << " has arrived." << endl;
        }
    }
};


// --- Elevator Class (The Subject in Observer Pattern) ---

class Elevator {
private:
    int id;
    int currentFloor;
    Direction direction;
    set<int> upStops;
    set<int> downStops;

    // List of observers (displays)
    vector<IDisplay*> observers;

    void notify() {
        for (IDisplay* observer : observers) {
            observer->update(id, currentFloor, direction);
        }
    }

public:
    Elevator(int elevatorId) : id(elevatorId), currentFloor(0), direction(Direction::IDLE) {}

    void attach(IDisplay* observer) {
        observers.push_back(observer);
    }

    // Main logic for elevator movement in one time step
    void move() {
        if (direction == Direction::IDLE) {
            if (!upStops.empty()) {
                direction = Direction::UP;
            } else if (!downStops.empty()) {
                direction = Direction::DOWN;
            } else {
                return; // Stay idle
            }
        }

        if (direction == Direction::UP) {
            if (!upStops.empty()) {
                // If current floor is a stop, "open doors" and remove it
                if (upStops.count(currentFloor)) {
                    cout << "Elevator " << id << " stopping at floor " << currentFloor << " going up." << endl;
                    upStops.erase(currentFloor);
                }
                // If there are still up stops, move up
                if (!upStops.empty()) {
                    currentFloor++;
                } else {
                    // No more up stops, check for down stops
                    direction = Direction::DOWN;
                    if (downStops.empty()) direction = Direction::IDLE;
                }
            }
        } 
        else if (direction == Direction::DOWN) {
            if (!downStops.empty()) {
                 // If current floor is a stop, "open doors" and remove it
                if (downStops.count(currentFloor)) {
                    cout << "Elevator " << id << " stopping at floor " << currentFloor << " going down." << endl;
                    downStops.erase(currentFloor);
                }
                // If there are still down stops, move down
                if (!downStops.empty()) {
                    currentFloor--;
                } else {
                    // No more down stops, check for up stops
                    direction = Direction::UP;
                    if (upStops.empty()) direction = Direction::IDLE;
                }
            }
        }
        
        notify(); // Notify displays of state change
    }

    // Add a request for a floor
    void addStop(int floor) {
        if (currentFloor < floor) {
            upStops.insert(floor);
        } else if (currentFloor > floor) {
            downStops.insert(floor);
        }
    }
    
    int getCurrentFloor() const { return currentFloor; }
    Direction getDirection() const { return direction; }
    int getId() const { return id; }
};

// --- Elevator System (Singleton Pattern) ---

class ElevatorSystem {
private:
    static ElevatorSystem* instance;
    vector<Elevator*> elevators;

    // Private constructor for Singleton
    ElevatorSystem() {}

public:
    // Delete copy constructor and assignment operator
    ElevatorSystem(const ElevatorSystem&) = delete;
    void operator=(const ElevatorSystem&) = delete;

    static ElevatorSystem* getInstance() {
        if (instance == nullptr) {
            instance = new ElevatorSystem();
        }
        return instance;
    }

    void addElevator(Elevator* elevator) {
        elevators.push_back(elevator);
    }
    
    // External request from a floor
    void requestElevator(int floor, Direction direction) {
        cout << "\n==> New Request: Floor " << floor << ", Direction " << (direction == Direction::UP ? "UP" : "DOWN") << " <==" << endl;
        
        // Simple Strategy: Find first idle elevator, otherwise first one available
        Elevator* bestElevator = nullptr;

        // Find first idle elevator
        for(Elevator* e : elevators) {
            if (e->getDirection() == Direction::IDLE) {
                bestElevator = e;
                break;
            }
        }
        // If no idle elevator, assign to the first one
        if (bestElevator == nullptr) {
            if (!elevators.empty()) {
                bestElevator = elevators[0];
            }
        }
        
        if (bestElevator) {
            cout << "Dispatching Elevator " << bestElevator->getId() << " for the request." << endl;
            bestElevator->addStop(floor);
        } else {
            cout << "No elevators available to handle the request." << endl;
        }
    }

    // Simulate one step of time for all elevators
    void step() {
        for (Elevator* e : elevators) {
            e->move();
        }
    }
};

// Initialize static instance
ElevatorSystem* ElevatorSystem::instance = nullptr;


// --- Building Class (The main container) ---

class Building {
private:
    int numFloors;
    vector<Elevator*> elevators;
    ElevatorSystem* systemController;
    vector<FloorDisplay*> floorDisplays;

public:
    Building(int floors, int numElevators) : numFloors(floors) {
        systemController = ElevatorSystem::getInstance();
        for (int i = 0; i < numElevators; ++i) {
            Elevator* elevator = new Elevator(i + 1);
            
            // Each elevator has its own internal display
            elevator->attach(new ElevatorDisplay());
            
            // Attach all floor displays to this elevator
            for(int j = 0; j < numFloors; ++j) {
                if (j >= floorDisplays.size()) {
                    floorDisplays.push_back(new FloorDisplay(j));
                }
                elevator->attach(floorDisplays[j]);
            }

            elevators.push_back(elevator);
            systemController->addElevator(elevator);
        }
    }

    void pressButton(int floor, Direction dir) {
        systemController->requestElevator(floor, dir);
    }

    // Simulate the system running for a number of steps
    void runSimulation(int steps) {
        cout << "\n--- Starting Elevator Simulation for " << steps << " steps ---" << endl;
        for (int i = 0; i < steps; ++i) {
            cout << "\n--- Time Step " << i + 1 << " ---" << endl;
            systemController->step();
        }
        cout << "\n--- Simulation Ended ---" << endl;
    }
    
    // For direct access to an elevator to add internal requests
    Elevator* getElevator(int id) {
        for(Elevator* e : elevators) {
            if(e->getId() == id) return e;
        }
        return nullptr;
    }

    ~Building() {
        // Basic cleanup
        for(Elevator* e : elevators) delete e;
        for(FloorDisplay* d : floorDisplays) delete d;
        delete systemController;
    }
};


// --- Main Function to run the simulation ---

int main() {
    // Create a building with 10 floors and 2 elevators
    Building building(10, 2);

    // Scenario:
    // 1. Someone on Floor 7 wants to go UP.
    building.pressButton(7, Direction::UP);

    // 2. Someone on Floor 3 wants to go DOWN.
    building.pressButton(3, Direction::DOWN);

    // Let the system run for a bit to dispatch elevators
    building.runSimulation(3);

    // 3. Someone gets into Elevator 1 (currently at floor 3) and wants to go to floor 0.
    Elevator* e1 = building.getElevator(1);
    if (e1) {
        cout << "\n>> Passenger in Elevator 1 presses button for floor 0." << endl;
        e1->addStop(0);
    }

    // 4. Someone gets into Elevator 2 (currently at floor 3) and wants to go to floor 9.
    Elevator* e2 = building.getElevator(2);
    if (e2) {
        cout << "\n>> Passenger in Elevator 2 presses button for floor 9." << endl;
        e2->addStop(9);
    }
    
    // Continue simulation to see requests being processed
    building.runSimulation(10);

    return 0;
}
