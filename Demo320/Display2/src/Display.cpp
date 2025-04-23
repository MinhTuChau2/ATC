#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>
#include <pthread.h>
#include <mutex>
#include <chrono>
#include <cmath>
#include <fstream>
#include <cstring>
#include <csignal>
#include <thread>
#include <sstream>

#define SHM_NAME "/aircraft_shm"
#define SHARED_MEM_SIZE sizeof(SharedMemory)
#define MAX_AIRCRAFT 10
#define GRID_SIZE 20  // Define a grid size (20x20)

std::mutex print_mutex;

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
    int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (shm_fd == -1) {
        std::cerr << "❌ shm_open error: " << strerror(errno) << "\n";
        exit(1);
    }

    shm = static_cast<SharedMemory*>(mmap(nullptr, SHARED_MEM_SIZE,
        PROT_READ, MAP_SHARED, shm_fd, 0));
    if (shm == MAP_FAILED) {
        std::cerr << "❌ mmap error: " << strerror(errno) << "\n";
        exit(1);
    }
}

// Function to create a grid and update it with aircraft positions
void display_radar() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::cout << "\033[2J\033[1;1H";  // Clear screen

        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << "\n🗺️  Aircraft Positions on Radar:\n";

        char grid[GRID_SIZE][GRID_SIZE];
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = 0; j < GRID_SIZE; ++j) {
                grid[i][j] = '.';
            }
        }

        std::cout << "\n📋 Active aircrafts on radar:\n";

        for (int i = 0; i < shm->count; ++i) {
            const auto& a = shm->aircraft[i];

            int grid_x = static_cast<int>(a.x);
            int grid_y = static_cast<int>(a.y);

            // Skip if out of bounds
            if (grid_x < 0 || grid_x >= GRID_SIZE || grid_y < 0 || grid_y >= GRID_SIZE)
                continue;

            char symbol = (a.id < 50) ? ('0' + a.id) : '*';

            // Only draw if the spot is free
            if (grid[grid_y][grid_x] == '.') {
                grid[grid_y][grid_x] = symbol;
            }

            // Print aircraft data
            std::cout << "✈️  ID " << a.id
                      << " at (" << a.x << ", " << a.y << ", " << a.z << ")"
                      << " speed (" << a.speedX << ", " << a.speedY << ", " << a.speedZ << ")\n";
        }

        std::cout << "\n📡 Radar Grid:\n";
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = 0; j < GRID_SIZE; ++j) {
                std::cout << grid[i][j] << " ";
            }
            std::cout << "\n";
        }

        std::cout << "\n";
    }
}

int main() {
    init_shared_memory();

    std::cout << "📡 Radar system initialized. Reading aircraft data...\n";

    display_radar();

    return 0;
}
