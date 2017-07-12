#ifndef _EVENT_INCLUDE
#define _EVENT_INCLUDE

#include "appGlobal.h"

#define EVENT_SUCCESS        (0)
#define EVENT_ERR            (-1)

S32 Event_CreateFd(void);
S32 Event_Read(S32 fd);
S32 Event_Write(S32 fd);

#endif
