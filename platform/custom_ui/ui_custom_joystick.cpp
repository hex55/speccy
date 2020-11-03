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
//	eCJPad::AllocateText
//-----------------------------------------------------------------------------
byte eCJPad::AllocateText(char key) const
{
	if (key == 's') return '$';
	if (key == 'c') return '#';
	if (key == 'e') return '=';
	return key;
}

//=============================================================================
//	eCJPad::Init
//-----------------------------------------------------------------------------
void eCJPad::Init()
{
	background = COLOR_BACKGROUND;
	ePoint margin(6, 6);
	eRect r_dlg(ePoint((FontSize().x * 5 + margin.x ) * 5 + margin.x, ( FontSize().y + margin.y ) * 3 + margin.y));
	if(gcw_fullscreen)
	{
		r_dlg.Move(ePoint(32, 24));
	}
	else
	{
		r_dlg.Move(ePoint(8, 8));
	}
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
		    // char kCustom[8] = {'O','P','Q','A','M',' ','1','2','3','4'};
		    // {<, >, ^, v, B, A, Y, X, L, R)}*/
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
					strcpy(text,"[ ] L"); //Left
			    	text[1] = AllocateText(kCustom[0]);
					b->Text(text);
				    break;
				case '>':
					strcpy(text,"[ ] R"); //Right
				    text[1] = AllocateText(kCustom[1]);
				    b->Text(text);
				    break;
				case '^':
					strcpy(text,"[ ] U"); //Up
				    text[1] = AllocateText(kCustom[2]);
				    b->Text(text);
				    break;
				case 'v':
					strcpy(text,"[ ] D"); // Down
				    text[1] = AllocateText(kCustom[3]);
				    b->Text(text);
				    break;
				case 'B':
					strcpy(text,"[ ] B");
				    text[1] = AllocateText(kCustom[4]);
				    b->Text(text);
				    break;
				case 'A':
					strcpy(text,"[ ] A");
				    text[1] = AllocateText(kCustom[5]);
				    b->Text(text);
				    break;
				case 'Y':
					strcpy(text,"[ ] Y");
				    text[1] = AllocateText(kCustom[6]);
				    b->Text(text);
				    break;
				case 'Z':
					strcpy(text,"[ ] X");
				    text[1] = AllocateText(kCustom[7]);
				    b->Text(text);
				    break;
				case 'L':
					strcpy(text,"[ ] L");
				    text[1] = AllocateText(kCustom[8]);
				    b->Text(text);
				    break;
				case 'R':
					strcpy(text,"[ ] R");
				    text[1] = AllocateText(kCustom[9]);
				    b->Text(text);
				    break;
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