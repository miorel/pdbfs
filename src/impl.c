/*
  pdbfs - mount the Protein Data Bank as a filesystem with FUSE
  Copyright (C) 2008 by Miorel-Lucian Palii
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "config.h"
#include "fs.h"
#include "rep.h"
#include "rsync.h"
#include "updater.h"
#include "util.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <time.h>
#include <zlib.h>

#ifdef HAVE_LIBFUSE

void* pdbfs_init(
#if FUSE_VERSION >= 26
		 struct fuse_conn_info *conn
#endif
		 ) {
  pdbfs_rep_init();
  pdbfs_updater_init();
  return NULL;
}

void pdbfs_destroy() {
  pdbfs_updater_destroy();
  pdbfs_rep_destroy();
}

int pdbfs_getattr(const char *path, struct stat *stbuf) {
  int ret = -ENOENT;
  unsigned int num;
  if(path_to_num(path, &num)) {
    if(num == PDB_COUNT) {
      memcpy(stbuf, &(pdbfs.p_root), sizeof(struct stat));
      ret = 0;
    }
    else if(pdbfs.p_fs[num] != NULL) {
      pthread_mutex_lock(&(pdbfs.p_fs[num]->pf_mutex));
      memcpy(stbuf, &(pdbfs.p_fs[num]->pf_stat), sizeof(struct stat));
      pthread_mutex_unlock(&(pdbfs.p_fs[num]->pf_mutex));
      ret = 0;
    }
  }
  return ret;
}

/*
 * Never saw this called while testing, 
 */
#if FUSE_VERSION >= 25
int pdbfs_fgetattr(const char *path, struct stat *buf,
		   struct fuse_file_info *fi) {
  return pdbfs_getattr(path, buf);
}
#endif

int pdbfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		  off_t offset, struct fuse_file_info *fi) {
  int ret = -ENOENT;
  int i;
  char filename[PDB_ID_LEN + 5];
  if(strcmp(path, "/") == 0) {
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    strcpy(filename + PDB_ID_LEN, ".pdb");
    for(i = 0; i != PDB_COUNT; ++i) if(pdbfs.p_fs[i] != NULL) {
	num_to_pdbid(&i, filename);
	filler(buf, filename, &(pdbfs.p_fs[i]->pf_stat), 0);
      }
    ret = 0;
  }
  return ret;
}


int pdbfs_statfs(const char *path, struct
#if FUSE_VERSION >= 25
		 statvfs
#else
		 statfs
#endif
		 *buf) {
  buf->f_bsize = 1024;
  buf->f_files = PDB_COUNT;
  buf->f_ffree = PDB_COUNT - pdbfs.p_file_count;
  buf->f_blocks = buf->f_files / buf->f_bsize;
  buf->f_bfree = buf->f_ffree / buf->f_bsize;
  buf->f_bavail = buf->f_ffree / buf->f_bsize;
#if FUSE_VERSION >= 25
  buf->f_namemax = PDB_ID_LEN + 4;
  buf->f_frsize = buf->f_bsize;
  buf->f_favail = buf->f_ffree;
#else
  buf->f_namelen = PDB_ID_LEN + 4;
#endif
  return 0;
}

#if FUSE_VERSION >= 25
int pdbfs_access(const char *path, int mask) {
  int ret;
  unsigned int num;
  if(path_to_num(path, &num)) {
    if(mask & W_OK) ret = -EROFS;
    else if((mask & X_OK) && num != PDB_COUNT) ret = -EACCES;
    else ret = 0;
  }
  else ret = -ENOENT;
  return ret;
}
#endif

int pdbfs_open(const char *path, struct fuse_file_info *fi) {
  struct stat st;
  off_t filesize;
  gzFile gzf;
  char buffer[1024];
  int read;
  int ret;
  unsigned int num;
  if(path_to_num(path, &num)) {
    if((fi->flags & O_WRONLY) || (fi->flags & O_RDWR) || (fi->flags & O_TRUNC))
      ret = -EROFS;
    else {
      pthread_mutex_lock(&(pdbfs.p_fs[num]->pf_mutex));
      gzf = pdbfs_get_file(path + 1, &st);
      if(!pdbfs.p_fs[num]->pf_size_checked) {
	filesize = 0;
	while(read = gzread(gzf, buffer, 1024)) filesize += (off_t) read;
	pdbfs.p_fs[num]->pf_stat.st_size = filesize;
	pdbfs.p_fs[num]->pf_size_checked = 1;
      }
      memcpy(&(fi->fh), &gzf, sizeof(gzFile));
      pthread_mutex_unlock(&(pdbfs.p_fs[num]->pf_mutex));
      ret = 0;
    }
  }
  else ret = (fi->flags & O_CREAT) ? -EROFS : -ENOENT;
  return ret;
}

int pdbfs_read(const char *path, char *buf, size_t size, off_t off,
	       struct fuse_file_info *fi) {
  int ret;
  unsigned int num;
  gzFile gzf;
  if(path_to_num(path, &num)) {
    if(num == PDB_COUNT) ret = -EISDIR;
    else {
      pthread_mutex_lock(&(pdbfs.p_fs[num]->pf_mutex));
      memcpy(&gzf, &(fi->fh), sizeof(gzFile));
      gzseek(gzf, off, SEEK_SET);
      ret = gzread(gzf, buf, size);
      pthread_mutex_unlock(&(pdbfs.p_fs[num]->pf_mutex));
    }
  }
  else ret = -EBADF;
  return ret;
}

int pdbfs_release(const char *path, struct fuse_file_info *fi) {
  gzFile gzf;
  memcpy(&gzf, &(fi->fh), sizeof(gzFile));
  gzclose(gzf);
  return 0;
}

/*
 * Symlink? I think not. The path is clearly invalid.
 */
int pdbfs_readlink(const char *path, char *buf, size_t len) {
  return -EINVAL;
}

/*
 * Who gave ya a file descriptor that lets you write? Not me, I hope!
 */
int pdbfs_write(const char *path, const char *buf, size_t size, off_t off,
		struct fuse_file_info *fi) {
  return -EBADF;
}

/*
 * This ought to be modeled after lockf(3) in the future.
 */
#if FUSE_VERSION >= 26
int pdbfs_lock(const char *path, struct fuse_file_info *fi, int cmd,
	       struct flock *lock) {
  return -EOPNOTSUPP;
}
#endif

/*
 * Read-only filesystem, so no creating allowed.
 * But maybe I shouldn't complain if the file actually exists?
 */
#if FUSE_VERSION >= 25
int pdbfs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
  return -EROFS;
}
#endif

/*
 * None of these are allowed on a read-only filesystem.
 */
int pdbfs_rename(const char *oldpath, const char *newpath) {return -EROFS;}
int pdbfs_unlink(const char *path) {return -EROFS;}
int pdbfs_rmdir(const char *path) {return -EROFS;}
int pdbfs_link(const char *oldpath, const char *newpath) {return -EROFS;}
int pdbfs_symlink(const char *linkname, const char *path) {return -EROFS;}
int pdbfs_chmod(const char *path, mode_t mode) {return -EROFS;}
int pdbfs_chown(const char *path, uid_t uid, gid_t gid) {return -EROFS;}
int pdbfs_truncate(const char *path, off_t size) {return -EROFS;}
#if FUSE_VERSION >= 25
int pdbfs_ftruncate(const char *path, off_t size, struct fuse_file_info *fi)
{return -EROFS;}
#endif
#if FUSE_VERSION >= 26
int pdbfs_utimens(const char *path, const struct timespec tv[2])
{return -EROFS;}
#else
int pdbfs_utime(const char *path, struct utimbuf *ut) {return -EROFS;}
#endif
int pdbfs_mknod(const char *path, mode_t mode, dev_t rdev) {return -EROFS;}
int pdbfs_mkdir(const char *path, mode_t mode) {return -EROFS;}

/*
 * No need to do anything for these, so I guess my work here is done...
 */
int pdbfs_opendir(const char *path, struct fuse_file_info *fi) {return 0;}
int pdbfs_fsyncdir(const char *path, int datasync, struct fuse_file_info *fi)
{return 0;}
int pdbfs_releasedir(const char *path, struct fuse_file_info *fi) {return 0;}
int pdbfs_fsync(const char *path, int datasync, struct fuse_file_info *fi)
{return 0;}
int pdbfs_flush(const char *path, struct fuse_file_info *fi) {return 0;}
#if FUSE_VERSION >= 25
int pdbfs_bmap(const char *path, size_t blocksize, uint64_t *idx) {return 0;}
#endif

/*
 * Don't want to be bothered with extended attributes...
 * Let's just say they're not supported.
 */
int pdbfs_setxattr(const char *path, const char *name, const char *value,
		   size_t size, int flags) {return -EOPNOTSUPP;}
int pdbfs_getxattr(const char *path, const char *name, char *value,
		   size_t size) {return -EOPNOTSUPP;}
int pdbfs_listxattr(const char *path, char *list, size_t size)
{return -EOPNOTSUPP;}
int pdbfs_removexattr(const char *path, const char *name) {return -EOPNOTSUPP;}

#endif /* HAVE_LIBFUSE */
