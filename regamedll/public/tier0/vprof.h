//========= Copyright ?1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Real-Time Hierarchical Profiling
//
// $NoKeywords: $
//=============================================================================//

#ifndef VPROF_H
#define VPROF_H

#include "tier0/dbg.h"
#include "tier0/fasttimer.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)
#endif

//-----------------------------------------------------------------------------
//
// Profiling instrumentation macros
//

#define MAXCOUNTERS 256


#ifdef VPROF_ENABLED

#define VPROF_VTUNE_GROUP

#define	VPROF( name )						VPROF_(name, 1, VPROF_BUDGETGROUP_OTHER_UNACCOUNTED, false, 0)
#define	VPROF_ASSERT_ACCOUNTED( name )		VPROF_(name, 1, VPROF_BUDGETGROUP_OTHER_UNACCOUNTED, true, 0)
#define	VPROF_( name, detail, group, bAssertAccounted, budgetFlags )		VPROF_##detail(name,group, bAssertAccounted, budgetFlags)

#define VPROF_BUDGET( name, group )					VPROF_BUDGET_FLAGS(name, group, BUDGETFLAG_OTHER)
#define VPROF_BUDGET_FLAGS( name, group, flags )	VPROF_(name, 0, group, false, flags)

#define VPROF_SCOPE_BEGIN( tag )	do { VPROF( tag )
#define VPROF_SCOPE_END()			} while (0)

#define VPROF_ONLY( expression )	expression

#define VPROF_ENTER_SCOPE( name )			g_VProfCurrentProfile.EnterScope( name, 1, VPROF_BUDGETGROUP_OTHER_UNACCOUNTED, false, 0 )
#define VPROF_EXIT_SCOPE()					g_VProfCurrentProfile.ExitScope()

#define VPROF_BUDGET_GROUP_ID_UNACCOUNTED 0


// Budgetgroup flags. These are used with VPROF_BUDGET_FLAGS.
// These control which budget panels the groups show up in.
// If a budget group uses VPROF_BUDGET, it gets the default 
// which is BUDGETFLAG_OTHER.
#define BUDGETFLAG_CLIENT	(1<<0)		// Shows up in the client panel.
#define BUDGETFLAG_SERVER	(1<<1)		// Shows up in the server panel.
#define BUDGETFLAG_OTHER	(1<<2)		// Unclassified (the client shows these but the dedicated server doesn't).
#define BUDGETFLAG_HIDDEN	(1<<15)
#define BUDGETFLAG_ALL		0xFFFF


// NOTE: You can use strings instead of these defines. . they are defined here and added
// in vprof.cpp so that they are always in the same order.
#define VPROF_BUDGETGROUP_OTHER_UNACCOUNTED			_T("Unaccounted")
#define VPROF_BUDGETGROUP_WORLD_RENDERING			_T("World Rendering")
#define VPROF_BUDGETGROUP_DISPLACEMENT_RENDERING	_T("Displacement_Rendering")
#define VPROF_BUDGETGROUP_GAME						_T("Game")
#define VPROF_BUDGETGROUP_NPCS						_T("NPCs")
#define VPROF_BUDGETGROUP_SERVER_ANIM				_T("Server Animation")
#define VPROF_BUDGETGROUP_PHYSICS					_T("Physics")
#define VPROF_BUDGETGROUP_STATICPROP_RENDERING		_T("Static_Prop_Rendering")
#define VPROF_BUDGETGROUP_MODEL_RENDERING			_T("Other_Model_Rendering")
#define VPROF_BUDGETGROUP_BRUSHMODEL_RENDERING		_T("Brush_Model_Rendering")
#define VPROF_BUDGETGROUP_SHADOW_RENDERING			_T("Shadow_Rendering")
#define VPROF_BUDGETGROUP_DETAILPROP_RENDERING		_T("Detail_Prop_Rendering")
#define VPROF_BUDGETGROUP_PARTICLE_RENDERING		_T("Particle/Effect_Rendering")
#define VPROF_BUDGETGROUP_ROPES						_T("Ropes")
#define VPROF_BUDGETGROUP_DLIGHT_RENDERING			_T("Dynamic_Light_Rendering")
#define VPROF_BUDGETGROUP_OTHER_NETWORKING			_T("Networking")
#define VPROF_BUDGETGROUP_CLIENT_ANIMATION			_T("Client_Animation")
#define VPROF_BUDGETGROUP_OTHER_SOUND				_T("Sound")
#define VPROF_BUDGETGROUP_OTHER_VGUI				_T("VGUI")
#define VPROF_BUDGETGROUP_OTHER_FILESYSTEM			_T("FileSystem")
#define VPROF_BUDGETGROUP_PREDICTION				_T("Prediction")
#define VPROF_BUDGETGROUP_INTERPOLATION				_T("Interpolation")
#define VPROF_BUDGETGROUP_SWAP_BUFFERS				_T("Swap_Buffers")
#define VPROF_BUDGETGROUP_PLAYER					_T("Player")
#define VPROF_BUDGETGROUP_OCCLUSION					_T("Occlusion")
#define VPROF_BUDGETGROUP_OVERLAYS					_T("Overlays")
#define VPROF_BUDGETGROUP_TOOLS						_T("Tools")
#define VPROF_BUDGETGROUP_LIGHTCACHE				_T("Light_Cache")
#define VPROF_BUDGETGROUP_DISP_HULLTRACES			_T("Displacement_Hull_Traces")
#define VPROF_BUDGETGROUP_TEXTURE_CACHE				_T("Texture_Cache")
#define VPROF_BUDGETGROUP_PARTICLE_SIMULATION		_T("Particle Simulation")
#define VPROF_BUDGETGROUP_SHADOW_DEPTH_TEXTURING	_T("Flashlight Shadows")
#define VPROF_BUDGETGROUP_CLIENT_SIM				_T("Client Simulation") // think functions, tempents, etc.
#define VPROF_BUDGETGROUP_STEAM						_T("Steam") 

//-------------------------------------

#ifndef VPROF_LEVEL
#define VPROF_LEVEL 0
#endif

#define	VPROF_0(name,group,assertAccounted,budgetFlags)	CVProfScope VProf_(name, 0, group, assertAccounted, budgetFlags);

#if VPROF_LEVEL > 0 
#define	VPROF_1(name,group,assertAccounted,budgetFlags)	CVProfScope VProf_(name, 1, group, assertAccounted, budgetFlags);
#else
#define	VPROF_1(name,group,assertAccounted,budgetFlags)	((void)0)
#endif

#if VPROF_LEVEL > 1 
#define	VPROF_2(name,group,assertAccounted,budgetFlags)	CVProfScope VProf_(name, 2, group, assertAccounted, budgetFlags);
#else
#define	VPROF_2(name,group,assertAccounted,budgetFlags)	((void)0)
#endif

#if VPROF_LEVEL > 2 
#define	VPROF_3(name,group,assertAccounted,budgetFlags)	CVProfScope VProf_(name, 3, group, assertAccounted, budgetFlags);
#else
#define	VPROF_3(name,group,assertAccounted,budgetFlags)	((void)0)
#endif

#if VPROF_LEVEL > 3 
#define	VPROF_4(name,group,assertAccounted,budgetFlags)	CVProfScope VProf_(name, 4, group, assertAccounted, budgetFlags);
#else
#define	VPROF_4(name,group,assertAccounted,budgetFlags)	((void)0)
#endif

//-------------------------------------

#define VPROF_INCREMENT_COUNTER(name,amount)			do { static CVProfCounter _counter( name ); _counter.Increment( amount ); } while( 0 )
#define VPROF_INCREMENT_GROUP_COUNTER(name,group,amount)			do { static CVProfCounter _counter( name, group ); _counter.Increment( amount ); } while( 0 )

#else

#define	VPROF( name )									((void)0)
#define	VPROF_ASSERT_ACCOUNTED( name )					((void)0)
#define	VPROF_( name, detail, group, bAssertAccounted )	((void)0)
#define VPROF_BUDGET( name, group )						((void)0)
#define VPROF_BUDGET_FLAGS( name, group, flags )		((void)0)

#define VPROF_SCOPE_BEGIN( tag )	do {
#define VPROF_SCOPE_END()			} while (0)

#define VPROF_ONLY( expression )	((void)0)

#define VPROF_ENTER_SCOPE( name )
#define VPROF_EXIT_SCOPE()

#define VPROF_INCREMENT_COUNTER(name,amount)			((void)0)
#define VPROF_INCREMENT_GROUP_COUNTER(name,group,amount) ((void)0)

#endif
 
//-----------------------------------------------------------------------------

#ifdef VPROF_ENABLED

//-----------------------------------------------------------------------------
//
// A node in the call graph hierarchy
//

class DBG_CLASS CVProfNode 
{
friend class CVProfRecorder;
friend class CVProfile;

public:
	CVProfNode( const char * pszName, int detailLevel, CVProfNode *pParent, const char *pBudgetGroupName, int budgetFlags );
	~CVProfNode();
	
	CVProfNode *GetSubNode( const char *pszName, int detailLevel, const char *pBudgetGroupName, int budgetFlags );
	CVProfNode *GetSubNode( const char *pszName, int detailLevel, const char *pBudgetGroupName );
	CVProfNode *GetParent();
	CVProfNode *GetSibling();		
	CVProfNode *GetPrevSibling();	
	CVProfNode *GetChild();		
	
	void MarkFrame();
	void ResetPeak();
	
	void Pause();
	void Resume();
	void Reset();

	void EnterScope();
	bool ExitScope();

	const char *GetName();

	int GetBudgetGroupID()
	{
		return -1;
	}

	// Only used by the record/playback stuff.
	void SetBudgetGroupID( int id )
	{

	}

	int	GetCurCalls();
	double GetCurTime();		
	int GetPrevCalls();
	double GetPrevTime();
	int	GetTotalCalls();
	double GetTotalTime();		
	double GetPeakTime();		

	double GetCurTimeLessChildren();
	double GetPrevTimeLessChildren();
	double GetTotalTimeLessChildren();

	int GetPrevL2CacheMissLessChildren();
	int GetPrevLoadHitStoreLessChildren();

	void ClearPrevTime();

	int GetL2CacheMisses();

	// Not used in the common case...
	void SetCurFrameTime( unsigned long milliseconds );
	
	void SetClientData( int iClientData )	{ }
	int GetClientData() const				{ return -1; }

#ifdef DBGFLAG_VALIDATE
	void Validate( CValidator &validator, char *pchName );		// Validate our internal structures
#endif // DBGFLAG_VALIDATE


// Used by vprof record/playback.
private:

	void SetUniqueNodeID( int id )
	{
	}

	int GetUniqueNodeID() const
	{
		return 0;
	}

	static int s_iCurrentUniqueNodeID;


private:
};

//-----------------------------------------------------------------------------
//
// Coordinator and root node of the profile hierarchy tree
//

enum VProfReportType_t
{
	VPRT_SUMMARY									= ( 1 << 0 ),
	VPRT_HIERARCHY									= ( 1 << 1 ),
	VPRT_HIERARCHY_TIME_PER_FRAME_AND_COUNT_ONLY	= ( 1 << 2 ),
	VPRT_LIST_BY_TIME								= ( 1 << 3 ),
	VPRT_LIST_BY_TIME_LESS_CHILDREN					= ( 1 << 4 ),
	VPRT_LIST_BY_AVG_TIME							= ( 1 << 5 ),	
	VPRT_LIST_BY_AVG_TIME_LESS_CHILDREN				= ( 1 << 6 ),
	VPRT_LIST_BY_PEAK_TIME							= ( 1 << 7 ),
	VPRT_LIST_BY_PEAK_OVER_AVERAGE					= ( 1 << 8 ),
	VPRT_LIST_TOP_ITEMS_ONLY						= ( 1 << 9 ),

	VPRT_FULL = (0xffffffff & ~(VPRT_HIERARCHY_TIME_PER_FRAME_AND_COUNT_ONLY|VPRT_LIST_TOP_ITEMS_ONLY)),
};

enum CounterGroup_t
{
	COUNTER_GROUP_DEFAULT=0,
	COUNTER_GROUP_NO_RESET,				// The engine doesn't reset these counters. Usually, they are used 
										// like global variables that can be accessed across modules.
	COUNTER_GROUP_TEXTURE_GLOBAL,		// Global texture usage counters (totals for what is currently in memory).
	COUNTER_GROUP_TEXTURE_PER_FRAME		// Per-frame texture usage counters.
}; 

class DBG_CLASS CVProfile 
{
public:
	CVProfile();
	~CVProfile();

	void Term();
	
	//
	// Runtime operations
	//
	
	void Start();
	void Stop();

	void EnterScope( const char *pszName, int detailLevel, const char *pBudgetGroupName, bool bAssertAccounted );
	void EnterScope( const char *pszName, int detailLevel, const char *pBudgetGroupName, bool bAssertAccounted, int budgetFlags );
	void ExitScope();

	void MarkFrame();
	void ResetPeaks();
	
	void Pause();
	void Resume();
	void Reset();
	
	bool IsEnabled() const;
	int GetDetailLevel() const;

	bool AtRoot() const;

	//
	// Queries
	//

#ifdef VPROF_VTUNE_GROUP
#	define MAX_GROUP_STACK_DEPTH 1024

	void EnableVTuneGroup( const char *pGroupName )
	{

	}
	void DisableVTuneGroup( void )
	{

	}
	
	inline void PushGroup( int nGroupID );
	inline void PopGroup( void );
#endif
	
	int NumFramesSampled()	{ return 0; }
	double GetPeakFrameTime();
	double GetTotalTimeSampled();
	double GetTimeLastFrame();
	
	CVProfNode *GetRoot();
	CVProfNode *FindNode( CVProfNode *pStartNode, const char *pszNode );

	void OutputReport( int type = VPRT_FULL, const char *pszStartNode = NULL, int budgetGroupID = -1 );

	const char *GetBudgetGroupName( int budgetGroupID );
	int GetBudgetGroupFlags( int budgetGroupID ) const;	// Returns a combination of BUDGETFLAG_ defines.
	int GetNumBudgetGroups( void );
	void GetBudgetGroupColor( int budgetGroupID, int &r, int &g, int &b, int &a );
	int BudgetGroupNameToBudgetGroupID( const char *pBudgetGroupName );
	int BudgetGroupNameToBudgetGroupID( const char *pBudgetGroupName, int budgetFlagsToORIn );
	void RegisterNumBudgetGroupsChangedCallBack( void (*pCallBack)(void) );

	int BudgetGroupNameToBudgetGroupIDNoCreate( const char *pBudgetGroupName ) { return FindBudgetGroupName( pBudgetGroupName ); }

	void HideBudgetGroup( int budgetGroupID, bool bHide = true );
	void HideBudgetGroup( const char *pszName, bool bHide = true ) { HideBudgetGroup( BudgetGroupNameToBudgetGroupID( pszName), bHide ); }

	int *FindOrCreateCounter( const char *pName, CounterGroup_t eCounterGroup=COUNTER_GROUP_DEFAULT  );
	void ResetCounters( CounterGroup_t eCounterGroup );
	
	int GetNumCounters( void ) const;
	
	const char *GetCounterName( int index ) const;
	int GetCounterValue( int index ) const;
	const char *GetCounterNameAndValue( int index, int &val ) const;
	CounterGroup_t GetCounterGroup( int index ) const;

	// Performance monitoring events.
	void PMEInitialized( bool bInit )		{ }
	void PMEEnable( bool bEnable )			{ }

	bool UsePME( void )						{ return false; }

#ifdef DBGFLAG_VALIDATE
	void Validate( CValidator &validator, char *pchName );		// Validate our internal structures
#endif // DBGFLAG_VALIDATE

protected:

	void FreeNodes_R( CVProfNode *pNode );

#ifdef VPROF_VTUNE_GROUP
	bool VTuneGroupEnabled()
	{ 
		return false; 
	}
	int VTuneGroupID() 
	{ 
		return -1; 
	}
#endif

	void SumTimes( const char *pszStartNode, int budgetGroupID );
	void SumTimes( CVProfNode *pNode, int budgetGroupID );
	void DumpNodes( CVProfNode *pNode, int indent, bool bAverageAndCountOnly );
	int FindBudgetGroupName( const char *pBudgetGroupName );
	int AddBudgetGroupName( const char *pBudgetGroupName, int budgetFlags );
};

//-------------------------------------

DBG_INTERFACE CVProfile g_VProfCurrentProfile;

//-----------------------------------------------------------------------------

#ifdef VPROF_VTUNE_GROUP
inline void CVProfile::PushGroup( int nGroupID )
{
}
#endif // VPROF_VTUNE_GROUP

#ifdef VPROF_VTUNE_GROUP
inline void CVProfile::PopGroup( void )
{
}
#endif // VPROF_VTUNE_GROUP

//-----------------------------------------------------------------------------

class CVProfScope
{
public:
	CVProfScope( const char * pszName, int detailLevel, const char *pBudgetGroupName, bool bAssertAccounted, int budgetFlags );
	~CVProfScope();
};

//-----------------------------------------------------------------------------
//
// CVProfNode, inline methods
//

inline CVProfNode::CVProfNode( const char * pszName, int detailLevel, CVProfNode *pParent, const char *pBudgetGroupName, int budgetFlags )
{

}

//-------------------------------------

inline CVProfNode::~CVProfNode()
{
}

//-------------------------------------

inline CVProfNode *CVProfNode::GetParent()		
{ 
	return NULL;
}

//-------------------------------------

inline CVProfNode *CVProfNode::GetSibling()		
{ 
	return NULL;
}

//-------------------------------------
// Hacky way to the previous sibling, only used from vprof panel at the moment,
// so it didn't seem like it was worth the memory waste to add the reverse
// link per node.

inline CVProfNode *CVProfNode::GetPrevSibling()		
{ 
	return NULL;
}

//-------------------------------------

inline CVProfNode *CVProfNode::GetChild()			
{ 
	return NULL; 
}

//-------------------------------------

inline const char *CVProfNode::GetName()				
{ 
	return NULL;
}

//-------------------------------------

inline int	CVProfNode::GetTotalCalls()		
{ 
	return 0; 
}

//-------------------------------------

inline double CVProfNode::GetTotalTime()		
{ 
	return 0;
}

//-------------------------------------

inline int	CVProfNode::GetCurCalls()		
{ 
	return 0; 
}

//-------------------------------------

inline double CVProfNode::GetCurTime()		
{ 
	return 0;
}

//-------------------------------------

inline int CVProfNode::GetPrevCalls()
{
	return 0;
}

//-------------------------------------

inline double CVProfNode::GetPrevTime()		
{ 
	return 0;
}

//-------------------------------------

inline double CVProfNode::GetPeakTime()		
{ 
	return 0;
}

//-------------------------------------

inline double CVProfNode::GetTotalTimeLessChildren()
{
	return 0;
}

//-------------------------------------

inline double CVProfNode::GetCurTimeLessChildren()
{
	return 0;
}

inline double CVProfNode::GetPrevTimeLessChildren()
{
	return 0;
}

//-----------------------------------------------------------------------------
inline int CVProfNode::GetPrevL2CacheMissLessChildren()
{
	return 0;
}

//-----------------------------------------------------------------------------
inline int CVProfNode::GetPrevLoadHitStoreLessChildren()
{
	return 0;
}


//-----------------------------------------------------------------------------
inline void CVProfNode::ClearPrevTime()
{
}

//-----------------------------------------------------------------------------
inline int CVProfNode::GetL2CacheMisses( void )
{ 
	return 0;
}

//-----------------------------------------------------------------------------
//
// CVProfile, inline methods
//

//-------------------------------------

inline bool CVProfile::IsEnabled() const	
{ 
	return false;
}

//-------------------------------------

inline int CVProfile::GetDetailLevel() const	
{ 
	return 0;
}

	
//-------------------------------------

inline bool CVProfile::AtRoot() const
{
	return false;
}
	
//-------------------------------------

inline void CVProfile::Start()	
{ 
}

//-------------------------------------

inline void CVProfile::Stop()		
{ 
}

//-------------------------------------

inline void CVProfile::EnterScope( const char *pszName, int detailLevel, const char *pBudgetGroupName, bool bAssertAccounted, int budgetFlags )
{
}

inline void CVProfile::EnterScope( const char *pszName, int detailLevel, const char *pBudgetGroupName, bool bAssertAccounted )
{
}

//-------------------------------------

inline void CVProfile::ExitScope()
{
}

//-------------------------------------

inline void CVProfile::Pause()
{
}

//-------------------------------------

inline void CVProfile::Resume()
{
}

//-------------------------------------

inline void CVProfile::Reset()
{
}

//-------------------------------------

inline void CVProfile::ResetPeaks()
{
}

//-------------------------------------

inline void CVProfile::MarkFrame()
{
}

//-------------------------------------

inline double CVProfile::GetTotalTimeSampled()
{
	return 0;
}

//-------------------------------------

inline double CVProfile::GetPeakFrameTime()
{
	return 0;
}

//-------------------------------------

inline double CVProfile::GetTimeLastFrame()
{
	return 0;
}
	
//-------------------------------------

inline CVProfNode *CVProfile::GetRoot()
{
	return NULL;
}


inline const char *CVProfile::GetBudgetGroupName( int budgetGroupID )
{
	return NULL;
}

inline int CVProfile::GetBudgetGroupFlags( int budgetGroupID ) const
{
	return NULL;
}


//-----------------------------------------------------------------------------

inline CVProfScope::CVProfScope( const char * pszName, int detailLevel, const char *pBudgetGroupName, bool bAssertAccounted, int budgetFlags )
{ 

}

//-------------------------------------

inline CVProfScope::~CVProfScope()					
{ 

}

class CVProfCounter
{
public:
	CVProfCounter( const char *pName, CounterGroup_t group=COUNTER_GROUP_DEFAULT )
	{
		m_pCounter = g_VProfCurrentProfile.FindOrCreateCounter( pName, group );
		Assert( m_pCounter );
	}
	~CVProfCounter()
	{
	}
	void Increment( int val ) 
	{ 
		Assert( m_pCounter );
		*m_pCounter += val; 
	}
private:
	int *m_pCounter;
};

#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif

//=============================================================================
