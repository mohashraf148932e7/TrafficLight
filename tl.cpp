#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <queue>
#include <mutex>

// Traffic Light Class
class TrafficLight {
private:
    enum State { RED, GREEN, YELLOW };
    State currentState;
    int redDuration;
    int greenDuration;
    int yellowDuration;

public:
    TrafficLight(int red, int green, int yellow)
        : currentState(RED), redDuration(red), greenDuration(green), yellowDuration(yellow) {}

    void changeState() {
        if (currentState == RED) {
            currentState = GREEN;
        } else if (currentState == GREEN) {
            currentState = YELLOW;
        } else {
            currentState = RED;
        }
    }

    void displayState() const {
        switch (currentState) {
            case RED:
                std::cout << "Traffic Light: RED\n";
                break;
            case GREEN:
                std::cout << "Traffic Light: GREEN\n";
                break;
            case YELLOW:
                std::cout << "Traffic Light: YELLOW\n";
                break;
        }
    }

    bool isGreen() const {
        return currentState == GREEN;
    }

    int getStateDuration() const {
        switch (currentState) {
            case RED:
                return redDuration;
            case GREEN:
                return greenDuration;
            case YELLOW:
                return yellowDuration;
        }
        return 0; // Default fallback
    }
};

// Pedestrian Light Class
class PedestrianLight {
private:
    enum State { WALK, DONT_WALK };
    State currentState;

public:
    PedestrianLight() : currentState(DONT_WALK) {}

    void changeState(bool trafficGreen) {
        currentState = trafficGreen ? WALK : DONT_WALK;
    }

    void displayState() const {
        if (currentState == WALK) {
            std::cout << "Pedestrian Light: WALK\n";
        } else {
            std::cout << "Pedestrian Light: DON'T WALK\n";
        }
    }
};

// Intersection Controller Class
class IntersectionController {
private:
    TrafficLight trafficLight;
    PedestrianLight pedestrianLight;
    bool emergencyMode;
    std::priority_queue<std::string> emergencyQueue;
    std::mutex queueMutex;

public:
    IntersectionController(int red, int green, int yellow)
        : trafficLight(red, green, yellow), pedestrianLight(), emergencyMode(false) {}

    void addEmergencyVehicle(const std::string& vehicleID) {
        std::lock_guard<std::mutex> lock(queueMutex);
        emergencyQueue.push(vehicleID);
        emergencyMode = true;
    }

    void handleEmergency() {
        while (!emergencyQueue.empty()) {
            std::lock_guard<std::mutex> lock(queueMutex);
            std::cout << "Handling emergency vehicle: " << emergencyQueue.top() << "\n";
            emergencyQueue.pop();
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        emergencyMode = false;
    }

    void manualOverride() {
        std::cout << "Manual Override: Enter 1 for RED, 2 for GREEN, 3 for YELLOW: ";
        int input;
        std::cin >> input;
        switch (input) {
            case 1:
                std::cout << "Traffic Light set to RED\n";
                break;
            case 2:
                std::cout << "Traffic Light set to GREEN\n";
                pedestrianLight.changeState(true);
                break;
            case 3:
                std::cout << "Traffic Light set to YELLOW\n";
                break;
            default:
                std::cout << "Invalid input!\n";
        }
    }

    void runSimulation(int cycles) {
        for (int i = 0; i < cycles; ++i) {
            if (emergencyMode) {
                handleEmergency();
                continue;
            }

            trafficLight.displayState();
            pedestrianLight.changeState(trafficLight.isGreen());
            pedestrianLight.displayState();

            std::this_thread::sleep_for(std::chrono::seconds(trafficLight.getStateDuration()));
            trafficLight.changeState();
            std::cout << "---------------------\n";
        }
    }
};

// Main Function
int main() {
    IntersectionController controller(5, 3, 2); // Red: 5s, Green: 3s, Yellow: 2s

    // Simulate in a separate thread
    std::thread simulationThread([&controller]() { controller.runSimulation(10); });

    // User interaction for emergency and manual overrides
    while (true) {
        std::cout << "Enter command: 1 for emergency, 2 for manual override, 3 to exit: ";
        int command;
        std::cin >> command;

        if (command == 1) {
            std::cout << "Enter emergency vehicle ID: ";
            std::string vehicleID;
            std::cin >> vehicleID;
            controller.addEmergencyVehicle(vehicleID);
        } else if (command == 2) {
            controller.manualOverride();
        } else if (command == 3) {
            break;
        } else {
            std::cout << "Invalid command!\n";
        }
    }

    simulationThread.join();
    return 0;
}
