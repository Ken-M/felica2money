using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace FeliCa2Money
{
    class Suica : Card
    {
        private int prevBalance = UndefBalance;
        private const int UndefBalance = -9999999;

        public Suica()
        {
            ident = "Suica";
            cardName = "Suica";
        }

        public override List<Transaction> ReadCard()
        {
            SfcPeep s = new SfcPeep();

            // IDm �ǂݍ���
            List<string> lines = s.Execute("-i");
            if (!lines[0].StartsWith("IDm:"))
	        {
                return null;
            }

            CardId = lines[0].Substring(4);

            // �����f�[�^�ǂݍ���
            lines = s.Execute("-h");
            if (lines.Count < 1 || !lines[0].StartsWith("HT00:"))
            {
                return null;
            }

            // �������]
            lines.Reverse();

            // Parse lines
            List<Transaction> transactions = new List<Transaction>();
            foreach (string line in lines)
            {
                Transaction t = new Transaction();

                string[] items = ParseLine(line);
                if (SetTransaction(t, items)) {
                    transactions.Add(t);
                }
            }
            return transactions;
        }

        private bool SetTransaction(Transaction t, string[] items)
        {
            // 0:�[����R�[�h,1:����,2:���t����,
            // 3:������R�[�h,4:���w���R�[�h,5:�����,6:���w��,
            // 7:�o����R�[�h,8:�o�w���R�[�h,9:�o���,10:�o�w��,
            // 11:�c��,12:����A��

            // ����
	        t.desc = items[1];
            if (t.desc == "----") {
                return false;	// ��G���g��
            }

            // �c��
	        t.balance = int.Parse(items[11]);

        	// ����z�v�Z
	        // Suica �̊e����ɂ́A�c�������L�^����Ă��Ȃ� (ouch!)
	        // �Ȃ̂ŁA�O��c���Ƃ̍����Ŏ���z���v�Z����
	        // ����āA�ŏ��̂P����͏����s�\�Ȃ̂œǂݔ�΂�
	        if (prevBalance == UndefBalance) {
		        prevBalance = t.balance;
                return false;
        	} else {
		        t.value = t.balance - prevBalance;
		        prevBalance = t.balance;
	        }

	        // ���t
            string d = items[2];
            int yy = int.Parse(d.Substring(0, 2)) + 2000;
            int mm = int.Parse(d.Substring(3, 2));
            int dd = int.Parse(d.Substring(6, 2));

            t.date = new DateTime(yy, mm, dd, 0, 0, 0);

            // ID
	        t.id = Convert.ToInt32(items[12], 16);

            // ����/����
	        if (items[5] != "") {
        	    // �^���̏ꍇ�A����Ђ�K�p�ɕ\��
                appendDesc(t, items[5]);

		        // ���l�ɓ��o���/�w�����L��
        	    t.memo = items[5] + "(" + items[6] + ")";
		        if (items[9] != "") {
                	t.memo += " - " + items[9] + "(" + items[10] + ")";
                }
	        } else {
		        // �����ɕ��̂̏ꍇ�A9, 10 �ɓX��������
                appendDesc(t, items[9]);
                appendDesc(t, items[10]);
            }

            // �g�����U�N�V�����^�C�v
            if (t.value < 0) {
                t.GuessTransType(false);
            } else {
                t.GuessTransType(true);
            }

            return true;
        }

        private void appendDesc(Transaction t, string d)
        {
            if (d == "" || d == "���o�^")
            {
                return;
            }

            if (t.desc == "�x��")
            {
                t.desc = d;       // "�x��"�͍폜���ď㏑��
            }
            else
            {
                t.desc += " " + d;
            }
        }
    }
}
