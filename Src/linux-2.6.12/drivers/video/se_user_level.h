/*
 * se.h -- definitions for the char module
 *
 * Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2001 O'Reilly & Associates
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.   No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 *
 * $Id: se.h,v 1.15 2004/11/04 17:51:18 rubini Exp $
 */

#ifndef _SE_DRIVER_H_
#define _SE_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "se_export.h"
#include "OSAL.h"
#include "hresult.h"
#include <pthread.h>


struct se_dev {
      void *CmdBuf;  /* Pointer to first quantum set */
	  void *CachedCmdBuf;
	  void *CmdBase;
	  void *CmdLimit;
      int wrptr;
      int v_to_p_offset;
      int size;
      se_cmd_counter sw_counter;
      se_cmd_counter hw_counter;
	  pthread_mutex_t lock_mutex;             //mutex for preventing race condition
};

/*
 * Prototypes for shared functions
 */

HRESULT
se_open(void);

HRESULT
se_close(void);

size_t
se_write(const char *buf, size_t count);

HRESULT
se_ioctl(unsigned int cmd, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* _SE_H_ */
