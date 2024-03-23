#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "pipeline.hpp"


/////////////////////////////////////////////
// declare function prototypes
 
 void FixHazards();
 void FixStructualHazards(int c);
 
/////////////////////////////////////////////
// global variables and class instances
   
   PipelineClass Pipe;

/////////////////////////////////////////////


int main(int argc, char * argv[])
{   
    int c=0;
    
    printf("-- WEEK3.cpp --\n\n");
    Pipe.StartupMessage();
    
    // configure CPU settings
    
       
       if(1) // single issue
       {
         Pipe.IssueWidth=1;
         Pipe.ReadPorts=2;
         Pipe.WritePorts=1;
         Pipe.IALUCount=1;
         Pipe.FPALUCount=1;
         Pipe.SHALUCount=1;
         Pipe.CacheMode=0;
       }
       else // superscalar 4
       {
         Pipe.IssueWidth=4;
         Pipe.ReadPorts=9;
         Pipe.WritePorts=3;
         Pipe.IALUCount=4;
         Pipe.FPALUCount=1;
         Pipe.SHALUCount=1;
         Pipe.CacheMode=0;
       }
        
    // load in test case and show buffer
    
        c=Pipe.ReadAssemblerCode(argv[1]);
        if(c<0){ return -1; }
        
        Pipe.DumpCodeList();
    
    // generate initial pipeline without and constraints
    
        Pipe.InitialSchedule();
                   
    // output resulting pipeline diagram, and test for hazards
        Pipe.DumpPipeline();
        Pipe.PipelineTest();
    
    // perform hazard fixes 
        //
        //FixHazards();
        //
    
    // output resulting pipeline diagram, and test for hazards
        //Pipe.DumpPipeline();
        //Pipe.PipelineTest();
    
    // end of code     
}

void FixHazards()
{
	for(int c=0; c< MAXSTAGES; c++)
	{
		FixStructualHazards(c);
	}
}

void FixStructualHazards(int c)
{
	// your solution for column c
}
