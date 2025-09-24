#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <fstream>
#include <sstream>

using namespace std;

// Enum for log levels for type safety and readability
enum class LogLevel {
    INFO,
    WARN,
    ERROR
};

// Helper to convert LogLevel enum to string
string levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARN: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

// #########################################
// ### STRATEGY PATTERN FOR FORMATTING ###
// #########################################

// Interface for formatting log messages
class ILogFormatter {
public:
    virtual ~ILogFormatter() = default;
    virtual string format(LogLevel level, const string& message) = 0;
};

// A concrete formatter implementation
class SimpleFormatter : public ILogFormatter {
public:
    string format(LogLevel level, const string& message) override {
        // Get current time
        auto now = chrono::system_clock::now();
        time_t time = chrono::system_clock::to_time_t(now);
        char timeStr[26]; // Buffer for the time string

        // Use ctime_s for Windows compatibility (thread-safe).
        // The original ctime_r is the POSIX equivalent.
        ctime_s(timeStr, sizeof(timeStr), &time);
        
        // Remove the newline character that ctime_s appends
        timeStr[24] = '\0'; 

        stringstream ss;
        ss << "[" << levelToString(level) << "] [" << timeStr << "] - " << message;
        return ss.str();
    }
};

// #######################################
// ### OBSERVER PATTERN FOR LOG SINKS ###
// #######################################

// Interface for log destinations (sinks/appenders)
class ILogSink {
public:
    virtual ~ILogSink() = default;
    virtual void log(LogLevel level, const string& message) = 0;
};

// Concrete sink that logs to the console
class ConsoleSink : public ILogSink {
private:
    ILogFormatter* formatter;
public:
    ConsoleSink(ILogFormatter* fmt) : formatter(fmt) {}
    
    void log(LogLevel level, const string& message) override {
        cout << formatter->format(level, message) << endl;
    }
};

// Concrete sink that logs to a file
class FileSink : public ILogSink {
private:
    ILogFormatter* formatter;
    ofstream logFile;
public:
    FileSink(const string& filePath, ILogFormatter* fmt) : formatter(fmt) {
        logFile.open(filePath, ios::app);
        if (!logFile.is_open()) {
            cerr << "Error: Could not open log file: " << filePath << endl;
        }
    }
    ~FileSink() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
    void log(LogLevel level, const string& message) override {
        if (logFile.is_open()) {
            logFile << formatter->format(level, message) << endl;
        }
    }
};

// #####################################
// ### LOGGER SINGLETON CLASS ###
// #####################################

class Logger {
private:
    static Logger* instance;
    vector<ILogSink*> sinks;
    LogLevel minLevel;

    // Private constructor for Singleton
    Logger() : minLevel(LogLevel::INFO) {}

public:
    // Delete copy constructor and assignment operator
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static Logger* getInstance() {
        if (instance == nullptr) {
            instance = new Logger();
        }
        return instance;
    }

    void addSink(ILogSink* sink) {
        sinks.push_back(sink);
    }

    void setLevel(LogLevel level) {
        minLevel = level;
    }

    void log(LogLevel level, const string& message) {
        if (level >= minLevel) {
            // Notify all observers (sinks)
            for (ILogSink* sink : sinks) {
                sink->log(level, message);
            }
        }
    }
    
    // Convenience methods
    void info(const string& message) {
        log(LogLevel::INFO, message);
    }
    void warn(const string& message) {
        log(LogLevel::WARN, message);
    }
    void error(const string& message) {
        log(LogLevel::ERROR, message);
    }
};

// Initialize static instance
Logger* Logger::instance = nullptr;

// ############################
// ### MAIN FUNCTION ###
// ############################

int main() {
    // 1. Get the single logger instance
    Logger* logger = Logger::getInstance();
    
    // 2. Create formatters (Strategies)
    ILogFormatter* simpleFormatter = new SimpleFormatter();

    // 3. Create sinks (Observers) and inject formatters
    ILogSink* consoleSink = new ConsoleSink(simpleFormatter);
    ILogSink* fileSink = new FileSink("application.log", simpleFormatter);

    // 4. Add sinks to the logger
    logger->addSink(consoleSink);
    logger->addSink(fileSink);

    // 5. Configure the logger's minimum level
    logger->setLevel(LogLevel::WARN);

    cout << "--- Logging with Level WARN. INFO messages should be ignored. ---" << endl;
    logger->info("This is an informational message."); // Should be ignored
    logger->warn("This is a warning message.");     // Should be logged
    logger->error("This is an error message.");    // Should be logged

    cout << "\n--- Changing log level to INFO. All messages should be logged. ---" << endl;
    logger->setLevel(LogLevel::INFO);
    logger->info("System startup successful.");
    logger->warn("Cache is running low on memory.");
    logger->error("Failed to connect to the database.");

    // Clean up dynamically allocated memory
    delete simpleFormatter;
    delete consoleSink;
    delete fileSink;
    // Note: In a real app, the Logger singleton might need a static destructor
    // or a dedicated cleanup function to delete itself. For this example, we skip it.
    
    return 0;
}
