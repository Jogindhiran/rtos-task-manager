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
## How It Works
- User connects via UART terminal (e.g., Tera Term, PuTTY)
- A menu is displayed with available options
- User inputs commands, which are parsed and routed to the appropriate FreeRTOS task
- Each feature is executed in its own task for clarity and modularity

