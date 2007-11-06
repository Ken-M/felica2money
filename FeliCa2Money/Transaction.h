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
#ifndef _TRANSACTION_H
#define _TRANSACTION_H

#include <vector>

using namespace std;

/**
  @brief ������
*/
typedef enum {
	// ����
	T_INT=0,	///< ����
	T_DIV,		///< �z��
	T_DIRECTDEP,	///< �U�������A�旧�����A���������߂�����
	T_DEP,		///< ���̑�����

	// �o��
	T_PAYMENT,	///< �����������Ƃ�
	T_CASH,		///< ���������o��
	T_ATM,		///< �J�[�h�ɂ������o��
	T_CHECK,	///< ���؎�֘A���
	T_DEBIT,	///< ���̑��o��
} trntype;

//
// �����ޖ� (��̒l�Ə�����v���邱��)
//
#ifdef DEFINE_TRNNAME
const char *trnname[] = {
	"INT", "DIV", "DIRECTDEP", "DEP",
	"PAYMENT", "CASH", "ATM", "CHECK", "DEBIT"
};
#endif

/**
   @brief �����ޕϊ��\
*/
struct trntable {
	const char	*key;	///< �L�[
	trntype		type;	///< ������
};

#define	T_INCOME	0
#define	T_OUTGO		1

/**
   @brief ���t���
*/
typedef struct {
	int year;
	int month;
	int date;
	int hour;
	int minutes;
	int seconds;
} DateTime;


/**
   @brief �g�����U�N�V�����f�[�^
*/
class Transaction {
    public:
	DateTime	date;		///< ���t
	unsigned long	id; 		///< ID
	AnsiString	desc;         	///< ����
        AnsiString	memo;		///< ����
	trntype		type;		///< ���
	long		value;		///< ���z
	long		balance;	///< �c��
	
	void SetTransactionType(const char *desc, int type);

	const char *GetTrnTypeStr(void);
};

class Card;

/**
   @brief �g�����U�N�V�����Ǘ��N���X

   �e�J�[�h���ɔh�������Ďg�p����B
   �h���N���X�ł́AIdent() �� GenerateTransaction() ���I�[�o���C�h���邱�ƁB
*/
class TransactionList {

protected:
	/// �g�����U�N�V�������X�g
	vector<Transaction*> list;
private:
	int prev_key, serial;
        AnsiString SFCPeepPath;

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

        char * TransactionList::getTabbedToken(char **pos);

public:

	inline TransactionList(void) { prev_key = serial = 0; }
	~TransactionList();
	int ParseLines(TStringList *lines, bool reverse = false);

	int GenerateTransactionId(int key);

	inline bool hasAnyTransaction(void) { return list.size() > 0 ? true : false; }

	inline vector<Transaction *>::iterator begin() {
        	return list.begin();
        }
        inline vector<Transaction *>::iterator end() {
        	return list.end();
        }
};

// ���[�e�B���e�B�֐�
AnsiString sjis2utf8(const AnsiString & sjis);

#endif

