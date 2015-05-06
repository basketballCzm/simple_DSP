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
#include <sstream>
#include <math.h>
#include "user_map.h" 

using namespace std;
using namespace user_map;
//#define MALLPOINT 10
#define ROBOT_SUM 500 //顾客数
#define NUM2 100//商场坐标数
#define START -2.0f
#define END 2.0f
#define STEP 5.0f
#define TARGET_RANGE 5.0f

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
        float x_delta;
        float y_delta;
        float target_x;
        float target_y;

        //顾客初始位置
        void init_random_point(){
            //srand(time(0));
            x=(float)rand()/RAND_MAX*1000;
            y=(float)rand()/RAND_MAX*625;
            target_x=(float)rand()/RAND_MAX*1000;
            target_y=(float)rand()/RAND_MAX*625;
            z=rand()%7+1;
            id=user_counter;
            ++ user_counter;
            
            set_target(); 
        }

        void set_target()
        {
            target_x=(float)rand()/RAND_MAX*1000;
            target_y=(float)rand()/RAND_MAX*625;
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
            float delta = (END-START)*(float)rand()/RAND_MAX+START;
            x_delta = delta;
            delta = (END-START)*(float)rand()/RAND_MAX+START;
            y_delta = delta;
            this->x=x+x_delta;
            this->y=y+y_delta;
            cout<<"x_delta="<<x_delta<<",y_delta="<<y_delta<<"\n";
            //pnpoly();
            //	cout<<"顾客"<<j<<":"<<x<<","<<y<<endl;
        }

        void wander()
        {
            float distance=fabsf(target_x-x)+fabsf(target_y-y);
            if(distance<TARGET_RANGE)
            {
                set_target();    
            }
            float x_offset=target_x-x;
            float y_offset=target_y-y;
            float length=fabsf(x_offset)+fabsf(y_offset);
            x_delta=x_offset/length*STEP;
            y_delta=y_offset/length*STEP;
            
            //add some noise
            float x_noise= (END-START)*(float)rand()/RAND_MAX+START;
            this->x=x+x_delta+x_noise;
            float y_noise = (END-START)*(float)rand()/RAND_MAX+START;
            this->y=y+y_delta+y_noise;
            if(id==PeoplePoint::user_counter-1)
            {
                cout<<"wander() user_id="<<id<<" x="<<x<<" y="<<y
                    <<" x_delta="<<x_delta<<" y_delta="<<y_delta<<" x_noise="<<x_noise
                    <<" y_noise="<<y_noise<<" target_x="<<target_x<<" target_y="<<target_y<<"\n";
            }

        }

        void update_location_via_web()
        {
            ostringstream cmd_line;
            cmd_line<<"node module/robot/robot.js user_add --user "<<id<<" --x "<< x<<" --y "<<y<<" --z "<<z;
            cout<<cmd_line.str()<<"\n";
            system(cmd_line.str().c_str());
        }

        void update_location_via_memory()
        {
           //cout<<"update_location_via_memory() id="<<id<<" x="<<x<<" y="<<y<<" z="<<z<<"\n";
           user_add(id,x,y,z); 
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
            //pt[i].Brownian();
            pt[i].wander();
            pt[i].update_location_via_memory();
        }
        sleep(3);
    }


    return 0;
}
