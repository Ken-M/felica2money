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

#include "MainForm.h"
#include "Convert.h"
#include "Card.h"
#include "Transaction.h"

static AnsiString dateStr(DateTime *dt)
{
	AnsiString str;

	/*              Y   M   D   H   M   S */
	str.sprintf("%4d%02d%02d%02d%02d%02d[+9:JST]",
		dt->year, dt->month, dt->date,
		dt->hour, dt->minutes, dt->seconds);
	return str;
}

static void
WriteOfx(FILE *fp, TransactionList *list, Card *card)
{
	unsigned long idoffset;
	Transaction *t, *last;

	last = list->Tail();
	t = list->Head();

	/* OFX �w�b�_ */
	fprintf(fp, "OFXHEADER:100\n");
	fprintf(fp, "DATA:OFXSGML\n");
	fprintf(fp, "VERSION:102\n");
	fprintf(fp, "SECURITY:NONE\n");
	fprintf(fp, "ENCODING:UTF-8\n");
	fprintf(fp, "CHARSET:CSUNICODE\n");
	fprintf(fp, "COMPRESSION:NONE\n");
	fprintf(fp, "OLDFILEUID:NONE\n");
	fprintf(fp, "NEWFILEUID:NONE\n");
	fprintf(fp, "\n");

	/* ���Z�@�֏��(�T�C���I�����X�|���X) */
	fprintf(fp, "<OFX>\n");
	fprintf(fp, "<SIGNONMSGSRSV1>\n");
	fprintf(fp, "<SONRS>\n");
	fprintf(fp, "  <STATUS>\n");
	fprintf(fp, "    <CODE>0\n");
	fprintf(fp, "    <SEVERITY>INFO\n");
	fprintf(fp, "  </STATUS>\n");
	fprintf(fp, "  <DTSERVER>%s\n", dateStr(&last->date).c_str());

	fprintf(fp, "  <LANGUAGE>JPN\n");
	fprintf(fp, "  <FI>\n");
	fprintf(fp, "    <ORG>%s\n", card->getIdent());
	fprintf(fp, "  </FI>\n");
	fprintf(fp, "</SONRS>\n");
	fprintf(fp, "</SIGNONMSGSRSV1>\n");

	/* �������(�o���N���b�Z�[�W���X�|���X) */
	fprintf(fp, "<BANKMSGSRSV1>\n");

	/* �a�������^���׏��쐬 */
	fprintf(fp, "<STMTTRNRS>\n");
	fprintf(fp, "<TRNUID>0\n");
	fprintf(fp, "<STATUS>\n");
	fprintf(fp, "  <CODE>0\n");
	fprintf(fp, "  <SEVERITY>INFO\n");
	fprintf(fp, "</STATUS>\n");

	fprintf(fp, "<STMTRS>\n");
	fprintf(fp, "  <CURDEF>JPY\n");

	fprintf(fp, "  <BANKACCTFROM>\n");
	fprintf(fp, "    <BANKID>%s\n", 	card->getIdent());
	fprintf(fp, "    <BRANCHID>%s\n", 	"000");
	fprintf(fp, "    <ACCTID>%s\n", 	card->getCardId());
	fprintf(fp, "    <ACCTTYPE>SAVINGS\n");
	fprintf(fp, "  </BANKACCTFROM>\n");

	/* ���׏��J�n(�o���N�g�����U�N�V�������X�g) */
	fprintf(fp, "  <BANKTRANLIST>\n");
	fprintf(fp, "    <DTSTART>%s\n", dateStr(&t->date).c_str());
	fprintf(fp, "    <DTEND>%s\n", dateStr(&last->date).c_str());

	/* �g�����U�N�V���� */
	do {
		fprintf(fp, "    <STMTTRN>\n");
		fprintf(fp, "      <TRNTYPE>%s\n", t->GetTrnTypeStr());
		fprintf(fp, "      <DTPOSTED>%s\n", dateStr(&t->date).c_str());
		fprintf(fp, "      <TRNAMT>%d\n", t->value);

		/* �g�����U�N�V������ ID �͓��t�Ǝ���ԍ��Ő��� */
		fprintf(fp, "      <FITID>%04d%02d%02d%07d\n",
			t->date.year, t->date.month, t->date.date,
			t->id);
		fprintf(fp, "      <NAME>%s\n", t->desc);
		fprintf(fp, "    </STMTTRN>\n");
	} while ((t = list->Next()) != NULL);

	fprintf(fp, "  </BANKTRANLIST>\n");

	/* �c�� */
	fprintf(fp, "  <LEDGERBAL>\n");
	fprintf(fp, "    <BALAMT>%d\n", last->balance);
	fprintf(fp, "    <DTASOF>%s\n", dateStr(&last->date).c_str());
	fprintf(fp, "  </LEDGERBAL>\n");

	/* OFX �I�� */
	fprintf(fp, "  </STMTRS>\n");
	fprintf(fp, "</STMTTRNRS>\n");
	fprintf(fp, "</BANKMSGSRSV1>\n");
	fprintf(fp, "</OFX>\n");
}

void Convert(AnsiString ofxfile, Card *card)
{
	TransactionList *t;

	// CSV �t�@�C����ǂ�
	t = card->ReadCard();

        if (!t) {
               	Application->MessageBox("�J�[�h��ǂނ��Ƃ��ł��܂���ł���", "�G���[", MB_OK);
        	return;
        }
        if (!t->hasAnyTransaction()) {
        	Application->MessageBox("�������ꌏ������܂���", "�G���[", MB_OK);
		return;               
        }

        // OFX �t�@�C���������o��
	FILE *fp = fopen(ofxfile.c_str(), "wb");
	if (!fp) {
        	Application->MessageBox("OFX�t�@�C�����J���܂���", "�G���[", MB_OK);
		return;
       	}
        WriteOfx(fp, t, card);
	fclose(fp);

        // Money �N��	
        ShellExecute(NULL, "open", ofxfile.c_str(),
        	NULL, NULL, SW_SHOW);
}

