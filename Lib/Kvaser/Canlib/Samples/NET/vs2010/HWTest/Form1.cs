using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using canlibCLSNET;

/*
 * A number of examples compiled into one form.  
 */

namespace KvaserHardwareTester
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

        }

        //Some global variables.
        bool periodicActive = false;
        
        int nrOfChannels;
        KvaserInterface interface1;
        KvaserInterface interface2;
        int handle1, handle2 = -1;

        canlibCLSNET.Canlib.canStatus stat;

        CanReader canReader; 
        System.Threading.Thread readerThread;


        private void Form1_Load(object sender, EventArgs e)
        {

            canlibCLSNET.Canlib.canInitializeLibrary();

            //List available channels
            canlibCLSNET.Canlib.canGetNumberOfChannels(out nrOfChannels);
            object o = new object();
            for (int i = 0; i < nrOfChannels; i++)
            {
                canlibCLSNET.Canlib.canGetChannelData(i, canlibCLSNET.Canlib.canCHANNELDATA_CHANNEL_NAME, out o);

                comboBoxInterface1.Items.Add(new KvaserInterface(i, o.ToString()));
                comboBoxInterface2.Items.Add(new KvaserInterface(i, o.ToString()));
            }

            comboBoxInterface1.SelectedIndex = 0;
            comboBoxInterface2.SelectedIndex = 0;
        }

        private void comboBoxInterface1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (handle1 >= 0)
            {
                canlibCLSNET.Canlib.canBusOff(handle1);
                canlibCLSNET.Canlib.canClose(handle1);
            }
            interface1 = ((KvaserInterface)comboBoxInterface1.SelectedItem);
            handle1 = canlibCLSNET.Canlib.canOpenChannel(interface1.ChannelNumber, canlibCLSNET.Canlib.canOPEN_ACCEPT_VIRTUAL);
            
            
            if (handle1 < 0)
            {
                textBoxResult.Text = "Error selecting " + interface1.InterfaceName + " - Error : " + handle1;
            }
        }

        private void comboBoxInterface2_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (handle2 >= 0)
            {
                canlibCLSNET.Canlib.canBusOff(handle2);
                canlibCLSNET.Canlib.canClose(handle2);
            }
            interface2 = ((KvaserInterface)comboBoxInterface2.SelectedItem);
            handle2 = canlibCLSNET.Canlib.canOpenChannel(interface2.ChannelNumber, canlibCLSNET.Canlib.canOPEN_ACCEPT_VIRTUAL);
            if (handle2 < 0)
            {
                textBoxResult.Text = "Error selecting " + interface2.InterfaceName + " - Error : " + handle1;
            }
        }

        private void ButtonInterface1GoOnBus_Click(object sender, EventArgs e)
        {
            textBoxResult.Text = canlibCLSNET.Canlib.canSetBusParams(handle1, canlibCLSNET.Canlib.canBITRATE_250K, 0, 0, 0, 0, 0).ToString(); ;
            stat = canlibCLSNET.Canlib.canBusOn(handle1);
            //canlibCLSNET.Canlib.canBusOn(readHandle);
        }

        private void buttonInterface1BusOff_Click(object sender, EventArgs e)
        {
            canlibCLSNET.Canlib.canBusOff(handle1);
        }

        private void buttonInterface2GoOnBus_Click(object sender, EventArgs e)
        {
            canlibCLSNET.Canlib.canSetBusParams(handle2, canlibCLSNET.Canlib.canBITRATE_250K, 0, 0, 0, 0, 0);
            stat = canlibCLSNET.Canlib.canBusOn(handle2);
        }

        private void buttonTestAutoBuffers_Click(object sender, EventArgs e)
        {
    


            /* Set up an auto response buffer
            
            int autoBufIndex = (int)(canlibCLSNET.Canlib.canObjBufAllocate(handle1, canlibCLSNET.Canlib.canOBJBUF_TYPE_AUTO_RESPONSE));
            stat = canlibCLSNET.Canlib.canObjBufWrite(handle1, autoBufIndex, 123, msg, 6, 0);
            //stat = canlibCLSNET.Canlib.canObjBufSetPeriod(handle1, autoBufIndex, 1000);
            stat = canlibCLSNET.Canlib.canObjBufEnable(handle1, autoBufIndex);
             */



            if (!periodicActive)
            {
                textBoxResult.Text += "\r\nStarting periodic sending..";
                byte[] msg = { 1, 2, 3, 4, 5, 6 };
                int autoBufIndex1 = (int)(canlibCLSNET.Canlib.canObjBufAllocate(handle1, canlibCLSNET.Canlib.canOBJBUF_TYPE_PERIODIC_TX));
                if (autoBufIndex1 > 0)
                {
                    stat = canlibCLSNET.Canlib.canObjBufWrite(handle1, autoBufIndex1, 1, msg, 0, 0);
                    stat = canlibCLSNET.Canlib.canObjBufSetPeriod(handle1, autoBufIndex1, 1000);
                    stat = canlibCLSNET.Canlib.canObjBufEnable(handle1, autoBufIndex1);
                    periodicActive = !periodicActive;
                }
                else
                {
                    textBoxResult.Text += "\r\n" + (canlibCLSNET.Canlib.canStatus)(autoBufIndex1);
                }
            }
            else
            {
                textBoxResult.Text += "\r\nStopping periodic sending..";
                canlibCLSNET.Canlib.canObjBufDisable(handle1, 0);
                periodicActive = !periodicActive;
            }
            
        }



        private void buttonSendWakeUp_Click(object sender, EventArgs e)
        {
            byte[] msg = { 0, 0, 0, 0, 0, 0, 0, 0 };
            stat = canlibCLSNET.Canlib.canWrite(handle1, 0x100, msg, 8, canlibCLSNET.Canlib.canMSG_WAKEUP);
            /*
            while (true)
            {
                stat = canlibCLSNET.Canlib.canWrite(handle1, 0x100, msg, 8, canlibCLSNET.Canlib.canMSG_WAKEUP);
                System.Threading.Thread.Sleep(50);
            }
            */
        }

        private void buttonSendErrorframe_Click(object sender, EventArgs e)
        {
            byte[] msg = { 0, 0, 0, 0, 0, 0, 0, 0 };
            canlibCLSNET.Canlib.canWrite(handle1, 0, msg, 0, canlibCLSNET.Canlib.canMSG_ERROR_FRAME);
        }


        private void buttonStartTimerRead_Click(object sender, EventArgs e)
        {
            timerRead.Enabled = true;
        }

        private void timerRead_Tick(object sender, EventArgs e)
        {
            byte[] msg = { 0, 0, 0, 0, 0, 0, 0, 0 };
            int dlc;
            int flag, id;
            long time;

            stat = canlibCLSNET.Canlib.canRead(handle2, out id, msg, out dlc, out flag, out time);
            while (stat == Canlib.canStatus.canOK)
            {

                if ((flag & canlibCLSNET.Canlib.canMSGERR_OVERRUN) != 0)
                {
                    radioButtonOverRun.Checked = true;
                }

                if ((flag & canlibCLSNET.Canlib.canMSG_ERROR_FRAME) != 0)
                {
                    textBoxIncommingFrames.Text = "ERROR FRAME\r\n" + textBoxIncommingFrames.Text;
                }
                else
                    textBoxIncommingFrames.Text = printMessage(id, msg, dlc, flag, time) + "\r\n" + textBoxIncommingFrames.Text;




                stat = canlibCLSNET.Canlib.canRead(handle2, out id, msg, out dlc, out flag, out time);

            }
        }



        private void buttonSendMessage_Click(object sender, EventArgs e)
        {
            byte[] msg = { 8, 7, 6, 5, 4, 3, 2, 1 };
            stat = canlibCLSNET.Canlib.canWrite(handle1, 0x100, msg, 8, 0);
        }

        private void buttonGetBusConfig_Click(object sender, EventArgs e)
        {
            object o = new object();
            long freq;
            int tseg1, tseg2, sjw, noSamp, syncmode;
            canlibCLSNET.Canlib.canGetBusParams(handle1, out freq, out tseg1, out tseg2, out sjw, out noSamp, out syncmode);
            textBoxResult.Text += "\r\nfreq =" + freq + ", tseg1=" + tseg1 + ", tseg2=" + tseg2 + ", sjw=" + sjw + ", noSamp=" + noSamp + ", syncmode=" + syncmode + "";
        }

        private void buttonSetFilter_Click(object sender, EventArgs e)
        {

            stat = canlibCLSNET.Canlib.canSetAcceptanceFilter(handle2, int.Parse(textBoxCode.Text), int.Parse(textBoxMask.Text), 0);


        }

        private void buttonGetChannelData_Click(object sender, EventArgs e)
        {

            object chData;
            canlibCLSNET.Canlib.canGetChannelData(interface1.ChannelNumber, canlibCLSNET.Canlib.canCHANNELDATA_CHANNEL_NAME, out chData);
            textBoxResult.Text = "canCHANNELDATA_CHANNEL_NAME: " + chData.ToString();
            canlibCLSNET.Canlib.canGetChannelData(interface1.ChannelNumber, canlibCLSNET.Canlib.canCHANNELDATA_CARD_SERIAL_NO, out chData);
            textBoxResult.Text += "\r\ncanCHANNELDATA_CARD_SERIAL_NO: " + chData.ToString();
            canlibCLSNET.Canlib.canGetChannelData(interface1.ChannelNumber, canlibCLSNET.Canlib.canCHANNELDATA_CARD_FIRMWARE_REV, out chData);
            textBoxResult.Text += "\r\ncanCHANNELDATA_CARD_FIRMWARE_REV: " + chData.ToString();
            canlibCLSNET.Canlib.canGetChannelData(interface1.ChannelNumber, canlibCLSNET.Canlib.canCHANNELDATA_CARD_HARDWARE_REV, out chData);
            textBoxResult.Text += "\r\ncanCHANNELDATA_CARD_HARDWARE_REV: " + chData.ToString();
            canlibCLSNET.Canlib.canGetChannelData(interface1.ChannelNumber, canlibCLSNET.Canlib.canCHANNELDATA_DEVDESCR_ASCII, out chData);
            textBoxResult.Text += "\r\ncanCHANNELDATA_DEVDESCR_ASCII: " + chData.ToString();
        }



        private void buttonReadTimer_Click(object sender, EventArgs e)
        {
            long timer = canlibCLSNET.Canlib.canReadTimer(handle1);
            textBoxResult.Text = timer.ToString();
        }

        private void buttonCanIOCTL_Click(object sender, EventArgs e)
        {

            object res = new object();
            canlibCLSNET.Canlib.canIoCtl(handle1, canlibCLSNET.Canlib.canIOCTL_GET_RX_BUFFER_LEVEL, ref res);
            textBoxResult.Text = "canIOCTL_GET_RX_BUFFER_LEVEL: " + res.ToString();
            
            canlibCLSNET.Canlib.canIoCtl(handle1, canlibCLSNET.Canlib.canIOCTL_SET_TXACK, ref res);
            textBoxResult.Text += "\r\ncanIOCTL_SET_TXACK: " + res.ToString();

        }

        private void buttonRxqueueSize_Click(object sender, EventArgs e)
        {
            object res = new object();
            uint queueSize = uint.Parse(textBoxRxQueueSize.Text);
            res = queueSize;
            textBoxResult.Text = canlibCLSNET.Canlib.canIoCtl(handle1, canlibCLSNET.Canlib.canIOCTL_SET_RX_QUEUE_SIZE, ref res).ToString();
        }

        private void timerQueueCheck_Tick(object sender, EventArgs e)
        {
            object res = new object();
            canlibCLSNET.Canlib.canIoCtl(handle2, canlibCLSNET.Canlib.canIOCTL_GET_RX_BUFFER_LEVEL, ref res);
            textBoxResult.Text = "canIOCTL_GET_RX_BUFFER_LEVEL: " + res.ToString();
        }


        private string printMessage(int id, byte[] msg, int dlc, int flag, long time)
        {
            string result = "";

            result += id.ToString() + " : " + dlc.ToString();
            for (int i = 0; i < dlc; i++)
            {
                result += " : " + msg[i];
            }
            return result;
        }

        private void buttonReadOne_Click(object sender, EventArgs e)
        {
            byte[] msg = { 1, 2, 3, 4, 5, 6, 7, 8 };

            int dlc;
            int flag, id = 123;
            long time;
            stat = canlibCLSNET.Canlib.canRead(handle2, out id, msg, out dlc, out flag, out time);
            if (stat == 0)
            {
                if ((flag & canlibCLSNET.Canlib.canMSGERR_OVERRUN) != 0)
                {
                    radioButtonOverRun.Checked = true;
                }

                if ((flag & canlibCLSNET.Canlib.canMSG_ERROR_FRAME) != 0)
                {
                    textBoxIncommingFrames.Text = "ERROR FRAME\r\n" + textBoxIncommingFrames.Text;
                }
                else
                    textBoxIncommingFrames.Text = printMessage(id, msg, dlc, flag, time) + "\r\n" + textBoxIncommingFrames.Text;

            }
            else
                textBoxResult.Text = stat.ToString();
        }

      

        private void buttonClearIncomming_Click(object sender, EventArgs e)
        {
            textBoxIncommingFrames.Text = "";
        }

        private void buttonReadSpecific_Click(object sender, EventArgs e)
        {
            byte[] msg = { 0, 0, 0, 0, 0, 0, 0, 0 };
            int dlc;
            int flag;
            long time;
            int id = int.Parse(textBoxSpecificID.Text);
            stat = canlibCLSNET.Canlib.canReadSpecific(handle2, id, msg, out dlc, out flag, out time);
            if (stat == 0)
            {
                if ((flag & canlibCLSNET.Canlib.canMSGERR_OVERRUN) != 0)
                {
                    radioButtonOverRun.Checked = true;
                }

                if ((flag & canlibCLSNET.Canlib.canMSG_ERROR_FRAME) != 0)
                {
                    textBoxIncommingFrames.Text = "ERROR FRAME\r\n" + textBoxIncommingFrames.Text;
                }
                else
                    textBoxIncommingFrames.Text = printMessage(id, msg, dlc, flag, time) + "\r\n" + textBoxIncommingFrames.Text;

            }
            else
                textBoxResult.Text = stat.ToString();
        }

        private void buttonReadSpecificSkip_Click(object sender, EventArgs e)
        {
            byte[] msg = { 0, 0, 0, 0, 0, 0, 0, 0 };
            int dlc;
            int flag;
            long time;
            int id = int.Parse(textBoxSpecificID.Text);
            stat = canlibCLSNET.Canlib.canReadSpecificSkip(handle1, id, msg, out dlc, out flag, out time);
            if (stat == 0)
            {
                if ((flag & canlibCLSNET.Canlib.canMSGERR_OVERRUN) != 0)
                {
                    radioButtonOverRun.Checked = true;
                }

                if ((flag & canlibCLSNET.Canlib.canMSG_ERROR_FRAME) != 0)
                {
                    textBoxIncommingFrames.Text = "ERROR FRAME\r\n" + textBoxIncommingFrames.Text;
                }
                else
                    textBoxIncommingFrames.Text = printMessage(id, msg, dlc, flag, time) + "\r\n" + textBoxIncommingFrames.Text;

            }
            else
                textBoxResult.Text = stat.ToString();
        }

        private void buttonSilentMode_Click(object sender, EventArgs e)
        {
           textBoxResult.Text = canlibCLSNET.Canlib.canSetBusOutputControl(handle1, canlibCLSNET.Canlib.canDRIVER_SILENT).ToString();
        }

        private void buttonGetBusStatistics_Click(object sender, EventArgs e)
        {
            canlibCLSNET.Canlib.canBusStatistics statistics;
            canlibCLSNET.Canlib.canRequestBusStatistics(handle1);
            canlibCLSNET.Canlib.canGetBusStatistics(handle1, out statistics);
            textBoxResult.Text = statistics.busLoad.ToString() + " " + statistics.errFrame.ToString(); 
        }

        private void buttonMessageBurst_Click(object sender, EventArgs e)
        {
            byte[] msg = { 0, 0, 0, 0, 0, 0, 0, 0 };

            canlibCLSNET.Canlib.canStatus buffer = canlibCLSNET.Canlib.canObjBufAllocate(handle1, canlibCLSNET.Canlib.canOBJBUF_TYPE_PERIODIC_TX);
            canlibCLSNET.Canlib.canObjBufWrite(handle1, (int)buffer, 0, msg, 0, 0);
            buffer = canlibCLSNET.Canlib.canObjBufAllocate(handle1, canlibCLSNET.Canlib.canOBJBUF_TYPE_PERIODIC_TX);
            canlibCLSNET.Canlib.canObjBufWrite(handle1, (int)buffer, 1, msg, 0, 0);

            for (int i = 0; i < 100; i++)
                canlibCLSNET.Canlib.canObjBufSendBurst(handle1, 0, 10000);
        }

        private void buttonReadErrorCounters_Click(object sender, EventArgs e)
        {
            int txerr, rxerr, overr;
            canlibCLSNET.Canlib.canReadErrorCounters(handle1, out txerr, out rxerr, out overr);
            textBoxResult.Text = "TX: " + txerr.ToString() + " RX: " + rxerr.ToString() + " Overrun: " + overr.ToString();

        }

        private void buttonSendRemoteFrame_Click(object sender, EventArgs e)
        {
            byte[] msg = { 0, 0, 0, 0, 0, 0, 0, 0 };
            stat = 0;
            stat = canlibCLSNET.Canlib.canWrite(handle1, 0x551, msg, 8, canlibCLSNET.Canlib.canMSG_RTR);
            
        }

        private void buttonStartReadThread_Click(object sender, EventArgs e)
        {
            canReader = new CanReader(handle2);
            readerThread = new System.Threading.Thread(new System.Threading.ThreadStart(canReader.startRead));

            readerThread.Start();
        }

        private void buttonStopReadThread_Click(object sender, EventArgs e)
        {
            canReader.stopRead();
        }

        private void buttonPingPongStart_Click(object sender, EventArgs e)
        {
            byte[] msg = { 1, 2, 3, 4, 5, 6, 7, 8 };

            int dlc;
            int flag, id = 123;
            long time;
            stat = Canlib.canStatus.canOK;
            if (stat == Canlib.canStatus.canOK)
            {
                while (stat == Canlib.canStatus.canOK)
                {
                    Canlib.canWrite(handle1, 1, msg, 8, 0);
                    stat = Canlib.canReadWait(handle1, out id, msg, out dlc, out flag, out time, 250);
                }
                textBoxResult.Text += "\r\nTimed out waiting for message";
                stat = Canlib.canReadWait(handle1, out id, msg, out dlc, out flag, out time, 1000);
                if (stat == Canlib.canStatus.canOK)
                    textBoxResult.Text += "\rMissing message arrived.";
            }
            else
            {
                textBoxResult.Text += "\r\nTimed out waiting for starting message";
            }

        }

        private void buttonWaitForPingPong_Click(object sender, EventArgs e)
        {
            byte[] msg = { 1, 2, 3, 4, 5, 6, 7, 8 };

            int dlc;
            int flag, id = 123;
            long time;
            stat = canlibCLSNET.Canlib.canReadWait(handle1, out id, msg, out dlc, out flag, out time,10000); 
            stat = Canlib.canStatus.canOK;
            if (stat == Canlib.canStatus.canOK)
            {
                while (stat == Canlib.canStatus.canOK)
                {
                    Canlib.canWrite(handle1, 1, msg, 8, 0);
                    stat = Canlib.canReadWait(handle1, out id, msg, out dlc, out flag, out time, 250);
                }
                textBoxResult.Text += "\r\nTimed out waiting for message";
                stat = Canlib.canReadWait(handle1, out id, msg, out dlc, out flag, out time, 1000);
                if (stat == Canlib.canStatus.canOK)
                    textBoxResult.Text += "\r\nMissing message arrived.";
            }
            else
            {
                textBoxResult.Text += "\r\nTimed out waiting for starting message";
            }
        }
    }

    public class CanReader
    {
        bool running = true;
        int handle;
        int nrOfFrames = 0;
        int nrOfErrorFrames = 0;
        int nrOfRemoteFrame = 0;
        
        public CanReader(int handle)
        {
            this.handle = handle;
        }

        public void stopRead()
        {
            running = false;
        }

        public void startRead()
        {
            byte[] msg = new byte[8];
            int dlc;
            int flag, id;
            long time;
            canlibCLSNET.Canlib.canStatus status;
            while (running)
            {
                status = canlibCLSNET.Canlib.canReadWait(handle, out id, msg, out dlc, out flag, out time, 250);
                while (status == canlibCLSNET.Canlib.canStatus.canOK)
                {
                    nrOfFrames++;
                    if ((flag & canlibCLSNET.Canlib.canMSG_ERROR_FRAME) > 0)
                    {
                        nrOfErrorFrames++;
                    }
                    if ((flag & canlibCLSNET.Canlib.canMSG_RTR) > 0)
                    {
                        nrOfRemoteFrame++;
                    }

                    // Do something with the message

                    status = canlibCLSNET.Canlib.canRead(handle, out id, msg, out dlc, out flag, out time);
                }
                // System.Threading.Thread.Sleep(100);
                
            }



        }
    }

}
     

       



