#include <string.h>
#include <stdio.h>

#include "buffer.h" 

int Buf_delete(BufHndl *hndl)
{
  int status=BUF_OK;

  if(hndl==NULL)
    return BUF_ERR;

  status = Que_delete(&hndl->emptyQue);
  status |= Que_delete(&hndl->fullQue);

  return status;
}

int Buf_create(BufHndl *hndl, BufCreate *bufInit)
{
  int status = BUF_OK;
  int i;

  if(hndl==NULL || bufInit==NULL)
    return BUF_ERR;

  if(  bufInit->numBuf >  BUF_NUM_MAX )
    return BUF_ERR;

  memset(hndl, 0, sizeof(BufHndl));

  status = Que_create(&hndl->emptyQue, bufInit->numBuf);

  if(status!=QUE_OK) {
    printf("Buf_create() = %d \r\n", status);
    return status;
  }

  status = Que_create(&hndl->fullQue, bufInit->numBuf);

  if(status!=QUE_OK) {
    Que_delete(&hndl->emptyQue);
    printf("Buf_create() = %d \r\n", status);
    return status;
  }

  hndl->numBuf   = bufInit->numBuf;

  for(i=0; i<hndl->numBuf; i++) {
    hndl->bufInfo[i].size = 0;
    hndl->bufInfo[i].flags = 0;
    hndl->bufInfo[i].timestamp = 0;
    hndl->bufInfo[i].physAddr = bufInit->bufPhysAddr[i];
    hndl->bufInfo[i].virtAddr = bufInit->bufVirtAddr[i];
    Que_put(&hndl->emptyQue, i, QUE_WAIT_FOREVER);
  }

  return status;
}

int Buf_getEmpty(BufHndl *hndl, int *bufId, uint32_t timeout)
{
  int status;

  if(hndl==NULL || bufId==NULL)
    return BUF_ERR;

  status = Que_get(&hndl->emptyQue, bufId, timeout);

  if(status!=QUE_OK) {
    *bufId = BUF_ID_INVALID;
  }

  return status;
}

int Buf_putFull (BufHndl *hndl, int bufId)
{
  int status;

  if(hndl==NULL)
    return BUF_ERR;

  if(bufId >= hndl->numBuf || bufId < 0)
    return BUF_ERR;

  status = Que_put(&hndl->fullQue, bufId, QUE_WAIT_FOREVER);

  return status;
}

int Buf_getFull(BufHndl *hndl, int *bufId, uint32_t timeout)
{
  int status;

  if(hndl==NULL || bufId==NULL)
    return BUF_ERR;

  status = Que_get(&hndl->fullQue, bufId, timeout);

  if(status!=QUE_OK) {
    *bufId = BUF_ID_INVALID;
  }

  return status;
}

int Buf_putEmpty(BufHndl *hndl, int bufId)
{
  int status;

  if(hndl==NULL)
    return BUF_ERR;

  if(bufId >= hndl->numBuf || bufId < 0)
    return BUF_ERR;

  status = Que_put(&hndl->emptyQue, bufId, QUE_WAIT_FOREVER);

  return status;
}


int Buf_switchFull(BufHndl *hndl, int *bufId)
{
  int status;
  int newBufId;

  status = Buf_getEmpty(hndl, &newBufId, QUE_NO_WAIT);

  if(status==BUF_OK) {

    if(*bufId!=BUF_ID_INVALID) {

      Buf_putFull(hndl, *bufId);
    }

    *bufId = newBufId;
  }

  return status;
}

int Buf_switchEmpty(BufHndl *hndl, int *bufId)
{
  int status;
  int newBufId;

  status = Buf_getFull(hndl, &newBufId, QUE_NO_WAIT);

  if(status==BUF_OK) {

    if(*bufId!=BUF_ID_INVALID) {

      Buf_putEmpty(hndl, *bufId);
    }

    *bufId = newBufId;
  }

  return status;
}

BufInfo *Buf_getBufInfo(BufHndl *hndl, int bufId)
{
  if(hndl==NULL)
    return NULL;

  if(bufId>=hndl->numBuf)
    return NULL;

  return &hndl->bufInfo[bufId];
}


