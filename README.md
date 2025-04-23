**COEN 320 - Air Traffic Control (ATC) System**
**Overview** <br/>
This project implements a simplified real-time Air Traffic Control (ATC) system for monitoring and controlling aircraft in the en-route airspace. The system is designed to simulate the management of aircraft flows in a 3D airspace, focusing on ensuring safety and maintaining proper separation between aircraft. The project is developed in C/C++ and tested on the QNX real-time operating system.

Key Features:
Aircraft Separation: The system ensures that aircraft maintain the required distance from each other both in height and in horizontal distance.

Radar Simulation: Simulates both primary and secondary radar functions to track aircraft.

Safety Violations Detection: Alerts the operator when safety violations are detected or imminent.

Logging and History: Logs aircraft movements and operator commands for analysis and debugging.

User Interface: Provides a simple operator console to manage aircraft movement and receive visual feedback.

Project Components
The ATC system is broken down into several major subsystems:

Radar Subsystem:

Simulates both primary (PSR) and secondary (SSR) radar.

Tracks aircraft in the 3D airspace, providing details such as ID, flight level, speed, and position.

Computer System:

Periodically checks for safety violations.

Generates alerts (auditory or visual) when a violation is detected or imminent.

Operator Console:

Allows the operator to send commands to the aircraft to change speed, altitude, or position.

Provides detailed information about any aircraft in the monitored airspace.

Data Display:

Visualizes the airspace and aircraft positions, updating every 5 seconds.

Displays aircraft IDs, positions, and other relevant flight data.

Communication System:

Handles the transmission of commands and requests between the operator and the aircraft.

System Requirements
QNX Real-Time OS (version 7.0 or later): The system must be developed and tested using QNX's real-time operating system.

C/C++: The implementation is written in C or C++.

QNX Momentics IDE: The development must be done using the QNX Momentics IDE.

POSIX Shared Memory: The system utilizes shared memory for inter-process communication (IPC) between the subsystems.

Input & Output Specifications
Input File Format:
Each aircraft entry in the input file contains:

Time: The moment the aircraft enters the monitored airspace.

ID: Unique identifier for the aircraft.

X, Y, Z: Coordinates of the aircraft when it enters the area.

SpeedX, SpeedY, SpeedZ: Speed components in each coordinate dimension.

Example input format:

ID X Y Z SpeedX SpeedY SpeedZ <br/>
1 15 10 10  -0.1  0   0 <br/>
2 5  10 10  0.1  0   0 <br/>
3 5  5  5    0.2    0.1   0 <br/>
4 10 15 8    0.1    0.1   0 <br/>

Output:
Aircraft Information: Logs the current state of each aircraft (ID, position, speed, etc.).

Safety Alerts: Emits an alarm (sonorous or visual) when a safety violation is detected.

History Log: Saves the airspace status every 20 seconds.

Operator Commands: Logs all commands issued by the operator (e.g., request to change aircraft speed/altitude).

Architecture
The system is designed using multiple independent QNX processes that communicate via shared memory. The primary subsystems include:

Radar Subsystem

Computer System

Operator Console

Data Display

Communication System

Shared Memory Architecture:
All subsystems exchange data using POSIX shared memory for synchronization and data consistency.

Radar Subsystem: Tracks aircraft and provides position data.

Computer System: Checks for separation violations and triggers alerts.

Operator Console: Receives inputs from the user (controller).

Data Display: Visualizes the current airspace and aircraft data.

Communication System: Simulates message transmission to aircraft.

Implementation Requirements
Aircraft Tasks: Each aircraft is implemented as a periodic task (process or thread), updating its location based on speed every second.

Radar Simulation: The radar subsystem polls each aircraft to get its location and status.

Safety Checks: The computer system performs periodic checks (every n seconds) to ensure aircraft maintain the required separation.

Alert Mechanism: The system emits alerts if a violation is detected, either immediately or within the next 2 minutes.


