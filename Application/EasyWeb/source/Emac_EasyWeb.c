/******************************************************************
 *****                                                        *****
 *****  Name: Emac_EsayWeb.c                                  *****
 *****  Ver.: 1.0                                             *****
 *****  Date: 07/05/2001                                      *****
 *****  Auth: Andreas Dannenberg                              *****
 *****        HTWK Leipzig                                    *****
 *****        university of applied sciences                  *****
 *****        Germany                                         *****
 *****  Func: implements a dynamic HTTP-server by using       *****
 *****        the easyWEB-API                                 *****
 *****                                                        *****
 ******************************************************************/
#if defined ( __CC_ARM   ) /*------------------RealView Compiler -----------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif
#define extern            // Keil: Line added for modular project management

#include "stdio.h"
#include "string.h"

#include "easyweb.h"
#include "webpage.h"

#include "EMAC.h"
#include "tcpip.h"
#include "LPC177x_8x.h"
#include "lpc177x_8x_timer.h"
#include "lpc177x_8x_pinsel.h"

#include "SDCard.h"
#include "ff.h"

/** @defgroup EMAC_EasyWeb  EMAC Easy Web
 * @ingroup EMAC_Examples
 * @{
 */



/*********************************************************************//**
 * @brief       Define vars
 * @param[in]   None
 * @return      None
 **********************************************************************/
const uint8_t GetResponse[] =              // 1st thing our server sends to a client
{
  "HTTP/1.0 200 OK\r\n"                    // protocol ver 1.0, code 200, reason OK
  "Content-Type: text/html\r\n"            // type of data we want to send
  "\r\n"                                   // indicate end of HTTP-header
};

uint8_t *PWebSide;                         // pointer to webside
unsigned int HTTPBytesToSend;              // bytes left to send
uint8_t HTTPStatus;                        // status byte
#define HTTP_SEND_PAGE         0x01        // help flag

/*********************************************************************//**
 * @brief       Initialization for timer
 * @param[in]   None
 * @return      None
 **********************************************************************/
void TC_Init(void)
{
    TIM_TIMERCFG_Type TIM_ConfigStruct;
    TIM_MATCHCFG_Type TIM_MatchConfigStruct ;
    // Initialize timer 0, prescale count time of 1ms
    TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
    TIM_ConfigStruct.PrescaleValue  = 1000;

    // use channel 0, MR0
    TIM_MatchConfigStruct.MatchChannel = 0;
    // Enable interrupt when MR0 matches the value in TC register
    TIM_MatchConfigStruct.IntOnMatch   = TRUE;
    //Enable reset on MR0: TIMER will reset if MR0 matches it
    TIM_MatchConfigStruct.ResetOnMatch = TRUE;
    //Stop on MR0 if MR0 matches it
    TIM_MatchConfigStruct.StopOnMatch  = FALSE;
    //Toggle MR0.0 pin if MR0 matches it
    TIM_MatchConfigStruct.ExtMatchOutputType =TIM_EXTMATCH_TOGGLE;
    // Set Match value, count value of 262mS
    TIM_MatchConfigStruct.MatchValue   = 262;

    // Set configuration for Tim_config and Tim_MatchConfig
    TIM_Init(LPC_TIM0, TIM_TIMER_MODE,&TIM_ConfigStruct);
    TIM_ConfigMatch(LPC_TIM0,&TIM_MatchConfigStruct);

    /* preemption = 1, sub-priority = 1 */
    NVIC_SetPriority(TIMER0_IRQn, ((0x01<<3)|0x01));
    /* Enable interrupt for timer 0 */
    NVIC_EnableIRQ(TIMER0_IRQn);
    // To start timer 0
    TIM_Cmd(LPC_TIM0,ENABLE);
}

/*********************************************************************//**
 * @brief       Easy web initialize
 * @param[in]   None
 * @return      None
 **********************************************************************/
void Easy_Web_Init(void)
{
    TC_Init();

    TCPLowLevelInit();
    
    HTTPStatus = 0;                             // clear HTTP-server's flag register

    TCPLocalPort = TCP_PORT_HTTP;               // set port we want to listen to
                                                                    
}

/*********************************************************************//**
 * @brief       Easy web execute
 * @param[in]   None
 * @return      None
 **********************************************************************/
void Easy_Web_Execute(void)
{
	// listen for incoming TCP-connection
	if (!(SocketStatus & SOCK_ACTIVE))
		TCPPassiveOpen();

	// handle network and easyWEB-stack			
	DoNetworkStuff();
	
	// events
	HTTPServer();
}

/*********************************************************************//**
 * @brief       Easy web execute
 * @param[in]   None
 * @return      result
 **********************************************************************/
uint8_t EasyWeb_Read_WebPage(void)
{
	if (SDCard_disk_initialize() == 0)
	{
		FATFS fat;
		FIL file;

		f_mount(0, &fat);
		int r = f_open(&file, "index.html", FA_READ);
		if (r == FR_OK)
		{
			//get HTML length, ignore trailing zero
			HTTPBytesToSend = file.fsize;
			//malloc webside
			PWebSide = (unsigned char *)malloc(file.fsize);
			//read webside
			unsigned int r = file.fsize;
			f_read(&file, PWebSide, file.fsize, &r);
			f_close(&file);
			return 1;
		}
	}
	return 0;
}

// This function implements a very simple dynamic HTTP-server.
// It waits until connected, then sends a HTTP-header and the
// HTML-code stored in memory. Before sending, it replaces
// some special strings with dynamic values.
// NOTE: For strings crossing page boundaries, replacing will
// not work. In this case, simply add some extra lines
// (e.g. CR and LFs) to the HTML-code.
/*********************************************************************//**
 * @brief       HTTP Server setup
 * @param[in]   None
 * @return      None
 **********************************************************************/
void HTTPServer(void)
{
  if (SocketStatus & SOCK_CONNECTED)             // check if somebody has connected to our TCP
  {
    if (SocketStatus & SOCK_DATA_AVAILABLE)      // check if remote TCP sent data
      TCPReleaseRxBuffer();                      // and throw it away

    if (SocketStatus & SOCK_TX_BUF_RELEASED)     // check if buffer is free for TX
    {
      if (!(HTTPStatus & HTTP_SEND_PAGE))        // init byte-counter and pointer to webside
      {                                          // if called the 1st time
		  if (EasyWeb_Read_WebPage() == 0)
		  {
			  HTTPBytesToSend = sizeof(WebSide) - 1; // get HTML length, ignore trailing zero
			  PWebSide = (uint8_t *)WebSide;         // pointer to HTML-code
		  }
      }

      if (HTTPBytesToSend > MAX_TCP_TX_DATA_SIZE)     // transmit a segment of MAX_SIZE
      {
        if (!(HTTPStatus & HTTP_SEND_PAGE))           // 1st time, include HTTP-header
        {
          memcpy(TCP_TX_BUF, GetResponse, sizeof(GetResponse) - 1);
          memcpy(TCP_TX_BUF + sizeof(GetResponse) - 1, PWebSide, MAX_TCP_TX_DATA_SIZE - sizeof(GetResponse) + 1);
          HTTPBytesToSend -= MAX_TCP_TX_DATA_SIZE - sizeof(GetResponse) + 1;
          PWebSide += MAX_TCP_TX_DATA_SIZE - sizeof(GetResponse) + 1;
        }
        else
        {
          memcpy(TCP_TX_BUF, PWebSide, MAX_TCP_TX_DATA_SIZE);
          HTTPBytesToSend -= MAX_TCP_TX_DATA_SIZE;
          PWebSide += MAX_TCP_TX_DATA_SIZE;
        }

        TCPTxDataCount = MAX_TCP_TX_DATA_SIZE;   // bytes to xfer
        InsertDynamicValues();                   // exchange some strings...
        TCPTransmitTxBuffer();                   // xfer buffer
      }
      else if (HTTPBytesToSend)                  // transmit leftover bytes
      {
        memcpy(TCP_TX_BUF, PWebSide, HTTPBytesToSend);
        TCPTxDataCount = HTTPBytesToSend;        // bytes to xfer
        InsertDynamicValues();                   // exchange some strings...
        TCPTransmitTxBuffer();                   // send last segment
        TCPClose();                              // and close connection
        HTTPBytesToSend = 0;                     // all data sent
      }

      HTTPStatus |= HTTP_SEND_PAGE;              // ok, 1st loop executed
    }
  }
  else
    HTTPStatus &= ~HTTP_SEND_PAGE;               // reset help-flag if not connected
}


// searches the TX-buffer for special strings and replaces them
// with dynamic values (AD-converter results)
/*********************************************************************//**
 * @brief       Insert Dynamic Value
 * @param[in]   None
 * @return      None
 **********************************************************************/
void InsertDynamicValues(void)
{
    uint8_t *Key;
    char NewKey[5];
  unsigned int i;

  if (TCPTxDataCount < 4) return;                     // there can't be any special string

  Key = TCP_TX_BUF;

  for (i = 0; i < (TCPTxDataCount - 3); i++)
  {
    if (*Key == 'A')
     if (*(Key + 1) == 'D')
       if (*(Key + 3) == '%')
         switch (*(Key + 2))
         {
           case '7' :                                 // "AD7%"?
           {
             sprintf(NewKey, "%3u", 1024);            // insert AD converter value
             memcpy(Key, NewKey, 3);                  // channel 7 (P6.7)
             break;
           }
           case 'A' :                                 // "ADA%"?
           {
             sprintf(NewKey, "%3u", 1024);            // insert AD converter value
             memcpy(Key, NewKey, 3);                  // channel 10 (temp.-diode)
             break;
           }
         }
    Key++;
  }
}

/**
 * @}
*/
