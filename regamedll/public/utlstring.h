//====== Copyright ?1996-2004, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef UTLSTRING_H
#define UTLSTRING_H
#ifdef _WIN32
#pragma once
#endif


#include "utlmemory.h"


//-----------------------------------------------------------------------------
// Simple string class. 
// NOTE: This is *not* optimal! Use in tools, but not runtime code
//-----------------------------------------------------------------------------
class CUtlString
{
public:
	CUtlString();
	CUtlString( const char *pString );
	CUtlString( const CUtlString& string );
	~CUtlString();

	bool		IsValid();
	const char	*Get( ) const;
	void		Set( const char *pValue );
	void		Append( const char *pValue );
	void		SetValue( const char *pValue ) { Set( pValue ); }

	void Clear() { Set( NULL ); }

	// Converts to c-strings
	operator const char*() const;

	// for compatibility switching items from UtlSymbol
	const char  *String() const { return Get(); }

	// Returns strlen
	int			Length() const;
	bool		IsEmpty() const;

	CUtlString &operator=( const CUtlString &src );
	CUtlString &operator=( const char *src );

	// Test for equality
	bool operator==( const CUtlString &src ) const;
	bool operator==( const char *src ) const;
	bool operator!=( const CUtlString &src ) const { return !operator==( src ); }
	bool operator!=( const char *src ) const { return !operator==( src ); }

private:
	char *m_pchString;
};


//-----------------------------------------------------------------------------
// Inline methods
//-----------------------------------------------------------------------------
inline CUtlString::CUtlString()
{
	m_pchString = NULL;
}

inline CUtlString::CUtlString( const char *pString )
{
	m_pchString = NULL;
	Set( pString );
}

inline CUtlString::CUtlString( const CUtlString& string )
{
	m_pchString = NULL;
	Set( string );
}

inline CUtlString::~CUtlString()
{
	if ( m_pchString )
	{
		free( m_pchString );
		m_pchString = NULL;
	}
}

inline CUtlString &CUtlString::operator=( const CUtlString &src )
{
	Set( src );
	return *this;
}

inline CUtlString &CUtlString::operator=( const char *src )
{
	Set( src );
	return *this;
}

inline bool CUtlString::operator==( const CUtlString &src ) const
{
	return ( strcmp( Get(), src ) == 0 );
}

inline bool CUtlString::operator==( const char *src ) const
{
	return ( strcmp( Get(), src ) == 0 );
}

inline bool CUtlString::IsValid()
{
	return m_pchString != NULL;
}

inline CUtlString::operator const char*() const
{
	return Get();
}

inline const char *CUtlString::Get() const
{
	if ( m_pchString )
		return m_pchString;

	return "";
}

inline void CUtlString::Set( const char *pValue )
{
	if ( m_pchString != NULL )
		free( m_pchString );

	if ( pValue && *pValue )
		m_pchString = _strdup( pValue );
	else
		m_pchString = NULL;
}

inline void CUtlString::Append( const char *pValue )
{
	if ( m_pchString )
		strcat( m_pchString, pValue );
	else
		SetValue( pValue );
}

inline int CUtlString::Length() const
{
	return m_pchString ? strlen( m_pchString ) : 0;
}

inline bool CUtlString::IsEmpty() const
{
	return Length() == 0;
}


#endif // UTLSTRING_H
