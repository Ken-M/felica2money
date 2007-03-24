using System;
using System.Collections.Generic;
using System.Text;

namespace FeliCa2Money
{
    enum TransType
    {
        Int,    // ����
        Div,    // �z��
        DirectDep,  // �U�荞�ݓ����A��藧�ē����A�����������Ƃ��߂�����
        Dep,    // ���̑�����

        Payment,
        Cash,
        ATM,
        Check,
        Debit       // ���̑��o��
    }

    class GuessTransTypeTable
    {
        private string key;
        private TransType type;

        public TransType Type
        {
            get { return type; }
        }

        public GuessTransTypeTable(string k, TransType t)
        {
            key = k;
            type = t;
        }

        public bool Match(string d)
        {
            if (d.Contains(key))
            {
                return true;
            }
            return false;
        }
    }

    class Transaction
    {
        public int id; // ID
        public DateTime date;
        public TransType type;        // �g�����U�N�V�����^�C�v
        public string desc;
        public string memo;
        public int value;      // ���z
        public int balance;    // �c��

        private static GuessTransTypeTable[] TransIncome = new GuessTransTypeTable[]
        {
            new GuessTransTypeTable("����", TransType.Int),
            new GuessTransTypeTable("�U��", TransType.DirectDep),
            new GuessTransTypeTable("�����", TransType.DirectDep),  // Edy �`���[�W
            new GuessTransTypeTable("����", TransType.DirectDep)    // Suica �`���[�W
        };

        private static GuessTransTypeTable[] TransOutgo = new GuessTransTypeTable[]
        {
            new GuessTransTypeTable("�`�s�l", TransType.ATM),
            new GuessTransTypeTable("ATM", TransType.ATM)
        };

        public void GuessTransType(bool isIncome)
        {
            GuessTransTypeTable[] tab = TransOutgo;

            if (isIncome)
            {
                tab = TransIncome;
            }

            foreach (GuessTransTypeTable e in tab)
            {
                if (e.Match(desc))
                {
                    type = e.Type;
                    return;
                }
            }

            // no match
            if (isIncome)
            {
                type = TransType.Dep;
            }
            else
            {
                type = TransType.Debit;
            }
        }
    }
}
