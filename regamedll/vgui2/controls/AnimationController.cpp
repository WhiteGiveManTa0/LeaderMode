//========= Copyright R 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
//=============================================================================
#pragma warning( disable : 4244 ) // conversion from 'double' to 'float', possible loss of data

#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/IVGui.h>
#include <KeyValues.h>
#include <vgui_controls/AnimationController.h>
#include "filesystem.h"
#include "filesystem_helpers.h"

#include <stdio.h>
#include <math.h>
#include "mempool.h"
#include "characterset.h"

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/dbg.h>
#include <tier0/memdbgon.h>

#include "APIProxy.h"
extern cl_enginefunc_t gEngfuncs;	// don't use random from some wired import.

using namespace vgui;

static CUtlSymbolTable g_ScriptSymbols(0, 128, true);

enum Interpolators_e
{
	INTERPOLATOR_LINEAR,
	INTERPOLATOR_ACCEL,
	INTERPOLATOR_DEACCEL,
	INTERPOLATOR_PULSE,
	INTERPOLATOR_FLICKER,
};

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
AnimationController::AnimationController(Panel *parent) : BaseClass(parent, NULL)
{
	m_bAutoReloadScript = false;

	// always invisible
	SetVisible(false);

	// get the names of common types
	m_sPosition = g_ScriptSymbols.AddString("position");
	m_sSize = g_ScriptSymbols.AddString("size"); 
	m_sFgColor = g_ScriptSymbols.AddString("fgcolor"); 
	m_sBgColor = g_ScriptSymbols.AddString("bgcolor");

}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
AnimationController::~AnimationController()
{
}

//-----------------------------------------------------------------------------
// Purpose: Sets which script file to use
//-----------------------------------------------------------------------------
bool AnimationController::SetScriptFile(const char *fileName, bool wipeAll /*=false*/ )
{
	m_sScriptFileName = g_ScriptSymbols.AddString(fileName);

	// clear the current script
	m_Sequences.RemoveAll();

	if ( wipeAll )
	{
		m_ActiveAnimations.RemoveAll();
		m_PostedMessages.RemoveAll();
	}

	// load the new script file
	return LoadScriptFile(fileName);
}

//-----------------------------------------------------------------------------
// Purpose: loads a script file from disk
//-----------------------------------------------------------------------------
bool AnimationController::LoadScriptFile(const char *fileName)
{
	FileHandle_t f = filesystem()->Open(fileName, "rt");
	if (!f)
	{
		ivgui()->DPrintf("Couldn't find script file %s\n", fileName);
		return false;
	}

	// read the whole thing into memory
	int size = filesystem()->Size(f);
	// read into temporary memory block
	char *pMem = (char *)malloc(size);
	int bytesRead = filesystem()->Read(pMem, size, f);
	assert(bytesRead < size);
	pMem[bytesRead] = 0;
	filesystem()->Close(f);
	// parse
	bool success = ParseScriptFile(pMem, bytesRead);
	free(pMem);
	return success;
}

//-----------------------------------------------------------------------------
// Purpose: parses a script into sequences
//-----------------------------------------------------------------------------
bool AnimationController::ParseScriptFile(char *pMem, int length)
{
	// get the scheme (for looking up color names)
	IScheme *scheme = vgui::scheme()->GetIScheme(GetScheme());

	// get our screen size (for left/right/center alignment)
	int screenWide, screenTall;
	surface()->GetScreenSize(screenWide, screenTall);

	// start by getting the first token
	char token[512];
	pMem = ParseFile(pMem, token, NULL);
	while (token[0])
	{
		// should be 'event'
		if (_stricmp(token, "event"))
		{
			ivgui()->DPrintf("Couldn't parse script file: expected 'event', found '%s'\n", token);
			return false;
		}

		// get the event name
		pMem = ParseFile(pMem, token, NULL);
		if (strlen(token) < 1)
		{
			ivgui()->DPrintf("Couldn't parse script file: expected <event name>, found nothing\n");
			return false;
		}
		// create a new sequence
		int seqIndex = m_Sequences.AddToTail();
		AnimSequence_t &seq = m_Sequences[seqIndex];
		seq.name = g_ScriptSymbols.AddString(token);
		seq.duration = 0.0f;

		// get the open brace
		pMem = ParseFile(pMem, token, NULL);
		if (_stricmp(token, "{"))
		{
			ivgui()->DPrintf("Couldn't parse script sequence '%s': expected '{', found '%s'\n", g_ScriptSymbols.String(seq.name), token);
			return false;
		}

		// walk the commands
		while (token && token[0])
		{
			// get the command type
			pMem = ParseFile(pMem, token, NULL);

			// skip out when we hit the end of the sequence
			if (token[0] == '}')
				break;

			// create a new command
			int cmdIndex = seq.cmdList.AddToTail();
			AnimCommand_t &animCmd = seq.cmdList[cmdIndex];
			memset(&animCmd, 0, sizeof(animCmd));
			if (!_stricmp(token, "animate"))
			{
				animCmd.commandType = CMD_ANIMATE;
				// parse out the animation commands
				AnimCmdAnimate_t &cmdAnimate = animCmd.cmdData.animate;
				// panel to manipulate
				pMem = ParseFile(pMem, token, NULL);
				cmdAnimate.panel = g_ScriptSymbols.AddString(token);
				// variable to change
				pMem = ParseFile(pMem, token, NULL);
				cmdAnimate.variable = g_ScriptSymbols.AddString(token);
				// target value
				pMem = ParseFile(pMem, token, NULL);
				if (cmdAnimate.variable == m_sPosition)
				{
					char *psz = token;
					bool xr = false, xc = false, yr = false, yc = false;
					int x, y;

					// Get first token
					if (psz[0] == 'R')
					{
						xr = true;
						psz++;
					}
					else if (psz[0] == 'C')
					{
						xc = true;
						psz++;
					}

					// get the number
					x = atoi(psz);

					// Get second token from "token"
					char token2[32];
					psz = ParseFile(psz, token2, NULL);
					psz = token2;

					if (psz[0] == 'R')
					{
						yr = true;
						psz++;
					}
					else if (psz[0] == 'C')
					{
						yc = true;
						psz++;
					}

					// get the number
					y = atoi(psz);

					// scale the values
					x = vgui::scheme()->GetProportionalScaledValue(x);
					y = vgui::scheme()->GetProportionalScaledValue(y);

					// adjust the positions
					if (xr)
					{
						x = screenWide - x;
					}
					if (xc)
					{
						x = (screenWide / 2) + x;
					}
					if (yr)
					{
						y = screenTall - y;
					}
					if (yc)
					{
						y = (screenTall / 2) + y;
					}

					// Position X goes into ".a"
					cmdAnimate.target.a = x;

					// Position Y goes into ".b"
					cmdAnimate.target.b = y;
				}
				else 
				{
					// parse the floating point values right out
					if (0 == sscanf(token, "%f %f %f %f", &cmdAnimate.target.a, &cmdAnimate.target.b, &cmdAnimate.target.c, &cmdAnimate.target.d))
					{
						// could be referencing a value in the scheme file, lookup
						Color col = scheme->GetColor(token, Color(0, 0, 0, 0));
						cmdAnimate.target.a = col[0];
						cmdAnimate.target.b = col[1];
						cmdAnimate.target.c = col[2];
						cmdAnimate.target.d = col[3];
					}
				}

				// fix up scale
				if (cmdAnimate.variable == m_sSize)
				{
					cmdAnimate.target.a = static_cast<float>( vgui::scheme()->GetProportionalScaledValue(cmdAnimate.target.a) );
					cmdAnimate.target.b = static_cast<float>( vgui::scheme()->GetProportionalScaledValue(cmdAnimate.target.b) );
				}
				
				// interpolation function
				pMem = ParseFile(pMem, token, NULL);
				if (!_stricmp(token, "Accel"))
				{
					cmdAnimate.interpolationFunction = INTERPOLATOR_ACCEL;
				}
				else if (!_stricmp(token, "Deaccel"))
				{
					cmdAnimate.interpolationFunction = INTERPOLATOR_DEACCEL;
				}
				else if (!_stricmp(token,"Pulse"))
				{
					cmdAnimate.interpolationFunction = INTERPOLATOR_PULSE;
					// frequencey
					pMem = ParseFile(pMem, token, NULL);
					cmdAnimate.interpolationParameter = (float)atof(token);
				}
				else if ( !_stricmp( token, "Flicker"))
				{
					cmdAnimate.interpolationFunction = INTERPOLATOR_FLICKER;
					// noiseamount
					pMem = ParseFile(pMem, token, NULL);
					cmdAnimate.interpolationParameter = (float)atof(token);
				}
				else
				{
					cmdAnimate.interpolationFunction = INTERPOLATOR_LINEAR;
				}
				// start time
				pMem = ParseFile(pMem, token, NULL);
				cmdAnimate.startTime = (float)atof(token);
				// duration
				pMem = ParseFile(pMem, token, NULL);
				cmdAnimate.duration = (float)atof(token);
				// check max duration
				if (cmdAnimate.startTime + cmdAnimate.duration > seq.duration)
				{
					seq.duration = cmdAnimate.startTime + cmdAnimate.duration;
				}
			}
			else if (!_stricmp(token, "runevent"))
			{
				animCmd.commandType = CMD_RUNEVENT;
				pMem = ParseFile(pMem, token, NULL);
				animCmd.cmdData.runEvent.event = g_ScriptSymbols.AddString(token);
				pMem = ParseFile(pMem, token, NULL);
				animCmd.cmdData.runEvent.timeDelay = (float)atof(token);
			}
			else if (!_stricmp(token, "stopevent"))
			{
				animCmd.commandType = CMD_STOPEVENT;
				pMem = ParseFile(pMem, token, NULL);
				animCmd.cmdData.runEvent.event = g_ScriptSymbols.AddString(token);
				pMem = ParseFile(pMem, token, NULL);
				animCmd.cmdData.runEvent.timeDelay = (float)atof(token);
			}
			else if (!_stricmp(token, "StopPanelAnimations"))
			{
				animCmd.commandType = CMD_STOPPANELANIMATIONS;
				pMem = ParseFile(pMem, token, NULL);
				animCmd.cmdData.runEvent.event = g_ScriptSymbols.AddString(token);
				pMem = ParseFile(pMem, token, NULL);
				animCmd.cmdData.runEvent.timeDelay = (float)atof(token);
			}
			else if (!_stricmp(token, "stopanimation"))
			{
				animCmd.commandType = CMD_STOPANIMATION;
				pMem = ParseFile(pMem, token, NULL);
				animCmd.cmdData.runEvent.event = g_ScriptSymbols.AddString(token);
				pMem = ParseFile(pMem, token, NULL);
				animCmd.cmdData.runEvent.variable = g_ScriptSymbols.AddString(token);
				pMem = ParseFile(pMem, token, NULL);
				animCmd.cmdData.runEvent.timeDelay = (float)atof(token);
			}
			else if ( !_stricmp( token, "SetFont" ))
			{
				animCmd.commandType = CMD_SETFONT;
				// Panel name
				pMem = ParseFile(pMem, token, NULL);
				animCmd.cmdData.runEvent.event = g_ScriptSymbols.AddString(token);
				// Font parameter
				pMem = ParseFile(pMem, token, NULL);
				animCmd.cmdData.runEvent.variable = g_ScriptSymbols.AddString(token);
				// Font name from scheme
				pMem = ParseFile(pMem, token, NULL);
				animCmd.cmdData.runEvent.variable2 = g_ScriptSymbols.AddString(token);

				// Set time
				pMem = ParseFile(pMem, token, NULL);
				animCmd.cmdData.runEvent.timeDelay = (float)atof(token);
			}
			else
			{
				ivgui()->DPrintf("Couldn't parse script sequence '%s': expected <anim command>, found '%s'\n", g_ScriptSymbols.String(seq.name), token);
				return false;
			}
			
		}

		// get the next token, if any
		pMem = ParseFile(pMem, token, NULL);
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: runs a frame of animation
//-----------------------------------------------------------------------------
void AnimationController::UpdateAnimations( float currentTime )
{
	m_flCurrentTime = currentTime;

	// check posted messages
	{for (int i = 0; i < m_PostedMessages.Count(); i++)
	{
		PostedMessage_t &msgRef = m_PostedMessages[i];
		if (currentTime < msgRef.startTime)
			continue;

		// take a copy of th message
		PostedMessage_t msg = msgRef;

		// remove the event
		// do this before handling the message because the message queue may be messed with
		m_PostedMessages.Remove(i);
		// reset the count, start the whole queue again
		i = -1;

		// handle the event
		switch (msg.commandType)
		{
		case CMD_RUNEVENT:
			RunCmd_RunEvent(msg);
			break;
		case CMD_STOPEVENT:
			RunCmd_StopEvent(msg);
			break;
		case CMD_STOPPANELANIMATIONS:
			RunCmd_StopPanelAnimations(msg);
			break;
		case CMD_STOPANIMATION:
			RunCmd_StopAnimation(msg);
			break;
		case CMD_SETFONT:
			RunCmd_SetFont(msg);
			break;
		}
	}}

	// iterate all the currently active animations
	for (int i = 0; i < m_ActiveAnimations.Count(); i++)
	{
		ActiveAnimation_t &anim = m_ActiveAnimations[i];

		// see if the anim is ready to start
		if (currentTime < anim.startTime)
			continue;

		if (!anim.started)
		{
			// start the animation from the current value
			anim.startValue = GetValue(anim, anim.panel, anim.variable);
			anim.started = true;
		}

		// get the interpolated value
		Value_t val;
		if (currentTime > anim.endTime)
		{
			// animation is done, use the last value
			val = anim.endValue;
		}
		else
		{
			// get the interpolated value
			val = GetInterpolatedValue(anim.interpolator, anim.interpolatorParam, currentTime, anim.startTime, anim.endTime, anim.startValue, anim.endValue);
		}

		// apply the new value to the panel
		SetValue(anim, anim.panel, anim.variable, val);

		// see if we can remove the animation
		if (currentTime > anim.endTime)
		{
			m_ActiveAnimations.Remove(i);
			--i;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: produces an interpolated value
//-----------------------------------------------------------------------------
AnimationController::Value_t AnimationController::GetInterpolatedValue(int interpolator, float interpolatorParam, float currentTime, float startTime, float endTime, Value_t &startValue, Value_t &endValue)
{
	// calculate how far we are into the animation
	float pos = (currentTime - startTime) / (endTime - startTime);

	// adjust the percentage through by the interpolation function
	switch (interpolator)
	{
	case INTERPOLATOR_ACCEL:
		pos *= pos;
		break;
	case INTERPOLATOR_DEACCEL:
		pos = sqrtf(pos);
		break;
	case INTERPOLATOR_PULSE:
		// Make sure we end at 1.0, so use cosine
		pos = 0.5f + 0.5f * ( cos( pos * 2.0f * M_PI * interpolatorParam ) );
		break;
	case INTERPOLATOR_FLICKER:
		if ( gEngfuncs.pfnRandomFloat( 0.0f, 1.0f ) < interpolatorParam )
		{
			pos = 1.0f;
		}
		else
		{
			pos = 0.0f;
		}
		break;
	case INTERPOLATOR_LINEAR:
	default:
		break;
	}

	// calculate the value
	Value_t val;
	val.a = ((endValue.a - startValue.a) * pos) + startValue.a;
	val.b = ((endValue.b - startValue.b) * pos) + startValue.b;
	val.c = ((endValue.c - startValue.c) * pos) + startValue.c;
	val.d = ((endValue.d - startValue.d) * pos) + startValue.d;
	return val;
}

//-----------------------------------------------------------------------------
// Purpose: sets that the script file should be reloaded each time a script is ran
//			used for development
//-----------------------------------------------------------------------------
void AnimationController::SetAutoReloadScript(bool state)
{
	m_bAutoReloadScript = state;
}

//-----------------------------------------------------------------------------
// Purpose: starts an animation sequence script
//-----------------------------------------------------------------------------
bool AnimationController::StartAnimationSequence(const char *sequenceName)
{
	if (m_bAutoReloadScript)
	{
		// reload the script file
		SetScriptFile(g_ScriptSymbols.String(m_sScriptFileName));
	}

	// lookup the symbol for the name
	UtlSymId_t seqName = g_ScriptSymbols.Find(sequenceName);
	if (seqName == UTL_INVAL_SYMBOL)
		return false;

	// Msg("Starting animation sequence %s\n", sequenceName);

	// remove the existing command from the queue
	RemoveQueuedAnimationCommands(seqName);

	// look through for the sequence
	int i;
	for (i = 0; i < m_Sequences.Count(); i++)
	{
		if (m_Sequences[i].name == seqName)
			break;
	}
	if (i >= m_Sequences.Count())
		return false;

	// execute the sequence
	for (int cmdIndex = 0; cmdIndex < m_Sequences[i].cmdList.Count(); cmdIndex++)
	{
		ExecAnimationCommand(seqName, m_Sequences[i].cmdList[cmdIndex]);
	}

	return true;	
}

//-----------------------------------------------------------------------------
// Purpose: gets the length of an animation sequence, in seconds
//-----------------------------------------------------------------------------
float AnimationController::GetAnimationSequenceLength(const char *sequenceName)
{
	// lookup the symbol for the name
	UtlSymId_t seqName = g_ScriptSymbols.Find(sequenceName);
	if (seqName == UTL_INVAL_SYMBOL)
		return 0.0f;

	// look through for the sequence
	int i;
	for (i = 0; i < m_Sequences.Count(); i++)
	{
		if (m_Sequences[i].name == seqName)
			break;
	}
	if (i >= m_Sequences.Count())
		return 0.0f;

	// sequence found
	return m_Sequences[i].duration;
}

//-----------------------------------------------------------------------------
// Purpose: removes an existing set of commands from the queue
//-----------------------------------------------------------------------------
void AnimationController::RemoveQueuedAnimationCommands(UtlSymId_t seqName)
{
	// Msg("Removing queued anims for sequence %s\n", g_ScriptSymbols.String(seqName));

	// remove messages posted by this sequence
	{for (int i = 0; i < m_PostedMessages.Count(); i++)
	{
		if (m_PostedMessages[i].seqName == seqName)
		{
			m_PostedMessages.Remove(i);
			--i;
		}
	}}

	// remove all animations
	for (int i = 0; i < m_ActiveAnimations.Count(); i++)
	{
		if (m_ActiveAnimations[i].seqName == seqName)
		{
			m_ActiveAnimations.Remove(i);
			--i;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: removes the specified queued animation
//-----------------------------------------------------------------------------
void AnimationController::RemoveQueuedAnimationByType(vgui::Panel *panel, UtlSymId_t variable, UtlSymId_t sequenceToIgnore)
{
	for (int i = 0; i < m_ActiveAnimations.Count(); i++)
	{
		if (m_ActiveAnimations[i].panel == panel && m_ActiveAnimations[i].variable == variable && m_ActiveAnimations[i].seqName != sequenceToIgnore)
		{
			// Msg("Removing queued anim %s::%s::%s\n", g_ScriptSymbols.String(m_ActiveAnimations[i].seqName), panel->GetName(), g_ScriptSymbols.String(variable));
			m_ActiveAnimations.Remove(i);
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: runs a single line of the script
//-----------------------------------------------------------------------------
void AnimationController::ExecAnimationCommand(UtlSymId_t seqName, AnimCommand_t &animCommand)
{
	if (animCommand.commandType == CMD_ANIMATE)
	{
		StartCmd_Animate(seqName, animCommand.cmdData.animate);
	}
	else
	{
		// post the command to happen at the specified time
		PostedMessage_t &msg = m_PostedMessages[m_PostedMessages.AddToTail()];
		msg.seqName = seqName;
		msg.commandType = animCommand.commandType;
		msg.event = animCommand.cmdData.runEvent.event;
		msg.variable = animCommand.cmdData.runEvent.variable;
		msg.variable2 = animCommand.cmdData.runEvent.variable2;
		msg.startTime = m_flCurrentTime + animCommand.cmdData.runEvent.timeDelay;
	}
}

//-----------------------------------------------------------------------------
// Purpose: starts a variable animation
//-----------------------------------------------------------------------------
void AnimationController::StartCmd_Animate(UtlSymId_t seqName, AnimCmdAnimate_t &cmd)
{
	// make sure the child exists
	Panel *panel = GetParent()->FindChildByName(g_ScriptSymbols.String(cmd.panel),true);
	if (!panel)
		return;

	// build a command to add to the animation queue
	ActiveAnimation_t &anim = m_ActiveAnimations[m_ActiveAnimations.AddToTail()];
	anim.panel = panel;
	anim.seqName = seqName;
	anim.variable = cmd.variable;
	anim.interpolator = cmd.interpolationFunction;
	anim.interpolatorParam = cmd.interpolationParameter;
	// timings
	anim.startTime = m_flCurrentTime + cmd.startTime;
	anim.endTime = anim.startTime + cmd.duration;
	// values
	anim.started = false;
	anim.endValue = cmd.target;
}

//-----------------------------------------------------------------------------
// Purpose: a posted message to run another event
//-----------------------------------------------------------------------------
void AnimationController::RunCmd_RunEvent(PostedMessage_t &msg)
{
	StartAnimationSequence(g_ScriptSymbols.String(msg.event));
}

//-----------------------------------------------------------------------------
// Purpose: a posted message to stop another event
//-----------------------------------------------------------------------------
void AnimationController::RunCmd_StopEvent(PostedMessage_t &msg)
{
	RemoveQueuedAnimationCommands(msg.event);
}

//-----------------------------------------------------------------------------
// Purpose: a posted message to stop all animations relevant to a specified panel
//-----------------------------------------------------------------------------
void AnimationController::RunCmd_StopPanelAnimations(PostedMessage_t &msg)
{
	Panel *panel = GetParent()->FindChildByName(g_ScriptSymbols.String(msg.event), true);
	Assert(panel != NULL);
	if (!panel)
		return;

	// loop through all the active animations cancelling any that 
	// are operating on said panel,	except for the event specified
	for (int i = 0; i < m_ActiveAnimations.Count(); i++)
	{
		if (m_ActiveAnimations[i].panel == panel && m_ActiveAnimations[i].seqName != msg.seqName)
		{
			m_ActiveAnimations.Remove(i);
			--i;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: a posted message to stop animations of a specific type
//-----------------------------------------------------------------------------
void AnimationController::RunCmd_StopAnimation(PostedMessage_t &msg)
{
	Panel *panel = GetParent()->FindChildByName(g_ScriptSymbols.String(msg.event), true);
	Assert(panel != NULL);
	if (!panel)
		return;

	RemoveQueuedAnimationByType(panel, msg.variable, msg.seqName);
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &msg - 
//-----------------------------------------------------------------------------
void AnimationController::RunCmd_SetFont( PostedMessage_t &msg )
{
	Panel *panel = GetParent()->FindChildByName(g_ScriptSymbols.String(msg.event), true);
	Assert(panel != NULL);
	if (!panel)
		return;

	KeyValues *inputData = new KeyValues(g_ScriptSymbols.String(msg.variable));
	inputData->SetString(g_ScriptSymbols.String(msg.variable), g_ScriptSymbols.String(msg.variable2));
	if (!panel->SetInfo(inputData))
	{
	//	assert(!("Unhandlable var in AnimationController::SetValue())"));
	}
	inputData->deleteThis();
}

//-----------------------------------------------------------------------------
// Purpose: Gets the specified value from a panel
//-----------------------------------------------------------------------------
AnimationController::Value_t AnimationController::GetValue(ActiveAnimation_t& anim, Panel *panel, UtlSymId_t var)
{
	Value_t val = { 0, 0, 0, 0 };
	if (var == m_sPosition)
	{
		int x, y;
		panel->GetPos(x, y);
		val.a = (float)x;
		val.b = (float)y;
	}
	else if (var == m_sSize)
	{
		int w, t;
		panel->GetSize(w, t);
		val.a = (float)w;
		val.b = (float)t;
	}
	else if (var == m_sFgColor)
	{
		Color col = panel->GetFgColor();
		val.a = col[0];
		val.b = col[1];
		val.c = col[2];
		val.d = col[3];
	}
	else if (var == m_sBgColor)
	{
		Color col = panel->GetBgColor();
		val.a = col[0];
		val.b = col[1];
		val.c = col[2];
		val.d = col[3];
	}
	else
	{
		KeyValues *outputData = new KeyValues(g_ScriptSymbols.String(var));
		if (panel->RequestInfo(outputData))
		{
			// find the var and lookup it's type
			KeyValues *kv = outputData->FindKey(g_ScriptSymbols.String(var));
			if (kv && kv->GetDataType() == KeyValues::TYPE_FLOAT)
			{
				val.a = kv->GetFloat();
				val.b = 0.0f;
				val.c = 0.0f;
				val.d = 0.0f;
			}
			else if (kv && kv->GetDataType() == KeyValues::TYPE_COLOR)
			{
				Color col = kv->GetColor();
				val.a = col[0];
				val.b = col[1];
				val.c = col[2];
				val.d = col[3];
			}
		}
		else
		{
		//	Assert(!("Unhandlable var in AnimationController::GetValue())"));
		}
		outputData->deleteThis();
	}
	return val;
}

//-----------------------------------------------------------------------------
// Purpose: Sets a value in a panel
//-----------------------------------------------------------------------------
void AnimationController::SetValue(ActiveAnimation_t& anim, Panel *panel, UtlSymId_t var, Value_t &value)
{
	if (var == m_sPosition)
	{
		panel->SetPos((int)value.a, (int)value.b);
	}
	else if (var == m_sSize)
	{
		panel->SetSize((int)value.a, (int)value.b);
	}
	else if (var == m_sFgColor)
	{
		Color col = panel->GetFgColor();
		col[0] = (unsigned char)value.a;
		col[1] = (unsigned char)value.b;
		col[2] = (unsigned char)value.c;
		col[3] = (unsigned char)value.d;
		panel->SetFgColor(col);
	}
	else if (var == m_sBgColor)
	{
		Color col = panel->GetBgColor();
		col[0] = (unsigned char)value.a;
		col[1] = (unsigned char)value.b;
		col[2] = (unsigned char)value.c;
		col[3] = (unsigned char)value.d;
		panel->SetBgColor(col);
	}
	else
	{
		KeyValues *inputData = new KeyValues(g_ScriptSymbols.String(var));
		// set the custom value
		if (value.b == 0.0f && value.c == 0.0f && value.d == 0.0f)
		{
			// only the first value is non-zero, so probably just a float value
			inputData->SetFloat(g_ScriptSymbols.String(var), value.a);
		}
		else
		{
			// multivalue, set the color
			Color col((unsigned char)value.a, (unsigned char)value.b, (unsigned char)value.c, (unsigned char)value.d);
			inputData->SetColor(g_ScriptSymbols.String(var), col);
		}
		if (!panel->SetInfo(inputData))
		{
		//	assert(!("Unhandlable var in AnimationController::SetValue())"));
		}
		inputData->deleteThis();
	}
}
