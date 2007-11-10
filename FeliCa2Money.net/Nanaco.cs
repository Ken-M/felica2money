using System;
using System.Collections.Generic;
using System.Text;
using FelicaLib;

namespace FeliCa2Money
{
    class Nanaco : CardWithFelicaLib
    {
	public Nanaco()
	{
	    ident = "Nanaco";
	    cardName = "Nanaco";

	    systemCode = 0xfe00;
	    serviceCode = 0x564f;
	    needReverse = true;
	}

	public override void analyzeCardId(Felica f)
	{
	    byte[] data = f.ReadWithoutEncryption(0x558b, 0);
	    if (data == null)
	    {
		throw new Exception("nanaco�ԍ���ǂݎ��܂���");
	    }
	    
	    cardId = "";
	    for (int i = 0; i < 8; i++) {
		cardId += data[i].ToString("X2");
	    }
	}

	public override void analyzeTransaction(Transaction t, byte[] data)
	{
	    // ���t
            int value = (data[9] << 24) + (data[10] << 16) + (data[11] << 8) + data[12];
	    int year = (value >> 21) + 2000;
            int month = (value >> 17) & 0xf;
            int date = (value >> 12) & 0x1f;
            int hour = (value >> 6) & 0x3f;
            int min = value & 0x3f;
	    t.date = new DateTime(year, month, date, hour, min, 0);

	    // ���z
            value = (data[1] << 24) + (data[2] << 16) + (data[3] << 8) + data[4];

	    // ���
            switch (data[0])
            {
                case 0x47:
                default:
                    t.type = TransType.Debit;   // �x����
		    t.desc = "nanaco�x��";
		    t.value = - value;
                    break;
                case 0x6f:
		    t.type = TransType.DirectDep;    // �`���[�W
		    t.desc = "nanaco�`���[�W";
		    t.value = value;
                    break;
	    }
	    t.memo = "";

	    // �c��
            value = (data[5] << 24) + (data[6] << 16) + (data[7] << 8) + data[8];
	    t.balance = value;

	    // �A��
            value = (data[13] << 8) + data[14];
	    t.id = value;
        }
    }
}
