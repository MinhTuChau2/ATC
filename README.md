https://www.youtube.com/watch?v=iU6O3JZoo2Q <br/>
**COEN 320 - Air Traffic Control (ATC) System** <br/> 
**Overview** <br/>
This project implements a simplified real-time Air Traffic Control (ATC) system for monitoring and controlling aircraft in the en-route airspace. The system is designed to simulate the management of aircraft flows in a 3D airspace, focusing on ensuring safety and maintaining proper separation between aircraft. The project is developed in C/C++ and tested on the QNX real-time operating system.

**Key Features:** <br/>
Aircraft Separation: <br/>The system ensures that aircraft maintain the required distance from each other both in height and in horizontal distance.<br/>

**Radar Simulation:**<br/> Simulates both primary and secondary radar functions to track aircraft.<br/>

**Safety Violations Detection:**<br/> Alerts the operator when safety violations are detected or imminent.<br/>

**Logging and History:** <br/>Logs aircraft movements and operator commands for analysis and debugging.<br/>

**User Interface:** <br/> Provides a simple operator console to manage aircraft movement and receive visual feedback.<br/>

**Project Components** <br/>
The ATC system is broken down into several major subsystems:<br/>

**Radar Subsystem:** <br/>

- Simulates both primary (PSR) and secondary (SSR) radar.<br/>

- Tracks aircraft in the 3D airspace, providing details such as ID, flight level, speed, and position.<br/>

**Computer System:** <br/>

- Periodically checks for safety violations.<br/>

- Generates alerts (auditory or visual) when a violation is detected or imminent.<br/>

**Operator Console:** <br/>

- Allows the operator to send commands to the aircraft to change speed, altitude, or position.<br/>

- Provides detailed information about any aircraft in the monitored airspace.<br/>

**Data Display:** <br/>

- Visualizes the airspace and aircraft positions, updating every 5 seconds.<br/>

- Displays aircraft IDs, positions, and other relevant flight data.<br/>

**Communication System:** <br/>

- Handles the transmission of commands and requests between the operator and the aircraft.<br/>

**System Requirements** <br/>
QNX Real-Time OS (version 7.0 or later): <br/>The system must be developed and tested using QNX's real-time operating system.<br/>

- C/C++: The implementation is written in C or C++.<br/>

- QNX Momentics IDE:<br/> The development must be done using the QNX Momentics IDE.<br/>

- POSIX Shared Memory:<br/> The system utilizes shared memory for inter-process communication (IPC) between the subsystems.<br/>

**Input & Output Specifications**<br/>

- Each aircraft entry in the input file contains:<br/>

T- ime: The moment the aircraft enters the monitored airspace.<br/>

- ID: Unique identifier for the aircraft.<br/>

- X, Y, Z: Coordinates of the aircraft when it enters the area.<br/>

- SpeedX, SpeedY, SpeedZ: Speed components in each coordinate dimension.<br/>

- Example input format:<br/>

ID X Y Z SpeedX SpeedY SpeedZ <br/>
1 15 10 10  -0.1  0   0 <br/>
2 5  10 10  0.1  0   0 <br/>
3 5  5  5    0.2    0.1   0 <br/>
4 10 15 8    0.1    0.1   0 <br/>

Output:<br/>
- Aircraft Information: Logs the current state of each aircraft (ID, position, speed, etc.).<br/>

- Safety Alerts: Emits an alarm (sonorous or visual) when a safety violation is detected.<br/>

- History Log: Saves the airspace status every 20 seconds.<br/>

- Operator Commands: Logs all commands issued by the operator (e.g., request to change aircraft speed/altitude).<br/>

Architecture <br/>
The system is designed using multiple independent QNX processes that communicate via shared memory. The primary subsystems include:

- Radar Subsystem

- Computer System

- Operator Console

- Data Display

- Communication System

Shared Memory Architecture:
- All subsystems exchange data using POSIX shared memory for synchronization and data consistency.

- Radar Subsystem: Tracks aircraft and provides position data.

- Computer System: Checks for separation violations and triggers alerts.

- Operator Console: Receives inputs from the user (controller).

- Data Display: Visualizes the current airspace and aircraft data.

- Communication System: Simulates message transmission to aircraft.

