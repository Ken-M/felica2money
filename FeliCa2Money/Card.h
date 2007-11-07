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

#ifndef	_CARD_H
#define	_CARD_H

#include <stdio.h>
#include "Transaction.h"

/**
   @brief �J�[�h���N���X
*/
class CardBase {
protected:	
    AnsiString	Ident;		///< �J�[�h�� (Ident)
    AnsiString	CardName;	///< �J�[�h��
    AnsiString	CardId;		///< �J�[�h�ŗLID (IDm)

public:
    /// �J�[�hID��ݒ肷��
    inline void SetCardId(AnsiString &id) {
	CardId = id;
    }

    /// Ident ��Ԃ�
    inline char *getIdent(void)	{ return Ident.c_str(); }

    /// �J�[�h����Ԃ�
    inline char *getCardName(void)	{ return CardName.c_str(); }

    /// �J�[�h�ŗLID��Ԃ�
    inline char *getCardId(void)    { return CardId.c_str(); }
};

/**
   @brief �J�[�h���N���X(�g�����U�N�V�����Ǘ���)
*/
class Card : public CardBase
{
protected:
    vector<Transaction*> list;	///< �g�����U�N�V�������X�g

private:
    int prev_key, serial;		///< �g�����U�N�V����ID�����p

public:
    Card();
    virtual ~Card();

    /**
       @brief �J�[�h��ǂݍ���
	   
       �J�[�h��ǂݍ��݁A�J�[�hID���擾���A�g�����U�N�V�������X�g���\������B
    */
    virtual int ReadCard(void) = 0;

    inline bool hasAnyTransaction(void) { return list.size() > 0 ? true : false; }

    inline vector<Transaction *>::iterator begin() {
	return list.begin();
    }
    inline vector<Transaction *>::iterator end() {
	return list.end();
    }

    /** �g�����U�N�V���� ID ���� */
    int GenerateTransactionId(int key);
};

/**
   @brief �J�[�h���N���X (���C���p�[�T�t��)
*/
class CardWithLineParser : public Card
{
public:
    int ParseLines(TStringList *lines, bool reverse = false);

private:    
    /**
       @brief �g�����U�N�V�����̐���
       @param nrows[in] ���̓J������
       @param rows[in] ���̓J�����̊e������
       @param err[out] �G���[�R�[�h
       @return �g�����U�N�V����

       SFCPeep �̊e�o�͍s�ɑ΂��A�e�s�̃J�����𕪉��������̂������ɓn�����B
       �{�֐��́A�������͂��A�g�����U�N�V�����𐶐����ĕԂ��B
    */
    virtual Transaction *GenerateTransaction(int nrows, AnsiString *rows, int *err) = 0;

    /** �^�u�ŋ�؂�ꂽ�g�[�N����؂�o�� */
    char * getTabbedToken(char **pos);
};

#endif	// _CARD_H
