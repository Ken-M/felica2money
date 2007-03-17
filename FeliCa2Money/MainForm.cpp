/*
 * Pasori2Money
 *
 * Copyright (C) 2001-2007 Takuya Murakami
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <Registry.hpp>
#include <ShellApi.h>

#include "MainForm.h"
#include "Convert.h"
#include "Card.h"
#include "SfcPeep.h"

#include "Edy.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMForm *MForm;
//---------------------------------------------------------------------------
__fastcall TMForm::TMForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TMForm::FormShow(TObject *Sender)
{
	cards.AddCard(new EdyCard);
        SfcPeep = new SFCPeep;

	LoadRegistry();
}
//---------------------------------------------------------------------------
void __fastcall TMForm::ButtonConvertClick(TObject *Sender)
{
	char tmppath[1024];
	GetTempPath(sizeof(tmppath), tmppath);

        AnsiString t = tmppath;
	AnsiString ofxfile = t + "Pasori2Money.ofx";
        AnsiString tmpfile = t + "Pasori2Money.csv";

        SfcPeep->SetSfcPeepPath(SFCPeepPath);
	SfcPeep->SetTempFile(tmpfile);
        
	::Convert(ofxfile, &cards);
}

//---------------------------------------------------------------------------

void __fastcall TMForm::ButtonQuitClick(TObject *Sender)
{
	Application->Terminate();
}
//---------------------------------------------------------------------------
void TMForm::LoadRegistry(void)
{
	// ���W�X�g������ݒ�̓ǂݍ���
	TRegistry *reg = new TRegistry();

	reg->RootKey = HKEY_CURRENT_USER;
	reg->OpenKey("\\Software\\Takuya Murakami\\Pasori2Money", true);

	SFCPeepPath = reg->ReadString("SFCPeepPath");
	if (SFCPeepPath.IsEmpty()) {
        	SFCPeepPath = "c:\\Program Files\\DENNO NET\\SFCPeep\\SFCPeep.exe";
        }
}
//---------------------------------------------------------------------------
void TMForm::SaveRegistry(void)
{
	TRegIniFile *ini;

	// ���W�X�g���ɐݒ��ۑ�
	TRegistry *reg = new TRegistry();

	reg->RootKey = HKEY_CURRENT_USER;
	reg->OpenKey("\\Software\\Takuya Murakami\\Pasori2Money", true);

	reg->WriteString("SFCPeepPath", SFCPeepPath);
}

//---------------------------------------------------------------------------
void __fastcall TMForm::ButtonHelpClick(TObject *Sender)
{
	ShellExecute(NULL, "open", "pasori2money.html",
        NULL, NULL, SW_SHOWDEFAULT);
}
//---------------------------------------------------------------------------


void __fastcall TMForm::ButtonConfigClick(TObject *Sender)
{
	if (OpenDialog->Execute()) {
        	SFCPeepPath = OpenDialog->FileName;
                SaveRegistry();
        }	
}
//---------------------------------------------------------------------------

