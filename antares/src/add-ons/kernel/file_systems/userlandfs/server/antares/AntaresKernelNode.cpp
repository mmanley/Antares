/*
 * Copyright 2009, Ingo Weinhold, ingo_weinhold@gmx.de.
 * Distributed under the terms of the MIT License.
 */

#include "AntaresKernelNode.h"

#include "AntaresKernelFileSystem.h"


AntaresKernelNode::~AntaresKernelNode()
{
	if (capabilities != NULL) {
		AntaresKernelFileSystem* fileSystem
			= static_cast<AntaresKernelFileSystem*>(FileSystem::GetInstance());
		fileSystem->PutNodeCapabilities(capabilities);
	}
}
