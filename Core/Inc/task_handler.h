/*
 * task_handler.h
 *
 *  Created on: Aug 20, 2025
 *      Author: Jogindhiran
 */

#ifndef INC_TASK_HANDLER_H_
#define INC_TASK_HANDLER_H_

#include "main.h"

// Max lengths
#define CMD_MAX_LEN 64

// Structs
typedef struct {
    char text[CMD_MAX_LEN];
} CommandMessage;

typedef struct {
    char text[128];
} PrintMessage;

typedef enum {
    MENU_MAIN,
    MENU_LED,
    MENU_AES,
    MENU_AES_INPUT_ENC,
    MENU_AES_INPUT_DEC
} MenuState;

// Extern Task Handles
extern TaskHandle_t ledTaskHandle;
extern TaskHandle_t sensorTaskHandle;
extern TaskHandle_t aesTaskHandle;
extern TaskHandle_t uartTaskHandle;
extern TaskHandle_t menuTaskHandle;

// Extern Queues
extern QueueHandle_t ledQueue;
extern QueueHandle_t sensorQueue;
extern QueueHandle_t aesQueue;
extern QueueHandle_t uartQueue;
extern QueueHandle_t menuQueue;

// Task function prototypes
void vTaskLED(void *pvParameters);
void vTaskSensor(void *pvParameters);
void vTaskAES(void *pvParameters);
void vTaskUART(void *pvParameters);
void vTaskMenu(void *pvParameters);

#endif /* INC_TASK_HANDLER_H_ */
