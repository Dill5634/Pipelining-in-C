#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "pipeline.hpp"

PipelineClass Pipe;
/////////////////////////////////////////////
// declare function prototypes
 
   void FixHazards();
   void FixStructuralHazards(int c);
   void FixMemoryHazards(int c);
   void RenameRegisters(int c);
   void FixRAWHazards(int c);
   void FixWARHazards(int c);
   void FixWAWHazards(int c);
   void DetectDependencies();
   void IterateOrderings();
 
/////////////////////////////////////////////
// global variables and class instances
   
   //PipelineClass Pipe;
   
   int DependencyGrid[MAXINSTRUCTIONS][MAXINSTRUCTIONS];

/////////////////////////////////////////////


int main(int argc, char * argv[])
{   
    int c=0;
    int Optimisation=1;
    
    for(int a=0;a<argc;a++)
    {
    
      if(strcmp(argv[a],"-noopt")==0)
      {
        Optimisation=0;
      }
    }
    
    Pipe.StartupMessage();
    printf("-- WEEK10.cpp --\n\n");
    printf("  Optimisation : "); 
    if(Optimisation==1){ printf("ON\n"); } else { printf("OFF\n"); }
    
    printf("\n");
   
    
    Pipe.DiagramSplit=15;
    
    // configure CPU settings
    
       
       // cpu config
       
         Pipe.IssueWidth =1;
         Pipe.ReadPorts  =2;
         Pipe.WritePorts =1;
         Pipe.IALUCount  =1;
         Pipe.FPALUCount =1;
         Pipe.SHALUCount =1;
         Pipe.CacheMode  =0;
      
        
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
        
        if(Optimisation==1) { FixHazards(); }
        
    
    // output resulting pipeline diagram, and test for hazards
        Pipe.DumpPipeline();
        Pipe.PipelineTest();
        
        Pipe.SetEpoch(2020);
        Pipe.ShowPowerEstimate();
 

    
    // end of code     
}



void FixHazards()
{  
    for(int c=0; c<MAXSTAGES;c++)
    {
       FixMemoryHazards(c);
       FixStructuralHazards(c);
       RenameRegisters(c);
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
              
                //printf("\033cm%d i%d c%d",Mcount,i,c);
            	//Pipe.DumpPipeline();
            	if(Mcount>1) 
            	{
            	   //printf(" Memory Hazard (%d,%d)\n",i,c);
            	   Mcount--;
            	   Pipe.InsertStall(i,c);
            	}
            	
            	//printf("\033cm%d i%d c%d",Mcount,i,c);
            	//Pipe.DumpPipeline();
            	//getc(stdin);
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

void DetectDependencies()
{    	
  // search column and row for register references
  
     printf("---- REGISTER DEPENDENCIES ----------\n");
     
     // zero the tracking grid
	     for(int i=0;i<MAXINSTRUCTIONS;i++)
	     {
 	      for(int c=0; c< MAXSTAGES;c++)
 	      {
 	      	  DependencyGrid[i][c]=0;
  	      }
  	     }
  	     
    // perform the search
     
        for(int i = 0; i < Pipe.OpCount; i++)  
        {
          for(int c=0;c<MAXSTAGES;c++)
          {
       
            // Check cell (i,c) for a reg reference
               if(Pipe.IsStageRR(i,c)||Pipe.IsStageWB(i,c))
               {             
                 // Found one so now search all preceding rows for a register reference 
                    for(int j = 0; j< i; j++)
                    {
                      for(int k = 0; k < MAXSTAGES; k++)
                      {
                       
                        // find a second register reference to pair with first
                   	if(Pipe.IsStageRR(j,k)||Pipe.IsStageWB(j,k))
                   	{
                   	  // ignore RAR cases
                   	  if(Pipe.IsStageRR(i,c)&&Pipe.IsStageRR(j,k)) { continue; } // do nothing if its RAR
                   	
                   	  // otherwise print info for the pair we have found if reg IDs match
                   	  if(Pipe.GetRegNum(i,c)==Pipe.GetRegNum(j,k))
                   	    {
                   	     if(Pipe.IsStageRR(i,c)&&Pipe.IsStageWB(j,k)) { printf(" RAW (%d,%d)\n",i,j); }
                   	     if(Pipe.IsStageWB(i,c)&&Pipe.IsStageWB(j,k)) { printf(" WAW (%d,%d)\n",i,j); }
                   	     if(Pipe.IsStageWB(i,c)&&Pipe.IsStageRR(j,k)) { printf(" WAR (%d,%d)\n",i,j); } 
                   	     
                   	     DependencyGrid[i][j]=1;  // tracking a dependency of (x,y) x on y.
                   	    }
                    	}
                       }   
                    }
               }       
          }
        }
        
    printf("----------------------------------------\n");    
}

void RenameRegisters(int c)
{
 
  static int RenameVal=0;
  
  	// auto reset Rename Val counter if we are at the start of a rename process
  	   if(c==0) { RenameVal=20; }
  
	// Search the column c for a register reference
	
           for(int i=0; i<MAXINSTRUCTIONS; i++)
           {              
           	if(Pipe.IsStageWB(i,c)) // detect Wxx case
           	{
           	    // search upper left search zone
           		
           		   for(int j=0;j<i;j++)  // rows preceding row i
           		   {
           		   	for(int k=c;k<MAXSTAGES;k++)  // columns c to end of sequence
           		   	{
           		   	   // we are looking for WAR and WAW , so we want Wxx
           		   	   
           		   	   	if((Pipe.IsStageWB(j,k))||Pipe.IsStageRR(j,k)) // detects xxW and xxR cases
           		   	   	{
           		   	   	   if(Pipe.GetRegNum(i,c)==Pipe.GetRegNum(j,k)) // check register ID's match
           		   	   	   {
           		   	   	   	// we have found a Wxx that pairs with the xxR or xxW
           		   	   	   	// to give WAR or WAW. So we need to rename
           		   	   	   	
           		   	   	   	// We need to rename every reference to RegNum
           		   	   	   	// that follows the current instruction (i,c)
           		   	   	   	// including the WB performed in (i,c) itself
           		   	   	   
           		   	   	   	    
           		   	   	   	   // search forward and update and RR or WB reference that matches
           		   	   	   	   
           		   	   	   	   	for(int p=i+1; p<MAXINSTRUCTIONS; p++)
           		   	   	   	   	{
           		   	   	   	   	  for(int q=0;q<MAXSTAGES; q++)
           		   	   	   	   	  {
           		   	   	   	   	  	if((Pipe.IsStageRR(p,q))||Pipe.IsStageWB(p,q))
           		   	   	   	   	  	{
           		   	   	   	   	  		if(Pipe.GetRegNum(i,c)==Pipe.GetRegNum(p,q))
           		   	   	   	   	  		{
           		   	   	   	   	  		printf("Search and Rename at %d,%d\n",p,q);
           		   	   	   	   	  	   	   Pipe.SetRegNum(p,q,RenameVal);
           		   	   	   	   	  	   	}
           		   	   	   	   	  	}
           		   	   	   	   	  }
           		   	   	   	   	}
           		   	   	   	   	
           		   	   	   	    // finally rename WB at (i,c)
           		   	   	   	   
           		   	   	   	       printf("Rename at %d,%d\n",i,c);
           		   	   	   	       Pipe.SetRegNum(i,c,RenameVal);	
           		   	   	   	       
           		   	   	   	  // increment rename counter
           		   	   	   	  
           		   	   	   	     RenameVal++;
           		   	   	   
           		   	   	   }
           		   	   	
           		   	   	}
           		   	}
           		   }
           	
           	}
           
           }
	
	
	
}



void swap (int *i, int *j) {
    int temp = *i;
    *i = *j;
    *j = temp;
}

void permute(int index, int* arr, int n) 
{

   if (index == n-1) 
   {
      // print the current permutation in elements arr[0]...arr[n-1] 
         for (int k = 0; k < n; ++k)  { printf ("%d ", arr[k]); }
       
      // decide if it is a valid permutation
      // remember we have the dependency grid containing all dependencies
      // with the form DependencyGrid[x][y] equating to a pair (x,y)
         
         // search for each arr[x] value in the dependency grid
         
            for(int x=0;x<n;x++)
            {
                for(int y=0;y<n;y++)
                {
                   if(DependencyGrid[arr[x]][y]>0)  // a Dependency exists
                   {
                   	// check if arr[y] comes before arr[x]
                   	
                   	for(int p=n;p>x;p--)
                   	{
                   	  if(arr[p]==y) 
                   	  { 
                   	    printf(" X(%d->%d) ",arr[x],arr[p]);                	   
                   	  }
                   	}
                   }
                }
            }
      
       printf ("\n");
       return;
   }
   for (int i = index; i < n; i++) {
       swap (arr + index, arr + i);
       permute (index+1, arr, n);
       swap (arr + i, arr + index);
   }
   return;
}

void IterateOrderings()
{  
	int n = Pipe.OpCount;
   int arr[n];
   
   for (int i = 0; i < n; ++i) { arr[i] = i; }
   
   permute(0, arr, n);
}
