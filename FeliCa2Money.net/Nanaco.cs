using System;
using System.Collections.Generic;
using System.Text;

namespace FeliCa2Money
{
    class Nanaco : Card
    {
	public Nanaco()
	{
	    ident = "Nanaco";
	    cardName = "Nanaco";
	}

	public override List<Transaction> ReadCard()
	{
	    List<Transaction> list = new List<Transaction>();

	    FelicaLib.FelicaLib f = new FelicaLib.FelicaLib();
	    f.Polling(0xfe00);

	    for (int i = 0; ; i++)
	    {
		Transaction t = readTransaction(f, i);
		if (t == null) break;

		list.Add(t);
	    }

	    list.Reverse();

	    return list;
	}

	private Transaction readTransaction(FelicaLib.FelicaLib f, int n)
	{
	    byte[] data = f.ReadWithoutEncryption(0x564f, n);
	    if (data == null) {
		return null;
	    }

	    Transaction t = new Transaction();

	    // ���t
            int value = (data[9] << 24) + (data[10] << 16) + (data[11] << 8) + data[12];
            int year = (value >> 21) + 2000;
            int month = (value >> 17) & 0xf;
            int date = (value >> 12) & 0x1f;
            int hour = (value >> 6) & 0x3f;
            int min = value & 0x3f;

	    // ���z
            value = (data[1] << 24) + (data[2] << 16) + (data[3] << 8) + data[4];

	    // ���
            switch (data[0])
            {
                case 0x47:
                default:
                    t.type = TransType.Debit;   // �x����
		    t.desc = "Nanaco�x��";
		    t.value = - value;
                    break;
                case 0x6f:
		    t.type = TransType.DirectDep;    // �`���[�W
		    t.desc = "Nanaco�`���[�W";
		    t.value = value;
                    break;
	    }

	    // �c��
            value = (data[5] << 24) + (data[6] << 16) + (data[7] << 8) + data[8];
	    t.balance = value;

	    // �A��
            value = (data[13] << 8) + data[14];
	    t.id = value;

	    return t;
        }
    }
}
