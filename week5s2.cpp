#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "pipeline.hpp"


/////////////////////////////////////////////
// declare function prototypes
 
   void FixHazards();
   void FixStructuralHazards(int c);
   void FixMemoryHazards(int c);
  
  
 
/////////////////////////////////////////////
// global variables and class instances
   
   PipelineClass Pipe;
   
   
/////////////////////////////////////////////


int main(int argc, char * argv[])
{   
    int c=0;
    
    printf("-- SOLUTION WEEK5s2.cpp --\n\n");
    Pipe.StartupMessage();
    
    // configure CPU settings
    
       
       if(1) // single issue
       {
         Pipe.IssueWidth=1;
         Pipe.ReadPorts=3;
         Pipe.WritePorts=1;
         Pipe.IALUCount=3;
         Pipe.FPALUCount=1;
         Pipe.SHALUCount=1;
         Pipe.CacheMode=0;
       }
       else // superscalar 4
       {
         Pipe.IssueWidth=1;
         Pipe.ReadPorts=2;
         Pipe.WritePorts=1;
         Pipe.IALUCount=1;
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
        
        printf("---- FIXING HAZARDS ----\n");
        
        FixHazards();
      
        
    
    // output resulting pipeline diagram, and test for hazards
        Pipe.DumpPipeline();
        Pipe.PipelineTest();
    
    // end of code     
}



void FixHazards()
{  
    for(int c=0; c<MAXSTAGES;c++)
    {
       FixMemoryHazards(c);
       FixStructuralHazards(c);
             
    }
}



void FixStructuralHazards(int c)
{
    int ia=0;
    int rr=0;
    int wb=0;
    int cx=0;
    
    
        
        for(int i = 0; i < Pipe.OpCount; i++)
        {
            if(Pipe.IsStageIALU(i,c))
            {
                ia++;
            }
           
            if(ia> Pipe.IALUCount) 
            { 
             // need to find start of this IA sequence
                for(cx=c;cx>=0;cx--)
                { 
                  if(Pipe.IsStageIALU(i,cx)==0) {cx++; break;}
                }              
            
                
             // then insert required number of stalls
                while(cx<=c)
                { 
                  Pipe.InsertStall(i,cx);
                  cx++;
                }
                
                ia--; 
            }
        }
       
        for(int i = 0; i < Pipe.OpCount; i++)
        {
            if(Pipe.IsStageRR(i,c))
            {
                rr++;
            }
           
            if(rr> Pipe.ReadPorts) { rr--; Pipe.InsertStall(i,c);}
        }
   
        for(int i = 0; i < Pipe.OpCount; i++)
        {
            if(Pipe.IsStageWB(i,c))
            {
                wb++;
            }
       
            if(wb> Pipe.WritePorts) { wb--; Pipe.InsertStall(i,c);}
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
            	   Icount--;
            	   Pipe.InsertStall(i,c);
            	}
            }
        }
}


