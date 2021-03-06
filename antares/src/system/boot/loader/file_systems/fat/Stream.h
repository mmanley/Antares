/*
 * Copyright 2008, Antares, Inc.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		François Revol <revol@free.fr>
 */
#ifndef STREAM_H
#define STREAM_H

#include "fatfs.h"
#include "Volume.h"

#include <sys/stat.h>

#define CLUSTER_MAP_CACHE_SIZE 50

class Node;

struct file_map_run;

namespace FATFS {

class Stream {
	public:
		Stream(Volume &volume, uint32 chain, off_t size, const char *name);
		~Stream();

		status_t InitCheck();
		Volume &GetVolume() const { return fVolume; }

		status_t GetName(char *nameBuffer, size_t bufferSize) const;
		status_t GetFileMap(struct file_map_run *runs, int32 *count);
		off_t	Size() const { return fSize; }
		uint32	FirstCluster() const { return fFirstCluster; }

		status_t ReadAt(off_t pos, uint8 *buffer, size_t *length);

	private:
		status_t		BuildClusterList();
		status_t		FindBlock(off_t pos, off_t &block, off_t &offset);
		Volume			&fVolume;
		uint32			fFirstCluster;
		uint32			fClusterCount;
		//uint32			*fClusters; // [fClusterCount]
		struct {
			off_t block;
			uint32 cluster;
		}			fClusterMapCache[CLUSTER_MAP_CACHE_SIZE];
		int			fClusterMapCacheLast;
		off_t			fSize;
		// we cache the name here, since FAT doesn't have inodes,
		// let alone names inside.
		char			fName[FATFS_NAME_LENGTH+1];
};

}	// namespace FATFS

#endif	/* STREAM_H */
