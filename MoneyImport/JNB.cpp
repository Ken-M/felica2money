/*
 * MoneyImport : Convert Bank csv file to MS Money OFX file.
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

#include <vcl.h>
#pragma hdrstop
#include <stdio.h>
#include "Account.h"
#include "Transaction.h"
#include "JNB.h"

//
// JNB �A�J�E���g
//
JNBAccount::JNBAccount(void)
{
	Ident = "JapanNetBank";
	BankName = "�W���p���l�b�g��s";
	BankId = "0033";
}

TransactionList * JNBAccount::ReadFile(FILE *fp)
{
	TransactionList *list = new JNBTransaction;
	if (list->ReadCsv(fp) < 0) {
		delete list;
		return NULL;
	}
	return list;
}

// 
// JNB �g�����U�N�V�������X�g
//
Transaction *JNBTransaction::GenerateTransaction(int nrows, char **rows, int *err)
{
	Transaction *trans = new Transaction;

	/* "�����(�N)","�����(��)","�����(��)","������ԍ�",
	   "�E�v",  "���x�����z","���a����z","�c��" */
	trans->date.year  = atoi(rows[0]);
	trans->date.month = atoi(rows[1]);
	trans->date.date  = atoi(rows[2]);

	trans->date.hour = 0;
	trans->date.minutes = 0;
	trans->date.seconds = 0;

	trans->id = atol(rows[3]);
	if (strcmp(rows[5], "") != 0) {
		trans->SetTransactionType(rows[4], T_OUTGO);
		trans->value = - atol(rows[5]);
	} else {
		trans->SetTransactionType(rows[4], T_INCOME);
		trans->value = atol(rows[6]);
	}
	trans->desc = utf8(rows[4]);
	trans->balance = atol(rows[7]);

	return trans;
}