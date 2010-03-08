/*
Author:	Jerome LEVEQUE
Email:	jerl1@caramail.com
*/
#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <View.h>
#include <Midi.h>

//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------

class Activity : public BMidi, public BView 
{
public:
	Activity(BRect rect);
	
	virtual void AttachedToWindow(void);
	virtual void Draw(BRect rect);
	virtual void Pulse(void);
	virtual void NoteOn(uchar channel, uchar note, uchar velocity, uint32 time = B_NOW);
	
private:
	bigtime_t fActivity[16]; //16 channels


};
//--------------
//--------------
//--------------
//--------------
//--------------
//--------------
//--------------
//--------------
//--------------
//--------------




#endif