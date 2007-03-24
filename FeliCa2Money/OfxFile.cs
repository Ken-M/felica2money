using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace FeliCa2Money
{
    class OfxFile
    {
        private string ofxFilePath;

        public OfxFile()
        {
            ofxFilePath = System.IO.Path.GetTempPath() + "FeliCa2Money.ofx";
        }

        private string dateStr(DateTime d)
        {
            string s = String.Format("{0}{1:00}{2:00}", d.Year, d.Month, d.Day);
            s += String.Format("{0:00}{1:00}{2:00}", d.Hour, d.Minute, d.Second);
            s += "[+9:JST]";
            return s;
        }

        private string transId(Transaction t)
        {
            /* �g�����U�N�V������ ID �͓��t�Ǝ���ԍ��Ő��� */
            string longId = String.Format("{0:0000}{1:00}{2:00}", t.date.Year, t.date.Month, t.date.Day);
            longId += String.Format("{0:0000000}", t.id);
            return longId;
        }

        public void WriteFile(Card card,  List<Transaction> transactions)
        {
            Transaction first = transactions[0];
            Transaction last = transactions[transactions.Count - 1];

            StreamWriter w = new StreamWriter(ofxFilePath, false); //, Encoding.UTF8);
            w.NewLine = "\n";

            w.WriteLine("OFXHEADER:100");
            w.WriteLine("DATA:OFXSGML");
            w.WriteLine("VERSION:102");
            w.WriteLine("SECURITY:NONE");
            w.WriteLine("ENCODING:UTF-8");
            w.WriteLine("CHARSET:CSUNICODE");
            w.WriteLine("COMPRESSION:NONE");
            w.WriteLine("OLDFILEUID:NONE");
            w.WriteLine("NEWFILEUID:NONE");
            w.WriteLine("");

            /* ���Z�@�֏��(�T�C���I�����X�|���X) */
            w.WriteLine("<OFX>");
            w.WriteLine("<SIGNONMSGSRSV1>");
            w.WriteLine("<SONRS>");
            w.WriteLine("  <STATUS>");
            w.WriteLine("    <CODE>0");
            w.WriteLine("    <SEVERITY>INFO");
            w.WriteLine("  </STATUS>");
            w.WriteLine("  <DTSERVER>{0}", dateStr(last.date));

            w.WriteLine("  <LANGUAGE>JPN");
            w.WriteLine("  <FI>");
            w.WriteLine("    <ORG>{0}", card.Ident);
            w.WriteLine("  </FI>");
            w.WriteLine("</SONRS>");
            w.WriteLine("</SIGNONMSGSRSV1>");

            /* �������(�o���N���b�Z�[�W���X�|���X) */
            w.WriteLine("<BANKMSGSRSV1>");

            /* �a�������^���׏��쐬 */
            w.WriteLine("<STMTTRNRS>");
            w.WriteLine("<TRNUID>0");
            w.WriteLine("<STATUS>");
            w.WriteLine("  <CODE>0");
            w.WriteLine("  <SEVERITY>INFO");
            w.WriteLine("</STATUS>");

            w.WriteLine("<STMTRS>");
            w.WriteLine("  <CURDEF>JPY");

            w.WriteLine("  <BANKACCTFROM>");
            w.WriteLine("    <BANKID>{0}", card.Ident);
            w.WriteLine("    <BRANCHID>000");
            w.WriteLine("    <ACCTID>{0}", card.CardId);
            w.WriteLine("    <ACCTTYPE>SAVINGS");
            w.WriteLine("  </BANKACCTFROM>");

            /* ���׏��J�n(�o���N�g�����U�N�V�������X�g) */
            w.WriteLine("  <BANKTRANLIST>");
            w.WriteLine("    <DTSTART>{0}", dateStr(first.date));
            w.WriteLine("    <DTEND>{0}", dateStr(last.date));

            /* �g�����U�N�V���� */
            foreach (Transaction t in transactions)
            {
                w.WriteLine("    <STMTTRN>");
                w.WriteLine("      <TRNTYPE>{0}", t.GetTransString());
                w.WriteLine("      <DTPOSTED>{0}", dateStr(t.date));
                w.WriteLine("      <TRNAMT>{0}", t.value);

                /* �g�����U�N�V������ ID �͓��t�Ǝ���ԍ��Ő��� */
                w.WriteLine("      <FITID>{0}", transId(t));
                w.WriteLine("      <NAME>{0}", t.desc);
                if (t.memo != null)
                {
                    w.WriteLine("      <MEMO>{0}", t.memo);
                }
                w.WriteLine("    </STMTTRN>");
            }

            w.WriteLine("  </BANKTRANLIST>");

            /* �c�� */
            w.WriteLine("  <LEDGERBAL>");
            w.WriteLine("    <BALAMT>{0}", last.balance);
            w.WriteLine("    <DTASOF>{0}", dateStr(last.date));
            w.WriteLine("  </LEDGERBAL>");

            /* OFX �I�� */
            w.WriteLine("  </STMTRS>");
            w.WriteLine("</STMTTRNRS>");
            w.WriteLine("</BANKMSGSRSV1>");
            w.WriteLine("</OFX>");

            w.Close();

        }

        public void Execute()
        {
            System.Diagnostics.Process.Start(ofxFilePath);
        }
    }
}
