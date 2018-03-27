/**
 * @file   az_linux_fs.h
 * @brief  
 * @date   05/02/16
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
 *   - 2016-02-05 Daehyun Kang initial create
 */

#ifndef AZ_LINUX_FS_H
#define AZ_LINUX_FS_H
#include "az_sys_linux.h"
#include "az_def.h"
#include "az_err.h"
#include "az_assert.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* constants */

/* basic macros */
#ifndef AZ_SYS_PATH_DELIMITER
#define AZ_SYS_PATH_DELIMITER       "/"
#define AZ_SYS_PATH_SEPERATOR       '/' 
#endif

/* basic types */
#ifndef AZ_LINUX_DEF_H
typedef int               az_sys_fd_t;
#endif
typedef DIR *             az_sys_dir_t;
typedef int               az_sys_file_t;
typedef struct dirent     az_sys_dirent_t;

#define AZ_SYS_DIR_INVALID    NULL
#define AZ_SYS_FILE_INVALID   (-1)

typedef struct {
  DIR *dirp;
  struct dirent *cur;
} az_sys_dirlist_ctx_t;

#define az_sys_dirent_getname(entry)  (((struct dirent *)(entry))->d_name)

/* structures */

/* structured types */

/* macros */
#define az_sys_fileno fileno
#define az_sys_dup    dup
#define az_sys_dup2   dup2

/* variabls exposed */

/* inline functions */

static inline az_size_t az_sys_fs_getpath(az_sys_file_t fd, char *path, az_size_t size)
{
  char proclnk[128];
  sprintf(proclnk, "/proc/self/fd/%d", fd);
  ssize_t r = readlink(proclnk, path, size);
  if (r >= 0) {
    path[r] = 0;
  }

  return r;
}

static inline az_r_t az_sys_fs_open(const az_str_t path, int flags, int mode, az_sys_file_t *pfile) 
{
  extern int az_sys_open(const char *, int, mode_t);
  az_r_t r = AZ_ERR(ARG_NULL);
  az_sys_file_t fd;
  az_assert(NULL != pfile);
  do {
    if (pfile && path) {
      fd = az_sys_open(path, flags, mode);
      if (fd < 0) {
        r = (az_r_t)fd;
        break;
      }
      *pfile = fd;
      r = AZ_SUCCESS;
    }
  } while (0);
  return r;
}

static inline az_r_t az_sys_fs_close(az_sys_file_t fd) 
{
  extern int az_sys_close(int);
  az_r_t r = AZ_SUCCESS;
  r = (az_r_t)az_sys_close(fd);
  return r;
}

static inline az_r_t az_sys_fs_create(const char * path, int mode, az_sys_file_t *pfile) 
{
  extern int az_sys_creat(const char *, mode_t);

  az_r_t r = AZ_ERR(ARG_NULL);
  az_sys_file_t fd;
  az_assert(NULL != pfile);
  do {
    if (pfile && path) {
      fd = az_sys_creat(path, mode);
      if (fd < 0) {
        r = (az_r_t)fd;
        break;
      }
      *pfile = fd;
      r = AZ_SUCCESS;
    }
  } while (0);

  return r;
}

static inline az_r_t az_sys_fs_delete(const char *path) 
{
  az_r_t r = AZ_ERR(ARG_NULL);
  if (path) {
      r = (az_r_t)unlink(path);
  }

  return r;
}


static inline az_r_t az_sys_fs_rename(az_str_t opath, az_str_t npath)
{
  az_r_t r = AZ_SUCCESS;
  r = (az_r_t)rename(opath, npath);
  return r;
}

static inline az_size_t az_sys_fs_size(az_str_t path)
{
  struct stat fileInfo;
  int error = stat(path, &fileInfo);
  az_size_t size;
  if (error < 0) {
    size = (az_size_t)error;
  } else {
    size = (az_size_t)fileInfo.st_size;
  }
  return size;
}
static inline az_size_t az_sys_fs_filesize(az_sys_file_t fd)
{
  struct stat fileInfo;
  int error = fstat(fd, &fileInfo);
  az_size_t size;
  if (error < 0) {
    size = (az_size_t)error;
  } else {
    size = (az_size_t)fileInfo.st_size;
  }
  return size;
}
static inline az_bool_t az_sys_fs_exist(const char *path)
{
  struct stat fileInfo;
  int error = stat(path, &fileInfo);
  return (0 == error)? AZ_TRUE:AZ_FALSE; 
}

static inline az_size_t az_sys_fs_read(az_sys_file_t fd, az_ref_t bp, az_size_t len) 
{
  extern ssize_t az_sys_read(int, void *, size_t);
  az_size_t r;
  r = (az_size_t)az_sys_read(fd, bp, len);
  return r;
}

static inline az_size_t az_sys_fs_write(az_sys_file_t fd, az_ref_t bp, az_size_t len) 
{
  extern ssize_t az_sys_write(int, const void *, size_t);
  az_size_t r;
  r = (az_size_t)az_sys_write(fd, bp, len);
  return r;
}

static inline az_size_t az_sys_fs_copy(const az_str_t ipath, const az_str_t opath) 
{
  az_r_t r = AZ_ERR(ARG_NULL);
  az_sys_file_t src, dst;
  off_t offset = 0;
  az_size_t wlen;
  az_size_t total;

  while (ipath && opath) {
    r = az_sys_fs_open(ipath, O_RDONLY, 0, &src);
    if (AZ_SUCCESS != r) break;
    r = az_sys_fs_open(opath, O_WRONLY|O_CREAT, 00666, &dst);
    if (AZ_SUCCESS != r) break;

    do {
      #ifdef CONFIG_AZ_PRELOAD_SO
      wlen = az_real_sendfile(dst, src, &offset, 4096);
      #else
      wlen = sendfile(dst, src, &offset, 4096);
      #endif
      if (wlen <= 0) break;
      total += wlen;
    } while (1);
    break;
  }
  if (AZ_SUCCESS != r) total = (az_size_t)r;
  return total;
}

static inline az_r_t az_sys_fs_lseek(az_sys_file_t fd, az_pos_t pos, az_int32_t whence)
{
  az_r_t r = AZ_SUCCESS;
  r = (az_r_t)lseek(fd, pos, whence);
  return r;
}
static inline az_r_t az_sys_fs_truncate(const char *path, az_size_t size)
{
  az_r_t r = AZ_SUCCESS;
  r = (az_r_t)truncate(path, (off_t)size);
  return r;
}
static inline az_r_t az_sys_fs_ftruncate(az_sys_file_t fd, az_size_t size)
{
  az_r_t r = AZ_SUCCESS;
  r = (az_r_t)ftruncate(fd, (off_t)size);
  return r;
}

static inline az_r_t az_sys_fs_createfolder(const char *path, az_int32_t mode)
{
  az_r_t result = AZ_SUCCESS;
  if ((result = mkdir(path, mode)) == -1) {
      switch (errno) {
        case EACCES:
          result = AZ_ERR(ACCESS);
          break;
        case EEXIST:
          result = AZ_ERR(EXIST);
          break;
        case EFAULT:
          result = AZ_ERR(FAULT);
          break;
        case ELOOP:
          result = AZ_ERR(OVERFLOW);
          break;
        case ENAMETOOLONG:
          result = AZ_ERR(TOO_LONG);
          break;
        case ENOENT: {
          result = AZ_ERR(NO_ENTRY);
          break;
        case ENOMEM:
          result = AZ_ERR(NO_MEM);
          break;
        case ENOSPC:
          result = AZ_ERR(NO_SPACE);
          break;
        case ENOTDIR:
          result = AZ_ERR(INVALID);
          break;
        case EPERM:
          result = AZ_ERR(NO_RIGHT);
          break;
        case EROFS:
          result = AZ_ERR(READONLY);
          break;
        default:
          result = AZ_ERR(FAIL);
          break;
      }
    }
  }
  return result;
}

static inline az_r_t az_sys_fs_deletefolder(const char *path)
{
  az_r_t result = AZ_SUCCESS;
  do {
    if ((result = rmdir(path) < 0)) {
      switch (errno) {
        case EACCES:
          result = AZ_ERR(ACCESS);
          break;
        case EBUSY:
          result = AZ_ERR(BUSY);
          break;
        case EFAULT:
          result = AZ_ERR(FAULT);
          break;
        case ELOOP:
          result = AZ_ERR(OVERFLOW);
          break;
        case EINVAL:
          result = AZ_ERR(INVALID);
          break;
        case ENAMETOOLONG:
          result = AZ_ERR(TOO_LONG);
          break;
        case ENOENT:
          result = AZ_ERR(NO_ENTRY);
          break;
        case ENOTDIR:
          result = AZ_ERR(NOT_DIR);
          break;
        case ENOTEMPTY:
          result = AZ_ERR(NOT_EMPTY);
          break;
        case EPERM:
          result = AZ_ERR(NO_RIGHT);
          break;
        case EROFS:
          result = AZ_ERR(READ_ONLY);
          break;
      }
    }
  } while (0);
  return result;
}

static inline az_r_t az_sys_fs_renamefolder(az_str_t opath, az_str_t npath)
{
  az_r_t r = AZ_SUCCESS;
  r = (az_r_t)rename(opath, npath);
  return r;
}

static inline az_r_t az_sys_fs_find(const char *foldername, az_str_t filename)
{
  az_r_t r = AZ_FAIL;
  DIR *d;
  struct dirent *dir;

  d = opendir(foldername);
  if (d) {
    do {
      dir = readdir(d);
      if (!dir) break;
      if (!strcmp(dir->d_name, filename)) {
        r = AZ_SUCCESS; 
        break;
      }
    } while (1);
    closedir(d);
  }
  return r;
}

static inline az_r_t az_sys_fs_getcwd(az_str_t path, az_size_t size)
{
  az_r_t r = AZ_ERR(ARG_NULL);
  if (path) {
    if (getcwd(path, size) != NULL) {
      r = AZ_SUCCESS;
    } else {
      r = AZ_ERR(FAIL);
    }
  }
  return r;
}

static inline az_r_t az_sys_fs_chdir(az_str_t path)
{
  az_r_t r = AZ_ERR(ARG_NULL);
  if (path) {
    r = chdir(path);
  }
  return r;
}

static inline az_size_t az_sys_fs_parsePath(az_str_t path, az_str_t path_buffer, az_str_t path_comp[])
{
  az_r_t r = AZ_ERR(ARG_NULL);
  az_size_t j = 0;
  char *ctx;
  az_str_t token;

  if (path && path_buffer && path_comp) {
    if (path != path_buffer) {
      strcpy(path_buffer, path);
    }
    r = AZ_SUCCESS;
    token = strtok_r(path_buffer, AZ_SYS_PATH_DELIMITER, &ctx);
    while (token) {
      path_comp[j++] = token;
      token = strtok_r(NULL, AZ_SYS_PATH_DELIMITER, &ctx);
      if (j == PATH_COMP_MAX) {
        r = AZ_FAIL;
        break;
      } }
    if (!r) {
      path_comp[j] = NULL;
    }
  }
  if (r) j = (az_size_t)r;
  return j;
}
static inline az_r_t az_sys_fs_combinePath(char *path1, char *path2, char *dest)
{
  //az_r_t result = AZ_ERR(ARG_NULL);
  az_size_t pos;
  char *src = path2;
  az_assert(NULL != path1 && NULL != path2 && NULL != dest);

  pos = strlen(path1);
  if (path1[pos] == AZ_SYS_PATH_SEPERATOR) {
    if (path2[0] == AZ_SYS_PATH_SEPERATOR) {
      pos--;
    }
    if (dest != path1) {
      memcpy(dest, path1, pos);
    }
    dest += pos;
  } else {
    if (dest != path1) {
      memcpy(dest, path1, pos);
    }
    dest += pos;
    if (path2[0] == AZ_SYS_PATH_SEPERATOR) {
      *dest++ = AZ_SYS_PATH_SEPERATOR;
      pos++;
    }
  }
  strcpy(dest, src); 

  return pos + strlen(src);
}
#if 0
#endif

/* function prototypes exposed */
extern az_r_t  az_sys_fs_listDir(char *path, az_sys_dirent_t *list, az_size_t listCount, void **ctx);
extern az_r_t  az_sys_fs_findDir(char *path, char *entryname, az_sys_dirent_t *entry);
extern az_size_t  az_sys_fs_dirSize(char *path);

#ifdef __cplusplus
}
#endif
#endif