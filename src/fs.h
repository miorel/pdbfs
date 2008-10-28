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

#ifndef _FS_H
#define _FS_H 1

#include "config.h"

#define FUSE_USE_VERSION 27 /* Developed with version 2.7 of FUSE. */
#define _FILE_OFFSET_BITS 64 /* Needed for proper compilation. */

#ifdef HAVE_LIBFUSE

#include <fuse.h>
#include <fuse_opt.h>

/*
 * There's nothing terribly exciting going on in this file, just declaring all
 * the functions that will make up the filesystem the way FUSE wants them.
 */

int pdbfs_getattr(const char *path, struct stat *buf);
int pdbfs_rename(const char *oldpath, const char *newpath);
int pdbfs_unlink(const char *path);
int pdbfs_rmdir(const char *path);
int pdbfs_symlink(const char *linkname, const char *path);
int pdbfs_link(const char *oldpath, const char *newpath);
int pdbfs_release(const char *path, struct fuse_file_info *fi);
int pdbfs_open(const char *path, struct fuse_file_info *fi);
int pdbfs_read(const char *path, char *buf, size_t size, off_t off,
	       struct fuse_file_info *fi);
int pdbfs_write(const char *path, const char *buf, size_t size, off_t off,
		struct fuse_file_info *fi);
int pdbfs_fsync(const char *path, int datasync, struct fuse_file_info *fi);
int pdbfs_flush(const char *path, struct fuse_file_info *fi);
int pdbfs_opendir(const char *path, struct fuse_file_info *fi);
int pdbfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		  off_t off, struct fuse_file_info *fi);
int pdbfs_fsyncdir(const char *path, int datasync, struct fuse_file_info *fi);
int pdbfs_releasedir(const char *path, struct fuse_file_info *fi);
int pdbfs_chmod(const char *path, mode_t mode);
int pdbfs_chown(const char *path, uid_t uid, gid_t gid);
int pdbfs_truncate(const char *path, off_t size);
int pdbfs_readlink(const char *path, char *buf, size_t len);
int pdbfs_mknod(const char *path, mode_t mode, dev_t rdev);
int pdbfs_mkdir(const char *path, mode_t mode);
int pdbfs_setxattr(const char *path, const char *name, const char *value,
		   size_t size, int flags);
int pdbfs_getxattr(const char *path, const char *name, char *value,
		   size_t size);
int pdbfs_listxattr(const char *path, char *list, size_t size);
int pdbfs_removexattr(const char *path, const char *name);
void pdbfs_destroy();

#if FUSE_VERSION >= 25
int pdbfs_create(const char *path, mode_t mode, struct fuse_file_info *fi);
int pdbfs_ftruncate(const char *path, off_t size, struct fuse_file_info *fi);
int pdbfs_access(const char *path, int mask);
int pdbfs_fgetattr(const char *path, struct stat *buf,
		   struct fuse_file_info *fi);
int pdbfs_statfs(const char *path, struct statvfs *buf);
#else
int pdbfs_statfs(const char *path, struct statfs *buf);
#endif

#if FUSE_VERSION >= 26
int pdbfs_lock(const char *path, struct fuse_file_info *fi, int cmd,
	       struct flock *lock);
int pdbfs_bmap(const char *path, size_t blocksize, uint64_t *idx);
int pdbfs_utimens(const char *path, const struct timespec tv[2]);
void* pdbfs_init(struct fuse_conn_info *conn);
#else
int pdbfs_utime(const char *path, struct utimbuf *ut);
void* pdbfs_init();
#endif

#else
#error Does your system support FUSE? I could not find it...
#endif /* HAVE_LIBFUSE */

#endif /* _FS_H */
