#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "list.h"
#include "mem/mem.h"
#include "log/log.h"


/*
* function: List_Init 
*
* description: Init List Head Node Structure
*
* input:  @pstListHeadNode: the ListHeadNode Structure pointer
*         @maxsize: the list max size
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
S32	List_Init(struct ListHeadNode *pstListHeadNode, U32 maxsize)
{
	if(NULL == pstListHeadNode)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_INIT, "Error: param = NULL");
		return LIST_ERR;
	}

	if(maxsize <= 0)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_INIT, "Error: maxsize = %u", maxsize);
		return LIST_ERR;
	}

	S32 ret = 0;

	pstListHeadNode->size = 0;
	pstListHeadNode->maxsize = maxsize;
	pstListHeadNode->front = NULL;
	pstListHeadNode->rear  = NULL;

	//init mutex
	ret = pthread_mutex_init(&(pstListHeadNode->mutex), NULL);
	if(0 != ret)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_INIT, "call:pthread_mutex_init failed!");
		return LIST_ERR;
	}

	return SUCCESS;
}


/*
* function: List_Size
*
* description: Get the List Size
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
U32 List_Size(struct ListHeadNode *pstListHeadNode)
{
	if(NULL == pstListHeadNode)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: param = NULL");
		return 0;
	}
	
	return pstListHeadNode->size;
}


/*
* function: List_IsEmpty
*
* description: 
*
* input:  @
*
* output: @
*
* return: 0: Empty, -1 : not empty
*
* author: linsheng.pan
*/
S32 List_IsEmpty(struct ListHeadNode *pstListHeadNode)
{
	if(NULL == pstListHeadNode)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: param = NULL");
		return LIST_ERR;
	}

	S32 ret = 0;

	pthread_mutex_lock(&(pstListHeadNode->mutex));
	ret = (0 == pstListHeadNode->size? 0 : -1); 
	pthread_mutex_unlock(&(pstListHeadNode->mutex));

	return ret;
}


/*
* function: List_IsFull
*
* description: 
*
* input:  @
*
* output: @
*
* return: 0:full, -1: not full
*
* author: linsheng.pan
*/
S32 List_IsFull(struct ListHeadNode *pstListHeadNode)
{
	if(NULL == pstListHeadNode)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: param = NULL");
		return LIST_ERR;
	}

	S32 ret = 0;

	pthread_mutex_lock(&(pstListHeadNode->mutex));
	ret = (pstListHeadNode->size == pstListHeadNode->maxsize ? 0 : -1);
	pthread_mutex_unlock(&(pstListHeadNode->mutex));

	return ret;
}


/*
* function: List_PushBackData
*
* description: Push the data into the end of list
*
* input:  @pstListHeadNode: List head node
*         @data: copy the data pointed to by the data pointer
*         @u32Len: the len of (*data) 
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
S32 List_PushBackData(struct ListHeadNode *pstListHeadNode, void *data, U32 u32Len)
{
	if((NULL == pstListHeadNode) || (NULL == data))
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: param = NULL");
		return LIST_ERR;
	}

	S32 ret = 0;

	ret = List_IsFull(pstListHeadNode);
	if(0 == ret)
	{
		//List is full
		return LIST_ERR;
	}
	
	struct ListNode *pstListNode = NULL;
	pthread_mutex_lock(&(pstListHeadNode->mutex));

	//init struct ListNode
	NEW2(pstListNode, sizeof(struct ListNode));
	pstListNode->u32Len = u32Len;
	NEW2(pstListNode->data, u32Len);
	memcpy(pstListNode->data, data, u32Len);
	pstListNode->pre  = NULL;
	pstListNode->next = NULL;

	//push back in the list
	if(0 == pstListHeadNode->size)
	{
		pstListHeadNode->front = pstListNode;
		pstListHeadNode->rear  = pstListNode;
	}
	else
	{
		pstListNode->pre = pstListHeadNode->rear;
		pstListHeadNode->rear->next = pstListNode;
		pstListHeadNode->rear = pstListNode;
	}
	
	++pstListHeadNode->size;

	pthread_mutex_unlock(&(pstListHeadNode->mutex));
	return SUCCESS;
}

#if 0
S32 List_PushBackNode(struct ListHeadNode *pstListHeadNode, struct ListNode *pstListNode)
{
	assert(pstListHeadNode);
	assert(pstListNode);

	S32 ret = 0;
	ret = List_IsFull(pstListHeadNode);
	if(0 == ret)
	{
		return ret;
	}
	
	pthread_mutex_lock(&(pstListHeadNode->mutex));
	//push back in the list
	//if(0 == pstListHeadNode->size)
	if(0 == pstListHeadNode->capacity)
	{
		pstListHeadNode->front = pstListNode;
		pstListHeadNode->rear  = pstListNode;
	}
	else
	{
		pstListNode->pre = pstListHeadNode->rear;
		pstListNode->next = NULL;
		pstListHeadNode->rear->next = pstListNode;
		pstListHeadNode->rear = pstListNode;
	}

	//++pstListHeadNode->size;
	++pstListHeadNode->capacity;
	pthread_mutex_unlock(&(pstListHeadNode->mutex));

	return ret;
}
#endif

/*
* function: List_PushFrontData
*
* description: Push the data into the end of list
*
* input:  @pstListHeadNode: List head node
*         @data: copy the data pointed to by the data pointer
*         @u32Len: the len of (*data) 
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
S32 List_PushFrontData(struct ListHeadNode *pstListHeadNode, void *data, U32 u32Len)
{
	assert(pstListHeadNode);
	assert(data);

	S32 ret = 0;
	ret = List_IsFull(pstListHeadNode);
	if(0 == ret)
	{
		//List is full
		return LIST_ERR;
	}
	
	struct ListNode *pstListNode;
	pthread_mutex_lock(&(pstListHeadNode->mutex));

	//init struct ListNode
	NEW2(pstListNode, sizeof(struct ListNode));
	pstListNode->u32Len = u32Len;
	NEW2(pstListNode->data, u32Len);
	memcpy(pstListNode->data, data, u32Len);
	pstListNode->pre  = NULL;
	pstListNode->next = NULL;

	//push back in the list
	if(0 == pstListHeadNode->size)
	{
		pstListHeadNode->front = pstListNode;
		pstListHeadNode->rear  = pstListNode;
	}
	else
	{
		pstListNode->next = pstListHeadNode->front;
		pstListHeadNode->front->pre = pstListNode;
		pstListHeadNode->front = pstListNode;
	}
	++pstListHeadNode->size;

	pthread_mutex_unlock(&(pstListHeadNode->mutex));

	return ret;
}

#if 0
S32 List_PushFrontNode(struct ListHeadNode *pstListHeadNode, struct ListNode *pstListNode)
{
	assert(pstListHeadNode);
	assert(pstListNode);

	S32 ret = 0;
	ret = List_IsFull(pstListHeadNode);
	if(0 == ret)
	{
		return ret;
	}
	
	pthread_mutex_lock(&(pstListHeadNode->mutex));
	//push back in the list
	//if(0 == pstListHeadNode->size)
	if(0 == pstListHeadNode->capacity)
	{
		pstListHeadNode->front = pstListNode;
		pstListHeadNode->rear  = pstListNode;
		pstListNode->next 	   = NULL;
		pstListNode->pre 	   = NULL;
	}
	else
	{
		pstListNode->next = pstListHeadNode->front;
		pstListNode->pre = NULL;
		pstListHeadNode->front->pre = pstListNode;
		pstListHeadNode->front = pstListNode;
	}

	//++pstListHeadNode->size;
	++pstListHeadNode->capacity;

	pthread_mutex_unlock(&(pstListHeadNode->mutex));

	return ret;
}
#endif


S32 List_FreeNode(struct ListNode *pstListNode)
{

	if(NULL == pstListNode)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: param = NULL");
		return LIST_ERR;
	}

	FREE(pstListNode->data);
	FREE(pstListNode);
	
	return SUCCESS;
}


struct ListNode * List_PopBack(struct ListHeadNode *pstListHeadNode)
{
	if(NULL == pstListHeadNode)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: param = NULL");
		return NULL;
	}

	S32 ret = 0;
	struct ListNode *pstListNode = NULL;
	
	ret = List_IsEmpty(pstListHeadNode);
	if(0 == ret)
	{
		return NULL;
	}

	pthread_mutex_lock(&(pstListHeadNode->mutex));

	pstListNode = pstListHeadNode->rear;
	pstListHeadNode->rear = pstListHeadNode->rear->pre;

	--pstListHeadNode->size;

	pstListNode->pre = NULL;
	pstListNode->next = NULL;

	pthread_mutex_unlock(&(pstListHeadNode->mutex));

	return pstListNode;
}


struct ListNode * List_PopFront(struct ListHeadNode *pstListHeadNode)
{
	if(NULL == pstListHeadNode)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: param = NULL");
		return NULL;
	}

	S32 ret = 0;
	struct ListNode *pstListNode = NULL;
	
	ret = List_IsEmpty(pstListHeadNode);
	if(0 == ret)
	{
		return NULL;
	}

	pthread_mutex_lock(&(pstListHeadNode->mutex));

	pstListNode = pstListHeadNode->front;
	pstListHeadNode->front = pstListHeadNode->front->next;

	--pstListHeadNode->size;

	pstListNode->pre = NULL;
	pstListNode->next = NULL;
	pthread_mutex_unlock(&(pstListHeadNode->mutex));

	return pstListNode;
}


S32 List_GetBack(struct ListHeadNode *pstListHeadNode, void *data, U32 *len)
{
	if((NULL == pstListHeadNode) || (NULL == data) || (NULL == len))
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: param = NULL");
		return LIST_ERR;
	}

	S32 ret = 0;
	ret = List_IsEmpty(pstListHeadNode);
	if(0 == ret)
	{
		return ret;
	}

	pthread_mutex_lock(&(pstListHeadNode->mutex));
	
	memcpy(data, pstListHeadNode->rear->data, pstListHeadNode->rear->u32Len);
	*len = pstListHeadNode->rear->u32Len;

	pthread_mutex_unlock(&(pstListHeadNode->mutex));

	return SUCCESS;
}


S32 List_GetFront(struct ListHeadNode *pstListHeadNode, void *data, U32 *len)
{
	if((NULL == pstListHeadNode) || (NULL == data) || (NULL == len))
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: param = NULL");
		return LIST_ERR;
	}

	S32 ret = 0;
	ret = List_IsEmpty(pstListHeadNode);
	if(0 == ret)
	{
		return LIST_ERR;
	}

	pthread_mutex_lock(&(pstListHeadNode->mutex));
	
	memcpy(data, pstListHeadNode->front->data, pstListHeadNode->front->u32Len);
	*len = pstListHeadNode->front->u32Len;

	pthread_mutex_unlock(&(pstListHeadNode->mutex));

	return SUCCESS;

}


S32 List_Clear(struct ListHeadNode *pstListHeadNode)
{
	if(NULL == pstListHeadNode)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: param == NULL");
		return LIST_ERR;
	}

	struct ListNode *pstListNode = NULL;
	
	pthread_mutex_lock(&(pstListHeadNode->mutex));
	
	while(pstListHeadNode->size > 0)
	{
		//pop list
		pstListNode = pstListHeadNode->front;
		pstListHeadNode->front = pstListHeadNode->front->next;
		--pstListHeadNode->size;

		//free list node
		FREE(pstListNode->data);
		FREE(pstListNode);
	}

	pthread_mutex_unlock(&(pstListHeadNode->mutex));

	return SUCCESS;
}


S32 List_Destroy(struct ListHeadNode *pstListHeadNode)
{
	if(NULL == pstListHeadNode)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: param == NULL");
		return LIST_ERR;
	}

	List_Clear(pstListHeadNode);

	pstListHeadNode->maxsize    = 0;
	pstListHeadNode->front 		= NULL;
	pstListHeadNode->rear 		= NULL;

	pthread_mutex_destroy(&(pstListHeadNode->mutex));

	return SUCCESS;
}


