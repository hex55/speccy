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
//	eCJPad::Init
//-----------------------------------------------------------------------------
void eCJPad::Init()
{
	background = COLOR_BACKGROUND;
	ePoint margin(6, 6);
	eRect r_dlg(ePoint((FontSize().x * 8 + margin.x ) * 4 + margin.x, ( FontSize().y + margin.y ) * 3 + margin.y));
	if(gcw_fullscreen)
	{
		r_dlg.Move(ePoint(32, 24));
	}
	else
	{
		r_dlg.Move(ePoint(8, 8));
	}
	Bound() = r_dlg;

	eRect r_item(ePoint(FontSize().x * 15, FontSize().y));
	ePoint delta;
	delta.x = FontSize().x * 15 + margin.x;
	delta.y = FontSize().y + margin.y;
	r_item.Move(margin);
	char* joystick = xPlatform::Handler()->CustomJoystick();
	for(int col = 0; col < 3; ++col)
	{
		eRect r = r_item;
		for(int row = 0; row < 5; ++row)
		{
			const char* s = joy_buttons[col * 3 + row];
			if(*s != ' ')
			{
				eButton* b = new eButton;
				b->Bound() = r;
				b->Id((byte)*s);
				switch(*s)
				{
				case 'X':
				    strcpy(text,"EXIT");
				    b->Text(text);
				    break;
				case '<':
					strcpy(text,"[ ]Left");
				    text[1] = joystick[0];
				    b->Text(text);
				    break;
				case '>':
					strcpy(text,"[ ]Right");
				    text[1] = joystick[1];
				    b->Text(text);
				    break;
				case '^':
					strcpy(text,"[ ]Up");
				    text[1] = joystick[2];
				    b->Text(text);
				    break;
				case 'v':
					strcpy(text,"[ ]Down");
				    text[1] = joystick[3];
				    b->Text(text);
				    break;
				case 'B':
					strcpy(text,"[ ]B");
				    text[1] = joystick[4];
				    b->Text(text);
				    break;
				case 'A':
					strcpy(text,"[ ]A");
				    text[1] = joystick[5];
				    b->Text(text);
				    break;
				case 'Y':
					strcpy(text,"[ ]Y");
				    text[1] = joystick[6];
				    b->Text(text);
				    break;
				case 'Z':
					strcpy(text,"[ ]X");
				    text[1] = joystick[7];
				    b->Text(text);
				    break;
				case 'L':
					strcpy(text,"[ ]L");
				    text[1] = joystick[8];
				    b->Text(text);
				    break;
				case 'R':
					strcpy(text,"[ ]R");
				    text[1] = joystick[9];
				    b->Text(text);
				    break;
				    //	char kCustom[5] = {'O','P','Q','A','M','Q','1','2','0','3'}; /* {<, >, ^, v, B, A, Y, X, L, R)}*/
				default:
					b->Text(s);
					break;
				}
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
		Clear();
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
			case 'A':
			case 'B':
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
			case '<': joystick[0] = key; break;
			case '>': joystick[1] = key; break;
			case '^': joystick[2] = key; break;
			case 'v': joystick[3] = key; break;
			case 'B': joystick[4] = key; break;
			case 'A': joystick[5] = key; break;
			case 'Y': joystick[6] = key; break;
			case 'Z': joystick[7] = key; break;
			case 'L': joystick[8] = key; break;
			case 'R': joystick[9] = key; break;
		}
	}
	Handler()->SetCustomJoystick(joystick);
}

}
//namespace xUi

#endif//CUSTOM_JOYSTICK