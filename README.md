# Kutsinta Automation System

A standalone embedded system designed to automate the preparation
process of Kutsinta, a traditional Filipino delicacy.
This project focuses on timed cooking/steaming, mixing, and process
control using a microcontroller.

------------------------------------------------------------------------

🚀 Features

-   Automated cooking/steaming process
-   Timed operation cycles
-   Heating element control
-   Optional mixing support
-   Standalone system (no mobile or web integration)
-   Simple and reliable automation logic
-   Expandable for small-scale production

------------------------------------------------------------------------

🧰 Hardware Requirements

-   ESP32 / Arduino-compatible controller
-   Heating element or steamer
-   Relay module (for heater control)
-   Mixing motor (optional)
-   Power supply
-   Cooking container / steamer setup
-   Optional:
    -   LCD display
    -   Buttons / switches
    -   Temperature sensor

------------------------------------------------------------------------

🔌 Core Functionality

-   Starts cooking/steaming cycle
-   Controls heating element
-   Runs process for defined duration
-   Stops automatically after cooking
-   Can include mixing before or during process

------------------------------------------------------------------------

⚙️ Workflow

1.  System powers on
2.  User starts process
3.  Heating begins
4.  Cooking/steaming runs for configured time
5.  System stops heating
6.  Process completes
7.  System waits for next cycle

------------------------------------------------------------------------

⏱ Control Logic

-   Cooking time is controlled via timing
-   Heating cycles can be adjusted in code
-   Supports simple state-based automation

------------------------------------------------------------------------

🔐 Safety Notes

-   Always use relay module for heating element
-   Do not connect high-power devices directly to controller
-   Ensure proper insulation and heat protection
-   Monitor system during initial testing
-   Keep electronics away from moisture

------------------------------------------------------------------------

📦 Libraries (if used)

-   Arduino core libraries
-   Optional timing libraries (Chrono / millis)

------------------------------------------------------------------------

🛠 Setup Instructions

1.  Connect heating element via relay
2.  Connect optional motor or sensors
3.  Upload firmware to controller
4.  Power the system
5.  Start cooking process and test

------------------------------------------------------------------------

📌 Notes

-   This is a separate standalone project
-   Designed for automation of Kutsinta preparation
-   Can be extended with temperature control or IoT features



------------------------------------------------------------------------

👨‍💻 Author

Kutsinta Automation Project
