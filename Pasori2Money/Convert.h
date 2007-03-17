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

#ifndef	_CONVERT_H
#define	_CONVERT_H

#if 0
//
// ������
//
typedef enum {
	// ����
	T_INT=0,	// ����
	T_DIV,		// �z��
	T_DIRECTDEP,	// �U�������A�旧�����A���������߂�����
	T_DEP,		// ���̑�����

	// �o��
	T_PAYMENT,	// �����������Ƃ�
	T_CASH,		// ���������o��
	T_ATM,		// �J�[�h�ɂ������o��
	T_CHECK,	// ���؎�֘A���	
	T_DEBIT,	// ���̑��o��
} trntype;

#define	T_INCOME	0
#define	T_OUTGO		1



// ���t�f�[�^
typedef struct {
	int year;
	int month;
	int date;
	int hour;
	int minutes;
	int seconds;
} DateTime;

//
// �g�����U�N�V�����f�[�^
//
typedef struct _transaction {
	struct _transaction *next;

	DateTime	date;	// ���t
	unsigned long	id; 	// ID
	AnsiString	desc;          // ����
	trntype		type;		// ���
	long		value;		// ���z
        long		balance;	// �c��
} Transaction;

// utility funcs
extern void SplitLine(char *line, char **rows);
extern trntype GetTrnType(const char *desc, int type);
extern AnsiString utf8(char *sjis);

#endif

class Cards;
extern void Convert(AnsiString ofxfile, Cards *cards);

#endif	// _CONVERT_H

