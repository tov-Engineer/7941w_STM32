/*
 * 7941w.h
 *
 *  Created on: Dec 3, 2024
 *      Author: tov.Engineer
 */

#ifndef INC_7941W_H_
#define INC_7941W_H_

#include "usart.h"

#define RFID_Debug 1
#define Debug_UART huart1

#define RFID_BUFsize 25
#define RFID_SerialDelay 15
#define RFID_UART huart2

typedef enum {
	RFID_Succes,
	RFID_Fail,
	RFID_Undef,
	RFID_NC
}RFID_STATUS;

void RFID_RXCallback (UART_HandleTypeDef *huart);
void RFID_BufRXclear();
uint8_t RFID_BufAvail();
RFID_STATUS HF_Read_UID (uint8_t* UID);
RFID_STATUS HF_Write_UID(uint8_t *UID);
RFID_STATUS LF_Read_UID(uint8_t *UID);
RFID_STATUS LF_Write_UID(uint8_t *UID);

#endif /* INC_7941W_H_ */
