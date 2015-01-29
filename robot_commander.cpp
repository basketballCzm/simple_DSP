/*
   （1）现在商场坐标只用了很小的数据进行测试，待完成的就是判断人是否在商场内吗，由于商场坐标我是用三个数组
   表示的，所以我的思想是先判断Z【i】（楼层），在进行相应楼层的对顾客是否在商场内进行判断。
   （2）判断顾客在哪个商店内，打算和判断是否在商场内相同的方法（如果商店是矩形的话则更容易实现，这个看需求是怎么样的）
   （3）在莫一时刻看某一顾客的位置，由于程序里顾客的位置不断变化（即while（1）不断进行），目前还不知用什么方法
   Written by determine  2015/01/15
   Modified by caster 2015/01/29
   */


#include <iostream>
#include <ctime>
#include <cstdlib>
#include <unistd.h>
#include <fstream>
using namespace std;
//#define MALLPOINT 10
#define ROBOT_SUM 50 //顾客数
#define NUM2 100//商场坐标数
#define START -5
#define END 5


//商场坐标
struct MallPoint{
    public:
        float x[NUM2];
        float y[NUM2];
        int z[NUM2];
        //int i=0;
        char ch;
        void init_point(){
            int i=0;
            ifstream file;
            char filename[512];
            cout<<"Please enter the filename:"<<endl;
            cin>>filename;//输入存储商场坐标文件的文件名
            file.open(filename,ios::in);
            if(file.fail())
            {
                cout<<"The file doesn't exit!"<<endl;
                file.close();
            }
            else
            {
                while(!file.eof()){
                    file>>x[i]>>y[i]>>z[i];
                    cout<<"("<<x[i]<<","<<y[i]<<","<<z[i]<<")"<<endl;
                    i++;
                }
            }

        }

};

//顾客位置
class PeoplePoint{
    public:
        float x;
        float y;
        int z;
        int id;

        //顾客初始位置
        void init_random_point(){
            //srand(time(0));
            x=(float)rand()/RAND_MAX*1000;
            y=(float)rand()/RAND_MAX*1000;
            z=rand()%3+1;
            id=user_counter;
            ++ user_counter;
            //	cout<<"("<<x<<","<<y<<","<<z<<")"<<"  RAND_MAX = "<<RAND_MAX<<endl;
        }

        //判断是否在商场内，还未实现
        int pnpoly () {
            int nvert; float *vertx; float *verty; float testx; float testy;
            int i,j,c=0;
            for (i=0,j=nvert-1;i<nvert;j=i++){
                if(((verty[i]>testy)!=(verty[j]>testy))&&(testx<(vertx[j]-vertx[i])*(testy-verty[i])/(verty[j]-verty[i])+vertx[i]))
                    c=!c;
            }
            return c;
        }

        //顾客随机走动
        void Brownian(){
            this->x=x+(END-START)*rand()/RAND_MAX+START;
            this->y=y+(END-START)*rand()/RAND_MAX+START;
            //pnpoly();
            //	cout<<"顾客"<<j<<":"<<x<<","<<y<<endl;
        }
        static int user_counter;

};

int PeoplePoint::user_counter=100001;

int main(void)
{
    int i;
    PeoplePoint pt[ROBOT_SUM];

    srand(time(0));
    for(i=0;i<ROBOT_SUM;i++){
        pt[i].init_random_point();
        cout<<"顾客"<<pt[i].id<<":  "<<"("<<pt[i].x<<","<<pt[i].y<<","<<pt[i].z<<")"<<endl;
        //			pt[i].Brownian();
    }


    while(1){
        //		Sleep(3000);
        for(i=0;i<ROBOT_SUM;i++){
            pt[i].Brownian();
            cout<<"顾客"<<i<<":  "<<"("<<pt[i].x<<"\t"<<","<<pt[i].y<<"\t"<<","<<pt[i].z<<")"<<"\t"<<endl;
            system("node");
        }
        sleep(3);
    }


    return 0;
}

