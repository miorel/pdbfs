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
#include "updater.h"
#include "util.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>
#include <time.h>

#define UPDATE_TOLERANCE ((time_t) (60 * 60 * 24 * 7))

#define BUF_SIZE 256
static void* pdbfs_updater_main(void* arg) {
  FILE* rsync;
  char line[BUF_SIZE];
  char pdbid[5];
  struct tm tms;
  int num;
  struct stat* st;
  struct pdbfs_file* pf;
  int i;
  off_t size;
  
  struct timespec tmspec = { .tv_sec = 0, .tv_nsec = 0 };
  
  do {
    time(&(tmspec.tv_sec));
    tmspec.tv_sec += UPDATE_TOLERANCE;
    rsync = popen(RSYNC " -az --list-only --port=33444 "
		  "rsync.wwpdb.org::ftp_data/structures/divided/pdb", "r");
    while(fgets(line, BUF_SIZE, rsync) != NULL) if((*line) != 'd') {
	sscanf(line + 10, "%d %d/%d/%d %d:%d:%d pdb/%*2s/pdb"
	       "%" PDB_ID_LEN_S "s.ent.gz", &size,
	       &(tms.tm_year), &(tms.tm_mon), &(tms.tm_mday),
	       &(tms.tm_hour), &(tms.tm_min), &(tms.tm_sec), pdbid);
	if(pdbid_to_num(pdbid, &num) && pdbfs.p_fs[num] == NULL) {
	  tms.tm_year -= 1900;
	  tms.tm_mon -= 1;
	  
	  /* This could probably go outside while loop, but it's bad to make
	     assumptions about how mktime() works. */
	  tms.tm_isdst = -1;
	  
	  pf = (struct pdbfs_file*) malloc(sizeof(struct pdbfs_file));
	  pdbfs_file_init(pf);
	  
	  pf->pf_stat.st_atime = pf->pf_stat.st_mtime = pf->pf_stat.st_ctime
	    = mktime(&tms);
	  pf->pf_stat.st_size = size * 20;
	  pf->pf_stat.st_ino = i;
	  
	  pdbfs.p_fs[num] = pf;
	  ++pdbfs.p_file_count;
	}
      }
    pclose(rsync);
    
    pthread_mutex_lock(&(pdbfs.p_updater_mutex));
    while(pdbfs.p_keep_updating && (tmspec.tv_sec > time(NULL)))
      pthread_cond_timedwait(&(pdbfs.p_updater_cond),
			     &(pdbfs.p_updater_mutex), &tmspec);
    pthread_mutex_unlock(&(pdbfs.p_updater_mutex));
    
  } while(pdbfs.p_keep_updating);
}

void pdbfs_updater_init() {
  if(pdbfs.p_root.st_uid == 0) {
    mkdir("/var/tmp/" PACKAGE_NAME, 0755);
    chdir("/var/tmp/" PACKAGE_NAME);
  }
  else {
    chdir(getpwuid(pdbfs.p_root.st_uid)->pw_dir);
    mkdir("." PACKAGE_NAME, 0755);
    chdir("." PACKAGE_NAME);
  }
  mkdir("cache", 0755);
  chdir("cache");
  
  pdbfs.p_keep_updating = 1;
  pthread_mutex_init(&(pdbfs.p_updater_mutex), NULL);
  pthread_cond_init(&(pdbfs.p_updater_cond), NULL);
  
  pthread_create(&(pdbfs.p_updater), NULL, pdbfs_updater_main, NULL);
}

void pdbfs_updater_destroy() {
  pthread_mutex_lock(&(pdbfs.p_updater_mutex));
  pdbfs.p_keep_updating = 0;
  pthread_cond_signal(&(pdbfs.p_updater_cond));
  pthread_mutex_unlock(&(pdbfs.p_updater_mutex));
  pthread_join(pdbfs.p_updater, NULL);
  pthread_mutex_destroy(&(pdbfs.p_updater_mutex));
  pthread_cond_destroy(&(pdbfs.p_updater_cond));
}
