using System;
using System.Collections.Generic;
using System.Text;

namespace CsAutoResponse
{
   public class HexTextBox : System.Windows.Forms.MaskedTextBox
   {
      protected override void OnKeyPress(System.Windows.Forms.KeyPressEventArgs e)
      {
         if ((e.KeyChar >= 'a') && (e.KeyChar <= 'f')) { 
            e.KeyChar = Char.ToUpper(e.KeyChar);
         }
         if ((e.KeyChar >= 'A' && e.KeyChar <= 'F') ||
             (e.KeyChar >= '0' && e.KeyChar <= '9'))
         {
            base.OnKeyPress(e);
         }
         else
         {
            e.Handled = true;
         }
      }  // OnKeyPress

      public Int32 TranslatedHexValue()
      {
         Int32 curValue = 0;
    
         try
         {
            curValue = Int32.Parse(this.Text, System.Globalization.NumberStyles.HexNumber);
         }

         catch (FormatException)
         {
            this.Text = "0";
         }

         catch (OverflowException)
         {
            this.Text = "0";
         }

         return (curValue);
      } // TranslateHexValue

   } // HexTextBox Class
} // CsAutoResponse namespace
