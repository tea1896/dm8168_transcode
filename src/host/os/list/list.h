#ifndef _LIST_INCLUDE

#define _LIST_INCLUDE

#include <pthread.h>

#include "appGlobal.h"

#define LIST_SUCCESS          (0)
#define LIST_ERR              (-1)

struct ListNode
{
	void 	*data;
	U32		u32Len;
	struct ListNode *pre;
	struct ListNode *next;
};

struct ListHeadNode
{
	U32 	size;
	//U32 	capacity;
	U32 	maxsize;
	struct ListNode *front;
	struct ListNode *rear;
	pthread_mutex_t		mutex;
};


/*
* function: List_Init
*
* description: Init List
*
* input:  @pstListHeadNode: the list header
*         @maxsize: the list max size
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
S32	List_Init(struct ListHeadNode *pstListHeadNode, U32 maxsize);


/*
* function: List_Size
*
* description: Get the list size
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
U32 List_Size(struct ListHeadNode *pstListHeadNode);


/*
* function: List_IsEmpty
*
* description: 
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
S32 List_IsEmpty(struct ListHeadNode *pstListHeadNode);


/*
* function: List_IsFull
*
* description: 
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
S32 List_IsFull(struct ListHeadNode *pstListHeadNode);


/*
* function: List_PushBackData
*
* description: push the data into the end of the list
*
* input:  @pstListHeadNode:
*         @data: the data pointer
*         @u32Len: the data size
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
S32 List_PushBackData(struct ListHeadNode *pstListHeadNode, void *data, U32 u32Len);
//S32 List_PushBackNode(struct ListHeadNode *pstListHeadNode, struct ListNode *pstListNode);


/*
* function: List_PushFrontData
*
* description: push the data into the head of the list
*
* input:  @pstListHeadNode:
*         @data: the data pointer
*         @u32Len: the data size
*
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
S32 List_PushFrontData(struct ListHeadNode *pstListHeadNode, void *data, U32 u32Len);
//S32 List_PushFrontNode(struct ListHeadNode *pstListHeadNode, struct ListNode *pstListNode);


/*
* function: List_FreeNode
*
* description: Free the list node
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
S32 List_FreeNode(struct ListNode *pstListNode);


/*
* function: List_PopBack
*
* description: 
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
struct ListNode *List_PopBack(struct ListHeadNode *pstListHeadNode);


/*
* function: List_PopFront
*
* description: 
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
struct ListNode *List_PopFront(struct ListHeadNode *pstListHeadNode);


/*
* function: List_GetBack
*
* description: 
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
S32 List_GetBack(struct ListHeadNode *pstListHeadNode, void *data, U32 *len);


/*
* function: List_GetFront
*
* description: 
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
S32 List_GetFront(struct ListHeadNode *pstListHeadNode, void *data, U32 *len);


/*
* function: List_Clear
*
* description: 
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
S32 List_Clear(struct ListHeadNode *pstListHeadNode);


/*
* function: List_Destroy
*
* description: 
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
S32 List_Destroy(struct ListHeadNode *pstListHeadNode);

#endif
