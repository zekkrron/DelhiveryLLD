#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <ctime>
#include <cstdlib>

using namespace std;

// Forward declarations
class Ball;
class Player;
class Team;

//------------------------------------------------------------------------------------
// 1. ENUMS AND SIMPLE DATA CLASSES
//------------------------------------------------------------------------------------

enum class RunType {
    ZERO, SINGLE, DOUBLE, TRIPLE, FOUR, SIX, WIDE, NO_BALL, WICKET
};

class Player {
public:
    string name;
    int runsScored = 0;
    int ballsFaced = 0;
    int wicketsTaken = 0;
    int ballsBowled = 0;

    Player(string name) : name(name) {}
};

class Team {
public:
    string name;
    vector<Player*> players;
    int totalRuns = 0;
    int wicketsFallen = 0;
    // CORRECTED: Renamed for clarity. This now tracks legal balls bowled.
    int legalDeliveriesBowled = 0;
    
    Team(string name, const vector<string>& playerNames) : name(name) {
        for (const auto& playerName : playerNames) {
            players.push_back(new Player(playerName));
        }
    }

    ~Team() {
        for (Player* p : players) {
            delete p;
        }
    }
};

class Ball {
public:
    Player* batsman;
    Player* bowler;
    RunType run;

    Ball(Player* b, Player* bo, RunType r) : batsman(b), bowler(bo), run(r) {}
};

//------------------------------------------------------------------------------------
// 2. OBSERVER PATTERN: For Scoreboard and Commentary
//------------------------------------------------------------------------------------

// Observer Interface
class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void update(const Ball& ball, const Team& battingTeam) = 0;
};

// Concrete Observer 1: Scoreboard
class Scoreboard : public IObserver {
public:
    void update(const Ball& ball, const Team& battingTeam) override {
        cout << "---------------- SCOREBOARD ----------------" << endl;
        cout << battingTeam.name << ": " << battingTeam.totalRuns << "/" << battingTeam.wicketsFallen
             << " (" << battingTeam.legalDeliveriesBowled / 6 << "." << battingTeam.legalDeliveriesBowled % 6 << " Overs)" << endl;
        // A real scoreboard would show more details (batsman scores, bowler figures, etc.)
        cout << "--------------------------------------------" << endl;
    }
};


// Concrete Observer 2: Commentary
class Commentary : public IObserver {
public:
    void update(const Ball& ball, const Team& battingTeam) override {
        cout << "COMMENTARY: " << ball.bowler->name << " to " << ball.batsman->name << ", ";
        switch (ball.run) {
            case RunType::ZERO:     cout << "no run." << endl; break;
            case RunType::SINGLE:   cout << "single." << endl; break;
            case RunType::DOUBLE:   cout << "two runs." << endl; break;
            case RunType::TRIPLE:   cout << "three runs." << endl; break;
            case RunType::FOUR:     cout << "FOUR!" << endl; break;
            case RunType::SIX:      cout << "SIX!" << endl; break;
            case RunType::WIDE:     cout << "WIDE." << endl; break;
            case RunType::NO_BALL:  cout << "NO BALL." << endl; break;
            case RunType::WICKET:   cout << "OUT!" << endl; break;
        }
    }
};


//------------------------------------------------------------------------------------
// 3. STRATEGY PATTERN: For different Match Formats (T20, ODI)
//------------------------------------------------------------------------------------

// Strategy Interface
class IMatchFormatStrategy {
public:
    virtual ~IMatchFormatStrategy() = default;
    virtual int getTotalOvers() const = 0;
    virtual int getMaxPlayers() const = 0;
};

// Concrete Strategy 1: T20
class T20Format : public IMatchFormatStrategy {
public:
    int getTotalOvers() const override { return 20; }
    int getMaxPlayers() const override { return 11; }
};

// Concrete Strategy 2: ODI
class ODIFormat : public IMatchFormatStrategy {
public:
    int getTotalOvers() const override { return 50; }
    int getMaxPlayers() const override { return 11; }
};

//------------------------------------------------------------------------------------
// 4. CORE LOGIC: Innings and Match
//------------------------------------------------------------------------------------

class Innings {
private:
    Team* battingTeam;
    Team* bowlingTeam;
    list<IObserver*> observers;
    int totalOvers;

public:
    Innings(Team* bat, Team* bowl, int overs) : battingTeam(bat), bowlingTeam(bowl), totalOvers(overs) {}

    void registerObserver(IObserver* observer) {
        observers.push_back(observer);
    }

    void unregisterObserver(IObserver* observer) {
        observers.remove(observer);
    }

    void notifyObservers(const Ball& ball) {
        for (IObserver* observer : observers) {
            observer->update(ball, *battingTeam);
        }
    }
    
    // CORRECTED: Simulation logic is now clearer.
    void play() {
        cout << "\n--- Starting Innings for " << battingTeam->name << " ---" << endl;
        
        Player* batsman = battingTeam->players[battingTeam->wicketsFallen];
        Player* nonStriker = battingTeam->players[battingTeam->wicketsFallen + 1];
        Player* bowler = bowlingTeam->players[bowlingTeam->players.size() - 1]; 

        int totalBalls = totalOvers * 6;
        while (battingTeam->legalDeliveriesBowled < totalBalls) {
            if (battingTeam->wicketsFallen >= battingTeam->players.size() - 1) {
                cout << "All out!" << endl;
                break;
            }

            int randomOutcome = rand() % 9;
            RunType run = static_cast<RunType>(randomOutcome);
            
            Ball ball(batsman, bowler, run);
            bool isLegalDelivery = true;

            switch (run) {
                case RunType::WICKET:
                    battingTeam->wicketsFallen++;
                    batsman = battingTeam->players[battingTeam->wicketsFallen + 1];
                    break;
                case RunType::SINGLE:
                    battingTeam->totalRuns += 1;
                    swap(batsman, nonStriker);
                    break;
                case RunType::TRIPLE:
                     battingTeam->totalRuns += 3;
                     swap(batsman, nonStriker);
                     break;
                case RunType::WIDE:
                case RunType::NO_BALL:
                    battingTeam->totalRuns += 1;
                    isLegalDelivery = false; // Does not count as a legal ball
                    break;
                case RunType::ZERO: break;
                case RunType::DOUBLE: battingTeam->totalRuns += 2; break;
                case RunType::FOUR: battingTeam->totalRuns += 4; break;
                case RunType::SIX: battingTeam->totalRuns += 6; break;
            }
            
            if (isLegalDelivery) {
                battingTeam->legalDeliveriesBowled++;
            }
            
            notifyObservers(ball);

            if (isLegalDelivery && battingTeam->legalDeliveriesBowled % 6 == 0) {
                cout << "--- End of Over " << battingTeam->legalDeliveriesBowled / 6 << " ---" << endl;
                swap(batsman, nonStriker); // Swap strike at end of over
            }
        }
         cout << "\n--- End of Innings for " << battingTeam->name << ". Final Score: "
             << battingTeam->totalRuns << "/" << battingTeam->wicketsFallen << " ---" << endl;
    }
};

class Match {
private:
    Team* teamA;
    Team* teamB;
    IMatchFormatStrategy* format;
    list<IObserver*> observers;
    Team* winner;

public:
    Match(Team* a, Team* b, IMatchFormatStrategy* f) : teamA(a), teamB(b), format(f), winner(nullptr) {}

    void registerObserver(IObserver* observer) {
        observers.push_back(observer);
    }

    void start() {
        cout << "Starting a " << (format->getTotalOvers() == 20 ? "T20" : "ODI") 
             << " match between " << teamA->name << " and " << teamB->name << endl;
        
        // First Innings
        Innings firstInnings(teamA, teamB, format->getTotalOvers());
        for (auto obs : observers) firstInnings.registerObserver(obs);
        firstInnings.play();

        // Second Innings
        Innings secondInnings(teamB, teamA, format->getTotalOvers());
        for (auto obs : observers) secondInnings.registerObserver(obs);
        secondInnings.play();
        
        // Determine Winner
        if (teamA->totalRuns > teamB->totalRuns) winner = teamA;
        else if (teamB->totalRuns > teamA->totalRuns) winner = teamB;
        else winner = nullptr; // Draw

        cout << "\n================= MATCH ENDED =================" << endl;
        if (winner) {
            cout << "Winner is " << winner->name << "!" << endl;
        } else {
            cout << "The match is a draw." << endl;
        }
        cout << "Final Score: " << teamA->name << " " << teamA->totalRuns << "/" << teamA->wicketsFallen
             << " | " << teamB->name << " " << teamB->totalRuns << "/" << teamB->wicketsFallen << endl;
        cout << "=============================================" << endl;
    }

    ~Match() {
        delete format;
    }
};

//------------------------------------------------------------------------------------
// 5. MAIN DRIVER
//------------------------------------------------------------------------------------

int main() {
    srand(time(0)); 

    Team india("India", {"Rohit", "Virat", "Surya", "Pant", "Hardik", "Jadeja", "Axar", "Shami", "Bumrah", "Arshdeep", "Chahal"});
    Team australia("Australia", {"Warner", "Finch", "Smith", "Maxwell", "Stoinis", "David", "Wade", "Cummins", "Starc", "Zampa", "Hazlewood"});

    IMatchFormatStrategy* t20Format = new T20Format();

    Match match(&india, &australia, t20Format);

    Scoreboard* scoreboard = new Scoreboard();
    Commentary* commentary = new Commentary();

    match.registerObserver(scoreboard);
    match.registerObserver(commentary);

    match.start();

    // Clean up
    delete scoreboard;
    delete commentary;
    
    return 0;
}
