#include <stdio.h>
#include <stdlib.h>

#include "queue.h" 

int Que_create(QueHndl *hndl, uint32_t maxLen)
{
  pthread_mutexattr_t mutex_attr;
  pthread_condattr_t cond_attr;
  int status=QUE_OK;

  hndl->curRd = hndl->curWr = 0;
  hndl->count = 0;
  hndl->len   = maxLen;
  hndl->queue = malloc(sizeof(int32_t)*hndl->len);
  
  if(hndl->queue==NULL) {
    printf("Que_Create() = %d \r\n", status);
    return -1;
  }
 
  status |= pthread_mutexattr_init(&mutex_attr);
  status |= pthread_condattr_init(&cond_attr);  
  
  status |= pthread_mutex_init(&hndl->lock, &mutex_attr);
  status |= pthread_cond_init(&hndl->condRd, &cond_attr);    
  status |= pthread_cond_init(&hndl->condWr, &cond_attr);  

  if(status!=QUE_OK)
    printf("Que_Create() = %d \r\n", status);
    
  pthread_condattr_destroy(&cond_attr);
  pthread_mutexattr_destroy(&mutex_attr);
    
  return status;
}

int Que_delete(QueHndl *hndl)
{
  if(hndl->queue!=NULL)
    free(hndl->queue);
    
  pthread_cond_destroy(&hndl->condRd);
  pthread_cond_destroy(&hndl->condWr);
  pthread_mutex_destroy(&hndl->lock);  
  
  return 0;
}



int Que_put(QueHndl *hndl, int32_t value, uint32_t timeout)
{
  int status = QUE_ERR;

  pthread_mutex_lock(&hndl->lock);

  while(1) {
    if( hndl->count < hndl->len ) {
      hndl->queue[hndl->curWr] = value;
      hndl->curWr = (hndl->curWr+1)%hndl->len;
      hndl->count++;
      status = QUE_OK;
      pthread_cond_signal(&hndl->condRd);
      break;
    } else {
      if(timeout == QUE_NO_WAIT)
        break;

      status = pthread_cond_wait(&hndl->condWr, &hndl->lock);
    }
  }

  pthread_mutex_unlock(&hndl->lock);

  return status;
}


int Que_get(QueHndl *hndl, int32_t *value, uint32_t timeout)
{
  int status = QUE_ERR;
  
  pthread_mutex_lock(&hndl->lock);
  
  while(1) {
    if(hndl->count > 0 ) {

      if(value!=NULL) {
        *value = hndl->queue[hndl->curRd];
      }
      
      hndl->curRd = (hndl->curRd+1)%hndl->len;
      hndl->count--;
      status = QUE_OK;
      pthread_cond_signal(&hndl->condWr);
      break;
    } else {
      if(timeout == QUE_NO_WAIT)
        break;

      status = pthread_cond_wait(&hndl->condRd, &hndl->lock);
    }
  }

  pthread_mutex_unlock(&hndl->lock);

  return status;
}


uint32_t Que_getQueuedCount(QueHndl *hndl)
{
  uint32_t queuedCount = 0;

  pthread_mutex_lock(&hndl->lock);
  queuedCount = hndl->count;
  pthread_mutex_unlock(&hndl->lock);
  return queuedCount;
}

int Que_peek(QueHndl *hndl, int32_t *value)
{
  int status = QUE_ERR;
  pthread_mutex_lock(&hndl->lock);
  if(hndl->count > 0 ) {
      if(value!=NULL) {
        *value = hndl->queue[hndl->curRd];
        status = QUE_OK;
      }
  }
  pthread_mutex_unlock(&hndl->lock);

  return status;
}

bool Que_isEmpty(QueHndl *hndl)
{
  bool isEmpty;

  pthread_mutex_lock(&hndl->lock);
  if (hndl->count == 0)
  {
      isEmpty = true;
  }
  else
  {
      isEmpty = false;
  }
  pthread_mutex_unlock(&hndl->lock);

  return isEmpty;
}
