
//ÿ��w��Persuaȡֵ�µģ�N_w(t)��N_d(t)��S(t)��N=1024;
//ÿ������1000�Σ���ÿ�εĽ����һ��txt�ı������;
//ÿ������1000�Σ�ƽ���������txt�ļ�;
#include <direct.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>     //ʱ�亯���� ���������ڻ��ʱ������
#include <cmath>     //ȫ���Ե���ֵ������ ��C���Լ̳ж���
#include <vector>    //��������
#include <queue>     //��������
#include <algorithm> //�㷨

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

double randf(){ return ( (double)genrand() * 2.3283064370807974e-10 ); }//����0��1�����С��
long randi(unsigned long LIM){ return((unsigned long)genrand() % LIM); }//����0��n-1���������

/********************** END of RNG ************************************/

#define population 1000//ϵͳ������������
#define T 10000//ʱ�䲽
#define cycle 1000//ѭ������
#define Q population//������洢������

int Nwt,Ndt,St,num1,num2,word;//ϵͳ��һ��������1
int diff[Q]={0};//��ͬ�ʻ���ϵͳ�������Ĵ洢��
double pan,persua,omiga,Nwmax,Ndmax,Consmean;
char Threebox[100];
char Ntbox[100],N2tbox[100],N3tbox[100];



//N�������� �����ΪQ*********************
struct agent
{
	int q[Q];
	int volume;//�ʻ���С
	int stubborn;//�ɹ�����
}a[population];


//speaker�����ѡȡһ�����������崴�������������ϵͳ�еĲ�һ����**********************************
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
			a[r2].q[num2]=name;//hearerѧϰspeaker������
			a[r2].volume++;	
		}
		else
			a[r2].stubborn--;
}


//�������ʼ��****************************
void initialize2()
{
	int i,j;
	word=0;//ϵͳ�ʻ��ʼ��
	for(i=0;i<population;i++)//�����嶨��
	{	
		for(j=0;j<Q;j++)
			a[i].q[j] = -1;//�����������ʼ��
		a[i].volume = 0;
		a[i].stubborn = 0;
	}
}

//����Nw,Nd,Surate�ļ��м���ͬw��txt�ļ�
//��������ʱ���ļ���
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

	double **Nt=(double **)malloc(cycle*sizeof(double)); //�����ȶ�̬����һ��������M��ָ������飬��ָ�ȷ���һ��������,ָ��������׵�ַ������ptr��
	for(int i=0;i<T;i++)
		Nt[i]=(double *)malloc(population*sizeof(double)); //Ϊָ�������ÿ��Ԫ�ظ�һ����ַ�������ַ��ָ��һά����ĵ�ַ��Ҳ����Ϊ��Ԫ�����ÿ��Ԫ�ط���һ������


	//�����ļ���
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
		initialize2();//ÿ��ѭ����ʼ����������,ϵͳ��������,����

		for(int circle=0;circle<cycle;circle++)
		{
			printf("��ǰΪ��%d��ѭ��\n",circle);
			for(t=0;t<T;t++)//����T��
			{			
				r1=randi(population);//���speaker
				do//���hearer
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
