using System;
using System.Collections.Generic;
using System.Text;
using FelicaLib;

namespace FeliCa2Money
{
    abstract class Card
    {
        protected string ident;
        protected string cardName;
        protected string cardId;

        public abstract List<Transaction> ReadCard();

        public string Ident
        {
            get { return this.ident; }
        }

        public string CardName
        {
            get { return this.cardName; }
        }
        
        public string CardId
        {
            set { this.cardId = value; }
            get { return this.cardId; }
        }

        protected string[] ParseLine(string line)
        {
            return line.Split('\t');
        }
    }

    abstract class CardWithFelicaLib : Card
    {
	protected int systemCode;   // �V�X�e���R�[�h
	protected int serviceCode;  // �T�[�r�X�R�[�h
	protected bool needReverse; // ���R�[�h�������t�]���邩�ǂ���

	// �J�[�h ID �擾
	public abstract void analyzeCardId(Felica f);

	// Transaction ���
	public abstract void analyzeTransaction(Transaction t, byte[] data);

	public override List<Transaction> ReadCard()
	{
	    List<Transaction> list = new List<Transaction>();

	    using (Felica f = new Felica())
	    {
		f.Polling(systemCode);
		analyzeCardId(f);

		for (int i = 0; ; i++)
		{
		    byte[] data = f.ReadWithoutEncryption(serviceCode, i);
		    if (data == null) break;

		    Transaction t = new Transaction();
		    analyzeTransaction(t, data);
		    list.Add(t);
		}
	    }
	    if (needReverse)
	    {
		list.Reverse();
	    }

	    return list;
	}
    }
}
