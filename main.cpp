
//
//  main.cpp
//  Cache
//
//  Created by Ramy ElGendi on 5/2/19.
//  Copyright © 2019 Ramy ElGendi. All rights reserved.
//

// Libraries
#include <iostream>
#include  <iomanip>
#include  <string>
#include <cmath>


using namespace std;

// Global Variables
string operation,view;//Type of operation
int bsize=0, csize=0, nblocks=0, compulsry=0, capacity=0, conflict=0; //Blocks Size, Cache Size, Number of blocks,....
int bcount=0;
const char *msg[2] = {"Miss","Hit"};
int lvl = 2;
int miss_rate = 0;
int miss_rate_2 = 0;
int miss_rate_3 = 0;
char iflvl;
// Functions
void direct_cache();
int lvls(int memGenX);

#define        DRAM_SIZE        (64*1024*1024)
#define        CACHE_SIZE        (64*1024)

enum cacheResType {MISS=0, HIT=1};

unsigned int m_w = 0xABABAB55;    /* must not be zero, nor 0x464fffff */
unsigned int m_z = 0x05080902;    /* must not be zero, nor 0x9068ffff */

unsigned int rand_()
{
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;  /* 32-bit result */
}


unsigned int memGen1()
{
    static unsigned int addr=0;
    return (addr+=(16*1024))%(256*1024);
}

unsigned int memGen2()
{
    static unsigned int addr=0;
    return (addr+=4)%(DRAM_SIZE);
}

unsigned int memGen3()
{
    return rand_()%(256*1024);
}

// Cache Simulator
cacheResType cacheSim(unsigned int addr, int cash[3][100000], int index, int tag,int n)
{
    
    if (operation=="direct")
    {
        if (cash[lvl][index]==tag)
        {
            return HIT;
        }
        else
        {
            cash[lvl][index] = tag;
            return MISS;
        } // end of directed mapped
    }
    else if (operation=="set")
    {
        index=index * n;
        for (int i=0; i < n ; i++)
        {
            if (cash[0][index+i]==tag)
            {
                return HIT;
            }
        }
        for (int j=0; j < n; j++)
        {
            if (cash[1][index+j] == -1)
            {
                cash[0][index+j]=tag;
                cash[1][index+j]=1;
                return MISS;
            }
        }
        
        int x=rand()%(n);
        cash[0][index+x]=tag;
        cash[1][index+x]=1;
        return MISS;
        
    }//end of set assciative
    else if (operation=="fully")        // fully associative **************************************
    {
        int shift=log2(bsize);
        bool detected=false;
        if(n == 0){ // MRU
            int j = 0;
            if (bcount < nblocks)
            {
                for (int i=0; i < nblocks; i++)
                {
                    if (cash[0][i]==addr >> shift)
                    {
                        detected=true;
                        cash[1][i]=j;
                        j++;
                        bcount--;
                        return HIT; //hit
                    }
                }
                
                if (!detected)
                {
                    cash[0][bcount]=addr>>shift;
                    cash[1][bcount]=j;
                    j++;
                    return MISS;  //miss
                }
            }
            else  // block counter is more than block size
            {
                //check for existence
                for (int i=0; i < nblocks; i++)
                {
                    if (cash[0][i]==(addr >> shift))
                    {
                        detected=true;
                        cash[1][i]=j;
                        j++;
                        return HIT;
                    }
                }
                
                if (!detected)
                {
                    int compare=0;
                    for (int i=1; i < nblocks; i++)
                    {
                        if (cash[1][compare] <= cash[1][i])
                            compare=i;
                    }
                    cash[0][compare]=addr >> shift;
                    cash[1][compare]=j;
                    j++;
                    return MISS; //miss
                    
                }
            }
        }
        if (n==1)    // LRU /////////
        {
            int j = 0;
            if (bcount < nblocks)
            {
                for (int i=0; i < nblocks; i++)
                {
                    if (cash[0][i]==addr >> shift)
                    {
                        detected=true;
                        cash[1][i]=j;
                        j++;
                        bcount--;
                        return HIT; //hit
                    }
                }
                
                if (!detected)
                {
                    cash[0][bcount]=addr>>shift;
                    cash[1][bcount]=j;
                    j++;
                    return MISS;  //miss
                }
            }
            else  // block counter is more than block size
            {
                //check for existence
                for (int i=0; i < nblocks; i++)
                {
                    if (cash[0][i]==(addr >> shift))
                    {
                        detected=true;
                        cash[1][i]=j;
                        j++;
                        return HIT; //hit
                    }
                }
                
                if (!detected)
                {
                    int compare=0;
                    for (int i=1; i < nblocks; i++)
                    {
                        if (cash[1][compare] > cash[1][i])
                            compare=i;
                    }
                    cash[0][compare]=addr >> shift;
                    cash[1][compare]=j;
                    j++;
                    return MISS; //miss
                    
                }
            }
        }   // end of LRU
        
        else if (n==2)   // LFU ///////////////
        {
            if (bcount < nblocks)
            {
                for (int i=0; i < nblocks; i++)
                {
                    if (cash[0][i]==addr >> shift)
                    {
                        detected=true;
                        cash[1][i]++;
                        bcount--;
                        return HIT; //hit
                    }
                }
                
                if (!detected)
                {
                    cash[0][bcount]=addr>>shift;
                    cash[1][bcount]++;
                    return MISS;  //miss
                }
            }
            else  // block counter is more than block size
            {
                //check for existence
                for (int i=0; i < nblocks; i++)
                {
                    if (cash[0][i]==(addr >> shift))
                    {
                        detected=true;
                        cash[1][i]++;
                        return HIT; //hit
                    }
                }
                if (!detected)
                {
                    int compare2=0;
                    for (int i=1; i < nblocks; i++)
                    {
                        if (cash[1][compare2] >= cash[1][i])
                            compare2=i;
                    }
                    cash[0][compare2]=addr >> shift;
                    cash[1][compare2]++;
                    return MISS; //miss
                }
            }
            
        }  // end if LFU
        
        else if (n==3)
        {
            int j = 0;
            int hits = 0;
            int acc = 0;
            bool LRU = true;
            double ratio;
            
            if(hits == 0 || acc == 0)
                ratio = 0;
            else
                ratio = hits / acc;
            if(ratio > 0.5)
                LRU = false;
            if (bcount < nblocks)
            {
                for (int i=0; i < nblocks; i++)
                {
                    if (cash[0][i]==addr >> shift)
                    {
                        detected=true;
                        if(!LRU)
                            cash[1][i]++;
                        else
                            cash[1][i]=j;
                        j++;
                        bcount--;
                        return HIT; //hit
                    }
                }
                
                
                
                
                if (!detected)
                {
                    cash[0][bcount]=addr>>shift;
                    if(!LRU)
                        cash[1][bcount]++;
                    else
                        cash[1][bcount]=j;
                    return MISS;  //miss
                }
            }
            else  // block counter is more than block size
            {
                //check for existence
                for (int i=0; i < nblocks; i++)
                {
                    if (cash[0][i]==(addr >> shift))
                    {
                        detected=true;
                        if(!LRU){
                            cash[1][i]++;
                            bcount--;
                        }
                        else
                            cash[1][i]=j;
                        return HIT; //hit
                    }
                }
                if(!detected){
                    int compare2 = 0;
                    for (int i=1; i < nblocks; i++)
                    {
                        if(!LRU){
                            if (cash[1][compare2] >= cash[1][i])
                                compare2=i;
                        }
                        if(LRU)
                            if (cash[1][compare2] > cash[1][i])
                                compare2=i;
                        
                    }
                    cash[0][compare2]=addr >> shift;
                    if(!LRU)
                        cash[1][compare2]++;
                    if(LRU)
                        cash[1][compare2]=j;
                    return MISS; //miss
                }
                
                
            }
        } //End of Adaptive
        else if (n==4)
        {
            if (bcount < nblocks)
            {
                for (int i=0; i < nblocks; i++)
                {
                    if (cash[0][i]==addr >> shift)
                    {
                        detected=true;
                        bcount--;
                        return HIT; //hit
                    }
                }
                
                if (!detected)
                {
                    cash[0][bcount]=addr>>shift;
                    return MISS;  //miss
                }
            }
            else  // block counter is more than block size
            {
                //check for existence
                for (int i=0; i < nblocks; i++)
                {
                    if (cash[0][i]==(addr >> shift))
                    {
                        detected=true;
                        return HIT; //hit
                    }
                }
                
                if (!detected)
                {
                    int m = 0;
                    m = rand()%nblocks;
                    cash[0][m]=addr >> shift;
                    return MISS; //miss
                }
            }
        }//end of RANDOM
    }  // end of Fully associative
    cout<<" Operation failed.\n";
    return HIT;
    
}
void direct_cache(int memGenX)
{
    // Initialization
    int iter=1000000;
    int cash[3][100000];
    unsigned int addr=0;
    int r, shift, index=0, tag=0;
    int hcount=0;
    float mcount=0;//Blocks Counter, Hits Counter, Misses Counter
    nblocks= (csize*1024)/bsize;
    
    for (int i=0; i < 2; i++)       // Initializing cache
        for (int j=0; j < nblocks; j++)
            cash[i][j]=-1;
    
    for(int i=0; i <iter ;i++)
    {
        if(memGenX==1)
            addr = memGen1();
        else if(memGenX==2)
            addr = memGen2();
        else if(memGenX==3)
            addr = memGen3();
        else return;
        shift= log2(bsize);
        index= (addr >> shift)% nblocks;
        shift= log2(nblocks+bsize);
        tag= addr >>shift;    // shifted the amount the offset and the index size
        r = cacheSim(addr, cash, index, tag,0);
        index=0;
        tag=0;
        if(view=="yes")
            cout <<"0x" << setfill('0') << setw(8) << hex << addr <<" ("<< msg[r] <<")\n";
        
        if (!strncmp (msg[r],"Hit",3))//            if (msg[r]=="Hit")
            hcount++;
        else if (!strncmp (msg[r],"Miss",4))
            mcount++;
    }
    cout << dec << "\nHits: "<<hcount<<"\nMisses: "<<mcount<<"\nHit Ratio: "<<(hcount/(hcount+mcount))*100<<"% \n";
}

void set(int memGenX, int n)
{
    int iter=1000000;
    int cash[3][100000];
    unsigned int addr=0;
    int r, shift, index=0, tag=0;
    int hcount=0;
    float mcount=0;//Blocks Counter, Hits Counter, Misses Counter
    compulsry=0; capacity=0; conflict=0;bcount=0;
    nblocks= (csize*1024)/(bsize*n);
    
    for (int i=0; i < 3; i++)   // setting all the cash with -1
        for (int j=0; j < 100000; j++)
            cash[i][j]=-1;
    
    for(int i=0; i <iter ;i++)
    {
        if(memGenX==1)
            addr = memGen1();
        else if(memGenX==2)
            addr = memGen2();
        else if(memGenX==3)
            addr = memGen3();
        else return;
        
        shift= log2(bsize);
        index= (addr >> shift)% (nblocks);
        shift= log2(nblocks+bsize);
        tag= addr >>shift;
        r = cacheSim(addr, cash, index, tag,n);
        if(view=="yes")
            cout <<"0x" << setfill('0') << setw(8) << hex << addr <<" ("<< msg[r] <<")\n";
        
        index=0;
        tag=0;
        
        if (!strncmp (msg[r],"Hit",3))//            if (msg[r]=="Hit")
            hcount++;
        else if (!strncmp (msg[r],"Miss",4))
            mcount++;
        
        bcount++;
    }
    cout << dec << "\nHits: "<<hcount<<"\nHit Ratio: "<<(hcount/(hcount+mcount))*100<<"% \n";
    cout << dec << "\nMisses: " << mcount <<"\n";
    
}

void fully(int memGenX,int n)
{
    int iter=1000000;
    int cash[3][100000];
    unsigned int addr=0;
    int r;
    int hcount=0;
    bcount=0;
    float mcount=0;//Blocks Counter, Hits Counter, Misses Counter
    compulsry=0; capacity=0; conflict=0;bcount=0;
    nblocks= (csize*1024)/bsize;
    
    for (int i=0; i < 2; i++)   // setting all the cash with -1
        for (int j=0; j < nblocks; j++)
            cash[i][j]=-10;
    
    for(int i=0; i <iter ;i++)
    {
        if(memGenX==1)
            addr = memGen1();
        else if(memGenX==2)
            addr = memGen2();
        else if(memGenX==3)
            addr = memGen3();
        else return;
        
        r = cacheSim(addr, cash, 0, 0,n);
        if(view=="yes")
            cout <<"0x" << setfill('0') << setw(8) << hex << addr <<" ("<< msg[r] <<")\n";
        
        if (!strncmp (msg[r],"Hit",3))//            if (msg[r]=="Hit")
            hcount++;
        else if (!strncmp (msg[r],"Miss",4))
            mcount++;
        
        bcount++;
        
    }
    
    cout << dec << "\nHits: "<<hcount<<"\nHit Ratio: "<<(hcount/(hcount+mcount))*100<<"% \n";
    cout << dec << "\nMisses: " << mcount <<"\n";
}
int lvls (int MemGenX)
{
    cacheResType r, m ,q;
    int cash[3][100000];
    int bsize_all = 64;
    int csize_2 = 256;
    int cycles = 0;
    int csize_1 = 32;
    int csize_3 = 8192;
    int shift_all = log2(bsize_all);
    nblocks = (csize_1*1024) /bsize_all;
    unsigned int addr=0;
    for(int i = 0; i < 2; i++)
        for(int j = 0; j < nblocks; j++)
            cash[i][j] = -1;
    for(int i=0; i <1000000; i++)
    {
        if(MemGenX==1)
            addr = memGen1();
        else if(MemGenX==2)
            addr = memGen2();
        else if(MemGenX==3)
            addr = memGen3();
        else return 0;
        
        
        if(lvl == 0)
        {
            nblocks = (csize_1*1024) /bsize_all;
            int index = (addr >> shift_all) %nblocks;
            int tag = addr >> shift_all;
            
            lvl = 0;
            r = cacheSim(addr, cash, index, tag, 0);
            cycles += 11;
            if(!strncmp (msg[r],"Miss",4))
            {
                cycles += 100;
            }
           
        }
        if(lvl == 1)
        {
            nblocks = (csize_1*1024) /bsize_all;
            int index = (addr >> shift_all) %nblocks;
            int tag = addr >> shift_all;
            int nblocks_2 = (csize_2*1024) /bsize_all;
            int index_2 = (addr >> shift_all) %nblocks_2;
            
            lvl = 0;
            r = cacheSim(addr, cash, index, tag, 0);
            cycles += 4;
            if(!strncmp (msg[r],"Miss",4))
            {
                
                cycles += 11;
                
            }
            lvl = 1;
            m = cacheSim(addr, cash, index_2, tag, 0);
            if(!strncmp (msg[m],"Miss",4))
            {
                cycles += 100;
            }
            
        }
        if(lvl == 2)
        {
            nblocks = (csize_1*1024) /bsize_all;
            int index = (addr >> shift_all) %nblocks;
            int tag = addr >> shift_all;
            int nblocks_2 = (csize_2*1024) /bsize_all;
            int index_2 = (addr >> shift_all) %nblocks_2;
            int nblocks_3 = (csize_3*1024) /bsize_all;
            int index_3 = (addr >> shift_all) %nblocks_3;
          
            lvl = 0;
            r = cacheSim(addr, cash, index, tag, 0);
              cycles += 4;
            if(!strncmp (msg[r],"Miss",4))
            {
               
                cycles += 11;
                
            }
            lvl = 1;
           q = cacheSim(addr, cash, index_2, tag, 0);
            if(!strncmp (msg[q],"Miss",4))
            {
              
                cycles += 30;
                
                
            }
            lvl = 2;
            m = cacheSim(addr, cash, index_3, tag, 0);
            if(!strncmp (msg[m],"Miss",4))
            {
                cycles +=100;
            }
        }
    }
    return cycles;
}
int main()
{
    // Input
    int n=0;
    int levels;
    cout<<"Cache Simulator\n===============\nPick An Operation, type:\ndirect - Direct Mapped Cache (one way)\nfully - Fully Associative Cache (n-way)\nset - Set Associative Cache (1<number of ways<n)\n\n";
    
    while(operation != "direct" && operation != "set" && operation != "fully")
    {
        cout<< "Simulator: ";
        cin >> operation;
    }
    while(csize<1 || csize>256)
    {
        cout << "Cache size: (between 1 – 256) (KB): ";
        cin >> csize;
    }
    while(bsize!=4 && bsize!=8 && bsize!=16 && bsize!=32 && bsize!=64 && bsize!=128)
    {
        cout << "Block size: (power of 2, between 4 - 128) (bytes): ";
        cin >> bsize;
    }
    while(view != "yes" && view != "no")
    {
        cout<< "Show all addresses? (yes/no): ";
        cin >> view;
    }
    if(operation == "direct"){
    while(iflvl != 'Y' && iflvl != 'N')
    {
        cout<< "Would you like a leveled cache? ";
        cin >> iflvl;
    }
    if(iflvl == 'Y'){
        cout << "Enter the no of levels" << endl;
        cin >> lvl;
        lvl--;
    }
    }
    // Direct Cache Operation
    if (operation=="direct" && iflvl =='N')
    {
        lvl = 0;
        cout<<"\nmemGen1:-\n";
        direct_cache(1);
        cout<<"\nmemGen2:-\n";
        direct_cache(2);
        cout<<"\nmemGen3:-\n";
        direct_cache(3);
    }
    if(operation == "direct" && iflvl == 'Y')
    {
        cout<<"\nmemGen1:-\n";
        cout << "Cycles " << lvls(1) << endl;
        cout<<"\nmemGen2:-\n";
        cout << "Cycles: "<< lvls(2) << endl;
        cout<<"\nmemGen3:-\n";
        cout << "Cycles: "<< lvls(3)<< endl;
    }
    
    else if ( operation=="set")
    {
        while(n!=2 && n!=4 && n!=8 && n!=16)
        {
            cout << "Number of ways for the set associative cache: (2,4,8,16): ";
            cin >> n;
        }
        cout<<"\nmemGen1:-\n";
        set(1,n);
        cout<<"\nmemGen2:-\n";
        set(2,n);
        cout<<"\nmemGen3:-\n";
        set(3,n);
    }
    else if (operation=="fully")
    {
      
            cout << "Types of replacements: (0: MRU)(1 - LRU) (2 - LFU) (3 - Adaptive) (4 - Random)\n\nType: ";
            cin >> n;
        
        cout<<"\nmemGen1:-\n";
        fully(1,n);
        cout<<"\nmemGen2:-\n";
        fully(2,n);
        cout<<"\nmemGen3:-\n";
        fully(3,n);
    }
    else
        cout<<" Operation failed.\n";
    return 0;
}

