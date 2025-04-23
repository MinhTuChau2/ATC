#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <csignal>
#include <chrono>
#include <sstream>

#define SHM_NAME "/aircraft_shm"
#define MAX_AIRCRAFT 10
#define SHARED_MEM_SIZE sizeof(SharedMemory)

struct Aircraft {
    int id;
    float x, y, z;
    float speedX, speedY, speedZ;
    pthread_t thread_id;
    bool active;
    bool commandPending;
    float newSpeedX, newSpeedY, newSpeedZ;
};

struct SharedMemory {
    int count;
    Aircraft aircraft[MAX_AIRCRAFT];
};

SharedMemory* shm = nullptr;

void init_shared_memory() {
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "❌ Cannot access shared memory.\n";
        exit(1);
    }

    shm = static_cast<SharedMemory*>(mmap(nullptr, SHARED_MEM_SIZE,
        PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (shm == MAP_FAILED) {
        std::cerr << "❌ mmap failed.\n";
        exit(1);
    }
}

void log_command(const std::string& command) {
    std::ofstream log("commands_log.txt", std::ios::app);
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    log << "[" << std::put_time(std::localtime(&now), "%F %T") << "] " << command << "\n";
}

void send_command() {
    int id;
    float vx, vy, vz;
    std::cout << "Enter aircraft ID: ";
    std::cin >> id;

    bool found = false;
    for (int i = 0; i < shm->count; ++i) {
        if (shm->aircraft[i].id == id && shm->aircraft[i].active) {
            found = true;
            std::cout << "Enter new speedX, speedY, speedZ: ";
            std::cin >> vx >> vy >> vz;
            shm->aircraft[i].newSpeedX = vx;
            shm->aircraft[i].newSpeedY = vy;
            shm->aircraft[i].newSpeedZ = vz;
            shm->aircraft[i].commandPending = true;

            std::stringstream ss;
            ss << "Command sent to Aircraft " << id << ": newSpeed(" << vx << ", " << vy << ", " << vz << ")";
            std::cout << "✅ " << ss.str() << "\n";
            log_command(ss.str());
            break;
        }
    }

    if (!found) std::cout << "❌ Aircraft ID not found or inactive.\n";
}

int main() {
    init_shared_memory();

    while (true) {
        std::cout << "\n🔧 Operator Console:\n";
        std::cout << "1. Send command to aircraft\n";
        std::cout << "2. Exit\n";
        std::cout << "Select an option: ";
        int choice;
        std::cin >> choice;

        if (choice == 1) send_command();
        else if (choice == 2) break;
        else std::cout << "Invalid option.\n";
    }

    return 0;
}
