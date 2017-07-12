#ifndef _INCLUDE_MEM
#define _INCLUDE_MEM

#include "appGlobal.h"

void *Mem_malloc(U32 u32Len, const char *filename, U32 u32Line);
void Mem_free(void *ptr, const char *filename, U32 u32Line);
void *Mem_resize(void *ptr, U32 u32Len, const char *filename, U32 u32Line);


#define NEW2(ptr, u32Len) ((ptr) = Mem_malloc(u32Len, __FILE__, __LINE__))
#define MALLOC(u32Len) Mem_malloc(u32Len, __FILE__, __LINE__)
#define RESIZE(ptr, u32Len) (Mem_resize(ptr, u32Len, __FILE__, __LINE__))

#define FREE(ptr) (Mem_free((ptr), __FILE__, __LINE__), (ptr) = 0)

#define SUCCESS 	(0)
#define MEM_ERROR   (-1)

#endif
