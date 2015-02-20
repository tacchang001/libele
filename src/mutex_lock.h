#pragma once

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SCOPED_RDLOCK(m) \
	pthread_rwlock_rdlock(&m); \
	void u_(pthread_rwlock_t** m_) { pthread_rwlock_unlock(*m_); } \
	__attribute__((__cleanup__(u_))) pthread_rwlock_t* scoped_lock_ = &m

#define SCOPED_WRLOCK(m) \
	pthread_rwlock_wrlock(&m); \
	void u_(pthread_rwlock_t** m_) { pthread_rwlock_unlock(*m_); } \
	__attribute__((__cleanup__(u_))) pthread_rwlock_t* scoped_lock_ = &m

#ifdef __cplusplus
}
#endif
