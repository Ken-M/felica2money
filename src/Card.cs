/*
 * FeliCa2Money
 *
 * Copyright (C) 2001-2008 Takuya Murakami
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

using System;
using System.Collections.Generic;
using System.Text;
using FelicaLib;

namespace FeliCa2Money
{
    // Card クラス
    public abstract class Card
    {
        protected string mIdent;              // 組織名
        protected string mBankId = "Felica2Money";  // 銀行ID
        protected string mBranchId = "0";   // 支店番号
        protected string mAccountId = "";   // 口座番号
        protected string mCardName;         // カード名
        protected List<Transaction> mTransactions; // 取引リスト

        public abstract void ReadCard();

        public string ident
        {
            get { return mIdent; }
        }

        public string bankId
        {
            get { return mBankId; }
        }
        public string branchId
        {
            get {
                if (mBranchId == "") return "0";
                return mBranchId;
            }
        }
        public string cardName
        {
            get { return mCardName; }
        }
        
        public string accountId
        {
            set { mAccountId = value; }
            get {
                if (mAccountId == "") return "0";
                return mAccountId;
            }
        }

        public List<Transaction> transactions
        {
            get { return mTransactions; }
        }

        // タブ区切りの分解 (SFCPeep用)
        protected string[] ParseLine(string line)
        {
            return line.Split('\t');
        }
    }

    // FeliCa カードクラス
    public abstract class CardWithFelicaLib : Card, IDisposable
    {
        protected int systemCode;   // システムコード
        protected int serviceCode;  // サービスコード
        protected int blocksPerTransaction = 1;  // 1トランザクションあたりのブロック数
        protected int maxTransactions = 100;     // 最大トランザクション数
        protected bool needReverse = false;      // レコード順序を逆転するかどうか
        protected bool needCalcValue = false;     // 入出金額を残高から計算するかどうか

        //--------------------------------------------------------------------
        // 以下のメソッドはサブクラスで必要に応じてオーバライドする

        // Transaction 解析
        public abstract bool analyzeTransaction(Transaction t, byte[] data);

        // 後処理
        protected virtual void PostProcess(List<Transaction> list) { }

        // Dispose 処理 
        public virtual void Dispose() { }

        // カード ID 取得
        public virtual bool analyzeCardId(IFelica f)
        {
            // デフォルトでは、IDm を用いる。
            byte[] data = f.IDm();
            if (data == null)
            {
                return false;
            }

            mAccountId = binString(data, 0, 8);

            return true;
        }

        //--------------------------------------------------------------------

        // カード読み込み
        public sealed override void ReadCard()
        {
            using (IFelica f = new Felica()) {
                mTransactions = ReadCard(f);
            }
        }

        // カード読み込み
        // Note: 本来はこのメソッドは private で良いが、UnitTest 用に public にしてある。
        public List<Transaction> ReadCard(IFelica f)
        {
            List<Transaction> list = new List<Transaction>();

            f.Polling(systemCode);

            if (!analyzeCardId(f)) {
                throw new Exception(Properties.Resources.CantReadCardNo);
            }

            for (int i = 0; i < maxTransactions; i++)
            {
                byte[] data = new byte[16 * blocksPerTransaction];
                byte[] block = null;

                for (int j = 0; j < blocksPerTransaction; j++)
                {
                    block = f.ReadWithoutEncryption(serviceCode, i * blocksPerTransaction + j);
                    if (block == null)
                    {
                        break;
                    }

                    block.CopyTo(data, j * 16);
                }
                if (block == null)
                {
                    break;
                }

                Transaction t = new Transaction();
                    
                // データが全0かどうかチェック
                int x = 0;
                foreach (int xx in data)
                {
                    x |= xx;
                }
                if (x == 0) 
                {
                    // データが全0なら無視(空エントリ)
                    t.Invalidate();
                }

                // トランザクション解析
                else if (!analyzeTransaction(t, data))
                {
                    t.Invalidate();
                }
                list.Add(t);
            }

            if (needReverse)
            {
                list.Reverse();
            }
            if (needCalcValue)
            {
                CalcValueFromBalance(list);
            }
            PostProcess(list);

            return list;
        }

        //-------------------------------------------------------------------
        // ユーティリティ

        // バイナリデータを16進文字列に変換
        protected string binString(byte[] data, int offset, int len)
        {
            string s = "";
            for (int i = offset; i < offset + len; i++)
            {
                s += data[i].ToString("X2");
            }
            return s;
        }

        // 残高から金額を計算する
        private void CalcValueFromBalance(List<Transaction> list)
        {
            int prevBalance = 0;

            foreach (Transaction t in list)
            {
                t.value = t.balance - prevBalance;
                prevBalance = t.balance;
            }
            list.RemoveAt(0);   // 最古のエントリは捨てる
        }

        // 複数バイト読み込み (big endian)
        protected int read2b(byte[] b, int pos)
        {
            int ret = b[pos] << 8 | b[pos + 1];
            return ret;
        }

        protected int read3b(byte[] b, int pos)
        {
            int ret = b[pos] << 16 | b[pos + 1] << 8 | b[pos + 2];
            return ret;
        }

        protected int read4b(byte[] b, int pos)
        {
            int ret = b[pos] << 24 | b[pos + 1] << 16 | b[pos + 2] << 8 | b[pos + 3];
            return ret;
        }

        // little endian
        protected int read2l(byte[] b, int pos)
        {
            int ret = b[pos + 1] << 8 | b[pos];
            return ret;
        }
    }
}
