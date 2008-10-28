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

#include <stdio.h>
#include <stdlib.h>

/*
 * Map between FUSE and pdbfs operations.
 */
static struct fuse_operations pdbfs_operations = {
  .getattr      = pdbfs_getattr,
  .rename       = pdbfs_rename,
  .unlink       = pdbfs_unlink,
  .rmdir        = pdbfs_rmdir,
  .symlink      = pdbfs_symlink,
  .link         = pdbfs_link,
  .release      = pdbfs_release,
  .open         = pdbfs_open,
  .read         = pdbfs_read,
  .write        = pdbfs_write,
  .fsync        = pdbfs_fsync,
  .flush        = pdbfs_flush,
  .statfs       = pdbfs_statfs,
  .opendir      = pdbfs_opendir,
  .readdir      = pdbfs_readdir,
  .fsyncdir     = pdbfs_fsyncdir,
  .releasedir   = pdbfs_releasedir,
  .chmod        = pdbfs_chmod,   
  .chown        = pdbfs_chown,
  .truncate	= pdbfs_truncate,
  .readlink	= pdbfs_readlink,
  .mknod        = pdbfs_mknod,
  .mkdir        = pdbfs_mkdir,
  .setxattr     = pdbfs_setxattr,
  .getxattr     = pdbfs_getxattr,
  .listxattr    = pdbfs_listxattr,
  .removexattr  = pdbfs_removexattr,
  .init         = pdbfs_init,
  .destroy      = pdbfs_destroy,
#if FUSE_VERSION >= 25
  .create       = pdbfs_create,
  .access       = pdbfs_access,
  .ftruncate    = pdbfs_ftruncate,
  .fgetattr     = pdbfs_fgetattr,
#endif
#if FUSE_VERSION >= 26
  .utimens      = pdbfs_utimens,
  .bmap         = pdbfs_bmap,
  .lock         = pdbfs_lock,
#else
  .utime        = pdbfs_utime,
#endif
};

/*
 * Wrap fuse_main().
 */
static inline int pdbfs_fuse_main(struct fuse_args *args) {
#if FUSE_VERSION >= 26
  return fuse_main(args->argc, args->argv, &pdbfs_operations, NULL);
#else
  return fuse_main(args->argc, args->argv, &pdbfs_operations);
#endif
}

/*
 * Print usage message. Currently this is kind of useless since it's barely
 * different from what FUSE would do on its own, but at least expanding on
 * this in the future will be straightforward!
 */
static inline void pdbfs_usage(const char* progname) {
  fprintf(stderr,
	  "Usage: %s mountpoint [options]\n"
	  "Mount the Protein Data Bank as a filesystem with FUSE.\n"
	  "\n"
	  "General options:\n"
	  "    -o opt,[opt...]        mount options\n"
	  "    -h   --help            print help\n"
	  "    -V   --version         print version\n"
	  "\n", progname);
}

/*
 * Define the keys to look for in pdbfs_opt_proc().
 */
enum {
  KEY_HELP,
  KEY_VERSION,
};

/*
 * Map between keys and options for pdbfs_opt_proc().
 */
#define PDBFS_OPT(t, p, v) { t, offsetof(struct pdbfs, p), v }
static struct fuse_opt pdbfs_opts[] = {
  FUSE_OPT_KEY("-V", KEY_VERSION),
  FUSE_OPT_KEY("--version", KEY_VERSION),
  
  FUSE_OPT_KEY("-h", KEY_HELP),
  FUSE_OPT_KEY("--help", KEY_HELP),
  
  FUSE_OPT_END
};

/*
 * Callback for processing options passed to program.
 */
static int pdbfs_opt_proc(void *data, const char *arg, int key,
                          struct fuse_args *outargs) {
  switch(key) {
  case FUSE_OPT_KEY_OPT:
    return 1; /* Let FUSE handle this. */
  case FUSE_OPT_KEY_NONOPT:
    return 1; /* Let FUSE handle this. */
  case KEY_HELP:
    pdbfs_usage(outargs->argv[0]);
    fuse_opt_add_arg(outargs, "-ho"); /* Omit FUSE help header. */
    pdbfs_fuse_main(outargs);
    exit(1);
  case KEY_VERSION:
    fprintf(stderr,
	    "This is pdbfs, version " PACKAGE_VERSION "\n"
	    "Copyright (c) 2008, Miorel-Lucian Palii"
	    "\n\n");
#if FUSE_VERSION >= 25
    fuse_opt_add_arg(outargs, "-V");
    pdbfs_fuse_main(outargs);
#endif
    exit(0);
  default:
    /* Execution should never get here! */
    fprintf(stderr, "Internal error!\n");
    abort();
  }
}

/*
 * Parse the command-line arguments then let FUSE do its job.
 */
int main(int argc, char *argv[]) {
  int ret;
  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
  if(fuse_opt_parse(&args, NULL, pdbfs_opts, pdbfs_opt_proc) == -1) exit(1);
  ret = pdbfs_fuse_main(&args);
  fuse_opt_free_args(&args);
  return ret;
}
