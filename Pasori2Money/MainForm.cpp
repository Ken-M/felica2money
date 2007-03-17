/*
 * MoneyImport : Convert Bank transaction csv file to MS Money OFX file.
 *
 * Copyright (c) 2001-2003 Takuya Murakami. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <Registry.hpp>
#include <ShellApi.h>

#include "MainForm.h"
#include "Convert.h"
#include "Account.h"

//#include "JNB.h"
//#include "SonyBank.h"
//#include "Ebank.h"

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
	//accounts.AddAcount(new JNBAccount);
	//accounts.AddAcount(new SBAccount);
	//accounts.AddAcount(new EbankAccount);

	LoadRegistry();
}
//---------------------------------------------------------------------------
void __fastcall TMForm::ButtonConvertClick(TObject *Sender)
{
	AnsiString ofxfile = ExtractFilePath(Application->ExeName) +
		"ImportMoney.ofx";

	::Convert(SFCPeepPath, ofxfile, &cards);
}

//---------------------------------------------------------------------------
void __fastcall TMForm::EditJNBAccountExit(TObject *Sender)
{
	SaveRegistry();
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
	ShellExecute(NULL, "open", "MoneyImport.chm",
        NULL, NULL, SW_SHOWDEFAULT);
}
//---------------------------------------------------------------------------

void __fastcall TMForm::AcGridExit(TObject *Sender)
{
	SaveRegistry();	
}
//---------------------------------------------------------------------------

