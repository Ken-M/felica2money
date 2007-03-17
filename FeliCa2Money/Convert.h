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

