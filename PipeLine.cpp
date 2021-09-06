#include <iostream>
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <unistd.h>
#include <vector>
#include<math.h>
#include <vector>
#include<sys/types.h>
#include<sys/wait.h>

using namespace std;

struct instruction{
    char opcode;                             // opcode
    char operand1;                          // 1st operand
    char operand2;                         // 2nd operand
    char operand3;                        // 3rd operand
};

struct data{
    char bit1;
    char bit2;
};

string converthextobin(char c)
  {
      switch(c)
      {
          case '0': return "0000";
          case '1': return "0001";
          case '2': return "0010";
          case '3': return "0011";
          case '4': return "0100";
          case '5': return "0101";
          case '6': return "0110";
          case '7': return "0111";
          case '8': return "1000";
          case '9': return "1001";
          case 'a': return "1010";
          case 'b': return "1011";
          case 'c': return "1100";
          case 'd': return "1101";
          case 'e': return "1110";
          case 'f': return "1111";
      }
      return "1";
  }

int hextoint(char c){
      switch(c)
      {
          case '0': return  0;
          case '1': return  1;
          case '2': return  2;
          case '3': return  3;
          case '4': return  4;
          case '5': return  5;
          case '6': return  6;
          case '7': return  7;
          case '8': return  8;
          case '9': return  9;
          case 'a': return 10;
          case 'b': return 11;
          case 'c': return 12;
          case 'd': return 13;
          case 'e': return 14;
          case 'f': return 15;
      }
      return 1;

}

int hextosignedint(struct data a){
   string s1 = converthextobin(a.bit1);
   string s2 = converthextobin(a.bit2);
   int x =0;
   for(int i=1;i<4;i++){
       x = x + (hextoint(s1[i]))*pow(2,7-i);
   }
   for(int i=0;i<4;i++){
      x = x+(hextoint(s2[i]))*pow(2,3-i);
   }
   x = x - (128*(hextoint(s1[0])));
   return x;
}

string inttohex(int a){
    string s;
    if(a>=0){
        int b = a/16;
        int c = a%16;
        if(b>=0&&b<=9){
          s[0] = '0'+b;
        }
        else{
          s[0]='a'+b-10; 
        }
        
        if(c>=0&&c<=9){
          s[1] = '0'+c;
        }
        else{
          s[1]='a'+c-10; 
        }
    }
    else{
        a = a + 256;
        int b = a/16;
        int c = a%16;
        if(b>=0&&b<=9){
          s[0] = '0'+b;
        }
        else{
          s[0]='a'+b-10; 
        }
        if(c>=0&&c<=9){
          s[1] = '0'+c;
        }
        else{
          s[1]='a'+c-10; 
        }
    }
    return s;
}

/**********************************************************************************************/
instruction* FetchInstructions(){
    int file_open = open("ICache.txt", O_RDWR);                           // opening ICache.txt file in read mode
    struct stat buf; 
    int error = fstat(file_open, &buf);
    if(error < 0){
        cout<<"error occured "<<endl;
        exit(0);
    }
    /*using mmap to get ptr to contiguous memory*/
    char *instcache = static_cast<char*>(mmap(NULL,buf.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,file_open,0));
    if(instcache == MAP_FAILED){
        cout<<"error occured"<<endl;
    }
    instruction *ProgInsts;

       ProgInsts=new instruction[128];

    int j=0;
    for(int i=0;i<768;i=i+6){                                             // storing opcode and operands in ProgInst array
       ProgInsts[j].opcode=instcache[i];
       ProgInsts[j].operand1=instcache[i+1];
       ProgInsts[j].operand2=instcache[i+3];
       ProgInsts[j].operand3=instcache[i+4];
       j++;
    }
    close(file_open);                                                   // closing ICache.txt file
    return ProgInsts;
}
/**********************************************************************************************/
int* FetchRegistervalues(){
    int file_open = open("RF.txt", O_RDWR);                               // opening RF.txt file in read mode
    struct stat buf;
    int error = fstat(file_open, &buf);
    if(error < 0){
        cout<<"error occured "<<endl;
        exit(0);
    }
    /*using mmap to get ptr to contiguous memory*/
    char *RegisterValues = static_cast<char*>(mmap(NULL,buf.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,file_open,0));
    if(RegisterValues == MAP_FAILED){
        cout<<"error occured"<<endl;
    }
     int* regval;
     regval=new int[16];
    
    int j=0;
    for(int i=0;i<48;i=i+3){
        struct data a;
        a.bit1 = RegisterValues[i];
        a.bit2 = RegisterValues[i+1];
        regval[j]=hextosignedint(a);                                         // converting hexadecimal values into decimal and storing in regval array
        j++;
    }
    close(file_open);                                                      // closing RF.txt file
    return regval;
}
/***********************************************************************************************/
data* FetchData(){
    int file_open = open("DCache.txt", O_RDWR);                             // opening DCache.txt file in read mode
    struct stat buf;
    int error = fstat(file_open, &buf);
    if(error < 0){
        cout<<"error occured "<<endl;
        exit(0);
    }
    /*using mmap to get ptr to contiguous memory*/
    char *datacache = static_cast<char*>(mmap(NULL,buf.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,file_open,0));
    if(datacache == MAP_FAILED){
        cout<<"error occured"<<endl;
    }
 
    data *Progdata;

       Progdata=new data[256];
    int j=0;
    for(int i=0;i<768;i=i+3){
       Progdata[j].bit1=datacache[i];                                            // storing datacache values in Progdata
       Progdata[j].bit2=datacache[i+1];
       j++;
    }
    close(file_open);                                                          // closing DCache.txt file
    return Progdata;
}
/***********************************************************************************************/
int main(){
    instruction* Prog_Inst = FetchInstructions();                                               // fetching instructions
    int* RegisterFile = FetchRegistervalues();                                                 // fetching register values
    data* Prog_Data = FetchData();                                                            // fetching datacache values
    int pc = -1;                                                                             // denotes program counter
    float total=0;                                                                          // stores total number of instructions
    int alu=0;                                                                              // stores number of alu instructions
    int logic=0;                                                                           // stores number of logical instructions
    int datas=0;                                                                          // stores number of data instructions
    int control=0;                                                                       // stores number of control instructions
    int halt=0;                                                                          // stores number of halt instructions
    int stalls=0;                                                                       // stores total number of stalls
    float cycles = 0;                                                                  // stores total number of cycles
    float cpi;                                                                        // cycles per instructions
    int a;                                            
    int b;
    int c;
    int size = 300;
    vector<vector<int>> PipeLine;                             
    PipeLine.resize(size, std::vector<int>(size));
    for(int i=0;i<size;i++){
        PipeLine[i].resize(size,-1);
    }
    int currfetchpos = 0;
    int currinst=0;
    int rawstalls =0;                                                                 // stores total number of RAW stalls
    int controlstalls = 0;                                                           // stores total number of controlstalls
   
    while(true){
      if(total >= size)
      {
         PipeLine.resize(2*size, std::vector<int>(2*size));
         for(int i=0;i<2*size;i++){
             PipeLine[i].resize(2*size,-1);
         }
        
        size = size*2;
      }
    
      pc=pc+1;                                                                     // incrementing program counter
     char opcode = Prog_Inst[pc].opcode;
     if(opcode=='0'){                                                              // addition operation
         a=hextoint(Prog_Inst[pc].operand1);
         b=hextoint(Prog_Inst[pc].operand2);
         c=hextoint(Prog_Inst[pc].operand3);
         RegisterFile[a]=RegisterFile[b]+RegisterFile[c];                          // 1st operand = 2nd operand + 3rd operand
         
         PipeLine[currinst][currfetchpos]=0;
         currfetchpos=currfetchpos+1;                                              // incrementing currfetchpos
         int l=1;
         while(l==1){
            int i=0;
            for(i = 0;i<currinst;i++){
                  if(PipeLine[i][currfetchpos]==b||PipeLine[i][currfetchpos]==c){          // checking for raw stalls
                     currfetchpos++;
                     rawstalls++;
                     break;
                  }
             }
             if(i==currinst){
                 l=0;
             }
         }
         PipeLine[currinst][currfetchpos]=0;                                                 // updating values of PipeLine matrix
         PipeLine[currinst][currfetchpos+1]=a;
         PipeLine[currinst][currfetchpos+2]=a;
         PipeLine[currinst][currfetchpos+3]=0;   
         currinst++;
         
         total=total+1;                                                                   // incrementing instruction count
         alu=alu+1;                                                                      // incrementing alu instruction count
      }
      else if(opcode=='1'){                                                           // subtraction operation
         a=hextoint(Prog_Inst[pc].operand1);
         b=hextoint(Prog_Inst[pc].operand2);
         c=hextoint(Prog_Inst[pc].operand3);
         RegisterFile[a]=RegisterFile[b]-RegisterFile[c];                             // 1st operand = 2nd operand - 3rd operand
         
         PipeLine[currinst][currfetchpos]=0;
         currfetchpos=currfetchpos+1;                                                // incrementing currfetchpos
         int l=1;
         while(l==1){
             int i=0;
            for(i = 0;i<currinst;i++){
                  if(PipeLine[i][currfetchpos]==b||PipeLine[i][currfetchpos]==c){               // checking for raw stalls
                     currfetchpos++;
                     rawstalls++;
                     break;
                  }
             }
             if(i==currinst){
                 l=0;
             }
         }
         PipeLine[currinst][currfetchpos]=0;                                             // updating values of PipeLine matrix
         PipeLine[currinst][currfetchpos+1]=a;
         PipeLine[currinst][currfetchpos+2]=a;
         PipeLine[currinst][currfetchpos+3]=0;    
         currinst++;

         total=total+1;                                                                 // incrementing instruction count
         alu=alu+1;                                                                     // incrementing alu instruction count
      }
      else if(opcode=='2'){                                                            // multiplication operation
         a=hextoint(Prog_Inst[pc].operand1);
         b=hextoint(Prog_Inst[pc].operand2);
         c=hextoint(Prog_Inst[pc].operand3);
         RegisterFile[a]=RegisterFile[b]*RegisterFile[c];                             // operand1 = operand2*operand3
         
         PipeLine[currinst][currfetchpos]=0;
         currfetchpos=currfetchpos+1;                                                // incrementing currfetchpos
         int l=1;
         while(l==1){
             int i=0;
            for(i = 0;i<currinst;i++){
                  if(PipeLine[i][currfetchpos]==b||PipeLine[i][currfetchpos]==c){            // checking for raw stalls
                     currfetchpos++;
                     rawstalls++;
                     break;
                  }
             }
             if(i==currinst){
                 l=0;
             }
         }
         PipeLine[currinst][currfetchpos]=0;                                     // updating values of PipeLine matrix
         PipeLine[currinst][currfetchpos+1]=a;
         PipeLine[currinst][currfetchpos+2]=a;
         PipeLine[currinst][currfetchpos+3]=0;    
         currinst++;
 
         total=total+1;                                                      // incrementing instruction count
         alu=alu+1;                                                         // incrementing alu instruction count
      }
      else if(opcode=='3'){                                                 // increment operation
         a=hextoint(Prog_Inst[pc].operand1);
         RegisterFile[a]=RegisterFile[a]+1;                                 // operand = operand+1
         
         PipeLine[currinst][currfetchpos]=0;
         currfetchpos=currfetchpos+1;                                      // incrementing currfetchpos
         int l=1;
         while(l==1){
             int i=0;
            for(i = 0;i<currinst;i++){
                  if(PipeLine[i][currfetchpos]==a){                        // checking for raw stalls                    
                     currfetchpos++;
                     rawstalls++;
                     break;
                  }
             }
             if(i==currinst){
                 l=0;
             }
         }
         PipeLine[currinst][currfetchpos]=0;                              // updating values of pipeline matrix
         PipeLine[currinst][currfetchpos+1]=a;
         PipeLine[currinst][currfetchpos+2]=a;
         PipeLine[currinst][currfetchpos+3]=0;    
         currinst++;

         total=total+1;                                                 // incrementing instruction count
         alu=alu+1;                                                      // incrementing alu instruction count
      }
      else if(opcode=='4'){                                              // AND operation
         a=hextoint(Prog_Inst[pc].operand1);
         b=hextoint(Prog_Inst[pc].operand2);
         c=hextoint(Prog_Inst[pc].operand3);
         RegisterFile[a]=RegisterFile[b]&RegisterFile[c];                // operand1 = operand2 & operand3
         
         PipeLine[currinst][currfetchpos]=0;
         currfetchpos=currfetchpos+1;                                    // incrementing currfetchpos
         int l=1;
         while(l==1){
             int i=0;
            for(i = 0;i<currinst;i++){                                                     // Checking for RAW stalls
                  if(PipeLine[i][currfetchpos]==b||PipeLine[i][currfetchpos]==c){
                     currfetchpos++;
                     rawstalls++;
                     break;
                  }
             }
             if(i==currinst){
                 l=0;
             }
         }
         PipeLine[currinst][currfetchpos]=0;
         PipeLine[currinst][currfetchpos+1]=a;
         PipeLine[currinst][currfetchpos+2]=a;
         PipeLine[currinst][currfetchpos+3]=0;    
         currinst++;

         total=total+1;                                                        // incrementing instruction count
         logic=logic+1;                                                        // incrementing logical instruction count
      }
      else if(opcode=='5'){                                                    // OR operation
         a=hextoint(Prog_Inst[pc].operand1);
         b=hextoint(Prog_Inst[pc].operand2);
         c=hextoint(Prog_Inst[pc].operand3);
         RegisterFile[a]=RegisterFile[b]|RegisterFile[c];                        // operand1 = operand2 | operand3
         
         PipeLine[currinst][currfetchpos]=0;
         currfetchpos=currfetchpos+1;                                          // incrementing currfetchpos
         int l=1;
         while(l==1){
             int i=0;
            for(i = 0;i<currinst;i++){
                  if(PipeLine[i][currfetchpos]==b||PipeLine[i][currfetchpos]==c){              // checking for RAW stalls
                     currfetchpos++;
                     rawstalls++;
                     break;
                  }
             }
             if(i==currinst){
                 l=0;
             }
         }
         PipeLine[currinst][currfetchpos]=0;
         PipeLine[currinst][currfetchpos+1]=a;
         PipeLine[currinst][currfetchpos+2]=a;
         PipeLine[currinst][currfetchpos+3]=0;    
         currinst++;

         total=total+1;                                                            // incrementing instruction count 
         logic=logic+1;                                                           // incrementing logical instruction count
      }
      else if(opcode=='6'){                                                       // NOT operation
         a=hextoint(Prog_Inst[pc].operand1);
         b=hextoint(Prog_Inst[pc].operand2);
         RegisterFile[a]=~RegisterFile[b];                                      // operand1 = ~ operand2
         
         PipeLine[currinst][currfetchpos]=0;
         currfetchpos=currfetchpos+1;                                           // incrementing currfetchpos
         int l=1;
         while(l==1){
             int i=0;
            for(i = 0;i<currinst;i++){
                  if(PipeLine[i][currfetchpos]==b){
                     currfetchpos++;
                     rawstalls++;
                     break;
                  }
             }
             if(i==currinst){
                 l=0;
             }
         }
         PipeLine[currinst][currfetchpos]=0;
         PipeLine[currinst][currfetchpos+1]=a;
         PipeLine[currinst][currfetchpos+2]=a;
         PipeLine[currinst][currfetchpos+3]=0;    
         currinst++;

         total=total+1;                                                             // incrementing instruction count
         logic=logic+1;                                                            // incrementing logical instruction count
      }
      else if(opcode=='7'){                                                        // XOR operation
         a=hextoint(Prog_Inst[pc].operand1);
         b=hextoint(Prog_Inst[pc].operand2);
         c=hextoint(Prog_Inst[pc].operand3);
         RegisterFile[a]=RegisterFile[b]^RegisterFile[c];                         // operand1 = operand2 ^ operand3
         
         PipeLine[currinst][currfetchpos]=0;
         currfetchpos=currfetchpos+1;
         int l=1;
         while(l==1){
             int i=0; 
            for(i = 0;i<currinst;i++){                                                      // checking for RAW stalls
                  if(PipeLine[i][currfetchpos]==b||PipeLine[i][currfetchpos]==c){
                     currfetchpos++;
                     rawstalls++;
                     break;
                  }
             }
             if(i==currinst){
                 l=0;
             }
         }
         PipeLine[currinst][currfetchpos]=0;
         PipeLine[currinst][currfetchpos+1]=a;
         PipeLine[currinst][currfetchpos+2]=a;
         PipeLine[currinst][currfetchpos+3]=0;    
         currinst++;

         total=total+1;                                                                    // incrementing instruction count
         logic=logic+1;                                                                    // incrementing logical instruction count
      }
      else if(opcode=='8'){                                                    // LOAD operation
         a=hextoint(Prog_Inst[pc].operand1);
         b=hextoint(Prog_Inst[pc].operand2);
         c=hextoint(Prog_Inst[pc].operand3);
         data x = Prog_Data[RegisterFile[b]+c];
         c = hextosignedint(x);
         RegisterFile[a]=c;                                                    
         
         PipeLine[currinst][currfetchpos]=0;
         currfetchpos=currfetchpos+1;                                                      // incrementing currfetchpos
         int l=1;
         while(l==1){
             int i=0;
            for(i = 0;i<currinst;i++){
                  if(PipeLine[i][currfetchpos]==b){
                     currfetchpos++;
                     rawstalls++;
                     break;
                  }
             }
             if(i==currinst){
                 l=0;
             }
         }
         PipeLine[currinst][currfetchpos]=0;
         PipeLine[currinst][currfetchpos+1]=a;
         PipeLine[currinst][currfetchpos+2]=a;
         PipeLine[currinst][currfetchpos+3]=0;    
         currinst++;

         total=total+1;                                                 // incrementing instruction count
         datas=datas+1;                                                 // incrementing data instruction count
      }
       else if(opcode=='9'){                                           // STORE operation
         a=hextoint(Prog_Inst[pc].operand1);
         b=hextoint(Prog_Inst[pc].operand2);
         char m=Prog_Inst[pc].operand3;
         c=hextoint(m);
         string s = inttohex(RegisterFile[a]);
         Prog_Data[RegisterFile[b]+c].bit1 = s[0];
         Prog_Data[RegisterFile[b]+c].bit2 = s[1];
         
         PipeLine[currinst][currfetchpos]=0;                                
         currfetchpos=currfetchpos+1;                             // incrementing currfetchpos
         int l=1;
         while(l==1){
             int i=0;
            for(i = 0;i<currinst;i++){                                          // checking for RAW stalls
                  if(PipeLine[i][currfetchpos]==b||PipeLine[i][currfetchpos]==a){
                     currfetchpos++;
                     rawstalls++;
                     break;
                  }
             }
             if(i==currinst){
                 l=0;
             }
         }
         PipeLine[currinst][currfetchpos]=0;
         PipeLine[currinst][currfetchpos+1]=0;
         PipeLine[currinst][currfetchpos+2]=0;
         PipeLine[currinst][currfetchpos+3]=0;    
         currinst++;

         total=total+1;                                              // incrementing instruction count
         datas=datas+1;                                             // incrementing data instruction count
      }
       else if(opcode=='a'){                                        // JMP operation
         char m=Prog_Inst[pc].operand1;
         char n=Prog_Inst[pc].operand2;
         struct data x;
         x.bit1=m;
         x.bit2=n;
         c =hextosignedint(x);
         pc=pc+c;                                                    // PC = PC + c
         
         PipeLine[currinst][currfetchpos]=0;
         currfetchpos=currfetchpos+1;                                   // incrementing currfetchpos 
         PipeLine[currinst][currfetchpos]=0;
         PipeLine[currinst][currfetchpos+1]=0;
         PipeLine[currinst][currfetchpos+2]=0;
         PipeLine[currinst][currfetchpos+3]=0;    
         currinst++;
         controlstalls=controlstalls+2;                           // incrementing control stalls by 2          
         currfetchpos=currfetchpos+2;                              // incrementing currfetchpos by 2
         
         total=total+1;                                            // incrementing total instruction count
         control=control+1;                                       // incrementing control instruction count
      }
       else if(opcode=='b'){                                       // BEQZ operation
         a=hextoint(Prog_Inst[pc].operand1);
         char m=Prog_Inst[pc].operand2;
         char n=Prog_Inst[pc].operand3;
         struct data x;
         x.bit1=m;
         x.bit2=n;
         c =hextosignedint(x);
         if(RegisterFile[a]==0){                                  // IF R[operand1] == 0
            pc=pc+c;     
         }
         
         PipeLine[currinst][currfetchpos]=0;
         currfetchpos=currfetchpos+1;                             // incrementing currfetchpos
         int l=1;
         while(l==1){
             int i=0;
            for(i = 0;i<currinst;i++){
                  if(PipeLine[i][currfetchpos]==a){                      // checking for RAW stalls
                     currfetchpos++;
                     rawstalls++;
                     break;
                  }
             }
             if(i==currinst){
                 l=0;
             }
         }
         PipeLine[currinst][currfetchpos]=0;
         PipeLine[currinst][currfetchpos+1]=0;
         PipeLine[currinst][currfetchpos+2]=0;
         PipeLine[currinst][currfetchpos+3]=0;    
         currinst++;
         currfetchpos=currfetchpos+2;                              // incrementing currfetchpos by 2          
         controlstalls=controlstalls+2;                            // incrementing control stalls by 2                                
         
         total=total+1;                                           // incrementing total instruction count                                
         control=control+1;                                       // incrementing control instruction count
      }
      else if(opcode=='f'){                                      // HLT operation
         total++;
         halt=halt+1;                                            // incrementing halt instruction count
         break;
      }
    }
    
    stalls = rawstalls+controlstalls;                             // calculating total number of stalls
    cycles = total+4+stalls;                                      // calculating total number of cycles
    cpi    = cycles/total;                                        // calculating cycles per instruction
    
    ofstream file2;
    file2.open("Output.txt");                                                  // creating Output.txt file
    file2<<"Total number of instructions executed"<<"  : "<<total<<endl;
    file2<<"Number of instructions in each class"<<endl;
    file2<<"Arthimetic Instructions"<<"\t"<<"\t"<<": "<<alu<<endl;
    file2<<"Logical Instructions"<<"\t"<<"\t"<<"\t"<<": "<<logic<<endl;
    file2<<"Data Instructions"<<"\t"<<"\t"<<"\t"<<": "<<datas<<endl;
    file2<<"Control Instructions"<<"\t"<<"\t"<<"\t"<<": "<<control<<endl;
    file2<<"Halt Instructions"<<"\t"<<"\t"<<"\t"<<": "<<halt<<endl;
    file2<<"Cycles per Instructions"<<"\t"<<"\t"<<": "<<cpi<<endl;
    file2<<"Total number of stalls"<<"\t"<<"\t"<<"\t"<<": "<<stalls<<endl;
    file2<<"Data Stalls (RAW)"<<"\t"<<"\t"<<"\t"<<": "<<rawstalls<<endl;
    file2<<"Control Stalls"<<"\t"<<"\t"<<"\t"<<"\t"<<": "<<controlstalls<<endl;
    
    file2.close();
    
    
    
    ofstream file;
    file.open("Dcache.txt");                                                   // Creating a new Dcache.txt file
    for(int j=0;j<256;j++){
        file<<Prog_Data[j].bit1<< Prog_Data[j].bit2<<endl;
    }
    
    file.close();
}
