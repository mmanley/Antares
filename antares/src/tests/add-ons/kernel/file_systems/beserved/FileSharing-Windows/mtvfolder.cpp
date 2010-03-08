// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


#include "stdafx.h"
#include "mtvfolder.h"


/////////////////////////////////////////////////////////////////////////////
// CMTVFolder properties

/////////////////////////////////////////////////////////////////////////////
// CMTVFolder operations

long CMTVFolder::GetHandle()
{
	long result;
	InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

CString CMTVFolder::GetDisplayName()
{
	CString result;
	InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString CMTVFolder::GetPathName()
{
	CString result;
	InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

VARIANT CMTVFolder::GetFolders()
{
	VARIANT result;
	InvokeHelper(0x7, DISPATCH_PROPERTYGET, VT_VARIANT, (void*)&result, NULL);
	return result;
}

CString CMTVFolder::GetShareName()
{
	CString result;
	InvokeHelper(0x8, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString CMTVFolder::GetName()
{
	CString result;
	InvokeHelper(0x9, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

BOOL CMTVFolder::GetCheck()
{
	BOOL result;
	InvokeHelper(0xa, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void CMTVFolder::SetCheck(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0xa, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

LPUNKNOWN CMTVFolder::GetShellFolder()
{
	LPUNKNOWN result;
	InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_UNKNOWN, (void*)&result, NULL);
	return result;
}

long CMTVFolder::GetAttributes(long dwMask)
{
	long result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xc, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms,
		dwMask);
	return result;
}

BOOL CMTVFolder::GetAttribute(long anAttribute)
{
	BOOL result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xd, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, parms,
		anAttribute);
	return result;
}