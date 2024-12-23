/*
 * 7941w.c
 *
 *  Created on: Dec 3, 2024
 *      Author: tov.Engineer
 */

#include "7941w.h"
#include "usart.h"
#include "parser.h"

uint8_t bufRFID[RFID_BUFsize] = { 0, };
uint8_t RFIDinByte[] = { 0 };
uint8_t RFID_datNum = 0;
uint32_t RFID_BufTim = 0;

void RFID_RXCallback(UART_HandleTypeDef *huart) {
	if (huart != &RFID_UART)
		return;
	bufRFID[RFID_datNum] = RFIDinByte[0];
	RFID_datNum++;
	if (RFID_datNum >= RFID_BUFsize)
		RFID_datNum = 0;
	RFID_BufTim = HAL_GetTick();
	HAL_UART_Receive_IT(&RFID_UART, RFIDinByte, 1);

	//HAL_UART_Transmit(&Debug_UART, RFIDinByte, 1, 100);
}

void RFID_BufRXclear() {
	HAL_UART_Receive_IT(&RFID_UART, RFIDinByte, 1);
	RFID_datNum = 0;
	for (uint8_t i = 0; i < RFID_BUFsize; i++) {
		bufRFID[i] = 0;
	}
}

uint8_t RFID_BufAvail() {
	if (RFID_datNum == 0)
		return 0;
	if (HAL_GetTick() - RFID_BufTim > RFID_SerialDelay)
		return 1;
	else
		return 0;
}

uint8_t xorCheck(uint8_t arr[], int from, int to) {
	int result = 0;
	for (int i = from; i <= to; i++) {
		result = result ^ arr[i];
	}
	return result;
}

void RFID_sendCommand(uint8_t address, uint8_t cmd, uint8_t data[],
		uint8_t dataLength) {
	int cmdLength = 6 + dataLength;
	uint8_t command[cmdLength];
	command[0] = 0xAB;
	command[1] = 0xBA;
	command[2] = address;
	command[3] = cmd;
	command[4] = dataLength;
	int cmdIndex = 5;
	for (int i = 0; i < dataLength; i++) {
		command[cmdIndex++] = data[i];
	}
	command[cmdLength - 1] = xorCheck(command, 3, cmdLength - 2); //XOR

#if (RFID_Debug)
	COMPrint("Sending command:\r\n");
	for (uint8_t c = 0; c < cmdLength; c++) {
		COMPrintH("t", command[c]);
	}
#endif
	RFID_BufRXclear();
	HAL_UART_Transmit(&RFID_UART, command, cmdLength, 100);
}

// Command 0x10: Read UID from 13.56MHz RFID
RFID_STATUS HF_Read_UID(uint8_t *UID) {
	uint8_t data[1];
	RFID_sendCommand(0, 0x10, data, 0);
	while (!RFID_BufAvail()) {
		;
	}
	if (bufRFID[3] == 0x81) {
		UID[0] = bufRFID[5];
		UID[1] = bufRFID[6];
		UID[2] = bufRFID[7];
		UID[3] = bufRFID[8];
	}
#if (RFID_Debug)
	COMPrint("Recive:\r\n");
	for (uint8_t i = 0; i < RFID_datNum; i++) {
		COMPrintH("r", bufRFID[i]);
	}
	if (bufRFID[3] == 0x80) {
		COMPrint("RFID_Fail\r\n");
	} else if (bufRFID[3] == 0x81) {
		COMPrint("RFID_Succes\r\n");
		COMPrint("UID:\r\n");
		for (uint8_t i = 0; i < 4; i++) {
			COMPrintH("d", UID[i]);
		}
	} else {
		COMPrint("RFID_Undef\r\n");
	}
#endif

	if (bufRFID[3] == 0x80) {
		return RFID_Fail;
	} else if (bufRFID[3] == 0x81) {
		return RFID_Succes;
	}
	return RFID_Undef;
}

// Command 0x11: Write UID to 13.56MHz RFID
RFID_STATUS HF_Write_UID(uint8_t *UID) {
	RFID_sendCommand(0, 0x11, UID, 4);
	while (!RFID_BufAvail()) {
		;
	}

#if (RFID_Debug)
	COMPrint("Recive:\r\n");
	for (uint8_t i = 0; i < RFID_datNum; i++) {
		COMPrintH("r", bufRFID[i]);
	}

	if (bufRFID[3] == 0x80) {
		COMPrint("RFID_Fail\r\n");
	} else if (bufRFID[3] == 0x81) {
		COMPrint("RFID_Succes\r\n");
	} else {
		COMPrint("RFID_Undef\r\n");
	}

#endif

	if (bufRFID[3] == 0x80) {
		return RFID_Fail;
	} else if (bufRFID[3] == 0x81) {
		return RFID_Succes;
	}
	return RFID_Undef;
}


// Command 0x15: Read ID number from 125KHz RFID
RFID_STATUS LF_Read_UID(uint8_t *UID) {
	uint8_t data[1];
	RFID_sendCommand(0, 0x15, data, 0);
	while (!RFID_BufAvail()) {
		;
	}
	if (bufRFID[3] == 0x81) {
		UID[0] = bufRFID[5];
		UID[1] = bufRFID[6];
		UID[2] = bufRFID[7];
		UID[3] = bufRFID[8];
		UID[4] = bufRFID[9];
	}
#if (RFID_Debug)
	COMPrint("Recive:\r\n");
	for (uint8_t i = 0; i < RFID_datNum; i++) {
		COMPrintH("r", bufRFID[i]);
	}
	if (bufRFID[3] == 0x80) {
		COMPrint("RFID_Fail\r\n");
	} else if (bufRFID[3] == 0x81) {
		COMPrint("RFID_Succes\r\n");
		COMPrint("UID:\r\n");
		for (uint8_t i = 0; i < 5; i++) {
			COMPrintH("d", UID[i]);
		}
	} else {
		COMPrint("RFID_Undef\r\n");
	}
#endif

	if (bufRFID[3] == 0x80) {
		return RFID_Fail;
	} else if (bufRFID[3] == 0x81) {
		return RFID_Succes;
	}
	return RFID_Undef;
}

// Command 0x16: Write ID number to 125KHz RFID
RFID_STATUS LF_Write_UID(uint8_t *UID) {
	RFID_sendCommand(0, 0x16, UID, 5);
	while (!RFID_BufAvail()) {
		;
	}

#if (RFID_Debug)
	COMPrint("Recive:\r\n");
	for (uint8_t i = 0; i < RFID_datNum; i++) {
		COMPrintH("r", bufRFID[i]);
	}

	if (bufRFID[3] == 0x80) {
		COMPrint("RFID_Fail\r\n");
	} else if (bufRFID[3] == 0x81) {
		COMPrint("RFID_Succes\r\n");
	} else {
		COMPrint("RFID_Undef\r\n");
	}

#endif

	if (bufRFID[3] == 0x80) {
		return RFID_Fail;
	} else if (bufRFID[3] == 0x81) {
		return RFID_Succes;
	}
	return RFID_Undef;
}
