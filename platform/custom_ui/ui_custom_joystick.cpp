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

#ifdef CUSTOM_JOYSTICK

#include "../platform.h"
#include "ui_custom_joystick.h"
#include "ui_keyboard.h"
#include "../../ui/ui_button.h"

#include "../../tools/options.h"
#include "../../options_common.h"

#include "../../speccy.h"

namespace xUi
{

static const char* joy_buttons[] =
{
	"X", "^", " ", "L", "R",
	"<", " ", ">", "Y", "Z",
	" ", "v", " ", "B", "A"
};

//=============================================================================
//	eCJPad::AllocateText
//-----------------------------------------------------------------------------
byte eCJPad::AllocateText(char key, bool first) const
{
	if (!first)
	{
		if (key == 's' || key == 'c') return 'S';
		if (key == 'e') return 'N';
		if (key == ' ') return 'p';
		return key;
	} 
	else 
	{
		if (key == 's' || key == 'c' || key == 'e') return key;
		if (key == ' ') return 'S';
		return ' ';
	}
}

//=============================================================================
//	eCJPad::Init
//-----------------------------------------------------------------------------
void eCJPad::Init()
{
	background = COLOR_BACKGROUND;
	ePoint margin(6, 6);
	eRect r_dlg(ePoint((FontSize().x * 5 + margin.x ) * 5 + margin.x, ( FontSize().y + margin.y ) * 3 + margin.y));
		

	#ifdef GCWZERO
	r_dlg.Move(ePoint(70, 24));
	#else
	r_dlg.Move(ePoint(8, 8));
	#endif
	
	Bound() = r_dlg;

	eRect r_item(ePoint(FontSize().x * 5, FontSize().y));	
	ePoint delta;
	delta.x = FontSize().x * 5 + margin.x;
	delta.y = FontSize().y + margin.y;
	r_item.Move(margin);
	
	for(int col = 0; col < 3; ++col)
	{
		eRect r = r_item;
		for(int row = 0; row < 5; ++row)
		{
			const char* s = joy_buttons[col * 5 + row];
			char key = '*';
		    // char kCustom[8] = {'O','P','Q','A','M',' ','1','2','3','4'};
		    // {<, >, ^, v, B, A, Y, X, L, R)}*/
			if(*s != ' ')
			{
				eButton* b = new eButton;
				b->Bound() = r;
				b->Id((byte)*s);
				switch(*s)
				{
					case 'X': strcpy(text,"EXIT"); break;
					case '<': strcpy(text,"L:  "); key = kCustom[0]; break; // Left
					case '>': strcpy(text,"R:  "); key = kCustom[1]; break; // Right
					case '^': strcpy(text,"U:  "); key = kCustom[2]; break; // Up
					case 'v': strcpy(text,"D:  "); key = kCustom[3]; break; // Down
					case 'B': strcpy(text,"B:  "); key = kCustom[4]; break; // B
					case 'A': strcpy(text,"A:  "); key = kCustom[5]; break; // A
					case 'Y': strcpy(text,"Y:  "); key = kCustom[6]; break; // Y
					case 'Z': strcpy(text,"X:  "); key = kCustom[7]; break; // X
					case 'L': strcpy(text,"L:  "); key = kCustom[8]; break; // L
					case 'R': strcpy(text,"R:  "); key = kCustom[9]; break; // R
					default: strcpy(text," "); break;
				}

				if (key != '*') 
				{
			    	text[2] = AllocateText(key, true);
			    	text[3] = AllocateText(key, false);
			    }
			    b->Text(text);
				Insert(b);
			}
			r.Move(ePoint(delta.x, 0));
		}
		r_item.Move(ePoint(0, delta.y));
	}
}

//=============================================================================
//	eCJPad::OnNotify
//-----------------------------------------------------------------------------
void eCJPad::OnNotify(byte n, byte from)
{
	byte lastKey = '\0';
	if(n == eButton::N_PUSH)
		lastKey = from;
	eInherited::OnNotify(lastKey, id);
}

//=============================================================================
//	eCustomJoystickDialog::Update
//-----------------------------------------------------------------------------
void eCustomJoystickDialog::Update()
{
	eInherited::Update();
	if(clear)
	{
		clear = false;
		Clear();
	}
	if(quitCustom)
	{
		using namespace xOptions;
		const char* name = xPlatform::OpLastFile();
		char* rawname = xPlatform::ConfigName(name);		
		StoreConfig(rawname); //Save custom joystick
		Clear();
		//Menu close
		showMenu = false;
		xPlatform::Handler()->VideoPaused(false);
		return;
	}
	if(keyboard)
	{
		keyboard = false;
		Clear();
		eKeyboard* k = new eKeyboard;
		k->Id('k');
		Insert(k);
	}
	else if(dpad)
	{
		dpad = false;
		Clear();
		eCJPad* p = new eCJPad;
		p->Id('p');
		Insert(p);
	}
}

//=============================================================================
//	eCustomJoystickDialog::OnNotify
//-----------------------------------------------------------------------------
void eCustomJoystickDialog::OnNotify(byte n, byte from)
{
	using namespace xPlatform;
	switch(from)
	{
	case 'p':
		switch(n)
		{
			case 'X':
				quitCustom = true;
				break;
			case '^':
			case 'v':
			case '<':
			case '>':
			case 'B':
			case 'A':
			case 'Y':
			case 'Z':
			case 'L':
			case 'R':
				clear = true;
				keyboard = true;
				dpad = false;
				joy_key = n;
				break;
		}
		break;
	case 'k':
		eKeyboard* k = (eKeyboard*)*childs;
		lastKey = k->Key();
		dword flags = k->Pressed() ? KF_DOWN : 0;
		flags |= k->Caps() ? KF_SHIFT : 0;
		flags |= k->Symbol() ? KF_ALT : 0;
		flags |= KF_UI_SENDER;
		ParseKey(lastKey, flags);
		clear = true;
		keyboard = false;
		dpad = true;
		break;
	}
	eInherited::OnNotify(n,id);
}

//=============================================================================
//	eCustomJoystickDialog::ParseKey
//-----------------------------------------------------------------------------
void eCustomJoystickDialog::ParseKey(char key, dword _flags)
{
	using namespace xPlatform;
	if(_flags&KF_DOWN)
	{
		switch(joy_key)
		{
			case '<': kCustom[0] = key; break;
			case '>': kCustom[1] = key; break;
			case '^': kCustom[2] = key; break;
			case 'v': kCustom[3] = key; break;
			case 'B': kCustom[4] = key; break;
			case 'A': kCustom[5] = key; break;
			case 'Y': kCustom[6] = key; break;
			case 'Z': kCustom[7] = key; break;
			case 'L': kCustom[8] = key; break;
			case 'R': kCustom[9] = key; break;
		}
	}
}

}
//namespace xUi

#endif//CUSTOM_JOYSTICK