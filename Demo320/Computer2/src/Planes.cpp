#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>
#include <pthread.h>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <cstring>
#include <csignal>
#include <thread>

#define SHM_NAME "/aircraft_shm"
#define SHARED_MEM_SIZE sizeof(SharedMemory)
#define MAX_AIRCRAFT 50

std::mutex print_mutex;
std::mutex shm_mutex;

bool running = true;
int prediction_interval = 30;

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

void signal_handler(int signum) {
    std::lock_guard<std::mutex> lock(print_mutex);
    std::cout << "\n😕 Interrupt received. Cleaning up shared memory...\n";
    running = false;
    munmap(shm, SHARED_MEM_SIZE);
    shm_unlink(SHM_NAME);
    exit(0);
}

float calculate_distance(const Aircraft* a1, const Aircraft* a2) {
    return std::sqrt(std::pow(a1->x - a2->x, 2) +
                     std::pow(a1->y - a2->y, 2) +
                     std::pow(a1->z - a2->z, 2));
}

bool check_separation_violation(const Aircraft& a1, const Aircraft& a2) {
    float dz = std::abs(a1.z - a2.z);
    float dx = std::abs(a1.x - a2.x);
    float dy = std::abs(a1.y - a2.y);
    float dxy = std::sqrt(dx * dx + dy * dy);
    return dz < 1 && dxy < 3;
}

Aircraft predict_position(const Aircraft& a, int seconds) {
    Aircraft future = a;
    future.x += a.speedX * seconds;
    future.y += a.speedY * seconds;
    future.z += a.speedZ * seconds;
    return future;
}

void* aircraft_thread(void* arg) {
    Aircraft* a = static_cast<Aircraft*>(arg);
    {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << "✈️  Aircraft " << a->id << " thread started.\n";
    }

    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<std::mutex> lock_shm(shm_mutex);
        if (!a->active) continue;
        if (a->commandPending) {
                a->speedX = a->newSpeedX;
                a->speedY = a->newSpeedY;
                a->speedZ = a->newSpeedZ;
                a->commandPending = false;

                std::lock_guard<std::mutex> lock_print(print_mutex);
                std::cout << "📡 Aircraft " << a->id << " received new command: SpeedX=" << a->speedX
                          << ", SpeedY=" << a->speedY << ", SpeedZ=" << a->speedZ << "\n";
            }
        a->x += a->speedX;
        a->y += a->speedY;
        a->z += a->speedZ;

        std::lock_guard<std::mutex> lock_print(print_mutex);
        std::cout << "📍 Aircraft " << a->id << " moved to: X=" << a->x << ", Y=" << a->y << ", Z=" << a->z << "\n";
    }
    return nullptr;
}

void* computer_thread(void* arg) {
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::lock_guard<std::mutex> lock_shm(shm_mutex);
        int count = shm->count;

        for (int i = 0; i < count; ++i) {
            for (int j = i + 1; j < count; ++j) {
                Aircraft& a1 = shm->aircraft[i];
                Aircraft& a2 = shm->aircraft[j];

                if (!a1.active || !a2.active) continue;

                float dist = calculate_distance(&a1, &a2);
                {
                    std::lock_guard<std::mutex> lock_print(print_mutex);
                    std::cout << std::fixed << std::setprecision(2);
                    std::cout << "📏 Distance between Aircraft " << a1.id << " and " << a2.id << ": "
                              << dist << " units\n";
                }

                if (check_separation_violation(a1, a2)) {
                    std::lock_guard<std::mutex> lock_print(print_mutex);
                    std::cout << "⚠️ ALERT: Live separation violation between Aircraft " << a1.id << " and " << a2.id << "\n";
                }

                Aircraft f1 = predict_position(a1, prediction_interval);
                Aircraft f2 = predict_position(a2, prediction_interval);

                if (check_separation_violation(f1, f2)) {
                    std::lock_guard<std::mutex> lock_print(print_mutex);
                    std::cout << "⚠️ WARNING: Predicted safety violation in " << prediction_interval << "s between Aircraft " << a1.id << " and " << a2.id << "\n";

                    if (prediction_interval <= 120) {
                        std::cout << "🚨 CRITICAL ALERT: Aircraft " << a1.id << " and " << a2.id << " may collide in " << prediction_interval << "s!\n";
                    }
                }

                  if (dist < 0.5f && std::abs(a1.z - a2.z) < 0.5f) {
                      a1.active = false;
                      a2.active = false;
                      std::lock_guard<std::mutex> lock_print(print_mutex);
                      std::cout << "❌ COLLISION happened between Aircraft " << a1.id << " and " << a2.id << "!\n";
                  }
            }
        }
    }
    return nullptr;
}

void* history_logger_thread(void* arg) {
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(20));

        std::ofstream log("airspace_history.log", std::ios::app);
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        log << "\nTimestamp: " << std::ctime(&now);

        std::lock_guard<std::mutex> lock(shm_mutex);
        for (int i = 0; i < shm->count; ++i) {
            const Aircraft& a = shm->aircraft[i];
            if (!a.active) continue;
            log << "Aircraft " << a.id << " Pos(" << a.x << ", " << a.y << ", " << a.z
                << ") Speed(" << a.speedX << ", " << a.speedY << ", " << a.speedZ << ")\n";
        }
        log.close();
    }
    return nullptr;
}

void create_aircraft(int id, float x, float y, float z, float vx, float vy, float vz) {
    if (!shm) return;
    std::lock_guard<std::mutex> lock(shm_mutex);
    if (shm->count < MAX_AIRCRAFT) {
        Aircraft* a = &shm->aircraft[shm->count++];
        a->id = id;
        a->x = x; a->y = y; a->z = z;
        a->speedX = vx; a->speedY = vy; a->speedZ = vz;
        a->active = true;

        pthread_create(&a->thread_id, nullptr, aircraft_thread, a);
        pthread_detach(a->thread_id);

        std::lock_guard<std::mutex> lock_print(print_mutex);
        std::cout << "✅ Created Aircraft " << id << " at position (" << x << ", " << y << ", " << z << ")\n";
    }
}

void init_shared_memory() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1 || ftruncate(shm_fd, SHARED_MEM_SIZE) == -1) exit(1);
    shm = static_cast<SharedMemory*>(mmap(nullptr, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (shm == MAP_FAILED) exit(1);
    shm->count = 0;
}

void load_aircraft_data(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) return;
    int id; float x, y, z, vx, vy, vz;
    while (file >> id >> x >> y >> z >> vx >> vy >> vz) {
        create_aircraft(id, x, y, z, vx, vy, vz);
    }
}

int main(int argc, char* argv[]) {
    signal(SIGINT, signal_handler);
    init_shared_memory();

    std::string aircraft_file = (argc > 1) ? argv[1] : "aircraft_data.txt";
    load_aircraft_data(aircraft_file);

    pthread_t computer_tid, logger_tid;
    pthread_create(&computer_tid, nullptr, computer_thread, nullptr);
    pthread_detach(computer_tid);
    pthread_create(&logger_tid, nullptr, history_logger_thread, nullptr);
    pthread_detach(logger_tid);

    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::lock_guard<std::mutex> lock(shm_mutex);
        std::lock_guard<std::mutex> lock_print(print_mutex);
        std::cout << "\n🌍  Current Aircraft Positions:\n";
        for (int i = 0; i < shm->count; ++i) {
            const auto& a = shm->aircraft[i];
            if (!a.active) continue;
            std::cout << "Aircraft " << a.id << ": X=" << a.x << ", Y=" << a.y << ", Z=" << a.z << "\n";
        }
    }
    return 0;
}
