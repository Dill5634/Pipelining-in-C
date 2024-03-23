#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


#define CNRM  "\x1B[0m"
#define CRED  "\x1B[31m"
#define CGRN  "\x1B[32m"
#define CYEL  "\x1B[33m"
#define CBLU  "\x1B[34m"
#define CMAG  "\x1B[35m"
#define CCYN  "\x1B[36m"
#define CWHT  "\x1B[37m"
#define CBRED  "\x1B[41m"
#define CBGRN  "\x1B[42m"
#define CBYEL  "\x1B[43m"
#define CBBLU  "\x1B[44m"
#define CBMAG  "\x1B[45m"
#define CBCYN  "\x1B[46m"
#define CBWHT  "\x1B[47m"

#define MAXSTAGES 200
#define MAXINSTRUCTIONS 20

#define TABLE_1PORTMEMORY  1

class PipelineClass 
{       
 public:             
    
    void StartupMessage(void);
    int  ReadAssemblerCode(char *FileName);
    void GetTerm(char *s, char *t, int n);
    
    
    
    void DumpCodeList();
    void DumpPipeline();
    void InitialSchedule();
    void PipelineTest();
    void ShowPowerEstimate();
    void PowerStat(const char * Prefix,float S, float D);

    void Init();
    void ResetTable();
    void ResetStages();
    
    int IsStageIF(int i, int c);
    int IsStageID(int i, int c);
    int IsStageDF(int i, int c);
    int IsStageDS(int i, int c);
    int IsStageOF(int i, int c);
    int IsStageRR(int i, int c);
    int IsStageWB(int i, int c);
    int IsStageIALU(int i, int c);
    int IsStageFPALU(int i, int c);
    int IsStageSHALU(int i, int c);
    int IsStageIDLE(int i, int c);
    
    int GetRegNum(int instr, int cyc);
    void SetRegNum(int instr, int cyc, int num);
    
    void InsertStage(int i, int c, char *s);
    void InsertStall(int i, int c);
    void DeleteStage(int i, int c);
    
    void InsertOp(int i,char * s1,char*s2,char*s3,char*s4);
    void MoveOp(int s, int d);
    void DeleteOp(int i);
    void SwapOps(int i, int j);
    
    void CalculateCycles();
           
    int GetOpCount();
    
    int GetStaticPower();
    
    int OpCount;
    
    int Epoch=2020;
    
    int StaticPowerID    =   20;
    
    int StaticPowerIF    =   100;
    int StaticPowerDF    =   100;
    int StaticPowerDS    =   100;
    int StaticPowerOF    =   100;
    
    int StaticPowerRR    =   5;
    int StaticPowerWB    =   5;
    int StaticPowerIALU  =   10;
    int StaticPowerFP    =   15;
    int StaticPowerSH    =   10;
    int StaticPowerIDLE  =   00; 
    
    int DynamicPowerID   =   40;
    
    int DynamicPowerIF   =   10;
    int DynamicPowerDF   =   10;
    int DynamicPowerDS   =   20;
    int DynamicPowerOF   =   10;
    
    int DynamicPowerRR   =   10;
    int DynamicPowerWB   =   10;
    int DynamicPowerIALU =   20;
    int DynamicPowerFP   =   30;
    int DynamicPowerSH   =   10;
    int DynamicPowerIDLE =   00; 
    
    void ConvertString(char *);
    int IsStageXX(int i, int c, char * s);
    
     void SetEpoch(int Year);
    int GetEpoch();
    
    float KoomeyStatic(float Power);
    float KoomeyDynamic(float Power);
    
    char Opcode[1024][8];
    char  OperX[1024][8];
    char  OperY[1024][8];
    char  OperZ[1024][8];
    
    char Stage[MAXINSTRUCTIONS][MAXSTAGES][8];
    
    int Table[20][MAXSTAGES];
    
    int DiagramSplit=10;
    int ReadPorts=3;
    int WritePorts=1;
    int IssueWidth=1;
    
    //int ICacheReadPorts=1;
    //int ICacheWritePorts=1;
    //int DCacheReadPorts=1;
    //int DCacheWritePorts=1;
    //int UCacheReadPorts=1;
    //int UCacheWritePorts=1;
    
    int IALUCount=2;
    int FPALUCount=2;
    int SHALUCount=2;
    
    int CacheMode=0;
    float RegFileSize=1;
    int ShowUtlisation=1;

    int ShowPower=1;
    
    int SequentialCycles=0;
    int PipelinedCycles=0;
    
    //int IALUStatic  = 2;
    //int FPALUStatic = 4;
    //int SHALUStatic = 1;
    //int RRStatic    = 3;
    //int WBStatic    = 3;
    
    //int IALUDynamic  = 4;
    //int FPALUDynamic = 8;
    //int SHALDynamic  = 2;
    //int RRDynamic    = 3;
    //int WBDynamic    = 3;
    
    
};
    
