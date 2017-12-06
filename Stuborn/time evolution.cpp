
//每个w和Persua取值下的：N_w(t)、N_d(t)、S(t)；N=1024;
//每个点跑1000次，把每次的结果开一个txt文本存出来;
//每个点跑1000次，平均结果记入txt文件;
#include <direct.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>     //时间函数库 本程序用于获得时间种子
#include <cmath>     //全局性的数值函数库 由C语言继承而来
#include <vector>    //向量容器
#include <queue>     //队列容器
#include <algorithm> //算法

#include <math.h>
#include <string.h>
#include <windows.h>
using namespace std;


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
		mt[i] = seed & 0xffff0000; seed = 69069 * seed + 1;
		mt[i] |= (seed & 0xffff0000) >> 16; seed = 69069 * seed + 1;
	}
	mti = NN;
}
void lsgenrand(unsigned long seed_array[])
{
	int i;
	for (i=0;i<NN;i++) mt[i] = seed_array[i]; 
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

double randf(){ return ( (double)genrand() * 2.3283064370807974e-10 ); }//生成0到1的随机小数
long randi(unsigned long LIM){ return((unsigned long)genrand() % LIM); }//生成0到n-1的随机整数

/********************** END of RNG ************************************/

#define population 1000//系统中智能体数量
#define T 10000//时间步
#define cycle 1000//循环次数
#define Q population//智能体存储库容量

int Nwt,Ndt,St,num1,num2,word;//系统第一个命名是1
int diff[Q]={0};//不同词汇在系统中数量的存储器
double pan,persua,omiga,Nwmax,Ndmax,Consmean;
char Threebox[100];
char Ntbox[100],N2tbox[100],N3tbox[100];



//N个智能体 记忆库为Q*********************
struct agent
{
	int q[Q];
	int volume;//词汇库大小
	int stubborn;//成功次数
}a[population];


//speaker创造或选取一个命名（个体创造出来的命名与系统中的不一样）**********************************
int speaker(int r1)
{
	int name;
	num1=a[r1].volume;
	if(num1==0)
	{
		a[r1].q[0]=word;
		word++;
		a[r1].volume=1;
	}
	num1=a[r1].volume;
	name=a[r1].q[randi(num1)];
	return name;
}

//hearer***********************************
void hearer(int r1,int r2,int name, int w)
{
	int i,j;
	num1=a[r1].volume;
	num2=a[r2].volume;
	if(num2==0)
		goto L1;
	for(i=0;i<num2;i++)
		if(a[r2].q[i]==name) 
		{
			St=1;
			for(j=0;j<num1;j++)
			{
				a[r1].q[j]=0;
			}
			for(j=0;j<num2;j++)
			{
				a[r2].q[j]=0;
			}
			a[r1].q[0]=name;
			a[r2].q[0]=name;
			a[r1].volume=1;
			a[r2].volume=1;
			a[r1].stubborn = w;        
			return;
		}	
L1:
		St=0;
		double k = randf();
		if(a[r2].stubborn == 0 || k<0.01)
		{
			a[r2].q[num2]=name;//hearer学习speaker的命名
			a[r2].volume++;	
		}
		else
			a[r2].stubborn--;
}


//智能体初始化****************************
void initialize2()
{
	int i,j;
	word=0;//系统词汇初始化
	for(i=0;i<population;i++)//智能体定性
	{	
		for(j=0;j<Q;j++)
			a[i].q[j] = -1;//智能体记忆库初始化
		a[i].volume = 0;
		a[i].stubborn = 0;
	}
}

//生成Nw,Nd,Surate文件夹及不同w下txt文件
//生成收敛时间文件夹
void creatfile1(int w)
{
	strcpy_s(Ntbox,"E:\\Silver\\Stubborn2\\Time evolution");
	char number[100];
	_itoa_s(w,number,10);
	char postname[15]=".txt";
	strcat_s(number,postname);
	strcat_s(Ntbox,number);
	cout<<Ntbox<<endl;
}

void main()//************************************
{
	int i,j,t,r1,r2,name,bi;
	sgenrand((unsigned) time(NULL));

	double **Nt=(double **)malloc(cycle*sizeof(double)); //这是先动态分配一个包含有M个指针的数组，即指先分配一个针数组,指针数组的首地址保存在ptr中
	for(int i=0;i<T;i++)
		Nt[i]=(double *)malloc(population*sizeof(double)); //为指针数组的每个元素赋一个地址，这个地址是指向一维数组的地址，也即是为针元数组的每个元素分配一个数组


	//生成文件夹
	_mkdir("E:\\Silver\\");
	_mkdir("E:\\Silver\\Stubborn2");

	for(int w=0;w<=10;w++)
	{
		for(i=0;i<cycle;i++)
		{
			for(j=0;j<population;j++)
			{
				Nt[i][j]=0;
			}
		}
		creatfile1(w);
		ofstream outfile(Ntbox,ios::binary);
		initialize2();//每次循环初始化个体性质,系统命名发端,除数

		for(int circle=0;circle<cycle;circle++)
		{
			printf("当前为第%d次循环\n",circle);
			for(t=0;t<T;t++)//进行T步
			{			
				r1=randi(population);//抽出speaker
				do//抽出hearer
				{
					r2=randi(population);
				}while(r2==r1);
				name=speaker(r1);
				hearer(r1,r2,name,w);
			}
			for(i=0;i<population;i++)
			{
				if(a[i].volume > 1)
					Nt[circle][i] = -1;
				else
					Nt[circle][i] = a[i].q[0];
			}
		}
		for(i=0;i<cycle;i++)
		{
			for(j=0;j<population;j++)
			{
				outfile<<Nt[i][j]<<"	";
			}
			outfile<<endl;
		}
		outfile.close();
	}
}
