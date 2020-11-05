/*
Portable ZX-Spectrum emulator.
Copyright (C) 2001-2010 SMT, Dexus, Alone Coder, deathsoft, djdron, scor

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "../platform.h"
#include "../io.h"
#include "ui_main.h"
#include "ui_menu.h"
#include "ui_keyboard.h"
#include "ui_file_open.h"
#ifdef CUSTOM_JOYSTICK
#include "ui_custom_joystick.h"
#endif
#include "../../tools/options.h"
#include "../../tools/profiler.h"
#include "../../options_common.h"

#include "../../speccy.h"

#ifdef USE_UI


	bool showMenu = false;

namespace xUi
{

#ifdef USE_PROFILER
eDialog* CreateProfiler();
#endif//USE_PROFILER

static struct eOptionOpenFile : public xOptions::eOptionB
{
	eOptionOpenFile() : on(false) { storeable = false; }
	virtual const char* Name() const { return "open file"; }
	virtual const char*	Value() const { return ">"; }
	virtual void Change(bool next = true) { if(next) on = true; }
	bool on;
} op_open_file;

#ifdef CUSTOM_JOYSTICK
static struct eOptionJCustom : public xOptions::eOptionB
{
	eOptionJCustom() : on(false) { storeable = false; }
	virtual const char* Name() const { return "custom joy"; }
	//virtual const char*	Value() const { char* s = new char[8]; s = xPlatform::Handler()->CustomJoystick(); s[11] = '\0'; return strcat(s," >"); }

	virtual const char*	Value() const { return ">"; }
	virtual void Change(bool next = true) { if(next) on = true; }
	virtual int Order() const { return 12; }
	bool on;
} custom_joy;
#endif

//=============================================================================
//	eMainDialog::eMainDialog
//-----------------------------------------------------------------------------
eMainDialog::eMainDialog() : clear(false)
{
}
//=============================================================================
//	eMainDialog::Update
//-----------------------------------------------------------------------------
void eMainDialog::Update()
{
	eInherited::Update();
	if(clear)
	{
		clear = false;
		Clear();
	}
	if(op_open_file.on)
	{
		op_open_file.on = false;
		Clear();
		eDialog* d = new eFileOpenDialog(xPlatform::OpLastFolder());
		d->Id(D_FILE_OPEN);
		Insert(d);
	}
	#ifdef CUSTOM_JOYSTICK
	if(custom_joy.on)
	{
		custom_joy.on = false;
		Clear();
		showMenu = false;
		eDialog* d = new eCustomJoystickDialog(kCustom);
		d->Id(D_CUSTOM_JOY);
		Insert(d);
	}
	#endif
}
//=============================================================================
//	eMainDialog::OnKey
//-----------------------------------------------------------------------------
bool eMainDialog::OnKey(char key, dword flags)
{
	bool f = Focused();
	byte id = f ? (*childs)->Id() : -1;
	switch(key)
	{
	case 'k':
		if ((!f || id != D_KEYS) && (!showMenu))
		{
			Clear();
			eKeyboard* d = new eKeyboard;
			d->Id(D_KEYS);
			Insert(d);
		}
		else if (!showMenu)
		{
			Clear();
		}
		return true;
	case 'm':
		Clear();
		if (!f || id != D_MENU)
		{	
			showMenu = true;
			eMenu* d = new eMenu;
			d->Id(D_MENU);
			Insert(d);
			using namespace xOptions;
			xPlatform::Handler()->VideoPaused(true);
		}
		else
		{
			showMenu = false;
			using namespace xOptions;
			#ifdef V90
			eOptionB* o = eOptionB::Find("pause (R2)");
			#else
			eOptionB* o = eOptionB::Find("pause");			
			#endif
			if (o->Values() != 0) //static const char* values[] = { "off", "on", NULL };
			{
				xPlatform::Handler()->VideoPaused(false);
			}
		}
		return true;
#ifdef USE_PROFILER
	case 'p':
		Clear();
		if(!f || id != D_PROFILER)
		{
			eDialog* d = CreateProfiler();
			d->Id(D_PROFILER);
			Insert(d);
		}
		return true;
#endif//USE_PROFILER
	}
	return eInherited::OnKey(key, flags);
}
//=============================================================================
//	eMainDialog::OnNotify
//-----------------------------------------------------------------------------
void eMainDialog::OnNotify(byte n, byte from)
{
	using namespace xPlatform;
	switch(from)
	{
	case D_FILE_OPEN:
		if(!clear)
		{
			eFileOpenDialog* d = (eFileOpenDialog*)*childs;
			Handler()->OnOpenFile(d->Selected());
			clear = true;
		}
		break;
	case D_KEYS:
		{
			eKeyboard* d = (eKeyboard*)*childs;
			byte key = d->Key();
			dword flags = d->Pressed() ? KF_DOWN : 0;
			flags |= d->Caps() ? KF_SHIFT : 0;
			flags |= d->Symbol() ? KF_ALT : 0;
			flags |= KF_UI_SENDER;
			Handler()->OnKey(key, flags);
		}
		break;

	#ifdef CUSTOM_JOYSTICK
	case D_CUSTOM_JOY:
		{
			eCustomJoystickDialog* d = (eCustomJoystickDialog*)*childs;
			if(d->QuitCustom()){
				clear = true;
			}
		}
		break;
	#endif
	}

}

}
//namespace xUi

#endif//USE_UI
