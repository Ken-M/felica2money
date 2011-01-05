// -*-  Mode:C++; c-basic-offset:4; tab-width:4; indent-tabs-mode:nil -*-
/*
 * FeliCa2Money
 *
 * Copyright (C) 2001-2011 Takuya Murakami
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

// CSV 取り込み処理

using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows.Forms;

namespace FeliCa2Money
{
    /// <summary>
    /// CSVアカウント
    /// </summary>
    class CsvAccount : Account
    {
        private CsvRules mRules = new CsvRules();
        private StreamReader mSr;
        private CsvRule mRule;

        /// <summary>
        /// CSVルールを読み込む
        /// </summary>
        /// <returns></returns>
        public bool LoadAllRules()
        {
            return mRules.LoadAllRules();
        }

        /// <summary>
        /// CSVファイルをオープン
        /// </summary>
        /// <param name="path">CSVファイルパス</param>
        /// <returns></returns>
        public bool OpenFile(string path)
        {
            // TODO: とりあえず SJIS で開く (UTF-8 とかあるかも?)
            mSr = new StreamReader(path, System.Text.Encoding.Default);

            string firstLine = mSr.ReadLine();

            // 合致するルールを探す
            mRule = mRules.FindRule(firstLine);

            CsvDialog dlg = new CsvDialog(mRules);

            // ルール/口座番号などを選択
            dlg.SelectRule(mRule);
            if (dlg.ShowDialog() == DialogResult.Cancel)
            {
                return false;
            }

            // 選択されたルールを取り出す
            mRule = dlg.SelectedRule();
            if (mRule == null)
            {
                MessageBox.Show(Properties.Resources.NoCsvRuleSelected, Properties.Resources.Error);
                return false;
            }

            // 銀行IDなどを設定
            mIdent = mRule.ident;
            mBankId = mRule.bankId;
            mBranchId = dlg.BranchId;
            mAccountId = dlg.AccountId;

            // 1行目から再度読み込み直す
            mSr.Close();
            mSr = new StreamReader(path, System.Text.Encoding.Default);

            // firstLine まで読み飛ばす
            if (mRule.firstLine != null)
            {
                while ((firstLine = mSr.ReadLine()) != null)
                {
                    if (firstLine == mRule.firstLine) break;
                }
            }

            return true;
        }

        /// <summary>
        /// CSVファイルクローズ
        /// </summary>
        public void Close()
        {
            mSr.Close();
        }

        private static int compareByDate(Transaction x, Transaction y)
        {
            return x.date.CompareTo(y.date);
        }

        /// <summary>
        /// CSVデータ読み込み
        /// </summary>
        public override void ReadCard()
        {
            List<Transaction> transactions = new List<Transaction>();
            string line;

            while ((line = mSr.ReadLine()) != null)
            {
                // CSV カラム分割
                string[] row = SplitCsv(line);
                if (row.Length <= 1) continue; // ad hoc...

                // パース
                Transaction t = mRule.parse(row);
                transactions.Add(t);
            }

            // ソート処理
            switch (mRule.sortOrder)
            {
                default:
                case CsvRule.SortOrder.Ascent:
                    break;

                case CsvRule.SortOrder.Descent:
                    transactions.Reverse();
                    break;

                case CsvRule.SortOrder.Auto:
                    transactions.Sort(compareByDate);
                    break;
            }

            // ID採番
            int idSerial = 0;
            DateTime prevDate = new DateTime(1900, 1, 1, 0, 0, 0);

            foreach (Transaction t in transactions)
            {
                if (t.id == Transaction.UNASSIGNED_ID)
                {
                    if (t.date == prevDate)
                    {
                       idSerial++;
                    }
                    else
                    {
                        idSerial = 0;
                        prevDate = t.date;
                    }
                    t.id = idSerial;
                }
            }
            
            mTransactions = transactions;
        }

        // CSV のフィールド分割
        private string[] SplitCsv(string line)
        {
            return CsvUtil.SplitCsv(line, mRule.isTSV);
        }
    }
}
