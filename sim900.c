#include <string.h>
#include <util/atomic.h>
//#include <avr/pgmspace.h>
#include "template.h"
#include "uart.h"
#include "sim900.h"

const char successResp[] = "OK";
const char failResp[] = "ERROR";
const char inNetResp[] = "+CREG: 0,1";
const char outNetResp[] = "+CREG: 0,2";
const char messageSendResp[] = "+CMGS: ";

static const char baudReq[] = "AT+IPR=9600";
static const char netReq[] = "AT+CREG?";

volatile unsigned char data_in[30];
volatile unsigned char data_out[30];
volatile unsigned char command_in[30];

unsigned char getCommByte(unsigned int data);
void sendCRLF();

char *getAnswer(){
    unsigned int c = UART_NO_DATA;
	int j=0;
	while(1){
		c = uart_getc();
		if (c != UART_NO_DATA){
			unsigned char answerChar = getCommByte(c);
			if (answerChar == CR && j!=0){
				break;
			} else if (answerChar != LF && answerChar!= CR){
			  data_in[j] = answerChar;
			  j++;
			}
			c = UART_NO_DATA;
		}
	}
	copy_command(j);
	return command_in;	
}

void copy_command(int len)
{
// The USART might interrupt this - don't let that happen!
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		// Copy the contents of data_in into command_in
		memcpy(command_in, data_in, len);
 
		// Now clear data_in, the USART can reuse it now
		memset(data_in[0], 0, len);
	}
}

void simPowerOn(){
	delay(startDelay);
	SetBit(pwrDDR, pwrPin);
	ClrBit(pwrPORT, pwrPin);
	delay(initDelay);
	ClrBit(pwrDDR, pwrPin);
}

unsigned char simCommInit(){
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	delay(commDelay);
	uart_puts(baudReq);
	sendCRLF();
	delay(commDelay);
	//if (strncmp( getAnswer(), successResp, 2 ) != 0){
	  //return FALSE;
	//}
	return TRUE;
}

void waitForReg(){
	//while(1){
		uart_puts(netReq);
		sendCRLF();
		for(int i=0; i<8; i++){
			delay(regDelay);
		}
//		if (strncmp( getAnswer(), "+CREG: 0,2", 10) == 0){
//			break;
//		}	
	//}
}

unsigned char simSetTextInput(){
	uart_puts("AT+CMGF=1");
	sendCRLF();
	delay(commDelay);
//	if (strncmp( getAnswer(), successResp, 2 ) != 0){
//	 return FALSE;
//	}
	return TRUE;
}

void simSetPhone(char *phone){
	data_out[30] = strcat(data_out, "AT+CMGS=\"");
	data_out[30] = strcat(data_out, phone);
	data_out[30] = strcat(data_out, "\"");
	uart_puts(data_out);
	sendCRLF();
	delay(commDelay);
}

void *searchContact(char *contact){
	data_out[30] = strcat(data_out, "AT+CPBF=\"");
	data_out[30] = strcat(data_out, contact);
	data_out[30] = strcat(data_out, "\"");
	uart_puts(data_out);
	sendCRLF();
	delay(commDelay);
	return getAnswer();
}

unsigned char simSendMessage(char *message){
	uart_puts(message);
	sendCRLF();
	delay(commDelay);
	uart_putc(inputEnd);
//	if (strncasecmp( getAnswer(), messageSendResp, 7) != 0){
//	 return FALSE;
//	}
	return TRUE;	
}

void simDialCall(char *phone){
	data_out[30] = strcat(data_out, "ATD + ");
	data_out[30] = strcat(data_out, phone);
	uart_puts(data_out);
	sendCRLF();
	delay(commDelay);
}

unsigned char getCommByte(unsigned int data){
	return (data & 0x00FF);
}

void sendCRLF(){
	uart_putc(CR);
	uart_putc(LF);
}

unsigned char init(){
	simPowerOn();
	unsigned char initFlag =  simCommInit();
	if (initFlag = TRUE){
		waitForReg();
	}else{
		return FALSE;
	}
}

unsigned char sendMessage(const char *phone, const char *message){
	unsigned char setTextFlag = simSetTextInput();
	simSetPhone(phone);
	unsigned char sendMessageFlag = simSendMessage(message);
	return setTextFlag && sendMessageFlag;
}

void dialCall(const char *phone){
	simDialCall(phone);
}

