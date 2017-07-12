/*
 * =====================================================================================
 *
 *       Filename:  mutex.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/11/17 09:10:20
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef _INCLUDE_MUTEX
#define _INCLUDE_MUTEX

#define MUTEX_ERR       (-1)

#include "appGlobal.h"

S32  Mutex_Init(void);
S32  Mutex_Lock(void);
S32  Mutex_Unlock(void);
S32  Mutex_Destroy(void);

#endif
