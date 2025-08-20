/*
 * task_handler.c
 *
 *  Created on: Aug 20, 2025
 *      Author: Jogindhiran
 */

#include "task_handler.h"

// Extern peripheral handles from main.c
extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef hadc1;
extern char rxChar;
extern char uartBuffer[CMD_MAX_LEN];
extern uint8_t uartIndex;

// Task functions go here exactly as in main.c
// vTaskLED, vTaskSensor, vTaskAES, vTaskUART, vTaskMenu

/* LED Task */
void vTaskLED(void *pvParameters)
{
    CommandMessage msg;
    while(1)
    {
        if(xQueueReceive(ledQueue, &msg, portMAX_DELAY) == pdPASS)
        {
            PrintMessage pmsg;
            if(strstr(msg.text, "LED_ON"))
            {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
                strcpy(pmsg.text, "LED turned ON\r\n");
            }
            else if(strstr(msg.text, "LED_OFF"))
            {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
                strcpy(pmsg.text, "LED turned OFF\r\n");
            }
            else if(strstr(msg.text, "LED_BLINK"))
            {
                for(int i=0;i<5;i++)
                {
                    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
                    vTaskDelay(pdMS_TO_TICKS(200));
                }
                strcpy(pmsg.text, "LED Blink done\r\n");
            }
            else strcpy(pmsg.text, "Unknown LED command\r\n");

            xQueueSend(uartQueue, &pmsg, portMAX_DELAY);
        }
    }
}

/* Sensor Task */
void vTaskSensor(void *pvParameters)
{
    CommandMessage msg;
    char uartMsg[50];

    while(1)
    {
        if(xQueueReceive(sensorQueue, &msg, portMAX_DELAY) == pdPASS)
        {
            HAL_ADC_Start(&hadc1);
            HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
            uint32_t val = HAL_ADC_GetValue(&hadc1);
            HAL_ADC_Stop(&hadc1);

            PrintMessage pmsg;
            sprintf(pmsg.text, "Sensor Value: %lu\r\n", val);
            xQueueSend(uartQueue, &pmsg, portMAX_DELAY);
        }
    }
}

/* AES Task */
void vTaskAES(void *pvParameters)
{
    CommandMessage msg;
    uint8_t key[16] = "testkey123456789";
    struct AES_ctx ctx;
    AES_init_ctx(&ctx, key);

    while(1)
    {
        if(xQueueReceive(aesQueue, &msg, portMAX_DELAY) == pdPASS)
        {
            PrintMessage pmsg;
            char *payload = msg.text + 8; // skip "AES_ENC " or "AES_DEC "
            size_t len = strlen(payload);
            uint8_t buffer[64] = {0};
            strncpy((char*)buffer, payload, sizeof(buffer)-1);
            size_t paddedLen = ((len + 15)/16)*16;

            if(strncmp(msg.text, "AES_ENC", 7) == 0)
            {
                // Encryption
                for(size_t i=0;i<paddedLen;i+=16)
                    AES_ECB_encrypt(&ctx, buffer+i);

                char *ptr = pmsg.text;
                ptr += sprintf(ptr, "AES Enc: ");
                for(size_t i=0;i<paddedLen;i++) ptr += sprintf(ptr, "%02X", buffer[i]);
                sprintf(ptr, "\r\n");
            }
            else if(strncmp(msg.text, "AES_DEC", 7) == 0)
            {
                // Decryption
                // Convert hex string to bytes
                size_t hexLen = strlen(payload);
                size_t byteLen = hexLen / 2;
                for(size_t i = 0; i < byteLen; i++) {
                    sscanf(payload + 2*i, "%2hhX", &buffer[i]);
                }

                size_t paddedLen = ((byteLen + 15)/16)*16;
                for(size_t i = 0; i < paddedLen; i += 16)
                    AES_ECB_decrypt(&ctx, buffer + i);

                // Null-terminate at original byte length
                buffer[byteLen] = '\0';

                sprintf(pmsg.text, "AES Dec: %s\r\n", buffer);
            }
            else
            {
                strcpy(pmsg.text, "Invalid AES command\r\n");
            }

            xQueueSend(uartQueue, &pmsg, portMAX_DELAY);
        }
    }
}


/* UART/Print Task */
void vTaskUART(void *pvParameters)
{
    PrintMessage msg;
    while(1)
    {
        if(xQueueReceive(uartQueue, &msg, portMAX_DELAY) == pdPASS)
        {
            HAL_UART_Transmit(&huart2, (uint8_t*)msg.text, strlen(msg.text), HAL_MAX_DELAY);
        }
    }
}

/* Menu Task */
void vTaskMenu(void *pvParameters)
{
    MenuState state = MENU_MAIN;
    CommandMessage cmdMsg;
    PrintMessage pmsg;

    while(1)
    {
        // Display menu once per state change
        switch(state)
        {
            case MENU_MAIN:
                strcpy(pmsg.text, "\r\n---MAIN MENU---\r\n1. LED Commands\r\n2. Read Sensor\r\n3. AES\r\n> ");
                xQueueSend(uartQueue, &pmsg, portMAX_DELAY);
                break;

            case MENU_LED:
                strcpy(pmsg.text, "\r\n---LED MENU---\r\n1. LED_ON\r\n2. LED_OFF\r\n3. LED_BLINK\r\n0. Back\r\n> ");
                xQueueSend(uartQueue, &pmsg, portMAX_DELAY);
                break;

            case MENU_AES:
                strcpy(pmsg.text, "\r\n---AES MENU---\r\n1. Encrypt\r\n2. Decrypt\r\n0. Back\r\n> ");
                xQueueSend(uartQueue, &pmsg, portMAX_DELAY);
                break;

            case MENU_AES_INPUT_ENC:
                strcpy(pmsg.text, "Enter string to ENCRYPT:\r\n> ");
                xQueueSend(uartQueue, &pmsg, portMAX_DELAY);

                if(xQueueReceive(menuQueue, &cmdMsg, portMAX_DELAY) == pdPASS)
                {
                    char temp[CMD_MAX_LEN];
                    snprintf(temp, CMD_MAX_LEN, "AES_ENC %s", cmdMsg.text);
                    strncpy(cmdMsg.text, temp, CMD_MAX_LEN);
                    xQueueSend(aesQueue, &cmdMsg, portMAX_DELAY);
                    state = MENU_MAIN;
                }
                break;

            case MENU_AES_INPUT_DEC:
                strcpy(pmsg.text, "Enter string (16 byte Hex) to DECRYPT:\r\n> ");
                xQueueSend(uartQueue, &pmsg, portMAX_DELAY);

                if(xQueueReceive(menuQueue, &cmdMsg, portMAX_DELAY) == pdPASS)
                {
                    char temp[CMD_MAX_LEN];
                    snprintf(temp, CMD_MAX_LEN, "AES_DEC %s", cmdMsg.text);
                    strncpy(cmdMsg.text, temp, CMD_MAX_LEN);
                    xQueueSend(aesQueue, &cmdMsg, portMAX_DELAY);
                    state = MENU_MAIN;
                }
                break;

        }

        // Wait for user input (from menuQueue, filled by UART ISR)
        if(xQueueReceive(menuQueue, &cmdMsg, portMAX_DELAY) == pdPASS)
        {
            switch(state)
            {
                case MENU_MAIN:
                    if(cmdMsg.text[0] == '1') state = MENU_LED;
                    else if(cmdMsg.text[0] == '2') xQueueSend(sensorQueue, &cmdMsg, portMAX_DELAY);
                    else if(cmdMsg.text[0] == '3') state = MENU_AES;
                    else
                    {
                        strcpy(pmsg.text, "Invalid option\r\n");
                        xQueueSend(uartQueue, &pmsg, portMAX_DELAY);
                    }
                    break;

                case MENU_LED:
                    if(cmdMsg.text[0] == '1') { strcpy(cmdMsg.text, "LED_ON"); xQueueSend(ledQueue, &cmdMsg, portMAX_DELAY); }
                    else if(cmdMsg.text[0] == '2') { strcpy(cmdMsg.text, "LED_OFF"); xQueueSend(ledQueue, &cmdMsg, portMAX_DELAY); }
                    else if(cmdMsg.text[0] == '3') { strcpy(cmdMsg.text, "LED_BLINK"); xQueueSend(ledQueue, &cmdMsg, portMAX_DELAY); }
                    else if(cmdMsg.text[0] == '0') state = MENU_MAIN;
                    else { strcpy(pmsg.text, "Invalid LED option\r\n"); xQueueSend(uartQueue, &pmsg, portMAX_DELAY); }
                    break;

                case MENU_AES:
                    if(cmdMsg.text[0] == '1')
                        state = MENU_AES_INPUT_ENC;  // Move to input string state
                    else if(cmdMsg.text[0] == '2')
                        state = MENU_AES_INPUT_DEC;  // Move to input string state
                    else if(cmdMsg.text[0] == '0')
                        state = MENU_MAIN;
                    else { strcpy(pmsg.text, "Invalid AES option\r\n"); xQueueSend(uartQueue, &pmsg, portMAX_DELAY); }
                    break;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

