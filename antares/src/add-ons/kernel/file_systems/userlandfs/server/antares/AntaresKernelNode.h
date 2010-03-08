/*
 * Copyright 2009, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */
#ifndef USERLAND_FS_ANTARES_KERNEL_NODE_H
#define USERLAND_FS_ANTARES_KERNEL_NODE_H

#include <fs_interface.h>
#include <SupportDefs.h>

#include <util/OpenHashTable.h>

#include "FSCapabilities.h"


namespace UserlandFS {

class AntaresKernelVolume;

using UserlandFSUtil::FSVNodeCapabilities;


struct AntaresKernelNode : fs_vnode {
	struct Capabilities;

			ino_t				id;
			AntaresKernelVolume*	volume;
			Capabilities*		capabilities;
			bool				published;

public:
	inline						AntaresKernelNode(AntaresKernelVolume* volume,
									ino_t vnodeID, void* privateNode,
									fs_vnode_ops* ops,
									Capabilities* capabilities);
								~AntaresKernelNode();

	static	AntaresKernelNode*	GetNode(fs_vnode* node);

			AntaresKernelVolume*	GetVolume() const	{ return volume; }
};


struct AntaresKernelNode::Capabilities {
	int32				refCount;
	fs_vnode_ops*		ops;
	FSVNodeCapabilities	capabilities;
	Capabilities*		hashLink;

	Capabilities(fs_vnode_ops* ops, FSVNodeCapabilities	capabilities)
		:
		refCount(1),
		ops(ops),
		capabilities(capabilities)
	{
	}
};


AntaresKernelNode::AntaresKernelNode(AntaresKernelVolume* volume, ino_t vnodeID,
	void* privateNode, fs_vnode_ops* ops, Capabilities* capabilities)
	:
	id(vnodeID),
	volume(volume),
	capabilities(capabilities),
	published(false)
{
	this->private_node = privateNode;
	this->ops = ops;
}


/*static*/ inline AntaresKernelNode*
AntaresKernelNode::GetNode(fs_vnode* node)
{
	return static_cast<AntaresKernelNode*>(node);
}


}	// namespace UserlandFS

using UserlandFS::AntaresKernelNode;

#endif	// USERLAND_FS_NODE_H
