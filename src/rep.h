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

#ifndef _REP_H
#define _REP_H 1

#include "config.h"

#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <zlib.h>

struct pdbfs_file {
  struct stat pf_stat;
  pthread_mutex_t pf_mutex;
  int pf_size_checked;
};

struct pdbfs {
  struct pdbfs_file** p_fs;
  unsigned int p_file_count;
  struct stat p_root;
  
  pthread_mutex_t p_updater_mutex;
  pthread_cond_t p_updater_cond;
  pthread_t p_updater;
  int p_keep_updating;
};

extern struct pdbfs pdbfs;

void pdbfs_file_init(struct pdbfs_file* pf);
void pdbfs_file_destroy(struct pdbfs_file* pf);

void pdbfs_rep_init();
void pdbfs_rep_destroy();

#endif /* _REP_H */
