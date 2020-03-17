   //=========== (C) Copyright 2000 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//=============================================================================

#if defined( _WIN32 )
#include <windows.h>		// for WideCharToMultiByte and MultiByteToWideChar
#include <direct.h>			// for _mkdir
#else
#include <wchar.h> // wcslen()
#define _alloca alloca
#define _mkdir(dir) mkdir( dir, S_IRWXU | S_IRWXG | S_IRWXO )
#define _snwprintf swprintf
#define _wtoi(arg) wcstol(arg, NULL, 10)
#define _heapchk() 0
#define _HEAPOK 0
#endif

#include <../public/KeyValues.h>
#include "filesystem.h"
#include <../public/IKeyValues.h>

#include <../public/Color.h>
#include <assert.h>
#include <stdlib.h>
#include "tier0/mem.h"
#include "utlvector.h"
#include "utlbuffer.h"
#include "strtools.h"

// memdbgon must be the last include file in a .cpp file!!!
#include <../public/tier0/memdbgon.h>

#define KEYVALUES_TOKEN_SIZE	1024

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
KeyValues::KeyValues( const char *setName )
{
	Init();
	SetName ( setName );
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
KeyValues::KeyValues( const char *setName, const char *firstKey, const char *firstValue )
{
	Init();
	SetName( setName );
	SetString( firstKey, firstValue );
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
KeyValues::KeyValues( const char *setName, const char *firstKey, const wchar_t *firstValue )
{
	Init();
	SetName( setName );
	SetWString( firstKey, firstValue );
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
KeyValues::KeyValues( const char *setName, const char *firstKey, int firstValue )
{
	Init();
	SetName( setName );
	SetInt( firstKey, firstValue );
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
KeyValues::KeyValues( const char *setName, const char *firstKey, const char *firstValue, const char *secondKey, const char *secondValue )
{
	Init();
	SetName( setName );
	SetString( firstKey, firstValue );
	SetString( secondKey, secondValue );
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
KeyValues::KeyValues( const char *setName, const char *firstKey, int firstValue, const char *secondKey, int secondValue )
{
	Init( );
	SetName( setName );
	SetInt( firstKey, firstValue );
	SetInt( secondKey, secondValue );
}

//-----------------------------------------------------------------------------
// Purpose: Initialize member variables
//-----------------------------------------------------------------------------
void KeyValues::Init()
{
	m_iKeyName = INVALID_KEY_SYMBOL;
	m_iDataType = TYPE_NONE;

	m_pSub = NULL;
	m_pPeer = NULL;

	m_pValue = NULL;

	m_iAllocationSize = 0;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
KeyValues::~KeyValues()
{
	KeyValues *dat;
	KeyValues *datNext = NULL;
	for ( dat = m_pSub; dat != NULL; dat = datNext )
	{
		datNext = dat->m_pPeer;
		dat->m_pPeer = NULL;
		delete dat;
	}

	for ( dat = m_pPeer; dat && dat != this; dat = datNext )
	{
		datNext = dat->m_pPeer;
		dat->m_pPeer = NULL;
		delete dat;
	}

	FreeAllocatedValue();
}

//-----------------------------------------------------------------------------
// Purpose: Get the name of the current key section
//-----------------------------------------------------------------------------
const char *KeyValues::GetName( void ) const
{
	return keyvalues()->GetStringForSymbol(m_iKeyName);
}

//-----------------------------------------------------------------------------
// Purpose: Get the symbol name of the current key section
//-----------------------------------------------------------------------------
int KeyValues::GetNameSymbol() const
{
	return m_iKeyName;
}

//-----------------------------------------------------------------------------
// Purpose: Reads a single token from the file
//-----------------------------------------------------------------------------
static char *ReadToken( char **buffer )
{
	static char buf[512];
	int bufC = 0;
	int c = 0;
	bool skipWhitespace = true;

	while (skipWhitespace)
	{
		skipWhitespace = false;

		// skip any whitespace
		do 
		{
			c = **buffer;
			(*buffer)++;
		}
		while ( isspace(c) );

		// skip // comments
		if ( c == '/' )
		{
			c = **buffer;	// read next char
			(*buffer)++;

			if ( c != '/' )
			{
				// push the char back on the stream
				if ( bufC < sizeof(buf) )
				{
					buf[bufC++] = '/';
				}
			}
			else
			{
				while (c > 0 && c != '\n')
				{
					c = **buffer;
					(*buffer)++;
				}
				skipWhitespace = true;
			}
		}
	}

	// read quoted strings specially
	if ( c == '\"' )
	{
		// read the token till we hit the endquote
		while ( 1 )
		{
			c = **buffer;
			(*buffer)++;

			if ( c < 0 )
				break;

			if ( c == '\"' )
				break;

			// check for special chars
			if ( c == '\\' )
			{
				// get the next char
				c = **buffer;
				(*buffer)++;

				switch ( c )
				{
				case 'n':  c = '\n'; break;
				case '\\': c = '\\'; break;
				case 't':  c = '\t'; break;
				case '\"': c = '\"'; break;
				default:   c = c;    break;
				}
			}

			if ( bufC < sizeof(buf) )
			{
				buf[bufC++] = c;
			}
		}
	}
	else
	{
		// read in the token until we hit a whitespace
		while ( 1 )
		{
			if ( c < 0 )
				break;

			if ( isspace(c) )
				break;

			if ( bufC < sizeof(buf) )
			{
				buf[bufC++] = c;
			}

			c = **buffer;
			(*buffer)++;
		}
	}

	// check for EOF
	if ( c < 0 && bufC == 0 )
		return NULL;

	// null terminate
	buf[bufC] = 0;
	return buf;
}

//-----------------------------------------------------------------------------
// Purpose: Load keyValues from disk
//-----------------------------------------------------------------------------

bool KeyValues::LoadFromFile( IBaseFileSystem *filesystem, const char *resourceName, const char *pathID )
{
	assert(filesystem);
	assert(_heapchk() == _HEAPOK);

	FileHandle_t f = filesystem->Open(resourceName, "rb", pathID);
	if (!f)
		return false;

	// load file into a null-terminated buffer
	int fileSize = filesystem->Size(f);
	char *buffer = (char*)malloc(fileSize + 1);
	if (!buffer)
		return false;

	filesystem->Read(buffer, fileSize, f); // read into local buffer

	buffer[fileSize] = 0; // null terminate file as EOF

	filesystem->Close( f );	// close file after reading

	bool retOK = LoadFromBuffer( buffer );

	free(buffer);

	return retOK;
}

//-----------------------------------------------------------------------------
// Read from a buffer...
//-----------------------------------------------------------------------------
bool KeyValues::LoadFromBuffer( const char *buffer )
{
	char *bufStart = const_cast<char *>(buffer);

	// the first thing must be a key
	char *s = ReadToken( &bufStart );
	if ( s )
	{
		int newName = keyvalues()->GetSymbolForString( s );
		m_iKeyName = newName;
	}

	// get the '{'
	s = ReadToken( &bufStart );
	if ( m_iKeyName > -1 && s && *s == '{' )
	{
		// header is valid so load the file
		RecursiveLoadFromBuffer( &bufStart );
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void KeyValues::RecursiveLoadFromBuffer( char **buffer )
{
	while ( 1 )
	{
		// get the key name
		char *s = ReadToken( buffer );
		if ( !s )
			break;
		if ( *s == '}' )
			break;

		KeyValues *dat = FindKey(s, true);

		// get the value
		s = ReadToken( buffer );
		
		if ( !strcmp(s,"{") )
		{
			// sub value list
			dat->RecursiveLoadFromBuffer( buffer );									
		}
		else 
		{
			int len = strlen( s );
			dat->FreeAllocatedValue();
			dat->AllocateValueBlock( len + 1 );
			memcpy( dat->m_sValue, s, len+1 );
			dat->m_iDataType = TYPE_STRING;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Save the keyvalues to disk
//			Creates the path to the file if it doesn't exist 
//-----------------------------------------------------------------------------
bool KeyValues::SaveToFile( IBaseFileSystem *filesystem, const char *resourceName, const char *pathID )
{
	// create a write file
	FileHandle_t f = filesystem->Open(resourceName, "wb", pathID);

	// if the file won't create, make sure the paths are all created
	if (!f)
	{
		const char *currentFileName = resourceName;
		char szBuf[KEYVALUES_TOKEN_SIZE] = "";

		// Creates any necessary paths to create the file
		while (1)
		{
			const char *fileSlash = strchr(currentFileName, '\\');
			if (!fileSlash)
				break;

			// copy out the first string
			int pathSize = fileSlash - currentFileName;
			
			if (szBuf[0] != 0)
			{
				Q_strcat(szBuf, "\\");
			}

			Q_strncat(szBuf, currentFileName, pathSize);

			// make sure the first path is created
			_mkdir(szBuf);

			// increment the parse point to create the next directory (if any)
			currentFileName += (pathSize + 1);
		}

		// try opening the file again
		f = filesystem->Open(resourceName, "wb", pathID);
	}

	if (!f)
		return false;

	CUtlBuffer buffer( 0, 4096, true );
	RecursiveSaveToFile(filesystem, buffer, 0);
	buffer.PutChar( 0 );
	filesystem->Write( buffer.Base(), buffer.TellPut(), f );
	filesystem->Close(f);

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: writes out a set of indenting
// Input  : indentLevel - 
//-----------------------------------------------------------------------------

void KeyValues::WriteIndents( CUtlBuffer& buffer, int indentLevel )
{
	for ( int i = 0; i < indentLevel; i++ )
	{
		buffer.Printf( "\t" );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Write out a string where we convert the double quotes to backslash double quote
//-----------------------------------------------------------------------------
void KeyValues::WriteConvertedString( CUtlBuffer& buffer, const char *pszString )
{
	// handle double quote chars within the string
	// the worst possible case is that the whole string is quotes
	int len = Q_strlen(pszString);
	char *convertedString = (char *) _alloca ((len + 1)  * sizeof(char) * 2);
	int j=0;
	for (int i=0; i <= len; i++)
	{
		if (pszString[i] == '\"')
		{
			convertedString[j] = '\\';
			j++;
		}	
		convertedString[j] = pszString[i];
		j++;
	}		

	buffer.Put(convertedString, strlen(convertedString));
}

//-----------------------------------------------------------------------------
// Purpose: Save keyvalues from disk, if subkey values are detected, calls
//			itself to save those
//-----------------------------------------------------------------------------
void KeyValues::RecursiveSaveToFile( IBaseFileSystem *filesystem, CUtlBuffer& buffer, int indentLevel )
{
	// write header
	WriteIndents( buffer, indentLevel );
	buffer.Put("\"", 1);
	buffer.Put(GetName(), strlen(GetName()));
	buffer.Put("\"\n", 2);
	WriteIndents( buffer, indentLevel );
	buffer.Put("{\n", 2);

	// loop through all our keys writing them to disk
	for ( KeyValues *dat = m_pSub; dat != NULL; dat = dat->m_pPeer )
	{
		if ( dat->m_pSub )
		{
			dat->RecursiveSaveToFile( filesystem, buffer, indentLevel + 1 );
		}
		else
		{
			// only write non-empty keys

			switch (dat->m_iDataType)
			{
			case TYPE_STRING:
				{
					if (dat->m_sValue && *(dat->m_sValue))
					{
						WriteIndents(buffer, indentLevel + 1);
						buffer.Put("\"", 1);
						buffer.Put(dat->GetName(), Q_strlen(dat->GetName()));
						buffer.Put("\"\t\t\"", 4);

						WriteConvertedString(buffer, dat->m_sValue);	

						buffer.Put("\"\n", 2);
					}
					break;
				}
			case TYPE_WSTRING:
				{
					if ( dat->m_wsValue )
					{
						static char buf[KEYVALUES_TOKEN_SIZE];
						keyvalues()->GetANSIFromLocalized(dat->m_wsValue, buf, KEYVALUES_TOKEN_SIZE);

						WriteIndents(buffer, indentLevel + 1);
						buffer.Put("\"", 1);
						buffer.Put(dat->GetName(), Q_strlen(dat->GetName()));
						buffer.Put("\"\t\t\"", 4);

						WriteConvertedString(buffer, buf);

						buffer.Put("\"\n", 2);
					}
					break;
				}

			case TYPE_INT:
				{
					WriteIndents(buffer, indentLevel + 1);
					buffer.Put("\"", 1);
					buffer.Put(dat->GetName(), Q_strlen(dat->GetName()));
					buffer.Put("\"\t\t\"", 4);

					char buf[32];
					Q_snprintf(buf, 32, "%d", dat->m_iValue);

					buffer.Put(buf, Q_strlen(buf));
					buffer.Put("\"\n", 2);
					break;
				}

			case TYPE_FLOAT:
				{
					WriteIndents(buffer, indentLevel + 1);
					buffer.Put("\"", 1);
					buffer.Put(dat->GetName(), Q_strlen(dat->GetName()));
					buffer.Put("\"\t\t\"", 4);

					char buf[48];
					Q_snprintf(buf, 48, "%f", dat->m_flValue);

					buffer.Put(buf, Q_strlen(buf));
					buffer.Put("\"\n", 2);
					break;
				}

			default:
				break;
			}
		}
	}

	// write tail
	WriteIndents(buffer, indentLevel);
	buffer.Put("}\n", 2);
}

//-----------------------------------------------------------------------------
// Purpose: looks up a key by symbol name
//-----------------------------------------------------------------------------
KeyValues *KeyValues::FindKey(int keySymbol) const
{
	for (KeyValues *dat = m_pSub; dat != NULL; dat = dat->m_pPeer)
	{
		if (dat->m_iKeyName == keySymbol)
			return dat;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Find a keyValue, create it if it is not found.
//			Set bCreate to true to create the key if it doesn't already exist 
//			(which ensures a valid pointer will be returned)
//-----------------------------------------------------------------------------
KeyValues *KeyValues::FindKey(const char *keyName, bool bCreate)
{
	// return the current key if a NULL subkey is asked for
	if (!keyName || !keyName[0])
		return this;

	// look for '/' characters deliminating sub fields
	char szBuf[256];
	const char *subStr = strchr(keyName, '/');
	const char *searchStr = keyName;

	// pull out the substring if it exists
	if (subStr)
	{
		int size = subStr - keyName;
		Q_memcpy( szBuf, keyName, size );
		szBuf[size] = 0;
		searchStr = szBuf;
	}

	// lookup the symbol for the search string
	HKeySymbol iSearchStr = keyvalues()->GetSymbolForString(searchStr);

	KeyValues *lastItem = NULL;
	KeyValues *dat;
	// find the searchStr in the current peer list
	for (dat = m_pSub; dat != NULL; dat = dat->m_pPeer)
	{
		lastItem = dat;	// record the last item looked at (for if we need to append to the end of the list)

		// symbol compare
		if (dat->m_iKeyName == iSearchStr)
		{
			break;
		}
	}

	// make sure a key was found
	if (!dat)
	{
		if (bCreate)
		{
			// we need to create a new key
			dat = new KeyValues( searchStr );
//			assert(dat != NULL);

			// insert new key at end of list
			if (lastItem)
			{
				lastItem->m_pPeer = dat;
			}
			else
			{
				m_pSub = dat;
			}
			dat->m_pPeer = NULL;

			// a key graduates to be a submsg as soon as it's m_pSub is set
			// this should be the only place m_pSub is set
			m_iDataType = TYPE_NONE;
		}
		else
		{
			return NULL;
		}
	}
	
	// if we've still got a subStr we need to keep looking deeper in the tree
	if ( subStr )
	{
		// recursively chain down through the paths in the string
		return dat->FindKey(subStr + 1, bCreate);
	}

	return dat;
}

//-----------------------------------------------------------------------------
// Purpose: Create a new key, with an autogenerated name.  
//			Name is guaranteed to be an integer, of value 1 higher than the highest 
//			other integer key name
//-----------------------------------------------------------------------------
KeyValues *KeyValues::CreateNewKey()
{
	int newID = 1;

	// search for any key with higher values
	for (KeyValues *dat = m_pSub; dat != NULL; dat = dat->m_pPeer)
	{
		// case-insensitive string compare
		int val = atoi(dat->GetName());
		if (newID <= val)
		{
			newID = val + 1;
		}
	}

	char buf[12];
	Q_snprintf( buf, sizeof(buf), "%d", newID );

	return FindKey(buf, true);
}

//-----------------------------------------------------------------------------
// Create a key
//-----------------------------------------------------------------------------

void KeyValues::AddSubKey( KeyValues *pSubkey )
{
	// add into subkey list
	if ( m_pSub == NULL )
	{
		m_pSub = pSubkey;
	}
	else
	{
		KeyValues *pTempDat = m_pSub;
		while ( pTempDat->GetNextKey() != NULL )
		{
			pTempDat = pTempDat->GetNextKey();
		}

		pTempDat->SetNextKey( pSubkey );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Sets this key's peer to the KeyValues passed in
//-----------------------------------------------------------------------------
void KeyValues::SetNextKey( KeyValues *pDat )
{
	m_pPeer = pDat;
}

//-----------------------------------------------------------------------------
// Purpose: Remove a subkey from the list
//-----------------------------------------------------------------------------
void KeyValues::RemoveSubKey(KeyValues *subKey)
{
	if (!subKey)
		return;

	// check the list pointer
	if (m_pSub == subKey)
	{
		m_pSub = subKey->m_pPeer;
	}
	else
	{
		// look through the list
		KeyValues *kv = m_pSub;
		while (kv->m_pPeer)
		{
			if (kv->m_pPeer == subKey)
			{
				kv->m_pPeer = subKey->m_pPeer;
				break;
			}
			
			kv = kv->m_pPeer;
		}
	}

	subKey->m_pPeer = NULL;
}



//-----------------------------------------------------------------------------
// Purpose: Return the first subkey in the list
//-----------------------------------------------------------------------------
KeyValues *KeyValues::GetFirstSubKey()
{
	return m_pSub;
}

//-----------------------------------------------------------------------------
// Purpose: Return the next subkey
//-----------------------------------------------------------------------------
KeyValues *KeyValues::GetNextKey()
{
	return m_pPeer;
}

//-----------------------------------------------------------------------------
// Purpose: Get the integer value of a keyName. Default value is returned
//			if the keyName can't be found.
//-----------------------------------------------------------------------------
int KeyValues::GetInt( const char *keyName, int defaultValue )
{
	KeyValues *dat = FindKey( keyName, false );
	if ( dat )
	{
		switch ( dat->m_iDataType )
		{
		case TYPE_STRING:
			return atoi(dat->m_sValue);
		case TYPE_WSTRING:
			return _wtoi(dat->m_wsValue);
		case TYPE_FLOAT:
			return (int)dat->m_flValue;
		case TYPE_UINT64:
			return 0;
		case TYPE_INT:
		case TYPE_PTR:
		default:
			return dat->m_iValue;
		};
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------
// Purpose: Get the integer value of a keyName. Default value is returned
//			if the keyName can't be found.
//-----------------------------------------------------------------------------
int KeyValues::GetUint64( const char *keyName, uint64 defaultValue )
{
	KeyValues *dat = FindKey( keyName, false );
	if ( dat )
	{
		switch ( dat->m_iDataType )
		{
		case TYPE_STRING:
			return atoi(dat->m_sValue);
		case TYPE_WSTRING:
			return _wtoi(dat->m_wsValue);
		case TYPE_FLOAT:
			return (int)dat->m_flValue;
		case TYPE_UINT64:
			return (uint64)dat->m_pValue;
		case TYPE_INT:
		case TYPE_PTR:
		default:
			return dat->m_iValue;
		};
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------
// Purpose: Get the pointer value of a keyName. Default value is returned
//			if the keyName can't be found.
//-----------------------------------------------------------------------------
void *KeyValues::GetPtr( const char *keyName, void *defaultValue )
{
	KeyValues *dat = FindKey( keyName, false );
	if ( dat )
	{
		switch ( dat->m_iDataType )
		{
		case TYPE_PTR:
			return dat->m_pValue;

		case TYPE_WSTRING:
		case TYPE_STRING:
		case TYPE_FLOAT:
		case TYPE_INT:
		default:
			return NULL;
		};
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------
// Purpose: Get the float value of a keyName. Default value is returned
//			if the keyName can't be found.
//-----------------------------------------------------------------------------
float KeyValues::GetFloat( const char *keyName, float defaultValue )
{
	KeyValues *dat = FindKey( keyName, false );
	if ( dat )
	{
		switch ( dat->m_iDataType )
		{
		case TYPE_STRING:
			return (float)atof(dat->m_sValue);
		case TYPE_WSTRING:
			return 0.0f;		// no wtof
		case TYPE_FLOAT:
			return dat->m_flValue;
		case TYPE_INT:
			return (float)dat->m_iValue;
		case TYPE_PTR:
		default:
			return 0.0f;
		};
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------
// Purpose: Get the string pointer of a keyName. Default value is returned
//			if the keyName can't be found.
//-----------------------------------------------------------------------------
const char *KeyValues::GetString( const char *keyName, const char *defaultValue )
{
	KeyValues *dat = FindKey( keyName, false );
	if ( dat )
	{
		// convert the data to string form then return it
		char buf[64];
		switch ( dat->m_iDataType )
		{
		case TYPE_FLOAT:
			Q_snprintf( buf, 64, "%f", dat->m_flValue );
			SetString( keyName, buf );
			break;
		case TYPE_INT:
		case TYPE_PTR:
			Q_snprintf( buf, 64, "%d", dat->m_iValue );
			SetString( keyName, buf );
			break;
		case TYPE_WSTRING:
		{
			// convert the string to char *, set it for future use, and return it
			char buf[512];
			keyvalues()->GetANSIFromLocalized( dat->m_wsValue, buf, sizeof( buf ) );
			SetString( keyName, buf );
			break;
		}
		case TYPE_UINT64:
		{
			Q_snprintf( buf, sizeof( buf ), "%I64i", (uint64)dat->m_pValue );
			SetString( keyName, buf );
			break;
		}
		case TYPE_STRING:
			break;
		default:
			return defaultValue;
		};
		
		return dat->m_sValue;
	}
	return defaultValue;
}

const wchar_t *KeyValues::GetWString( const char *keyName, const wchar_t *defaultValue)
{
	KeyValues *dat = FindKey( keyName, false );
	if ( dat )
	{
		wchar_t wbuf[64];
		switch ( dat->m_iDataType )
		{
		case TYPE_FLOAT:
			_snwprintf(wbuf, sizeof(wbuf), L"%f", dat->m_flValue);
			SetWString( keyName, wbuf);
			break;
		case TYPE_INT:
		case TYPE_PTR:
			_snwprintf( wbuf, sizeof(wbuf), L"%d", dat->m_iValue );
			SetWString( keyName, wbuf );
			break;
		case TYPE_UINT64:
		{
			_snwprintf( wbuf, sizeof( wbuf ), L"%I64i", (uint64)dat->m_pValue );
			SetWString( keyName, wbuf );
			break;
		}
		case TYPE_WSTRING:
			break;
		case TYPE_STRING:
		{
			static wchar_t wbuftemp[512]; // convert to wide	
			keyvalues()->GetLocalizedFromANSI( dat->m_sValue, wbuftemp, sizeof( wbuftemp ) );
			SetWString( keyName, wbuftemp);
			break;
		}
		default:
			return defaultValue;
		};
		
		return (const wchar_t* )dat->m_wsValue;
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------
// Purpose: Gets a color
//-----------------------------------------------------------------------------
Color KeyValues::GetColor( const char *keyName )
{
	Color color(0, 0, 0, 0);
	KeyValues *dat = FindKey( keyName, false );
	if ( dat )
	{
		if ( dat->m_iDataType == TYPE_COLOR )
		{
			color[0] = dat->m_Color[0];
			color[1] = dat->m_Color[1];
			color[2] = dat->m_Color[2];
			color[3] = dat->m_Color[3];
		}
		else if ( dat->m_iDataType == TYPE_FLOAT )
		{
			color[0] = dat->m_flValue;
		}
		else if ( dat->m_iDataType == TYPE_INT )
		{
			color[0] = dat->m_iValue;
		}
		else if ( dat->m_iDataType == TYPE_STRING )
		{
			// parse the colors out of the string
			float a, b, c, d;
			sscanf(dat->m_sValue, "%f %f %f %f", &a, &b, &c, &d);
			color[0] = (unsigned char)a;
			color[1] = (unsigned char)b;
			color[2] = (unsigned char)c;
			color[3] = (unsigned char)d;
		}
	}
	return color;
}

//-----------------------------------------------------------------------------
// Purpose: Sets a color
//-----------------------------------------------------------------------------
void KeyValues::SetColor( const char *keyName, Color value)
{
	KeyValues *dat = FindKey( keyName, true );

	if ( dat )
	{
		dat->FreeAllocatedValue();
		dat->m_iDataType = TYPE_COLOR;
		dat->m_Color[0] = value[0];
		dat->m_Color[1] = value[1];
		dat->m_Color[2] = value[2];
		dat->m_Color[3] = value[3];
	}
}

//-----------------------------------------------------------------------------
// Purpose: Set the string value of a keyName. 
//-----------------------------------------------------------------------------
void KeyValues::SetString( const char *keyName, const char *value )
{
	KeyValues *dat = FindKey( keyName, true );

	if ( dat )
	{
		dat->FreeAllocatedValue();

		if (!value)
		{
			// ensure a valid value
			value = "";
		}

		// allocate memory for the new value and copy it in
		int len = Q_strlen( value );
		dat->AllocateValueBlock( len + 1 );
		Q_memcpy( dat->m_sValue, value, len+1 );

		dat->m_iDataType = TYPE_STRING;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Set the string value of a keyName. 
//-----------------------------------------------------------------------------
void KeyValues::SetWString( const char *keyName, const wchar_t *value )
{
	KeyValues *dat = FindKey( keyName, true );

	if ( dat )
	{
		dat->FreeAllocatedValue();

		if (!value)
		{
			// ensure a valid value
			value = L"";
		}

		// allocate memory for the new value and copy it in
		int len = wcslen( value );
		dat->AllocateValueBlock( ( len + 1 ) * sizeof( wchar_t ) );
		Q_memcpy( dat->m_wsValue, value, (len+1) * sizeof(wchar_t) );

		dat->m_iDataType = TYPE_WSTRING;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Set the integer value of a keyName. 
//-----------------------------------------------------------------------------
void KeyValues::SetInt( const char *keyName, int value )
{
	KeyValues *dat = FindKey( keyName, true );

	if ( dat )
	{
		dat->FreeAllocatedValue();

		dat->m_iValue = value;
		dat->m_iDataType = TYPE_INT;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Set the integer value of a keyName. 
//-----------------------------------------------------------------------------
void KeyValues::SetUint64( const char *keyName, uint64 value )
{
	KeyValues *dat = FindKey( keyName, true );

	if ( dat )
	{
		dat->FreeAllocatedValue();
		dat->AllocateValueBlock( sizeof( uint64 ) );

		*(uint64 *)dat->m_pValue = value;
		dat->m_iDataType = TYPE_UINT64;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Set the float value of a keyName. 
//-----------------------------------------------------------------------------
void KeyValues::SetFloat( const char *keyName, float value )
{
	KeyValues *dat = FindKey( keyName, true );

	if ( dat )
	{
		dat->FreeAllocatedValue();

		dat->m_flValue = value;
		dat->m_iDataType = TYPE_FLOAT;
	}
}

void KeyValues::SetName( const char * setName )
{
	m_iKeyName = keyvalues()->GetSymbolForString( setName );
}

//-----------------------------------------------------------------------------
// Purpose: Set the pointer value of a keyName. 
//-----------------------------------------------------------------------------
void KeyValues::SetPtr( const char *keyName, void *value )
{
	KeyValues *dat = FindKey( keyName, true );

	if ( dat )
	{
		dat->FreeAllocatedValue();

		dat->m_pValue = value;
		dat->m_iDataType = TYPE_PTR;
	}
}

void KeyValues::AllocateValueBlock( int iAllocSize )
{
	if ( iAllocSize <= sizeof( KeyValues ) )
	{
		m_sValue = (char *)keyvalues()->AllocKeyValuesMemory( iAllocSize );
	}
	else
	{
		m_sValue = new char [iAllocSize];
	}

	m_iAllocationSize = iAllocSize;
}

void KeyValues::FreeAllocatedValue( void )
{
	if ( m_iAllocationSize != 0 )
	{
		if ( m_iAllocationSize <= sizeof( KeyValues ) )
		{
			keyvalues()->FreeKeyValuesMemory( m_sValue );
		}
		else
		{
			if ( m_sValue != NULL )
				delete [] m_sValue;
		}

		m_iAllocationSize = 0;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Makes a copy of the whole key-value pair set
//-----------------------------------------------------------------------------
KeyValues *KeyValues::MakeCopy( void ) const
{
	KeyValues *newKeyValue = new KeyValues(GetName());

	// copy data
	newKeyValue->m_iDataType = m_iDataType;
	switch ( m_iDataType )
	{
	case TYPE_STRING:
		{
			if ( m_sValue )
			{
				int len = Q_strlen( m_sValue );
				assert( !newKeyValue->m_sValue );
				newKeyValue->AllocateValueBlock( len + 1 );
				Q_memcpy( newKeyValue->m_sValue, m_sValue, len+1 );
			}
		}
		break;
	case TYPE_WSTRING:
		{
			if ( m_wsValue )
			{
				int len = wcslen( m_wsValue );
				newKeyValue->AllocateValueBlock( ( len + 1 ) * sizeof( wchar_t ) );
				Q_memcpy( newKeyValue->m_wsValue, m_wsValue, (len+1)*sizeof(wchar_t));
			}
		}
		break;

	case TYPE_UINT64:
		newKeyValue->AllocateValueBlock( sizeof( uint64 ) );
		*(uint64 *)newKeyValue->m_pValue = *(uint64 *)m_pValue;
		break;

	case TYPE_INT:
		newKeyValue->m_iValue = m_iValue;
		break;

	case TYPE_FLOAT:
		newKeyValue->m_flValue = m_flValue;
		break;

	case TYPE_PTR:
		newKeyValue->m_pValue = m_pValue;
		break;
		
	case TYPE_COLOR:
		newKeyValue->m_Color[0] = m_Color[0];
		newKeyValue->m_Color[1] = m_Color[1];
		newKeyValue->m_Color[2] = m_Color[2];
		newKeyValue->m_Color[3] = m_Color[3];
		break;

	};

	// recursively copy subkeys
	// Also maintain ordering....
	KeyValues *pPrev = NULL;
	for ( KeyValues *sub = m_pSub; sub != NULL; sub = sub->m_pPeer )
	{
		// take a copy of the subkey
		KeyValues *dat = sub->MakeCopy();
		 
		// add into subkey list
		if (pPrev)
		{
			pPrev->m_pPeer = dat;
		}
		else
		{
			newKeyValue->m_pSub = dat;
		}
		dat->m_pPeer = NULL;
		pPrev = dat;
	}


	return newKeyValue;
}

//-----------------------------------------------------------------------------
// Purpose: Check if a keyName has no value assigned to it.
//-----------------------------------------------------------------------------
bool KeyValues::IsEmpty(const char *keyName)
{
	KeyValues *dat = FindKey(keyName, false);
	if (!dat)
		return true;

	if (dat->m_iDataType == TYPE_NONE && dat->m_pSub == NULL)
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Clear out all subkeys, and the current value
//-----------------------------------------------------------------------------
void KeyValues::Clear( void )
{
	delete m_pSub;
	m_pSub = NULL;
	FreeAllocatedValue();
	m_iDataType = TYPE_NONE;
}

//-----------------------------------------------------------------------------
// Purpose: Get the data type of the value stored in a keyName
//-----------------------------------------------------------------------------
KeyValues::types_t KeyValues::GetDataType(const char *keyName)
{
	KeyValues *dat = FindKey(keyName, false);
	if (dat)
		return (types_t)dat->m_iDataType;

	return TYPE_NONE;
}

//-----------------------------------------------------------------------------
// Purpose: Deletion, ensures object gets deleted from correct heap
//-----------------------------------------------------------------------------
void KeyValues::deleteThis()
{
	delete this;
}

#include "tier0/memdbgoff.h"

//-----------------------------------------------------------------------------
// Purpose: memory allocator
//-----------------------------------------------------------------------------
void *KeyValues::operator new( size_t iAllocSize )
{
	return keyvalues()->AllocKeyValuesMemory(iAllocSize);
}

void *KeyValues::operator new( size_t iAllocSize, int nBlockUse, const char *pFileName, int nLine )
{
	return keyvalues()->AllocKeyValuesMemory(iAllocSize);
}

//-----------------------------------------------------------------------------
// Purpose: deallocator
//-----------------------------------------------------------------------------
void KeyValues::operator delete( void *pMem )
{
	keyvalues()->FreeKeyValuesMemory(pMem);
}

#include "tier0/memdbgon.h"
