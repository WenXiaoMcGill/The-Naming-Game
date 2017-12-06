//函数包调用
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
using namespace std;


//以下是随机数发生器，不用管它的具体实现，只需要知道randf()产生[0,1)的随机小数，randi(N)产生0到N-1的随机整数即可
/*************************** RNG procedures ****************************************/
#define NN 624
#define MM 397
#define MATRIX_A 0x9908b0df   /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)

static unsigned long mt[NN]; /* the array for the state vector  */
static int mti=NN+1; /* mti==NN+1 means mt[NN] is not initialized */
void sgenrand(unsigned long seed)
{
    int i;
    for (i=0;i<NN;i++)
    {
        mt[i] = seed & 0xffff0000;
        seed = 69069 * seed + 1;
        mt[i] |= (seed & 0xffff0000) >> 16;
        seed = 69069 * seed + 1;
    }
    mti = NN;
}
void lsgenrand(unsigned long seed_array[])
{
    int i;
    for (i=0;i<NN;i++)
    mt[i] = seed_array[i];
    mti=NN;
}
double genrand()
{
    unsigned long y;
    static unsigned long mag01[2]={0x0, MATRIX_A};
    if (mti >= NN)
    {
        int kk;
        if (mti == NN+1) sgenrand(4357);
        for (kk=0;kk<NN-MM;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+MM] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        for (;kk<NN-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(MM-NN)] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        y = (mt[NN-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[NN-1] = mt[MM-1] ^ (y >> 1) ^ mag01[y & 0x1];
        mti = 0;
    }
    y = mt[mti++]; y ^= TEMPERING_SHIFT_U(y); y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
    y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C; y ^= TEMPERING_SHIFT_L(y);
    return y;
}

double randf(){ return ( (double)genrand() * 2.3283064370807974e-10 ); }
long randi(unsigned long LIM){ return((unsigned long)genrand() % LIM); }

/********************** END of RNG ************************************/

//常量宏定义
void chenggong();
void shibai();
const int N=10000;


//个体定义
struct Agent                 //政府个体
{
    int Inventory[N/2];     //存储每个个体的词汇库
    int VobNum;             // 词汇库中词的个数

};
struct Agent Government[N];  //存储政府中的个体


//输出变量定义
void Initial(void)

{
	//
    int i,j;
    //给每个人的词汇库都附上0，意味初始是空的.同时让VobNum=0;
    for (i=0;i<N;i++)
    {
    for(j=0;j<N/2;j++)
    {
        Government[i].Inventory[j]=-1;
    }
    Government[i].VobNum=0;
    }
}


// verb parament initial

int vobNow = 0, newvob = 0;

// create verb
void CreatVob(int w)
{
    Government[w].Inventory[0]=vobNow;
    vobNow++;
    Government[w].VobNum=1;
}

// fail judgement

void shibai(int y)
{
	int r;
    r = Government[y].VobNum++;
    Government[y].Inventory[r] = newvob;
}

// success judgement

void chenggong(int x,int y)
{
	int i,j;
    for (i=0;i<Government[x].VobNum;i++)
        Government[x].Inventory[i]=-1;
    for (j=0;j<Government[y].VobNum;j++)
        Government[y].Inventory[i]=-1;
    Government[x].Inventory[0]=newvob;
    Government[x].VobNum=1;
    Government[y].Inventory[0]=newvob;
    Government[y].VobNum=1;

}

// comunication

void Talk(int s, int h)
{
	int vob,i,q=0,flag=0;
  //首先判断  speaker的词库是否为空，VobNum==0，如果是空的，造词
  //然后让speaker在词库里随机算一个词出来，说给Hearer，成功，失败；
  if(Government[s].VobNum==0)
  {
      CreatVob(s);
	  q++;  //判断speaker的词库是否为空
  }

  vob = randi(Government[s].VobNum);
  newvob = Government[s].Inventory[vob];

  for ( i = 0; i < Government[h].VobNum; i++ )
  {
      if (Government[h].Inventory[i] == newvob)
      {
          flag = 1;
          break;
      }
  }
  if ( flag == 0 )
  {
     shibai( h );
  }
  else
  {	 
     chenggong( s, h );
  }

}


int main()
{
    sgenrand((unsigned)time(NULL)); //initialize RNG
	ofstream outfile; 
    //执行次数
	int steps;  
	int x, y, i, j, sum;
	float a,rate[N];//a:中间量

    for(j=0;j<N;j++)//initiate
	{
		rate[j]=0;
    }
    
	outfile.open("result110.txt");

	//迭代过程
	Initial();//初始化
	for(steps=0;steps<1100000;steps++)
	{
	    //随机选两个个体x，y
	    x=randi(N);
	    do{
	        y=randi(N);
	    }while(y==x);
	    Talk(x,y);
	} 


	for (i=0;i<N;i++)
    {
    for(j=0;j<Government[i].VobNum;j++)
    {
		if(Government[i].Inventory[j]>=0)
       rate[Government[i].Inventory[j]]++;
    }
    }
	
	for(j=0;j<N;j++)
    {
        rate[j]=rate[j]/N;
    }
	
	for (i=0;i<N;i++)
    {
    for(j=i+1;j<N;j++)
    {
		if(rate[i]<rate[j])
		{
		a=rate[j];
		rate[j]=rate[i];
		rate[i]=a;
		}
	}
	}

    for(steps=0;steps<N;steps++)
	{
		cout<<"第"<<steps+1<<"覆盖率："<<rate[steps]<<endl;
		outfile<<steps+1<<"\t"<<rate[steps]<<endl;
	}
	outfile.close();
	return 0;  
}

