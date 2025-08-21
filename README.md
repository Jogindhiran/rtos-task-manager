# RTOS Task Manager
## Overview
RTOS-based UART menu interface that allows task management, LED control, sensor reading using ADC, and AES command handling. The system demonstrates how FreeRTOS tasks, queues, and state machines can be used to build an interactive command-driven interface.

## Features
- Menu-driven command interface over UART
- LED Control – ON, OFF, BLINK
- Sensor Reading – Dummy sensor value simulation
- AES Encryption/Decryption – Demonstrates basic data security handling
- Task Management – Uses FreeRTOS tasks and queues for modular design
## Menu Options
```
Main Menu:
1. Configure LED
   - LED_ON
   - LED_OFF
   - LED_BLINK
2. Read Sensor Value (ADC)
3. AES Operations
   - AES_ENC <text>
   - AES_DEC <ciphertext>
```
## Hardware and Software Requirements
- Hardware:
   - STM32F446 Nucleo or similar board
   - Onboard UART interface (via ST-LINK VCP or external USB-UART module)
   - LED connected to GPIO pin for ON/OFF/Blink control
   - Analog sensor (e.g., potentiometer, temperature sensor) connected to ADC pin
   - Power supply (USB or external 5V)

- Software:
   - STM32CubeIDE (for project build and flashing)
   - FreeRTOS (task scheduling and queue management)
   - Tiny-AES-c library (AES encryption/decryption)
   - Serial terminal (e.g., PuTTY, Tera Term) for sending commands
   - Python GUI for encrypted UART communication (Optional)
## How It Works
- User connects via UART terminal (e.g., Tera Term, PuTTY)
- A menu is displayed with available options
- User inputs commands, which are parsed and routed to the appropriate FreeRTOS task
- Each feature is executed in its own task for clarity and modularity
