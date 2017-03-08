using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace TimerTest
{
    public partial class Form1 : Form
    {
        int handle1, handle2, handle3, handle4, handle5, handle1r, handle2r, handle3r, handle4r;
        public Form1()
        {
            
            InitializeComponent();
            canlibCLSNET.Canlib.canInitializeLibrary();
            handle1 = canlibCLSNET.Canlib.canOpenChannel(0, 0); 
            handle2 = canlibCLSNET.Canlib.canOpenChannel(1, 0);
            handle3 = canlibCLSNET.Canlib.canOpenChannel(2, 0);
            handle4 = canlibCLSNET.Canlib.canOpenChannel(3, 0);
            handle5 = canlibCLSNET.Canlib.canOpenChannel(4, 0);


            handle1r = canlibCLSNET.Canlib.canOpenChannel(0, 0);
            handle2r = canlibCLSNET.Canlib.canOpenChannel(1, 0);
            handle3r = canlibCLSNET.Canlib.canOpenChannel(2, 0);
            handle4r = canlibCLSNET.Canlib.canOpenChannel(3, 0);

            canlibCLSNET.Canlib.canSetBusParams(handle1, canlibCLSNET.Canlib.canBITRATE_1M, 0, 0, 0, 0, 0);  
            canlibCLSNET.Canlib.canSetBusParams(handle2, canlibCLSNET.Canlib.canBITRATE_1M, 0, 0, 0, 0, 0);
            canlibCLSNET.Canlib.canSetBusParams(handle3, canlibCLSNET.Canlib.canBITRATE_1M, 0, 0, 0, 0, 0);
            canlibCLSNET.Canlib.canSetBusParams(handle4, canlibCLSNET.Canlib.canBITRATE_1M, 0, 0, 0, 0, 0);
            canlibCLSNET.Canlib.canSetBusParams(handle5, canlibCLSNET.Canlib.canBITRATE_1M, 0, 0, 0, 0, 0);
            canlibCLSNET.Canlib.canBusOn(handle1);
            canlibCLSNET.Canlib.canBusOn(handle2);
            canlibCLSNET.Canlib.canBusOn(handle3);
            canlibCLSNET.Canlib.canBusOn(handle4);


            canlibCLSNET.Canlib.canBusOn(handle1r);
            canlibCLSNET.Canlib.canBusOn(handle2r);
            canlibCLSNET.Canlib.canBusOn(handle3r);
            canlibCLSNET.Canlib.canBusOn(handle4r);

            //canlibCLSNET.Canlib.canBusOn(handle5);

            CanWriter cw1 = new CanWriter(handle1);
            System.Threading.Thread wt1 = new System.Threading.Thread(new System.Threading.ThreadStart(cw1.startWrite));
            wt1.Start();

            CanWriter cw2 = new CanWriter(handle2);
            System.Threading.Thread wt2 = new System.Threading.Thread(new System.Threading.ThreadStart(cw2.startWrite));
            wt2.Start();

            CanWriter cw3 = new CanWriter(handle3);
            System.Threading.Thread wt3 = new System.Threading.Thread(new System.Threading.ThreadStart(cw3.startWrite));
            wt3.Start();

            CanWriter cw4 = new CanWriter(handle4);
            System.Threading.Thread wt4 = new System.Threading.Thread(new System.Threading.ThreadStart(cw4.startWrite));
            wt4.Start();

            //CanReader cr1 = new CanReader(handle1, labelTime1);
            //System.Threading.Thread rt1 = new System.Threading.Thread(new System.Threading.ThreadStart(cr1.startRead));
            //rt1.Start();

            //CanReader cr2 = new CanReader(handle2, labelTime2);
            //System.Threading.Thread rt2 = new System.Threading.Thread(new System.Threading.ThreadStart(cr2.startRead));
            //rt2.Start();

            //CanReader cr3 = new CanReader(handle3, labelTime3);
            //System.Threading.Thread rt3 = new System.Threading.Thread(new System.Threading.ThreadStart(cr3.startRead));
            //rt3.Start();

            //CanReader cr4 = new CanReader(handle4, labelTime4);
            //System.Threading.Thread rt4 = new System.Threading.Thread(new System.Threading.ThreadStart(cr4.startRead));
            //rt4.Start();

            //Control.CheckForIllegalCrossThreadCalls = false;
            short test = canlibCLSNET.Canlib.canGetVersion();
        }
    }

    public class CanWriter
    {
        int handle;
        public CanWriter(int handle)
        {
            this.handle = handle;
        }

        public void startWrite()
        {
            while (true)
            {
                byte[] msg = { 0, 0, 0, 0, 0, 0, 0, 0 };
                canlibCLSNET.Canlib.canWrite(handle, 0, msg, 0, 0);
                canlibCLSNET.Canlib.canWriteSync(handle, 100);
                //System.Threading.Thread.Sleep(5);
            }
        }
    }

    public class CanReader
    {
        int handle;
        Label writeLabel;
        public CanReader(int handle, Label writeLabel)
        {
            this.handle = handle;
            this.writeLabel = writeLabel;
        }

        public void startRead()
        {
            byte[] msg = new byte[8];
            int dlc;
            int flag, id;
            long time;
            
            canlibCLSNET.Canlib.canStatus status;
            while (true)
            {
                status = canlibCLSNET.Canlib.canReadWait(handle, out id, msg, out dlc, out flag, out time, 250);
                while (status == canlibCLSNET.Canlib.canStatus.canOK)
                {
                    writeLabel.Text = time.ToString();
                    status = canlibCLSNET.Canlib.canRead(handle, out id, msg, out dlc, out flag, out time);
                }
                System.Threading.Thread.Sleep(100);
            }
        }

    }

}
