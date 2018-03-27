/**
 * @file   az_ion.h
 * @brief  
 * @date   12/03/16
 * @author fware
 *
 * @copyright
 * Copyright (c) Fware, 2013-2017 - All Rights Reserved
 * You may use, distribute and modify this code under the 
 * terms of the "Aurora Source Code License".
 * See the file LICENSE for full license details.\n\n
 *
 * You should have received a copy of  the LICENSE with this file.\n\n
 *
 * If not, please contact to Fware with the information in the file CONTACTS.
 *
 * History:
 *   - 2016-03-12 fware initial create
 */

#ifndef AZ_ION_H
#define AZ_ION_H

#include "az_var.h"
#include "az_array.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#ifdef  CONFIG_AZ_ION_MAX
#define AZ_ION_MAX    CONFIG_AZ_ION_MAX
#else
#define AZ_ION_MAX    4096
#endif

#include "az_ion_types.h"

/* structured types */
#ifdef  CONFIG_AZ_DYNAMIC_ION
typedef az_array_t az_ion_list_t;
#else
typedef struct az_ion_list {
  int count;
  int size;
} az_ion_list_t;
#endif

/* macros */

/* variabls exposed */
#ifdef  CONFIG_AZ_DYNAMIC_ION
  extern az_ion_list_t *az_ion_PRefList;
  #define AZ_ION_LIST() az_ion_PRefList

  #define AZ_ION_IDLE() (NULL == AZ_ION_LIST())
  #define AZ_ION_BUSY() (NULL != AZ_ION_LIST())

  #define az_ion(_index) \
    (((unsigned)(_index) < (unsigned)(AZ_ION_LIST()->size))? \
    ((az_ion_t *)(*az_array_element_ptr_at(AZ_ION_LIST(), _index))):NULL) 
#else
  extern az_ion_list_t az_ion_list; 
  extern az_ion_t *az_ions[];
  #define AZ_ION_LIST() ((az_ion_list_t *)&az_ion_list)

  #define AZ_ION_IDLE() (0 == az_ion_list.size)
  #define AZ_ION_BUSY() (0 != az_ion_list.size)

  #define az_ion(_index) \
    (((unsigned)(_index) < (unsigned)(AZ_ION_LIST()->size))? az_ions[_index]:NULL) 
#endif

#define az_ion_lock()
#define az_ion_unlock()

static void az_ion_invalidate(az_ion_t *ion, int is_static)
{
  az_assert(NULL != ion);
  ion->id = AZ_SYS_IO_INVALID;
  ion->type = 0;
  if (is_static) {
    az_refcount_init_static(&ion->refCount);
  } else {
    az_refcount_init_dynamic(&ion->refCount);
  }
}

/* inline functions */
static inline void az_ion_init(az_array_t *refList)
{
  if (AZ_ION_IDLE()) {
    #ifdef  CONFIG_AZ_DYNAMIC_ION
    az_ion_PRefList = refList;
    az_array_init(refList, refList->size, refList->list, NULL, sizeof(az_ref_t)); 
    #else
    az_ion_list.count = 0;
    az_ion_list.size = AZ_ION_MAX;
    memset(az_ions, 0, sizeof(az_ion_t *) * AZ_ION_MAX);
    #endif
  }
}
static inline void az_ion_deinit()
{
  if (AZ_ION_BUSY()) {
    #ifdef  CONFIG_AZ_DYNAMIC_ION
    az_array_deinit(az_ion_PRefList);
    az_ion_PRefList = NULL;
    #else
    az_ion_list.count = 0;
    az_ion_list.size = 0; 
    memset(az_ions, 0, sizeof(az_ion_t) * AZ_ION_MAX);
    #endif
  
  }
}
typedef int (*az_ion_compare_t)(void *, void *);

static inline az_ion_t *az_ion_find(void *var, az_ion_compare_t compare_f) 
{
  int j;
  az_ion_list_t *list = AZ_ION_LIST();
  az_ion_t  *ion = NULL;
  int count = list->count;
  for (j = 0; j < list->size && count > 0; j++) {
    ion = az_ion(j); 
    //az_sys_eprintf("sz:%ld count:%d %p\n", list->size, count, ion);
    if (ion == NULL) continue;
    if (!compare_f(var, ion)) {
      break;
    }
    count--;
  }
  if (count == 0) ion = NULL;

  return ion;
}
static inline az_size_t az_ion_count()
{
  az_ion_list_t *list = AZ_ION_LIST();
  return list->count;
}
typedef char *(*az_var_value2Str_t)(void *);

/* function prototypes exposed */
extern az_r_t az_ion_register(az_ion_t *ion, az_ion_type_t type);
extern az_r_t az_ion_deregister(az_ion_t *ion);
extern az_size_t az_ion_toStr(az_ion_t *ion, char *tag, char *bp, az_size_t blen);
extern az_size_t az_ion_valuesOnRow(az_ion_t *ion, char *tag, char *wlist, az_var_value2Str_t *convlist, char *bp, az_size_t blen);

#ifdef __cplusplus
}
#endif
#endif
