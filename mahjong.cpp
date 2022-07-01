#include <iostream>
#include <cstring>
#include <algorithm>
using namespace std;

int mycard[60]={0};
int hidecard[60]={0};
int trycard[60]={0};
int value_playcard[60]={0};//储存(摸或碰后)打出某牌的评估函数值 
int value_chicard[3][60]={0};//储存吃的评估函数值 
int turnID;
int playerID;
int quan;
int hua[4]={0};
int lastcard;//最新打出的牌 
char op[50];//读取命令的字符串 

int trans(char*c);//字符串转麻将代号 
char detrans(int x);//麻将代号转首字母 
bool request();
void response();
void opplay();//决定如何出牌 
void oppeng();//决定如何碰牌 
void opchi();//决定如何吃牌 
int optin();//返回听牌应该打的牌 

void mydraw();//记录未出现牌的减少，相应出牌时应该补偿 
void buhua();
void draw();
void play();
void peng();
void chi();
void gang();
void bugang();
void mypass();
void myplay(); 
void mygang();//空函数 
void mybugang();//空函数 
void mypeng();
void mychi();//由于吃不一定成功，需要由下一句判断 

bool pengable(int x,int*c); //优先级高于chiable,不碰无用的风牌 
bool chiable(int x,int*c); //不吃含有幺九的牌 
int value(int*c); //评估函数 
int mynum();

int main()
{
	int mark;
	char card[3];
    cin >> turnID;
    if(turnID<=2){
    	cout<<"PASS";
    	return 0;
	}
	cin>>mark>>playerID>>quan;//注意门风、圈风是0~3，对于牌号31~34 
	cin>>op;
	cin>>mark>>hua[0]>>hua[1]>>hua[2]>>hua[3];
	for(int i=0;i<50;++i)hidecard[i]=4;//初始化未出现的牌 
	for(int i=50;i<60;++i)hidecard[i]=1;
	for(int i=0;i<13;++i){
		cin>>card;
		mycard[trans(card)]++;
		hidecard[trans(card)]--;
	} 
	cin.getline(op,50);//处理掉花牌，此处未作统计
	cin.getline(op,50);
	for(int i=3;i<turnID;++i){
		request();
		response();
	}
	
	if(request()){//轮到自己出牌
		if(mynum()>2)opplay();
		else{
			int t=optin();
			cout<<"PLAY "<<detrans(t)<<t%10<<endl;
		}
	}
	else if(lastcard==0){
		cout<<"PASS"<<endl;
	}
	else{
		if(mynum()==1){
			if(mycard[lastcard]==1&&op[2]-'0'!=playerID)cout<<"HU"<<endl;
			else cout<<"PASS"<<endl;
		}
		else if(pengable(lastcard,mycard)&&op[2]-'0'!=playerID)oppeng();
		else if(chiable(lastcard,mycard)&&(playerID-(op[2]-'0')+4)%4==1)opchi();
		else cout<<"PASS"<<endl;
	} 

//测试用 
/*	for(int i=0;i<60;++i)cout<<mycard[i]<<" ";
	cout<<endl;
	for(int i=0;i<50;++i)cout<<4-hidecard[i]<<" ";
	cout<<endl;
	cout<<value(mycard)<<endl;
    return 0;*/
}


int trans(char*c)//将麻将字符转换成标号 
{
	int x=0;
	switch(c[0]) {
		case 'W':break;
		case 'B':x+=10;break;
		case 'T':x+=20;break;
		case 'F':x+=30;break;
		case 'J':x+=40;break;
		case 'H':x+=50;break;
	}
	x=x+c[1]-'0';
	return x;
}


char detrans(int x)//返回对应麻将标号的首字母 
{
	char c;
	switch(x/10){
		case 0:c='W';break;
		case 1:c='B';break;
		case 2:c='T';break;
		case 3:c='F';break;
		case 4:c='J';break;
		case 5:c='H';break;
		default:c='C'; //此行只是为了美观 
	}
	return c;
} 


bool request()//每回合输入的复盘 
{
	memset(op,0,sizeof(op));
	cin.getline(op,50);
	if(op[0]=='2'){
		mydraw();//自家摸牌，可能出现问题 
		return true;
	}
	else{
		if(op[4]=='B'&&op[6]=='H')buhua();//补花 
		if(op[4]=='D')draw();//摸牌 
		if(op[4]=='P'&&op[5]=='L')play();//出牌
		if(op[4]=='P'&&op[5]=='E')peng();//碰牌 
		if(op[4]=='C')chi();//吃牌 
		if(op[4]=='G')gang();//杠牌 
		if(op[4]=='B'&&op[6]=='G')bugang();//补杠 
		return false;
	} 
}


void response()//每回合输出的复盘
{
	memset(op,0,sizeof(op));
	cin.getline(op,50);
	if(op[0]=='P'&&op[1]=='A')mypass();
	if(op[0]=='P'&&op[1]=='L')myplay();
	if(op[0]=='G')mygang();
	if(op[0]=='B')mybugang();
	if(op[0]=='P'&&op[1]=='E')mypeng();
	if(op[0]=='C')mychi();
} 

void opplay()
{
	int choice=0,best=0;
	for(int i=0;i<60;++i)value_playcard[i]=-1;
	for(int i=0;i<60;++i){
		if(mycard[i]>0){
			for(int j=0;j<60;++j)trycard[j]=mycard[j];
			trycard[i]--;
			value_playcard[i]=value(trycard);
		}
	}
	for(int i=0;i<60;++i)best=max(best,value_playcard[i]);
	for(int i=0;i<60;++i){
		if(best==value_playcard[i])choice=i;
	}
	cout<<"PLAY "<<detrans(choice)<<choice%10<<endl;
}

void oppeng()
{
	mycard[lastcard]-=2;
	int choice=0,best=0;
	for(int i=0;i<60;++i)value_playcard[i]=-1;
	for(int i=0;i<60;++i){
		if(mycard[i]>0){
			for(int j=0;j<60;++j)trycard[j]=mycard[j];
			trycard[i]--;
			value_playcard[i]=value(trycard);
		}
	}
	for(int i=0;i<60;++i)best=max(best,value_playcard[i]);
	for(int i=0;i<60;++i){
		if(best==value_playcard[i])choice=i;
	}
	if(mynum()==2)choice=optin();//若听牌 
	cout<<"PENG "<<detrans(choice)<<choice%10<<endl;
} 

void opchi()
{
	int cchoice=0,pchoice=0,best=0;
	int x=lastcard;
	for(int j=0;j<3;++j){
		for(int i=0;i<60;++i)value_chicard[j][i]=-1;
	}
	if(mycard[x-2]>0&&mycard[x-1]>0&&(x-2)%10!=1&&x%10!=9){
		mycard[lastcard-2]--;
		mycard[lastcard-1]--;
		for(int i=0;i<60;++i){
			if(mycard[i]>0){
				for(int j=0;j<60;++j)trycard[j]=mycard[j];
				trycard[i]--;
				value_chicard[0][i]=value(trycard);
			}
		}
		mycard[lastcard-1]++;
		mycard[lastcard-2]++;
	} 
	if(mycard[x-1]>0&&mycard[x+1]>0&&(x-1)%10!=1&&(x+1)%10!=9){
		mycard[lastcard-1]--;
		mycard[lastcard+1]--;
		for(int i=0;i<60;++i){
			if(mycard[i]>0){
				for(int j=0;j<60;++j)trycard[j]=mycard[j];
				trycard[i]--;
				value_chicard[1][i]=value(trycard);
			}
		}
		mycard[lastcard+1]++;
		mycard[lastcard-1]++;
	}
	if(mycard[x+1]>0&&mycard[x+2]>0&&x%10!=1&&(x+2)%10!=9){
		mycard[lastcard+2]--;
		mycard[lastcard+1]--;
		for(int i=0;i<60;++i){
			if(mycard[i]>0){
				for(int j=0;j<60;++j)trycard[j]=mycard[j];
				trycard[i]--;
				value_chicard[2][i]=value(trycard);
			}
		}
		mycard[lastcard+1]++;
		mycard[lastcard+2]++;
	}
	for(int j=0;j<3;++j){
		for(int i=0;i<60;++i)best=max(best,value_chicard[j][i]);
	}
	for(int j=0;j<3;++j){
		for(int i=0;i<60;++i){
			if(best==value_chicard[j][i]){
				cchoice=lastcard+j-1;
				pchoice=i;
			}
		}
	}
	cout<<"CHI "<<detrans(cchoice)<<cchoice%10<<" "<<detrans(pchoice)<<pchoice%10<<endl;
}

int optin()
{
	int x=4,choice=0,tincard[60];
	for(int i=0;i<60;++i)tincard[i]=-1;
	for(int i=0;i<60;++i){
		if(mycard[i]>0){
			tincard[i]=hidecard[i];
			x=min(x,hidecard[i]);
		}
	}
	for(int i=0;i<60;++i){
		if(mycard[i]>0){
			if(i>30||i%10==1||i%10==9){
				choice=i;break;
			}
			else if(tincard[i]==x)choice=i;
		}
	}
	return choice;
} 

void mydraw()//记录未出现牌的减少，相应出牌时应当补偿 
{
	lastcard=0;
	mycard[trans(&(op[2]))]++;
	hidecard[trans(&(op[2]))]--;
}

void buhua()
{
	lastcard=0; 
	hidecard[trans(&(op[10]))]--;
}

void draw()
{
	lastcard=0;
}

void play()
{	
	lastcard=trans(&(op[9]));
	hidecard[lastcard]--;
}

void peng()
{
	hidecard[lastcard]-=2;
	lastcard=trans(&(op[9]));
	hidecard[lastcard]--;
} 

void chi()
{
	if(op[2]-'0'==playerID){
		mycard[lastcard]++;
		hidecard[lastcard]--; 
	
		mycard[trans(&(op[8]))-1]--;
		hidecard[trans(&(op[8]))-1]++;
		mycard[trans(&(op[8]))]--;
		hidecard[trans(&(op[8]))]++;
		mycard[trans(&(op[8]))+1]--;
		hidecard[trans(&(op[8]))+1]++;
	
		mycard[trans(&(op[11]))]--;
		hidecard[trans(&(op[11]))]++;
	}
	hidecard[lastcard]++;
	hidecard[trans(&(op[8]))-1]--;
	hidecard[trans(&(op[8]))]--;
	hidecard[trans(&(op[8]))+1]--;
	lastcard=trans(&(op[11])); 
	hidecard[lastcard]--;
}

void gang()
{
	if(lastcard!=0){
		hidecard[lastcard]=0;
		lastcard=0;
	}
}

void bugang()
{
	hidecard[trans(&(op[11]))]=0;
	lastcard=0; 
}

void mypass()//本身就需要为空 
{
	;
}

void myplay()
{
	mycard[trans(&(op[5]))]--;
	hidecard[trans(&(op[5]))]++;
}

void mygang()//空函数 
{
	;
}

void mybugang()//空函数
{
	;
} 

void mypeng()
{
	mycard[lastcard]-=2;
	hidecard[lastcard]+=2;
	mycard[trans(&(op[5]))]--;
	hidecard[trans(&(op[5]))]++;
}


void mychi()//转移到chi函数中 
{
	;
}

bool pengable(int x,int*c)
{
	if(c[x]>=2){
		if(x<30||x>40)return true;
		else if (x%10==playerID+1||x%10==quan+1)return true;
		else return false;
	}
	else return false;
}

bool chiable(int x,int*c)
{
	if(x>30)return false;
	else if(c[x-2]>0&&c[x-1]>0&&(x-2)%10!=1&&x%10!=9)return true;
	else if(c[x-1]>0&&c[x+1]>0&&(x-1)%10!=1&&(x+1)%10!=9)return true;
	else if(c[x+1]>0&&c[x+2]>0&&x%10!=1&&(x+2)%10!=9)return true;
	else return false;
}

int value(int*c)
{
	int x=0;
	for(int i=0;i<5;++i){
		for(int j=1;j<10;++j){
			if(pengable(10*i+j,c))x=x+3*hidecard[10*i+j];
			else if(chiable(10*i+j,c))x+=hidecard[10*i+j];
		}
	}
	return x;
} 

int mynum()
{
	int x=0;
	for(int i=0;i<60;++i)x+=mycard[i];
	return x;
}

/*输入样例 (玩家位于东风，圈风为北风)
12
0 0 3
PASS
1 0 2 2 0 W9 T2 B7 W6 T5 T8 W9 J1 F4 W8 B8 T5 W2 H3 H4 H8 H7
PASS
2 B3
PLAY F4
3 0 PLAY F4
PASS
3 1 DRAW
PASS
3 1 PLAY W4
PASS
3 2 DRAW
PASS
3 2 PLAY F3
PASS
3 3 DRAW
PASS
3 3 PLAY T1
PASS
2 B3
PLAY W9
3 0 PLAY W9
*/


