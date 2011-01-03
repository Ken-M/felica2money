﻿using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

using NUnit.Framework;
using FeliCa2Money;

namespace FeliCa2Money.test
{
    class TestAccount : Account
    {
        public TestAccount()
        {
            mTransactions = new List<Transaction>();
        }

        public override void ReadCard()
        {
            // do nothing
        }
    }

    [TestFixture]
    class OfxTest
    {
        private Ofx ofx;
        private Transaction T1, T2;
        private List<Account> accounts;

        [SetUp]
        public void setUp()
        {
            ofx = new Ofx();

            accounts = new List<Account>();

            T1 = new Transaction();
            T1.date = new DateTime(2000, 1, 1);
            T1.desc = "T1";
            T1.type = TransType.Payment;
            T1.value = 1000;
            T1.balance = 10000;

            T2 = new Transaction();
            T2.date = new DateTime(2010, 12, 31);
            T2.desc = "T2";
            T2.type = TransType.Payment;
            T2.value = 2000;
            T2.balance = 12000;
        }

        [TearDown]
        public void tearDown()
        {
        }

        [Test]
        public void noEntry()
        {
            TestAccount account = new TestAccount();
            accounts.Add(account);

            Assert.Throws<InvalidOperationException>(() => ofx.genOfx(accounts));
        }

        [Test]
        public void bankOnly()
        {
            TestAccount bank = new TestAccount();
            bank.isCreditCard = false;
            bank.transactions.Add(T1);
            bank.transactions.Add(T2);
            accounts.Add(bank);

            TestAccount card = new TestAccount();
            card.isCreditCard = true;
            accounts.Add(card);

            ofx.genOfx(accounts);
            XmlDocument doc = ofx.doc;

            Assert.NotNull(doc);
            Assert.NotNull(doc.SelectSingleNode("/OFX/BANKMSGSRSV1"));
            Assert.Null(doc.SelectSingleNode("/OFX/CREDITCARDMSGSRSV1"));
        }

        [Test]
        public void cardOnly()
        {
            TestAccount bank = new TestAccount();
            bank.isCreditCard = false;
            accounts.Add(bank);

            TestAccount card = new TestAccount();
            card.isCreditCard = true;
            card.transactions.Add(T1);
            card.transactions.Add(T2);
            accounts.Add(card);

            ofx.genOfx(accounts);
            XmlDocument doc = ofx.doc;

            Assert.NotNull(doc);
            Assert.Null(doc.SelectSingleNode("/OFX/BANKMSGSRSV1"));
            Assert.NotNull(doc.SelectSingleNode("/OFX/CREDITCARDMSGSRSV1"));
        }

        [Test]
        public void normalTest()
        {
            TestAccount bank = new TestAccount();
            bank.isCreditCard = false;
            bank.transactions.Add(T1);
            accounts.Add(bank);

            TestAccount card = new TestAccount();
            card.isCreditCard = true;
            card.transactions.Add(T2);
            accounts.Add(card);

            ofx.genOfx(accounts);
            XmlDocument doc = ofx.doc;

            Assert.NotNull(doc);
            Assert.NotNull(doc.SelectSingleNode("/OFX/BANKMSGSRSV1"));
            Assert.NotNull(doc.SelectSingleNode("/OFX/CREDITCARDMSGSRSV1"));

            XmlNode node;

            // 各エントリチェック
            testNodeText(doc, "/OFX/SIGNONMSGSRSV1/SONRS/DTSERVER", "20101231000000[+9:JST]");

            node = doc.SelectSingleNode("/OFX/BANKMSGSRSV1/STMTTRNRS/STMTRS/BANKTRANLIST/DTSTART");
            Assert.AreEqual("20000101000000[+9:JST]", node.InnerText);

            node = doc.SelectSingleNode("/OFX/BANKMSGSRSV1/STMTTRNRS/STMTRS/LEDGERBAL/DTASOF");
            Assert.AreEqual("20000101000000[+9:JST]", node.InnerText);

            node = doc.SelectSingleNode("/OFX/BANKMSGSRSV1/STMTTRNRS/STMTRS/LEDGERBAL/BALAMT");
            Assert.AreEqual("10000", node.InnerText);

            node = doc.SelectSingleNode("/OFX/CREDITCARDMSGSRSV1/CCSTMTTRNRS/CCSTMTRS/BANKTRANLIST/DTSTART");
            Assert.AreEqual("20101231000000[+9:JST]", node.InnerText);

            node = doc.SelectSingleNode("/OFX/CREDITCARDMSGSRSV1/CCSTMTTRNRS/CCSTMTRS/LEDGERBAL/DTASOF");
            Assert.AreEqual("20101231000000[+9:JST]", node.InnerText);

            node = doc.SelectSingleNode("/OFX/CREDITCARDMSGSRSV1/CCSTMTTRNRS/CCSTMTRS/LEDGERBAL/BALAMT");
            Assert.AreEqual("12000", node.InnerText);
        }

        private void testNodeText(XmlDocument doc, string path, string expected) {
            XmlNode node = doc.SelectSingleNode(path);
            Assert.NotNull(node);
            Assert.AreEqual(expected, node.InnerText);
        }
    }
}
