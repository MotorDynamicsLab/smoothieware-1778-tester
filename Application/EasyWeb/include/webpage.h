#ifndef __WEB_PAGE_H__
#define __WEB_PAGE_H__

const unsigned char ROMWebSide[] = 
{
	"<html>\r\n"
	"<head>\r\n"
	"<meta http-equiv=\"refresh\" content=\"5\">\r\n"
	"<title>easyWEB - dynamic Webside</title>\r\n"
	"</head>\r\n"
	"\r\n"
	"<body bgcolor=\"#3030A0\" text=\"#FFFF00\">\r\n"
	"<p><b><font color=\"#FFFFFF\" size=\"6\"><i>Hello World!</i></font></b></p>\r\n"
	"\r\n"
	"<p><b>This is a dynamic website hosted by the embedded Webserver</b> <b>easyWEB.</b></p>\r\n"
	"<p><b>Hardware:</b></p>\r\n"
	"<ul>\r\n"
	"<li><b>Current Board: LDO Pulse Controller base on LPC1778</b></li>\r\n"
	"<li><b>Using IC: LPC1778 (ARM Cortex-M3) with 120MHz, 512KB Flash, 96 KB SRAM</b></li>\r\n"
	"<li><b>Embedded EMAC Ethernet Controller</b></li>\r\n"
	"</ul>\r\n"
	"\r\n"
	"</body>\r\n"
	"</html>\r\n"
	"\r\n"
};

unsigned char SDWebSide[2000] = { 0 };

#endif
