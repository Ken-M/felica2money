using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace FeliCa2Money
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void buttonQuit_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void buttonEdy_Click(object sender, EventArgs e)
        {
            doConvert(new Edy());
        }

        private void buttonSuica_Click(object sender, EventArgs e)
        {
            //
        }

        private void doConvert(Card c)
        {
            List<Transaction> list = c.ReadCard();

            if (list == null)
            {
                MessageBox.Show("�J�[�h��ǂނ��Ƃ��ł��܂���ł���", "�G���[");
                return;
            }
            if (list.Count == 0)
            {
                MessageBox.Show("�������ꌏ������܂���", "�G���[");
                return;
            }

            // OFX �t�@�C������
            OfxFile ofx = new OfxFile();
            ofx.WriteFile(c, list);

            // Money �N��
            ofx.Execute();
        }
    }
}