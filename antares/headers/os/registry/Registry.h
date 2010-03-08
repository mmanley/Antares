/****************************************************************************
Alustead Antares
Copyright 2009-2010 Alustead Systems, a Division of NasuTek-Alliant Enterprises

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*****************************************************************************/

/*
	File: Registry.h
	Definition: Registry Management
*/

#ifndef REGISTRY_H
#define REGISTRY_H

#include <akey.h>
#include <sqlite3.h>
#include <String.h>

/**	@class BRegKey
	@brief A Antares Registry Key
*/
class BRegKey {
	public:
		/**	@var rName
			@brief The Key's Name
		*/
		BString rName;
		/**	@var rValue
			@brief The Key's Value
		*/
		BString rValue;
		/**	@var rPath
			@brief The Path of the Key in non DB Terms (ex AKEY_LOCAL_MACHINE/Path/To/Key)
		*/
		BString rPath;
		/**	@fn BRegKey::BRegKey()
			@brief BRegKey Constructor
		*/
		BRegKey();
		/**	@fn BRegKey::~BRegKey()
			@brief BRegKey Destructor
		*/
		~BRegKey();
};

/**	@class BRegistry
	@brief Antares Registry Management System
*/
class BRegistry {
	public:
		/**	@fn BRegKey BRegistry::getRegKey(BString rPath, BString rName)
			@brief Returns a BRegKey from the Registry Database
			@param rPath Registry Path (ex. AKEY_LOCAL_MACHINE/Software/Alustead Systems)
			@param rName The Key's Name
		*/
		BRegKey getRegKey(BString rPath, BString rName);
		/**	@fn BRegistry::getRegKey(BRegKey rKey)
			@brief Sets the key in registry from its BRegKey
			@param rKey The BRegKey Object you want to write to the registry with
		*/
		status_t setRegKey(BRegKey rKey);
		/**	@fn BRegistry::BRegistry()
			@brief BRegistry Constructor
		*/
		BRegistry(REG_PATH path);
		/**	@fn BRegistry::~BRegistry()
			@brief BRegistry Destructor
		*/
		~BRegistry();
	private:
		int mode;
		sqlite3 *db;
};

#endif /* REGISTRY_H */
