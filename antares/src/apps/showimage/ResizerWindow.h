/*
 * Copyright 2006-2009, Antares, Inc. All Rights Reserved.
 * Copyright 2004-2005 yellowTAB GmbH. All Rights Reserverd.
 * Copyright 2006 Bernd Korz. All Rights Reserved
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		yellowTAB GmbH
 *		Bernd Korz
 *		Michael Pfeiffer
 *		Ryan Leavengood
 */
#ifndef RESIZER_WINDOW_H
#define RESIZER_WINDOW_H


#include <Messenger.h>
#include <View.h>
#include <Window.h>

class BCheckBox;
class BControl;
class BTextControl;


class ResizerWindow : public BWindow {
	public:			
						ResizerWindow(BMessenger target, int32 width,
							int32 height );

		virtual void	MessageReceived(BMessage* msg);
		virtual	bool	QuitRequested();

		virtual			~ResizerWindow();

		// the public message command constants ('what')
		enum {
			kActivateMsg = 'RSRa',
				// activates the window
			kUpdateMsg,
				// provides the new size of the image in two "int32" fields
				// "width" and "height"
		};

	private:
		enum {
			kResolutionMsg = 'Rszr',
			kWidthModifiedMsg,
			kHeightModifiedMsg,
			kApplyMsg,
		};

		void			AddControl(BView* parent, BControl* control,
							float column2, BRect& rect);
		void			AddSeparatorLine(BView* parent, BRect& rect);
		void			LeftAlign(BControl* control);

		BTextControl*	fWidth;
		BTextControl*	fHeight;
		BCheckBox*		fAspectRatio;
		BButton*		fApply;
		// the original size of the image use for aspect ratio calculation
		// to avoid rounding errors
		int32			fOriginalWidth;
		int32			fOriginalHeight;
		BMessenger		fTarget;
};


#endif	// RESIZER_WINDOW_H
