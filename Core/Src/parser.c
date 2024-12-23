/*
 * parser.c
 *
 *  Created on: May 18, 2024
 *      Author: tov.Engineer
 */
#include "parser.h"
#include "main.h"
#include "usart.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

#include "7941w.h"

uint8_t bufCom[COM_BUFsize] = { 0, };
uint8_t inByte[] = { 0 };
uint8_t COM_symNum = 0;
uint32_t COM_BufTim = 0;

void COM_RXCallback(UART_HandleTypeDef *huart) {
	if (huart != &COMUART)
		return;
	bufCom[COM_symNum] = inByte[0];
	COM_symNum++;
	if (COM_symNum >= COM_BUFsize)
		COM_symNum = 0;
	COM_BufTim = HAL_GetTick();
	HAL_UART_Receive_IT(&COMUART, inByte, 1);
	//HAL_UART_Transmit(&huart2, inByte, 1, 100);
}

void COM_BufRXclear() {
	HAL_UART_Receive_IT(&COMUART, inByte, 1);
	COM_symNum = 0;
	for (uint8_t j = 0; j < COM_BUFsize; j++) {
		bufCom[j] = 0;
	}
}

uint8_t COM_BufAvail() {
	if (COM_symNum == 0)
		return 0;
	if (HAL_GetTick() - COM_BufTim > COM_SerialDelay)
		return 1;
	else
		return 0;
}

uint8_t strCompare(char *aData, uint8_t *bData) {
	for (uint8_t i = 0; i < strlen(aData); i++) {
		if (aData[i] != bData[i])
			return 0;
	}
	return 1;
}

void COMPrint(char *Dat) {
	HAL_UART_Transmit(&COMUART, (uint8_t*) Dat, strlen(Dat), 100);
}

void COMPrintD(char *Index, int Val) {
	uint8_t DataToSend[25];
	sprintf((char*) DataToSend, "%s= %d\r\n", Index, Val);   //%s= Index,
	HAL_UART_Transmit(&COMUART, DataToSend, strlen((char*) DataToSend), 100);
}

void COMPrintH(char *Index, uint8_t Val) {
	uint8_t DataToSend[25];
	sprintf((char*) DataToSend, "%s- 0x%x\r\n", Index, Val);   //%s= Index,
	HAL_UART_Transmit(&COMUART, DataToSend, strlen((char*) DataToSend), 100);
}

uint16_t strToint(uint8_t *bData) {
	char Buf[5];
	uint8_t n = strlen((char*) bData);
	uint8_t j = 0;
	for (uint8_t i = 0; i < n; i++) {
		if (bData[i] >= '0' && bData[i] <= '9') {
			Buf[j] = bData[i];
			j++;
		}
	}
	if (j == 0)
		return 0xFFFF;
	return (uint16_t) atoi(Buf);
}

uint32_t hex2int(char *hex) {
	uint32_t val = 0;
	while (*hex) {
		// get current character then increment
		uint8_t byte = *hex++;
		// transform hex character to the 4bit equivalent number, using the ascii table indexes
		if (byte >= '0' && byte <= '9')
			byte = byte - '0';
		else if (byte >= 'a' && byte <= 'f')
			byte = byte - 'a' + 10;
		else if (byte >= 'A' && byte <= 'F')
			byte = byte - 'A' + 10;
		// shift 4 to make space for new digit, and add the 4 bits of the new digit
		val = (val << 4) | (byte & 0xF);
	}
	return val;
}
void COM_tick() {
	if (!COM_BufAvail())
		return;

	if (strCompare("VSW?", bufCom)) {
		COMPrintD("VSW", 1);
	} else if (strCompare("HFR", bufCom)) {
		COMPrint("OK\r\n");
		uint8_t cuUID [4];
		HF_Read_UID ((uint8_t*)&cuUID);
	} else if (strCompare("HFW", bufCom)) {
		COMPrint("OK\r\n");
		uint8_t cuUID [] = {0x61,0xE0,0xF1,0x89};
		HF_Write_UID ((uint8_t*)&cuUID);
	} else if (strCompare("LFR", bufCom)) {
		COMPrint("OK\r\n");
		uint8_t cuUID [5];
		LF_Read_UID ((uint8_t*)&cuUID);
	} else if (strCompare("LFW", bufCom)) {
		COMPrint("OK\r\n");
		uint8_t cuUID [] = {0x3f, 0x00, 0x6f, 0x73, 0x2d};
		LF_Write_UID ((uint8_t*)&cuUID);
	}

	//else if (strCompare("SERV-", bufCom)) {
//		uint8_t Valbuf = strToint(bufCom);
//		if (Valbuf <= 1) {
//			COMPrint("OK\r\n");
//			servMod = Valbuf;
//		} else
//			COMPrint("ERROR\r\n");
//	} else if (strCompare("SERV?", bufCom)) {
//		COMPrintD("SERV", servMod);
//
//	} else if (strCompare("EMG?", bufCom)) {  // get EMG state
//		COMPrintD("EMG", EMG_Flag);
//	} else if (servMod) {
//		ServPars();
//	} else if (!EMG_Flag) {
//		CNCPars();
//		MeasPars();
//		TestPars();
	else {
		COMPrint("ERROR\r\n");
	}

	COM_BufRXclear();
}
