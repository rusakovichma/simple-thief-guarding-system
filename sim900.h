#define startDelay 3000
#define initDelay 3000
#define uartDelay 600
#define commDelay 500
#define regDelay 3000
#define inputEnd 0x1A

#define CR 0x0D
#define LF 0x0A

#define UART_BAUD_RATE      9600  

#define pwrPORT PORTC
#define pwrDDR DDRC
#define pwrPin 6 

extern unsigned char init();

extern unsigned char sendMessage(const char *phone, const char *message);

extern void dialCall(const char *phone);

extern void *searchContact(char *contact);

