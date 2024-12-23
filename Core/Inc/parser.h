/*
 * parser.h
 *
 *  Created on: May 18, 2024
 *      Author: tov.Engineer
 */
#ifndef INC_PARSER_H_
#define INC_PARSER_H_

#include "usart.h"

#define COM_BUFsize 25
#define COM_SerialDelay 15
#define COMUART huart1
void COM_RXCallback (UART_HandleTypeDef *huart);

void COM_BufRXclear();

uint8_t COM_BufAvail();

uint8_t strCompare(char *aData, uint8_t *bData);

void COMPrint(char *Dat);

void COMPrintD(char *Index, int Val);

void COMPrintF(char *Index, float Val);

uint16_t strToint (uint8_t *bData);

void COMPrintH(char *Index, uint8_t Val);

uint32_t hex2int(char *hex);

void COM_tick();

#endif /* INC_PARSER_H_ */
