/*
 * Copyright 2004-2009 Antares Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Jérôme Duval
 *		Axel Dörfler, axeld@pinc-software.de.
 */
#ifndef KEYMAP_H
#define KEYMAP_H


#include <InterfaceDefs.h>
#include <Entry.h>
#include <Messenger.h>
#include <String.h>


enum dead_key_index {
	kDeadKeyAcute = 1,
	kDeadKeyGrave,
	kDeadKeyCircumflex,
	kDeadKeyDiaeresis,
	kDeadKeyTilde
};


class Keymap {
public:
							Keymap();
							~Keymap();

			void			SetTarget(BMessenger target,
								BMessage* modificationMessage);

			status_t		Load(entry_ref& ref);
			status_t		Save(entry_ref& ref);

			void			DumpKeymap();

			bool			IsModifierKey(uint32 keyCode);
			uint32			Modifier(uint32 keyCode);
			uint32			KeyForModifier(uint32 modifier);
			status_t		SetModifier(uint32 keyCode, uint32 modifier);

			uint8			IsDeadKey(uint32 keyCode, uint32 modifiers,
								bool* isEnabled = NULL);
			bool			IsDeadSecondKey(uint32 keyCode, uint32 modifiers,
								uint8 activeDeadKey);
			void			SetDeadKeyEnabled(uint32 keyCode, uint32 modifiers,
								bool enabled);
			void			GetDeadKeyTrigger(dead_key_index deadKeyIndex,
								BString& outTrigger);
			void			SetDeadKeyTrigger(dead_key_index deadKeyIndex,
								const BString& trigger);

			void			GetChars(uint32 keyCode, uint32 modifiers,
								uint8 activeDeadKey, char** chars,
								int32* numBytes);
			status_t		Use();
			bool			Equals(const Keymap& map) const;

			void			SetKey(uint32 keyCode, uint32 modifiers,
								int8 deadKey, const char* bytes,
								int32 numBytes = -1);

			void			SetName(const char* name);

			const key_map&	Map() const { return fKeys; }
			key_map&		Map() { return fKeys; }

			Keymap&			operator=(const Keymap& other);

private:
			int32			_Offset(uint32 keyCode, uint32 modifiers,
								uint32* _table = NULL);
			bool			_SetChars(int32 offset, const char* bytes,
								int32 numBytes);
			uint8			_GetDeadKeyIndex(int32 offset);

			char*			fChars;
			key_map			fKeys;
			uint32			fCharsSize;
			char			fName[B_FILE_NAME_LENGTH];

			BMessenger		fTarget;
			BMessage*		fModificationMessage;
};


#endif	// KEYMAP_H