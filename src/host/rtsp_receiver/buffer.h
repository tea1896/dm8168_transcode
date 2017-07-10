#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdint.h>
#include <stdbool.h>
#include "queue.h" 

#define BUF_NO_WAIT       0
#define BUF_WAIT_FOREVER -1

#define BUF_OK    0
#define BUF_ERR  -1 

#define BUF_NUM_MAX       (100)

#define BUF_ID_INVALID    (-1)

typedef struct {

  int 		size;
  int 		count;
  int 		flags;
  uint32_t	timestamp;
  int 		width;
  int 		height;
  uint32_t 	isKeyFrame;
  uint16_t 	codecType;
  void 		*physAddr;
  void 		*virtAddr;

} BufInfo;

typedef struct {

  BufInfo bufInfo[BUF_NUM_MAX];
  
  QueHndl emptyQue;
  QueHndl fullQue;
      
  int numBuf;

} BufHndl;

typedef struct {

  void *bufPhysAddr[BUF_NUM_MAX];
  void *bufVirtAddr[BUF_NUM_MAX];  
  
  int numBuf;

} BufCreate;

#ifdef __cplusplus
extern "C" {
#endif

int  Buf_create(BufHndl *hndl, BufCreate *bufInit);
int  Buf_delete(BufHndl *hndl);

int  Buf_getFull(BufHndl *hndl, int *bufId, uint32_t timeout);
int  Buf_putEmpty(BufHndl *hndl, int bufId);

int  Buf_getEmpty(BufHndl *hndl, int *bufId, uint32_t timeout);
int  Buf_putFull(BufHndl *hndl, int bufId);

int  Buf_switchFull (BufHndl *hndl, int *bufId);
int  Buf_switchEmpty(BufHndl *hndl, int *bufId);

BufInfo *Buf_getBufInfo(BufHndl *hndl, int bufId);

#ifdef __cplusplus
}
#endif

#endif 



