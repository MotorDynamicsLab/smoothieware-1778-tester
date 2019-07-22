/******************************************************************
 *****                                                        *****
 *****  Name: easyweb.h                                       *****
 *****  Ver.: 1.0                                             *****
 *****  Date: 07/05/2001                                      *****
 *****  Auth: Andreas Dannenberg                              *****
 *****        HTWK Leipzig                                    *****
 *****        university of applied sciences                  *****
 *****        Germany                                         *****
 *****  Func: header-file for easyweb.c                       *****
 *****                                                        *****
 ******************************************************************/

#ifndef __EASYWEB_H
#define __EASYWEB_H
#include "lpc_types.h"

void InitOsc(void);                        // prototypes
void InitPorts(void);
void HTTPServer(void);

void Easy_Web_Init(void);
void Easy_Web_Execute(void);

#endif

