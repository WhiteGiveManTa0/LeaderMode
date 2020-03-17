//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#ifndef IKEYVALUES_H
#define IKEYVALUES_H
#ifdef _WIN32
#pragma once
#endif

#include "interface.h"

// handle to a KeyValues key name symbol
typedef int HKeySymbol;
#define INVALID_KEY_SYMBOL (-1)

//-----------------------------------------------------------------------------
// Purpose: Interface to shared data repository for KeyValues (included in vgui_controls.lib)
//			allows for central data storage point of KeyValues symbol table
//-----------------------------------------------------------------------------
class IKeyValues : public IBaseInterface
{
public:
	// registers the size of the KeyValues in the specified instance
	// so it can build a properly sized memory pool for the KeyValues objects
	// the sizes will usually never differ but this is for versioning safety
	virtual void RegisterSizeofKeyValues(int size) = 0;

	// allocates/frees a KeyValues object from the shared mempool
	virtual void *AllocKeyValuesMemory(int size) = 0;
	virtual void FreeKeyValuesMemory(void *pMem) = 0;

	// symbol table access (used for key names)
	virtual HKeySymbol GetSymbolForString(const char *name) = 0;
	virtual const char *GetStringForSymbol(HKeySymbol symbol) = 0;

	// returns the wide version of ansi, also does the lookup on #'d strings
	virtual void GetLocalizedFromANSI( const char *ansi, wchar_t *outBuf, int unicodeBufferSizeInBytes) = 0;
	virtual void GetANSIFromLocalized( const wchar_t *wchar, char *outBuf, int ansiBufferSizeInBytes ) = 0;

    // for debugging, adds KeyValues record into global list so we can track memory leaks
    virtual void AddKeyValuesToMemoryLeakList(void *pMem, HKeySymbol name) = 0;
    virtual void RemoveKeyValuesFromMemoryLeakList(void *pMem) = 0;
};

IKeyValues *keyvalues();

#define KEYVALUES_INTERFACE_VERSION "KeyValues003"

#endif // IKEYVALUES_H
