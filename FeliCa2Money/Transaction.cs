using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

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

    class Transaction
    {
        public int id; // ID
        public DateTime date;
        public TransType type;        // �g�����U�N�V�����^�C�v
        public string desc;
        public string memo;
        public int value;      // ���z
        public int balance;    // �c��

        private static Hashtable TransIncome;
        private static Hashtable TransOutgo;
        private static Hashtable TransStrings;

        public Transaction()
        {
            if (TransStrings != null) return;

            // initialize
            TransStrings = new Hashtable();
            TransStrings[TransType.Int] = "INT";
            TransStrings[TransType.Div] = "DIV";
            TransStrings[TransType.DirectDep] = "DIRECTDEP";
            TransStrings[TransType.Dep] = "DEP";
            TransStrings[TransType.Payment] = "PAYMENT";
            TransStrings[TransType.Cash] = "CASH";
            TransStrings[TransType.ATM] = "ATM";
            TransStrings[TransType.Check] = "CHECK";
            TransStrings[TransType.Debit] = "DEBIT";

            TransIncome = new Hashtable();
            TransIncome["����"] = TransType.Int;
            TransIncome["�U��"] = TransType.DirectDep;
            TransIncome["�����"]= TransType.DirectDep;  // Edy �`���[�W
            TransIncome["����"] = TransType.DirectDep;    // Suica �`���[�W

            TransOutgo = new Hashtable();
            TransOutgo["�`�s�l"] = TransType.ATM;
            TransOutgo["ATM"]    = TransType.ATM;
        }

        public string GetTransString()
        {
            return (string)TransStrings[type];
        }

        public void GuessTransType(bool isIncome)
        {
            Hashtable h = TransOutgo;

            if (isIncome)
            {
                h = TransIncome;
            }

            foreach (string key in h.Keys)
            {
                if (desc.Contains(key))
                {
                    type = (TransType)h[key];
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
