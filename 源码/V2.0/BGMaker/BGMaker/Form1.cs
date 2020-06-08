using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace BGMaker
{
    public partial class Form1 : Form
    {
        char[] source;
        char[] config;
        bool flag1 = false, flag2=false;
        [DllImport("SAMaker.dll", CallingConvention = CallingConvention.Cdecl)]
        extern static int BeginMakeFromBMS(char []config, char []source);

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            OpenFileDialog opf = new OpenFileDialog();
            if (DialogResult.OK == opf.ShowDialog())
            {
                config = opf.FileName.ToCharArray();
            }
            flag1 = true;
            textBox1.Text = opf.FileName;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            OpenFileDialog opf = new OpenFileDialog();
            if (DialogResult.OK == opf.ShowDialog())
            {
                source = opf.FileName.ToCharArray();
            }
            flag2 = true;
            textBox2.Text = opf.FileName;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (flag1 && flag2)
            {
                BeginMakeFromBMS(config, source);
                MessageBox.Show("转换完成");
            }
            else
            {
                MessageBox.Show("请先选择文件");
            }
        }
    }
}
