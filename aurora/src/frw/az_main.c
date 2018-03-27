/**
 * @file   az_main.c
 * @brief  
 * @date   09/02/16
 * @author Daehyun Kang
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
 *   - 2016-02-09 Daehyun Kang initial create
 */

/* include header files */
#include "aurora.h"
#include "frw/az_frw.h"
#include "cli/az_cli.h"
#include "az_printf.h"

/* declare global variables */

/* declare static variables */


/* implement static functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */


/* implement global functions */

/**
 * @fn        function name
 * @brief     function-description
 * @param     input-output-parameters
 * @return    return-value
 * @warning   warnings
 * @exception none
 */
az_xu_t xu1 = NULL, xu2 = NULL;
void *az_xu_entry1(void *arg)
{
  int count = 0;
  az_printf("%s start...\n", __FUNCTION__);
  while (1) {
    az_printf("%s count=%d...\n", __FUNCTION__, count++);
    az_xu_sleep(2000000000);
  }
}
void *az_xu_entry2(void *arg)
{
  int count = 0;
  az_printf("%s start...\n", __FUNCTION__);
  while (1) {
    az_printf("%s count=%d...\n", __FUNCTION__, count++);
    az_xu_sleep(3000000000);
    if (count & 1) {
      az_xu_suspend(xu1);
    } else {
      az_xu_resume(xu1);
    }
  }
}

az_xu_t xu3 = NULL, xu4 = NULL;
void *az_xu_entry3(void *arg)
{
  int count = 0;
  az_r_t r;
  az_xu_event_t revents;
  az_printf("%s start...\n", __FUNCTION__);
  while (1) {
    r = az_xu_recvEvent(0x3, 0, 1000000000, &revents);
    if (r == AZ_ERR(TIMEOUT)) {
      az_printf("%s timout=%d...\n", __FUNCTION__, count++);
      continue;
    } else if (r < 0) {
      az_printf("%s error=%s:"AZ_FMT_RET(1)"\n", __FUNCTION__, az_err_str(r), r);
      break;
    }
    //az_printf("%s count=%d recv %x result="AZ_FMT_RET(1)"\n", __FUNCTION__, count, revents, r); 
    az_rprintf(r, "%s count=%d recv %x\n", __FUNCTION__, count, revents); 
  }
  return NULL;
}

void *az_xu_entry4(void *arg)
{
  int count = 0;
  az_r_t r;
  az_xu_event_t sevents;

  az_printf("%s start...\n", __FUNCTION__);
  while (1) {
    az_xu_sleep(2000000000);
    if (count & 1) {
      sevents = 1;
      r = az_xu_sendEvent(xu3, sevents);
      az_printf("%s count=%d, send %x result="AZ_FMT_RET(1)"\n", __FUNCTION__, count++, sevents, r);
    } else {
      sevents = 2;
      r = az_xu_sendEvent(xu3, sevents);
      az_printf("%s count=%d, send %x result="AZ_FMT_RET(1)"\n", __FUNCTION__, count++, sevents, r);
    }
  }
  return NULL;
}

az_ring_t az_ring;

az_xu_t xu5a = NULL, xu6a= NULL;
az_xu_t xu5b = NULL, xu6b = NULL;
void *az_xu_entry5(void *arg)
{
  az_ring_value_t value;
  az_pos_t pos;
  az_xu_t xu = az_xu_self();

  az_printf("%s start...\n", __FUNCTION__);
  while (1) {
    //az_xu_sleep(1000000);
    if ((pos = az_ring_pop(&az_ring, &value)) < 0) {
    //az_printf("%s:%s pop  pos=%d value=%d fails=%d\n", AZ_XU_NAME(xu), __FUNCTION__, pos, value.b32, az_ring.popfails);
      //break;;
      az_xu_sleep(1000);
      continue;
    }
    az_printf("%s:%s pop  pos="AZ_FMT_POS(1)" value=%d fails=%d\n", AZ_XU_NAME(xu), __FUNCTION__, pos, value.b32, az_ring.popfails);
  }
}
void *az_xu_entry6(void *arg)
{
  az_ring_value_t value;
  az_pos_t pos;
  az_xu_t xu = az_xu_self();

  az_printf("%s start...\n", __FUNCTION__);
  value.b32 = 1;
  while (1) {
    //az_xu_sleep(1000000);
    if ((pos = az_ring_push(&az_ring, &value)) < 0) {
    //az_printf("%s:%s push pos=%d value=%d fails=%d\n", AZ_XU_NAME(xu), __FUNCTION__, pos, value.b32, az_ring.pushfails);
      //break;;
      az_xu_sleep(1000);
      continue;
    }
    az_printf("%s:%s push pos="AZ_FMT_POS(1)" value=%d fails=%d\n", AZ_XU_NAME(xu), __FUNCTION__, pos, value.b32, az_ring.pushfails);
    value.b32++;
  }
}
az_fifo_t az_fifo;

az_xu_t xu7a = NULL, xu8a = NULL;
az_xu_t xu7b = NULL, xu8b = NULL;
void *az_xu_entry7(void *arg)
{
  az_pos_t pos;
  az_xu_t xu = az_xu_self();

  az_printf("%s:%s start... %p\n", AZ_XU_NAME(xu), __FUNCTION__, xu);
  while (1) {
    //az_xu_sleep(1000000);
    if ((pos = az_fifo_pop(&az_fifo)) < 0) continue;
    az_printf("%s:%s pop  pos="AZ_FMT_POS(1)" fail=%u\n", AZ_XU_NAME(xu), __FUNCTION__, pos, az_fifo.popfails);
  }
}
void *az_xu_entry8(void *arg)
{
  az_pos_t pos;
  az_xu_t xu = az_xu_self();

  az_printf("%s:%s start... %p\n", AZ_XU_NAME(xu), __FUNCTION__, xu);
  while (1) {
    //az_xu_sleep(1000000);
    if ((pos = az_fifo_push(&az_fifo)) < 0) continue;
    az_printf("%s:%s push pos="AZ_FMT_POS(1)" fail=%u\n", AZ_XU_NAME(xu), __FUNCTION__, pos, az_fifo.pushfails);
  }
}

az_xu_config_t az_xu_main_config = {
  .arg_name[0] = 0,
  .stackSize = 0,
  .coremask = 0x1,
  .policy = SCHED_OTHER,
  .priority = 1,
  .startdelay = -1,
};
az_xu_t az_xu_main = NULL;

struct az_xu_main_arg {
  int argc;
  char **argv;
} az_xu_main_arg;

void *az_xu_main_entry(struct az_xu_main_arg *arg)
{
  az_assert(NULL != arg);

#ifdef  CONFIG_AZ_TEST
  return az_test_main(arg->argc, arg->argv);
#else
  return az_frw_main();
#endif
}

//extern void  az_rstdio_begin(void);

az_uint32_t az_ring_ds32[2];
int main(int argc, char *argv[])
{
  az_r_t r;

  //setvbuf(stdout, NULL, _IONBF, 0);
  //setvbuf(stdin, NULL, _IONBF, 0);

  //az_rstdio_begin();

  az_core_init();

  az_log_dup_stdout_start(); 

  az_trace_start_default_thread();

  az_mcheck_init();

  r = az_frw_parse_options(argc, argv);
  if (r < 0) {
    exit(r);
  }

  az_xu_main_arg.argc = argc;
  az_xu_main_arg.argv = argv;

  r = az_xu_create("main", az_xu_main_entry, &az_xu_main_arg, &az_xu_main_config, &az_xu_main);
  az_printf("create main xu : %p, result=%ld\n", az_xu_main, r);

  #ifdef  CONFIG_AZ_TRACE_START
  az_trace_start();
  #endif

  if (r == AZ_SUCCESS) {
    return (int)az_xu_entry(az_xu_main);
  } else {
    return (int)r;
  }
}

/* === end of AZ_MAIN_C === */
