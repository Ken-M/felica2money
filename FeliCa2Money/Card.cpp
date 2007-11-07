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
#include <stdio.h>

#include "Card.h"

/// �R���X�g���N�^
Card::Card()
{
    prev_key = serial = 0;
}

/// �f�X�g���N�^
Card::~Card()
{
    vector<Transaction*>::iterator it;

    for (it = list.begin(); it != list.end(); it++) {
	delete *it;
    }
}

/**
   @brief �g�����U�N�V����ID�̐���
   @param[in] �L�[
   @return �V���A���ԍ�

   �L�[���O��ƈقȂ��Ă���Ώ�� 0 ��Ԃ�
   �����ꍇ�̓V���A���ԍ����C���N�������g���ĕԂ�
*/
int Card::GenerateTransactionId(int key)
{
    if (key != prev_key) {
	serial = 0;
	prev_key = key;
    } else {
	serial++;
    }
    return serial;
}

/**
   @brief �^�u�ŋ�؂�ꂽ�e�s����͂���
   @param[in] lines ��������s(�����s)
   @param[in] reverse �t���ɏ������邩�ǂ����̃t���O
   @return 0 �Ő����A-1 �ŃG���[

   ��͌��ʂ� list �Ɋi�[�����
*/
int CardWithLineParser::ParseLines(TStringList *lines, bool reverse)
{
    char buf[3000];
    AnsiString rows[30];
    int i;
    int start, incr, end, count, err;

    count = lines->Count;
    if (reverse) {
	start = count - 1;
	end = -1;
	incr = -1;
    } else {
	start = 0;
	end = count;
	incr = 1;
    }
    for (i = start; i != end; i += incr) {
	strncpy(buf, lines->Strings[i].c_str(), sizeof(buf));

	// �^�u��؂�𕪉�
	char *p;
	char *pp = buf;

	int n;
	for (n= 0; (p = getTabbedToken(&pp)) != NULL; n++) {
	    rows[n] = p;
	}

	Transaction *t = GenerateTransaction(n, rows, &err);
	if (!t) {
	    if (err) return -1;	// fatal error
	    continue;
	}

	list.push_back(t);
    }
    return 0;
}

/**
   @brief �^�u�ŋ�؂�ꂽ token ���擾����
   @param[in,out] pos �ǂݍ��݈ʒu
   @return token
*/
char * CardWithLineParser::getTabbedToken(char **pos)
{
    char *ret = *pos;

    if (*pos == NULL) {
	return NULL;	// no more token
    }

    char *nextpos = strchr(*pos, '\t');
    if (nextpos) {
	*nextpos = '\0';
	*pos = nextpos + 1;
    } else {
	*pos = NULL;	// no more token
    }
    return ret;
}
