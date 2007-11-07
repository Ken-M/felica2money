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

#include <vcl.h>
#pragma hdrstop
#include <stdio.h>
#include "Card.h"
#include "Transaction.h"
#include "SfcPeep.h"
#include "Suica.h"

//
// Suica
//

SuicaCard::SuicaCard(void)
{
    Ident = "Suica";
    CardName = "Suica";
    prevBalance = UndefBalance;
}

int SuicaCard::ReadCard(void)
{
    //
    // IDm�ǂݍ���
    //
    if (SfcPeep->Execute("-i") < 0) return -1;

    // ��s�ڂ��m�F
    TStringList *lines = SfcPeep->Lines();
    if (lines->Count < 1) {
	// no data
	return -1;
    }
    AnsiString head = lines->Strings[0];
    lines->Delete(0);

    if (head.SubString(1,4) != "IDm:") {
	return -1;
    }
    CardId = head.SubString(5, head.Length() - 4);

    //
    // �����f�[�^�ǂݍ���
    //
    if (SfcPeep->Execute("-h") < 0) return -1;

    // ��s�ڃ`�F�b�N
    lines = SfcPeep->Lines();
    if (lines->Count < 1) {
	// no data
	return -1;
    }
    head = lines->Strings[0];
    if (head.SubString(1,5) != "HT00:") {
	return -1;
    }

    // transaction ���
    if (ParseLines(lines, true) < 0) {
	return -1;
    }
    return 0;
}

//
// �g�����U�N�V�������X�g
//
Transaction *SuicaCard::GenerateTransaction(int nrows, AnsiString *rows, int *err)
{
    // 0:�[����R�[�h,1:����,2:���t����,
    // 3:������R�[�h,4:���w���R�[�h,5:�����,6:���w��,
    // 7:�o����R�[�h,8:�o�w���R�[�h,9:�o���,10:�o�w��,
    // 11:�c��,12:����A��

    // �c��
    long balance = rows[11].ToInt();
    long value;

    // ����z�v�Z
    // Suica �̊e����ɂ́A�c�������L�^����Ă��Ȃ� (ouch!)
    // �Ȃ̂ŁA�O��c���Ƃ̍����Ŏ���z���v�Z����
    // ����āA�ŏ��̂P����͏����s�\�Ȃ̂œǂݔ�΂�
    if (prevBalance == UndefBalance) {
	prevBalance = balance;
	*err = 0;
	return NULL;
    } else {
	value = balance - prevBalance;
	prevBalance = balance;
    }

    // ����
    AnsiString desc = rows[1];
    if (desc == "----") {
	return NULL;	// ��G���g��
    }


    Transaction *trans = new Transaction;

    trans->value = value;
    trans->balance = balance;

    // ���t
    AnsiString date = rows[2];
    trans->date.year  = date.SubString(1, 2).ToInt() + 2000;
    trans->date.month = date.SubString(5, 2).ToInt();
    trans->date.date  = date.SubString(9, 2).ToInt();

    trans->date.hour    = 0;
    trans->date.minutes = 0;
    trans->date.seconds = 0;

    // ID
    AnsiString hex = "0x";
    hex += rows[12];
    trans->id = StrToInt(hex.c_str());

    // ����
    AnsiString memo;
    if (!rows[5].IsEmpty()) {
	// �^���̏ꍇ�A����Ђ�K�p�ɒǉ�
	desc += " ";
	desc += rows[5];

	// ���l�ɓ��o���/�w�����L��
	memo = rows[5] + "(" + rows[6] + ")";
	if (!rows[9].IsEmpty()) {
	    memo += " - " + rows[9] + "(" + rows[10] + ")";
	}
    } else {
	// �����ɕ��̂̏ꍇ�A9, 10 �ɓX��������
	if (!rows[9].IsEmpty() && rows[9] != "���o�^") {
	    desc += " ";
	    desc += rows[9];
	}
	if (!rows[10].IsEmpty() && rows[10] != "���o�^") {
	    desc += " ";
	    desc += rows[10];
	}

	// ���ꏈ��
	if (desc == "����") {
	    // ���o�^�X�܂��ƓK�p�����ׂ�"����"�݂̂ɂȂ��Ă��܂��B
	    // ����ƁAMoney ������ɉߋ��̗�������X�ܖ���⊮���Ă��܂��A
	    // �s���������B�̂ŁA�����ł͒ʂ��ԍ���t���Ă���
	    desc += " ";
	    desc += rows[12];
	}
    }

    if (value < 0) {
	trans->SetTransactionType(desc.c_str(), T_OUTGO);
    } else {
	trans->SetTransactionType(desc.c_str(), T_INCOME);
    }

    trans->desc = sjis2utf8(desc);
    if (!memo.IsEmpty()) {
	trans->memo = sjis2utf8(memo);
    }

    return trans;
}
