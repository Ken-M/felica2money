/*
 * FeliCa2Money
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

#include <shellapi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <iomanip>

using namespace std;

#include "MainForm.h"
#include "Convert.h"
#include "Card.h"
#include "Transaction.h"

/**
   @brief �R���o�[�g���s
   @param[in] card �ǂݍ��݌� Card
   @param[out] ofxfile �����o�����s�� OFX �t�@�C����

   �J�[�h����ǂݍ��݁AOFX �t�@�C���ɏ����o��
*/
void Converter::Convert(Card *c, AnsiString ofxfile)
{
	card = c;

	// CSV �t�@�C����ǂ�
	int ret = card->ReadCard();

        if (ret) {
               	Application->MessageBox("�J�[�h��ǂނ��Ƃ��ł��܂���ł���", "�G���[", MB_OK);
        	return;
        }
        if (!card->hasAnyTransaction()) {
        	Application->MessageBox("�������ꌏ������܂���", "�G���[", MB_OK);
		return;               
        }

        // OFX �t�@�C���������o��
	ofstream ofs(ofxfile.c_str());
	if (!ofs) {
        	Application->MessageBox("OFX�t�@�C�����J���܂���", "�G���[", MB_OK);
		return;
       	}
	WriteOfx(ofs);
	ofs.close();

        // Money �N��	
        ShellExecute(NULL, "open", ofxfile.c_str(),
        	NULL, NULL, SW_SHOW);
}

/**
   @brief OFX �t�@�C�������o��
   @param[in] ofs �o�̓X�g���[��
*/
void Converter::WriteOfx(ofstream& ofs)
{
	unsigned long idoffset;
        vector<Transaction*>::iterator begin, last;

        begin = card->begin();
	last = card->end();
	last--;

        AnsiString beginDate = dateStr((*begin)->date);
        AnsiString endDate = dateStr((*last)->date);

	/* OFX �w�b�_ */
	ofs << "OFXHEADER:100" << endl;
	ofs << "DATA:OFXSGML" << endl;
	ofs << "VERSION:102" << endl;
	ofs << "SECURITY:NONE" << endl;
	ofs << "ENCODING:UTF-8" << endl;
	ofs << "CHARSET:CSUNICODE" << endl;
	ofs << "COMPRESSION:NONE" << endl;
	ofs << "OLDFILEUID:NONE" << endl;
	ofs << "NEWFILEUID:NONE" << endl;
	ofs << endl;

	/* ���Z�@�֏��(�T�C���I�����X�|���X) */
	ofs << "<OFX>" << endl;
	ofs << "<SIGNONMSGSRSV1>" << endl;
	ofs << "<SONRS>" << endl;
	ofs << "  <STATUS>" << endl;
	ofs << "    <CODE>0" << endl;
	ofs << "    <SEVERITY>INFO" << endl;
	ofs << "  </STATUS>" << endl;
	ofs << "  <DTSERVER>" << beginDate.c_str() << endl;

	ofs << "  <LANGUAGE>JPN" << endl;
	ofs << "  <FI>" << endl;
	ofs << "    <ORG>" <<  card->getIdent() << endl;
	ofs << "  </FI>" << endl;
	ofs << "</SONRS>" << endl;
	ofs << "</SIGNONMSGSRSV1>" << endl;

	/* �������(�o���N���b�Z�[�W���X�|���X) */
	ofs << "<BANKMSGSRSV1>" << endl;

	/* �a�������^���׏��쐬 */
	ofs << "<STMTTRNRS>" << endl;
	ofs << "<TRNUID>0" << endl;
	ofs << "<STATUS>" << endl;
	ofs << "  <CODE>0" << endl;
	ofs << "  <SEVERITY>INFO" << endl;
	ofs << "</STATUS>" << endl;

	ofs << "<STMTRS>" << endl;
	ofs << "  <CURDEF>JPY" << endl;

	ofs << "  <BANKACCTFROM>" << endl;
	ofs << "    <BANKID>" << card->getIdent() << endl;
	ofs << "    <BRANCHID>" << "000" << endl;
	ofs << "    <ACCTID>" << card->getCardId() << endl;
	ofs << "    <ACCTTYPE>SAVINGS" << endl;
	ofs << "  </BANKACCTFROM>" << endl;

	/* ���׏��J�n(�o���N�g�����U�N�V�������X�g) */
	ofs << "  <BANKTRANLIST>" << endl;
	ofs << "    <DTSTART>" << beginDate.c_str() << endl;
	ofs << "    <DTEND>" << endDate.c_str() << endl;

	/* �g�����U�N�V���� */
	vector<Transaction*>::iterator it;

        for (it = card->begin(); it != card->end(); it++) {
        	Transaction *t = *it;

		ofs << "    <STMTTRN>" << endl;
		ofs << "      <TRNTYPE>" <<  t->GetTrnTypeStr() << endl;
		ofs << "      <DTPOSTED>" << dateStr(t->date).c_str() << endl;
		ofs << "      <TRNAMT>" <<  t->value << endl;

		ofs << "      <FITID>" << transIdStr(t).c_str() << endl;

		ofs << "      <NAME>" << t->desc.c_str() << endl;
                if (!t->memo.IsEmpty()) {
			ofs << "      <MEMO>" << t->memo.c_str() << endl;
                }
		ofs << "    </STMTTRN>" << endl;
	};

	ofs << "  </BANKTRANLIST>" << endl;

	/* �c�� */
	ofs << "  <LEDGERBAL>" << endl;
	ofs << "    <BALAMT>" << (*last)->balance << endl;
	ofs << "    <DTASOF>" << endDate.c_str() << endl;
	ofs << "  </LEDGERBAL>" << endl;

	/* OFX �I�� */
	ofs << "  </STMTRS>" << endl;
	ofs << "</STMTTRNRS>" << endl;
	ofs << "</BANKMSGSRSV1>" << endl;
	ofs << "</OFX>" << endl;
}

AnsiString Converter::dateStr(const DateTime & dt)
{
	AnsiString str;

	/*              Y   M   D   H   M   S */
	str.sprintf("%4d%02d%02d%02d%02d%02d[+9:JST]",
		dt.year, dt.month, dt.date,
		dt.hour, dt.minutes, dt.seconds);
	return str;
}

/**
   @brief �g�����U�N�V����ID���擾
   @param[in] t �g�����U�N�V����
   @out �g�����U�N�V����ID

   ���t�ƃg�����U�N�V����ID ���當����𐶐�����
*/
AnsiString Converter::transIdStr(const Transaction *t)
{
	AnsiString str;

	str.sprintf("%04d%02d%02d%07d",
		    t->date.year, t->date.month, t->date.date, t->id);
	return str;
}


