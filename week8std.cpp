#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "pipeline.hpp"


/////////////////////////////////////////////
// declare function prototypes
 
   void FixHazards();
   void FixStructuralHazards(int c);
   void FixMemoryHazards(int c);
  
   void FixRAWHazards(int c);
   void FixWARHazards(int c);
   void FixWAWHazards(int c);
  
 
/////////////////////////////////////////////
// global variables and class instances
   
   PipelineClass Pipe;
   
   
/////////////////////////////////////////////


int main(int argc, char * argv[])
{   
    int c=0;
    
    printf("-- WEEK8std.cpp --\n\n");
    Pipe.StartupMessage();
    
    // configure CPU settings
    
       Pipe.DiagramSplit=20;
       
         Pipe.IssueWidth=1;
         Pipe.ReadPorts=4;
         Pipe.WritePorts=4;
         Pipe.IALUCount=4;
         Pipe.FPALUCount=1;
         Pipe.SHALUCount=1;
         Pipe.CacheMode=0;
      
        
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
        //FixStructuralHazardsAdvanced();
        
    
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
       
       FixRAWHazards(c);
       FixWARHazards(c);
       FixWAWHazards(c);           
    }
}



void FixRAWHazards(int c)
{    	 
        for(int i = 0; i < Pipe.OpCount; i++)
        {
            // Find a RR case
               if(Pipe.IsStageRR(i,c))
               {             
                 // Find a WB in the search zone
                    
                    int Flag=0;
                    
                    for(int j = i-1;  ((j>=0)&&(Flag==0)); j--)
                    {
                      for(int k = c; ((k < MAXSTAGES)&&(Flag==0)); k++)
                      {
                   	if(Pipe.IsStageWB(j,k))
                   	{
                   	   // check if register IDs match
                   	   
                   	      if(Pipe.GetRegNum(i,c)==Pipe.GetRegNum(j,k))
                   	      {
                   	        // output details (can be commented out)
                   	         printf("RAW R%d RR(%d,%d) WB(%d,%d)\n",Pipe.GetRegNum(i,c),i,c,j,k);
                   	           
                   	        // insert stall cycle
                   	           Pipe.InsertStall(i,c);	
                   	           Flag=1;
                   	      }
                   	}
                   	
                      }   
                                
                    }
                }       
        }
}

void FixWARHazards(int c)
{    
        for(int i = 0; i < Pipe.OpCount; i++)
        {
            // Find a WB case
               if(Pipe.IsStageWB(i,c))
               {             
                 // Find a RR in the search zone
                   
                    int Flag=0;
                    
                    for(int j = i-1; ((j>=0)&&(Flag==0)); j--)
                    {
                      for(int k = c; ((k < MAXSTAGES)&&(Flag==0)); k++)
                      {
                   	if(Pipe.IsStageRR(j,k))
                   	{
                   	   // check if register IDs match
                   	   
                   	      if(Pipe.GetRegNum(i,c)==Pipe.GetRegNum(j,k))
                   	      {
                   	        // output details (can be commented out)
                   	         printf("WAR R%d WB(%d,%d) RR(%d,%d)\n",Pipe.GetRegNum(i,c),i,c,j,k);
                   	           
                   	        // insert stall cycle
                   	           Pipe.InsertStall(i,c);	
                   	           Flag=1;             	        
                   	      }
                   	}
                      }              
                    }
                }       
        }
}

void FixWAWHazards(int c)
{    
        for(int i = 0; i < Pipe.OpCount; i++)
        {
            // Find a WB case
               if(Pipe.IsStageWB(i,c))
               {             
                 // Find a WB in the search zone
                 
                    int Flag=0;
                    
                    for(int j = i-1; ((j>=0)&&(Flag==0)); j--)
                    {
                      
                      for(int k = c; ((k < MAXSTAGES)&&(Flag==0)); k++)
                      {
                   	 if(Pipe.IsStageWB(j,k))
                   	 {
                   	   // check if register IDs match
                   	   
                   	      if(Pipe.GetRegNum(i,c)==Pipe.GetRegNum(j,k))
                   	      {
                   	        // output details (can be commented out)
                   	         printf("WAW R%d WB(%d,%d) WB(%d,%d)\n",Pipe.GetRegNum(i,c),i,c,j,k);
                   	           
                   	        // insert stall cycle
                   	           Pipe.InsertStall(i,c);	
                   	           Flag=1;
                   	      }
                   	      
                   	    
                   	 }
                   	 
                   	
                      }              
                    }
                }       
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


