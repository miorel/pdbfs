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
#include "rep.h"
#include "util.h"

#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct pdbfs pdbfs;

void pdbfs_file_init(struct pdbfs_file* pf) {
  memset(&(pf->pf_stat), 0, sizeof(struct stat));
  pf->pf_stat.st_mode = S_IFREG | 0444;
  pf->pf_stat.st_nlink = 1;
  pf->pf_stat.st_uid = pdbfs.p_root.st_uid;
  pf->pf_stat.st_gid = pdbfs.p_root.st_gid;
  
  pthread_mutex_init(&(pf->pf_mutex), NULL);
  pf->pf_size_checked = 0;
}

void pdbfs_file_destroy(struct pdbfs_file* pf) {
  pthread_mutex_destroy(&(pf->pf_mutex));
}

void pdbfs_rep_init() {
  pdbfs.p_fs = (struct pdbfs_file**) calloc(PDB_COUNT,
					    sizeof(struct pdbfs_file*));
  pdbfs.p_file_count = 0;
  
  pdbfs.p_root.st_mode = S_IFDIR | 0755;
  pdbfs.p_root.st_nlink = 2;
  pdbfs.p_root.st_uid = geteuid();
  pdbfs.p_root.st_gid = getegid();
  pdbfs.p_root.st_ino = PDB_COUNT;
}

void pdbfs_rep_destroy() {
  int i;
  for(i = 0; i <= PDB_COUNT; ++i)
    if(pdbfs.p_fs[i] != NULL) {
      pdbfs_file_destroy(pdbfs.p_fs[i]);
      free(pdbfs.p_fs[i]);
    }
  free(pdbfs.p_fs);
}
