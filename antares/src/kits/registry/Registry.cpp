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
	File: Registry.cpp
	Definition: Registry Management
*/

#include <Registry.h>
#include <stdio.h>

int callback(void *NotUsed, int argc, char **argv, char **azColName);
int nullcallback(void *NotUsed, int argc, char **argv, char **azColName);
BString tempObject;
BString sqlCommand;
int rc;

BRegKey::BRegKey() {
	//Do Nothing
}

BRegKey::~BRegKey() {
	rName.~BString();
	rValue.~BString();
	rPath.~BString();
}

BRegistry::BRegistry(REG_PATH path) {
	//Open the Registry Database
	if(path == GLOBAL_REG) sqlite3_open("/boot/system/config/localmachine", &db);
	if(path == USER_REG) sqlite3_open("/boot/home/config/.registry", &db);
}

BRegistry::~BRegistry() {
	sqlite3_close(db);
}

BRegKey BRegistry::getRegKey(BString rPath, BString rName) {
	//Turn the Registry Path into its SQLite3 DB Table Name
	BString path = BString(rPath);
	path.ReplaceAll("/", "_");
	path.ReplaceAll(" ", "_");
	path.ReplaceAll("-", "_");
	
	//Now create a BRegKey object
	BRegKey key = BRegKey();
	
	//Now lets get us the data we need
	sqlCommand = BString("select value from @path where key='@key';");
	sqlCommand.ReplaceAll("@path", path.String());
	sqlCommand.ReplaceAll("@key", rName.String());
	rc = sqlite3_exec(db, sqlCommand.String(), callback, 0, NULL);

	//Now that we got that LETS SET ARE BRegKey
	key.rName = BString(rName);
	key.rValue = BString(tempObject);
	key.rPath = BString(rPath);

	//Now return it
	return key;
}

status_t BRegistry::setRegKey(BRegKey rKey) {
	BString rep = BString(rKey.rPath);
	rep.ReplaceAll("/", "_");
	rep.ReplaceAll(" ", "_");
	rep.ReplaceAll("-", "_");

	sqlCommand = BString("UPDATE @table SET value='@value' WHERE key='@key';");
	sqlCommand.ReplaceAll("@table", rep.String());
	sqlCommand.ReplaceAll("@key", rKey.rName.String());
	sqlCommand.ReplaceAll("@value", rKey.rValue.String());
	rc = sqlite3_exec(db, sqlCommand.String(), nullcallback, 0 , NULL);

	if (rc != SQLITE_OK) {
		return B_OK;
	} else {
		return B_ERROR;
	}
}

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
	tempObject = BString(argv[0] ? argv[0] : "NULL");
	return 0;
}
int nullcallback(void *NotUsed, int argc, char **argv, char **azColName) { return 0; }
