// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
//
//	Copyright (c) 2004, Antares
//
//  This software is part of the Antares distribution and is covered 
//  by the Antares license.
//
//
//  File:        Keymap.h
//  Author:      Jérôme Duval
//  Description: Keyboard input server addon
//  Created :    September 7, 2004
// 
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

#ifndef KEYMAP_H
#define KEYMAP_H

#include <InterfaceDefs.h>
#include <Entry.h>

class Keymap 
{
public:
	Keymap();
	~Keymap();
	void DumpKeymap();
	bool IsModifierKey(uint32 keyCode);
	uint32 Modifier(uint32 keyCode);
	uint32 KeyForModifier(uint32 modifier);
	uint8 IsDeadKey(uint32 keyCode, uint32 modifiers);
	bool IsDeadSecondKey(uint32 keyCode, uint32 modifiers, uint8 activeDeadKey);
	void GetChars(uint32 keyCode, uint32 modifiers, uint8 activeDeadKey, char** chars, int32* numBytes);
	uint32 Locks() { return fKeys.lock_settings; };
	status_t LoadCurrent();
private:
	char *fChars;
	key_map fKeys;
	ssize_t fCharsSize;
};


#endif //KEYMAP_H