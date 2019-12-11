#ifndef _SERVERACTION_H_
#define _SERVERACTION_H_

#include "cmsis_os.h"

void ServerThreadFunc();
void SetServerThreadID(osThreadId threadID);

#endif