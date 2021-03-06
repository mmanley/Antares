/*
 * Copyright 2004-2007, Antares, Inc. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Andrew McCall <mccall@@digitalparadise.co.uk>
 *		Mike Berg <mike@berg-net.us>
 *		Julun <host.antares@gmx.de>
 *		Philippe Saint-Pierre <stpere@gmail.com>
 */
#ifndef DATE_TIME_VIEW_H
#define DATE_TIME_VIEW_H


#include <View.h>


class TDateEdit;
class TTimeEdit;
class BRadioButton;
class TAnalogClock;


namespace BPrivate {
	class BCalendarView;
}
using BPrivate::BCalendarView;


class DateTimeView : public BView {
	public:
						DateTimeView(BRect frame);
		virtual 		~DateTimeView();

		virtual void 	AttachedToWindow();
		virtual void 	Draw(BRect updaterect);
		virtual void 	MessageReceived(BMessage *message);

		bool			CheckCanRevert();
		bool			GetUseGmtTime();

	private:
		void 			_InitView();
		void 			_ReadRTCSettings();
		void			_WriteRTCSettings();
		void			_UpdateGmtSettings();
		void 			_UpdateDateTime(BMessage *message);
		void			_Revert();
		time_t			_PrefletUptime() const;

	private:
		BRadioButton 	*fLocalTime;
		BRadioButton 	*fGmtTime;
		TDateEdit 		*fDateEdit;
		TTimeEdit 		*fTimeEdit;
		BCalendarView 	*fCalendarView;
		TAnalogClock 	*fClock;

		bool			fUseGmtTime;
		bool			fOldUseGmtTime;
		bool			fInitialized;

		time_t			fTimeAtStart;
		bigtime_t		fSystemTimeAtStart;
};

#endif	// DATE_TIME_VIEW_H

