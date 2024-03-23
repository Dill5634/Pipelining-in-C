#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "pipeline.hpp"


/////////////////////////////////////////////
// declare function prototypes
 
 void FixHazards();
 void FixMemoryHazards(int c);
 
/////////////////////////////////////////////
// global variables and class instances
   
   PipelineClass Pipe;

/////////////////////////////////////////////


int main(int argc, char * argv[])
{   
    int c=0;
    
   printf("-- SOLUTION WEEK5s1.cpp --\n\n");
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
         Pipe.CacheMode=1;
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
        
        FixHazards();
        //
    
    // output resulting pipeline diagram, and test for hazards
        Pipe.DumpPipeline();
        Pipe.PipelineTest();
    
    // end of code     
}


void FixHazards()
{
	for(int c=0; c< MAXSTAGES; c++)
	{
		FixMemoryHazards(c);
	}
}

void FixMemoryHazards(int c)
{
 int Icount = 0;
 int Dcount = 0;
 int Mcount = 0;
 
   
        for(int i = 0; i < Pipe.OpCount; i++)
        {   
            if(Pipe.CacheMode==0) // Unified cache
            {
              if(Pipe.IsStageIF(i,c)) { Mcount++; }
              if(Pipe.IsStageOF(i,c)) { Mcount++; }
              if(Pipe.IsStageDF(i,c)) { Mcount++; }
              if(Pipe.IsStageDS(i,c)) { Mcount++; }
              
            	if(Mcount>1) 
            	{
            	   //printf(" Memory Hazard (%d,%d)\n",i,c);
            	   Mcount--;
            	   Pipe.InsertStall(i,c);
            	}
            }
            
            if(Pipe.CacheMode==1) // Split cache
            {
              if(Pipe.IsStageIF(i,c)) { Icount++; }
              if(Pipe.IsStageOF(i,c)) { Icount++; }
              if(Pipe.IsStageDF(i,c)) { Dcount++; }
              if(Pipe.IsStageDS(i,c)) { Dcount++; }
              
            	if(Icount>1) 
            	{
            	   //printf(" ICache Memory Hazard (%d,%d)\n",i,c);
            	   Icount--;
            	   Pipe.InsertStall(i,c);
            	}
            	
            	if(Dcount>1) 
            	{
            	   //printf(" DCache Memory Hazard (%d,%d)\n",i,c);
            	   Dcount--;
            	   Pipe.InsertStall(i,c);
            	}
            }
        }
            
}
