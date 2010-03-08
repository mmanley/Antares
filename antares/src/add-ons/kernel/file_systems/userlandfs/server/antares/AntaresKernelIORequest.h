/*
 * Copyright 2009, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef USERLAND_FS_ANTARES_KERNEL_IO_REQUEST_H
#define USERLAND_FS_ANTARES_KERNEL_IO_REQUEST_H

#include <fs_interface.h>

#include <util/OpenHashTable.h>

#include "../IORequestInfo.h"


namespace UserlandFS {

class AntaresKernelVolume;

struct AntaresKernelIORequest : IORequestInfo {

	AntaresKernelVolume*		volume;
	int32					refCount;
	AntaresKernelIORequest*	hashLink;

	AntaresKernelIORequest(AntaresKernelVolume* volume, const IORequestInfo& info)
		:
		IORequestInfo(info),
		volume(volume),
		refCount(1)
	{
	}
};

struct AntaresKernelIterativeFDIOCookie {
	int						fd;
	AntaresKernelIORequest*	request;
	iterative_io_get_vecs	getVecs;
	iterative_io_finished	finished;
	void*					cookie;

	AntaresKernelIterativeFDIOCookie(int fd, AntaresKernelIORequest* request,
		iterative_io_get_vecs getVecs, iterative_io_finished finished,
		void* cookie)
		:
		fd(fd),
		request(request),
		getVecs(getVecs),
		finished(finished),
		cookie(cookie)
	{
	}
};

}	// namespace UserlandFS


using UserlandFS::AntaresKernelIORequest;
using UserlandFS::AntaresKernelIterativeFDIOCookie;


#endif	// USERLAND_FS_ANTARES_KERNEL_IO_REQUEST_H
