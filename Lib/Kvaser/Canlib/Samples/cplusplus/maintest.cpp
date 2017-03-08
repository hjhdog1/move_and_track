#include <stdio.h>
#include "canlib_class.h"
void main()
{
	Canlib *lib = new Canlib();
	Canlib *lib2 = new Canlib();
	canStatus stat;
	char msg[8] = {1,2,3,4,5,6,7,8};
	int i; 

	lib->canInitializeLibrary();
    
	stat = lib->canOpenChannel(0, 0);
	if (stat != canOK)
	{
		printf("Error opening channel, %d", stat);
	}
	
	stat = lib->canSetBusParams(canBITRATE_500K, 0, 0, 0, 0, 0);
	if(stat != canOK)
	{
		printf("Error setting bus parameters, %d", stat);
	}

	stat = lib->canBusOn();
	if(stat != canOK)
	{
		printf("Error going buson, %d", stat);
	}

	for(i = 0; i<100; i++)
	{
	  stat = lib->canWrite(123, msg, 8, 0);
	  if(stat != canOK)
	  {
		printf("Error sending data, %d", stat);
	  }
	  
	}
	//wait for the messages to be sent before going busoff
	stat = lib->canWriteSync(1000);
	if(stat != canOK)
	{
		printf("Couldn't send the messages within the 1000ms.");
	}

	stat = lib->canBusOff();
	  if(stat != canOK)
	  {
	     printf("Error going busoff, %d", stat);
	  }

}
	
