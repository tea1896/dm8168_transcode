#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

#define QUE_NO_WAIT       0
#define QUE_WAIT_FOREVER -1

#define QUE_OK    0
#define QUE_ERR  -1 

typedef struct {

  uint32_t curRd;
  uint32_t curWr;
  uint32_t len;
  uint32_t count;

  int32_t *queue;

  pthread_mutex_t lock;
  pthread_cond_t  condRd;
  pthread_cond_t  condWr;
  
} QueHndl;

#ifdef __cplusplus
extern "C" {
#endif

int Que_create(QueHndl *hndl, uint32_t maxLen);
int Que_delete(QueHndl *hndl);
int Que_put(QueHndl *hndl, int32_t  value, uint32_t timeout);
int Que_get(QueHndl *hndl, int32_t *value, uint32_t timeout);
int Que_peek(QueHndl *hndl, int32_t *value);
uint32_t Que_getQueuedCount(QueHndl *hndl);
bool Que_isEmpty(QueHndl *hndl);

#ifdef __cplusplus
}
#endif

#endif 



