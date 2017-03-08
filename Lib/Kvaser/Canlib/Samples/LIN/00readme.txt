
LIN sample program
----------------------------------------------------------------------
Simple test program for the LIN Bus.                                 
Requires LAPcan + DRVLIN.                                            
Sends and receives messages.                                         

linlib.dll must be somewhere in the path for the application to run.
                                                                     
The program is written in C and has been                             
tested with Borland and Microsoft compilers.                         
                                                                      
Usage:                                                               
You may use one of the following flags:                              
 n	Where n is the channel number                                  
 -m	The channel is a master with this flag, without it is a slave  
                                                                     
Example: linTest 1 -m                                                
      Will go on bus with channel 1 as master                        
                                                                     
----------------------------------------------------------------------

Run it in two different console windows:                             
  1) linTest 0                                                       
  2) linTest 1 -m                                                    
                                                                      
  (1) will then operate as a slave on channel 0,                     
  and (2) will operate as a master on channel 1.                     

----------------------------------------------------------------------

