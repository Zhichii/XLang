#ifndef X_BASIC
#define X_BASIC

#include <malloc.h>
#include <stdio.h>
#include <string.h>

typedef long long x_sint;
typedef unsigned long long x_uint;
typedef char x_schr;
typedef unsigned char x_byte;
typedef wchar_t x_char; // Do not be confused with x_schr and x_byte. 
typedef enum x_bool x_bool;
enum x_bool {
	false = 0,
	true = 1
};
typedef enum x_stat x_stat;
enum x_stat {
	X_STAT_OK,
	X_STAT_ALREADY,
	X_STAT_ERROR,
	X_STAT_NO_MEM,
	X_STAT_OVERFLOW
};
x_bool x_stat_pass(x_stat stat) {
	if (stat == X_STAT_ERROR) return false;
	if (stat == X_STAT_NO_MEM) return false;
	return true;
}

void* x_mem_new(x_uint size) {
	void* block = malloc(size);
	if (block) memset(block, 0, size);
	return block;
}
//inline void x_mem_del(void* ptr) { free(ptr); }

#define new(TYPE) x_mem_new(sizeof(TYPE))
#define newlist(TYPE,CNT) x_mem_new(sizeof(TYPE)*CNT)
#define del(PTR) free(PTR)

#ifdef X_STAT_CHK_DBG
#define x_check_stat_voidret(STAT,RET) if (!x_stat_pass(STAT)) {  __debugbreak(); return; }
#define x_check_stat_do(STAT, DO) if (!x_stat_pass(STAT)) { __debugbreak(); DO return STAT; }
#define x_check_stat(STAT) if (!x_stat_pass(STAT)) { __debugbreak(); return STAT; }
#else
#define x_check_stat_voidret(STAT,RET) if (!x_stat_pass(STAT)) return;
#define x_check_stat_do(STAT, DO) if (!x_stat_pass(STAT)) { DO return STAT; }
#define x_check_stat(STAT) if (!x_stat_pass(STAT)) return STAT;
#endif

typedef struct x_node x_node;
struct x_node {
	x_node* prev;
	void* value;
	x_node* next;
};
typedef struct x_li x_li;
struct x_li {
	x_node* head;
	x_node* tail;
	x_uint size;
};
typedef x_stat x_li_del_func(void*, void*);
static x_stat x_li_new(x_li** pthis) {
	if (!pthis) return X_STAT_ERROR;
	x_li* this = *pthis; 
	if (this) return X_STAT_ERROR;
	this = new(x_li);
	if (!this) return X_STAT_NO_MEM;
	this->head = NULL;
	this->tail = NULL;
	this->size = 0;
	*pthis = this;
	return X_STAT_OK;
}
static x_stat x_li_add_tail(x_li** pthis, void* value) {
	if (!pthis) return X_STAT_ERROR;
	x_li* this = *pthis;
	if (!this) {
		x_stat stat = x_li_new(pthis);
		x_check_stat(stat);
	}
	x_node* node = new(x_node);
	if (!node) return X_STAT_NO_MEM;
	node->value = value;
	node->next = NULL;
	if (!this->tail) this->head = node;
	else this->tail->next = node;
	node->prev = this->tail;
	this->tail = node;
	this->size++;
	return X_STAT_OK;
}
static x_stat x_li_add_head(x_li** pthis, void* value) {
	if (!pthis) return X_STAT_ERROR;
	x_li* this = *pthis;
	if (!this) {
		x_stat stat = x_li_new(pthis);
		x_check_stat(stat);
	}
	x_node* node = new(x_node);
	if (!node) return X_STAT_NO_MEM;
	node->value = value;
	node->prev = NULL;
	if (!this->head) this->tail = node;
	else this->head->prev = node;
	node->next = this->head;
	this->head = node;
	this->size++;
	return X_STAT_OK;
}
static x_stat x_li_pop_tail(x_li** pthis, void** value_ptr) {
	if (!pthis) return X_STAT_ERROR;
	x_li* this = *pthis;
	if (!this) return X_STAT_ALREADY;
	if (!this->tail) return X_STAT_ALREADY;
	else {
		if (value_ptr) *value_ptr = this->tail->value;
		x_node* prev = this->tail->prev;
		if (this->tail == this->head) this->head = NULL;
		del(this->tail);
		this->tail = prev;
		if (this->tail) this->tail->next = NULL;
	}
	this->size--;
	return X_STAT_OK;
}
static x_stat x_li_pop_head(x_li** pthis, void** value_ptr) {
	if (!pthis) return X_STAT_ERROR;
	x_li* this = *pthis;
	if (!this) return X_STAT_ALREADY;
	if (!this->head) return X_STAT_ALREADY;
	else {
		if (value_ptr) *value_ptr = this->head->value;
		x_node* next = this->head->next;
		if (this->head == this->tail) this->tail = NULL;
		del(this->head);
		this->head = next;
		if (this->head) this->head->prev = NULL;
	}
	this->size--;
	return X_STAT_OK;
}
static x_stat x_li_del(x_li** pthis, x_li_del_func* del_func, void* client_data) {
	if (!pthis) return X_STAT_ERROR;
	x_li* this = *pthis;
	if (!this) return X_STAT_ALREADY;
	x_node* cur_next = NULL;
	for (x_node* cur = this->head; cur != NULL; cur = cur_next) {
		cur_next = cur->next;
		if (del_func) del_func(&cur->value, client_data);
		del(cur);
	}
	del(this);
	*pthis = NULL;
	return X_STAT_OK;
}

#endif