using System;
using System.Collections.Generic;
using System.Text;

namespace KvaserHardwareTester
{
    class KvaserInterface
    {
        public int ChannelNumber { get; set; }
        public string InterfaceName { get; set; }

        public KvaserInterface(int ChannelNumber, string InterfaceName)
        {
            this.ChannelNumber = ChannelNumber;
            this.InterfaceName = InterfaceName;
        }

        public override string ToString()
        {
            return InterfaceName;
        }

    }
}
