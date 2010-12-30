/*
 * FeliCa2Money
 *
 * Copyright (C) 2001-2010 Takuya Murakami
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

/*
   OFX ファイル生成
 */

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Xml;
using System.Windows.Forms;

namespace FeliCa2Money
{
    /// <summary>
    /// OFXファイル生成: 基底クラス
    /// </summary>
    abstract class OfxFile
    {
        protected string mOfxFilePath;
        protected XmlDocument mDoc;

        /// <summary>
        /// OFXファイルインスタンス生成
        /// </summary>
        /// <param name="version">バージョン</param>
        /// <returns>OfxFile</returns>
        public static OfxFile newOfxFile(int version)
        {
            switch (version)
            {
                case 1:
                    return new OfxFileV1();
                case 2:
                    return new OfxFileV2();
            }
            return null;
        }

        /// <summary>
        /// コンストラクタ
        /// </summary>
        public OfxFile()
        {
            //mDoc = new XmlDocument();
        }

        /// <summary>
        /// OFXファイルパスを設定する
        /// </summary>
        /// <param name="path">OFXファイルパス</param>
        public void SetOfxFilePath(String path)
        {
            mOfxFilePath = path;
        }

        /// <summary>
        /// OFXファイル書き出し
        /// </summary>
        /// <param name="accounts">アカウントリスト</param>
        public abstract void WriteFile(List<Account> accounts);

        /// <summary>
        /// OFXファイル書き出し (backward compat)
        /// </summary>
        /// <param name="account">アカウント</param>
        public void WriteFile(Account account)
        {
            List<Account> accounts = new List<Account>();
            accounts.Add(account);
            WriteFile(accounts);
        }

        /// <summary>
        /// OFX ファイルをアプリケーションで開く
        /// </summary>
        public void Execute()
        {
            System.Diagnostics.Process.Start(mOfxFilePath);
        }


        // ------------ private I/F

        // OFX要素生成
        protected void genOfxElement(XmlDocument doc, List<Account> accounts)
        {
            Transaction allFirst, allLast;
            getFirstLastDate(accounts, out allFirst, out allLast);
            if (allFirst == null)
            {
                throw new System.InvalidOperationException("No entry");
            }
            
            XmlElement root = doc.CreateElement("OFX");
            doc.AppendChild(root);

            // Signon MessageSet Response
            signonMessageSetResponse(root, allLast.date);

            // Bank / CreditCard MessageSet Response
            List<Account> banks = new List<Account>();
            List<Account> creditCards = new List<Account>();

            foreach (Account account in accounts)
            {
                if (account.transactions.Count > 0)
                {
                    if (account.isCreditCard)
                    {
                        creditCards.Add(account);
                    }
                    else
                    {
                        banks.Add(account);
                    }
                }
            }

            if (banks.Count > 0)
            {
                bankMessageSetResponse(root, banks);
            }
            if (creditCards.Count > 0)
            {
                creditCardMessageSetResponse(root, creditCards);
            }
        }

        // Signon MessageSet Response (SIGNONMSGSRSV1)
        private void signonMessageSetResponse(XmlElement parent, DateTime dtserver)
        {
            XmlElement e = appendElement(parent, "SIGNONMSGSRSV1");

            // Signon (Response)
            XmlElement sonrs = appendElement(e, "SONRS");

            XmlElement status = appendElement(sonrs, "STATUS");
            appendElementWithText(status, "CODE", "0");
            appendElementWithText(status, "SEVERITY", "INFO");
            appendElementWithText(sonrs, "DTSERVER", dateStr(dtserver));
            appendElementWithText(sonrs, "LANGUAGE", "JPN");

            XmlElement fi = appendElement(sonrs, "FI");
            appendElementWithText(fi, "ORG", "FeliCa2Money");
            // FITIDは？
        }

        // Bank MessageSet Response (BANKMSGSRSV1)
        private void bankMessageSetResponse(XmlElement parent, List<Account> accounts)
        {
            // XXXMSGSRSV1 生成
            XmlElement e = appendElement(parent, "BANKMSGSRSV1");
            foreach (Account account in accounts)
            {
                statementTransactionResponse(e, account);
            }
        }

        // Credit Card MessageSet Response (CREDITCARDMSGSRSV1)
        private void creditCardMessageSetResponse(XmlElement parent, List<Account> accounts)
        {
            XmlElement e = appendElement(parent, "CREDITCARDMSGSRSV1");
            foreach (Account account in accounts)
            {
                statementTransactionResponse(e, account);
            }
        }

        // Statement Transaction Response (STMTTRNRS / CCSTMTTRNRS)
        private void statementTransactionResponse(XmlElement parent, Account account)
        {
            XmlElement e;
            if (!account.isCreditCard)
            {
                e = appendElement(parent, "STMTTRNRS");
            }
            else
            {
                e = appendElement(parent, "CCSTMTTRNRS");
            }

            // TRNUID
            appendElementWithText(e, "TRNUID", "0");

            // STATUS
            XmlElement status = appendElement(e, "STATUS");
            appendElementWithText(status, "CODE", "0");
            appendElementWithText(status, "SEVERITY", "INFO");

            // STMTRS / CCSTMTRS
            statementResponse(e, account);
        }

        // Statement Response (STMTRS or CCSTMTRS)
        private void statementResponse(XmlElement parent, Account account)
        {
            Transaction first, last;
            getFirstLastDate(account, out first, out last);

            XmlElement e;
            if (!account.isCreditCard)
            {
                e = appendElement(parent, "STMTRS");
            }
            else
            {
                e = appendElement(parent, "CCSTMTRS");
            }

            // CURDEF
            appendElementWithText(e, "CURDEF", "JPY");

            // BANKACCTFROM / CCACCTFROM
            accountFrom(e, account);

            // BANKTRANLIST
            bankTransactionList(e, account, first, last);

            // LEDGERBAL
            ledgerBal(e, account, last);
        }

        // Account From (BANKACCTFROM or CCACCTFROM)
        private void accountFrom(XmlElement parent, Account account)
        {
            XmlElement e;
            if (!account.isCreditCard)
            {
                e = appendElement(parent, "BANKACCTFROM");
            }
            else
            {
                e = appendElement(parent, "CCACCTFROM");
            }

            if (!account.isCreditCard)
            {
                appendElementWithText(e, "BANKID", account.bankId.ToString());
                appendElementWithText(e, "BRANCHID", account.branchId);
            }
            appendElementWithText(e, "ACCTID", account.accountId);
            if (!account.isCreditCard)
            {
                appendElementWithText(e, "ACCTTYPE", "SAVINGS");
            }
        }

        // Bank Transaction List
        private void bankTransactionList(XmlElement parent, Account account, Transaction first, Transaction last)
        {
            XmlElement e = appendElement(parent, "BANKTRANLIST");
            
            appendElementWithText(e, "DTSTART", dateStr(first.date));
            appendElementWithText(e, "DTEND", dateStr(last.date));

            foreach (Transaction t in account.transactions)
            {
                statementTransaction(e, t);
            }
        }

        // Statement Transaction
        private void statementTransaction(XmlElement parent, Transaction t)
        {
            // Statement Transaction
            XmlElement e = appendElement(parent, "STMTTRN");

            appendElementWithText(e, "TRNTYPE", t.GetTransString());
            appendElementWithText(e, "DTPOSTED", dateStr(t.date));
            appendElementWithText(e, "TRNAMT", t.value.ToString());

            // トランザクションの ID は日付と取引番号で生成
            appendElementWithText(e, "FITID", transId(t));
            appendElementWithText(e, "NAME", quoteString(limitString(t.desc, 32)));
            if (t.memo != null)
            {
                appendElementWithText(e, "MEMO", quoteString(t.memo));
            }
        }

        // 残高
        private void ledgerBal(XmlElement parent, Account account, Transaction last)
        {
            XmlElement e = appendElement(parent, "LEDGERBAL");

            int balance;
            if (account.hasBalance)
            {
                balance = account.balance;
            }
            else
            {
                balance = last.balance;
            }

            appendElementWithText(e, "BALAMT", balance.ToString());
            appendElementWithText(e, "DTASOF", dateStr(last.date));
        }

        // 要素追加
        private XmlElement appendElement(XmlElement parent, string elem)
        {
            XmlElement e = mDoc.CreateElement(elem);
            parent.AppendChild(e);
            return e;
        }

        // 要素追加 (テキストノード付き)
        private void appendElementWithText(XmlElement parent, string elem, string text)
        {
            XmlElement e = mDoc.CreateElement(elem);
            e.AppendChild(mDoc.CreateTextNode(text));
            parent.AppendChild(e);
        }

        protected string dateStr(DateTime d)
        {
            string s = String.Format("{0}{1:00}{2:00}", d.Year, d.Month, d.Day);
            s += String.Format("{0:00}{1:00}{2:00}", d.Hour, d.Minute, d.Second);
            s += "[+9:JST]";
            return s;
        }

        protected string transId(Transaction t)
        {
            /* トランザクションの ID は日付と取引番号で生成 */
            string longId = String.Format("{0:0000}{1:00}{2:00}", t.date.Year, t.date.Month, t.date.Day);
            longId += String.Format("{0:0000000}", t.id);
            return longId;
        }

        protected string quoteString(string s)
        {
            s = s.Replace("&", "&amp;");
            s = s.Replace("<", "&lt;");
            s = s.Replace(">", "&gt;");
            //s = s.Replace("'", "&apos;");
            //s = s.Replace("\"", "&quot;");
            return s;
        }

        protected string limitString(string s, int maxlen)
        {
            if (s.Length <= maxlen)
            {
                return s;
            }
            else
            {
                return s.Substring(0, maxlen);
            }
        }

        // 最初のトランザクションと最後のトランザクションを取り出しておく
        // (日付範囲取得のため)
        protected void getFirstLastDate(List<Account> accounts, out Transaction allFirst, out Transaction allLast)
        {
            allFirst = null;
            allLast = null;
            foreach (Account account in accounts) {
                foreach (Transaction t in account.transactions)
                {
                    // 先頭エントリ: 同じ日付の場合は、前のエントリを優先
                    if (allFirst == null || t.date < allFirst.date)
                    {
                        allFirst = t;
                    }
                    // 最終エントリ: 同じ日付の場合は、後のエントリを優先
                    if (allLast == null || t.date >= allLast.date)
                    {
                        allLast = t;
                    }
                }
            }
        }

        protected void getFirstLastDate(Account account, out Transaction first, out Transaction last)
        {
            first = null;
            last = null;
            foreach (Transaction t in account.transactions)
            {
                // 先頭エントリ: 同じ日付の場合は、前のエントリを優先
                if (first == null || t.date < first.date)
                {
                    first = t;
                }
                // 最終エントリ: 同じ日付の場合は、後のエントリを優先
                if (last == null || t.date >= last.date)
                {
                    last = t;
                }
            }
        }
    }
}
