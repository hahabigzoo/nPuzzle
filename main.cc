// Copyright (c) 2021
//
// Author: Zhou Yating and Zhang Bin
//
// Lisence: GPL
//
// File: AI/main.cc
/**********************************************************************************************************************************/
#include"State.h"
#include"Heuristic.h"
#include"Search.h"
#include"Times.h"
#include<time.h>
#include<unistd.h>
#include<signal.h>
#include<iostream>
#include<cstdio>
#include <pthread.h>
#include <stdlib.h>
#include <windows.h>
using namespace std;
class State;
class Heuristic;
double dur;
clock_t start_t=0,end_t=0;
State *start,*target;
Heuristic* heuristic=NULL;


/**
 judge whether the puzzle is solvable.
**/
bool isSolvable(State *start,State *target){
	int dimension = start->dimension_;
	int cnt_ini=0,cnt_tar=0;
	for(int i = 0; i < dimension*dimension; i ++)
		for(int j = i; j < dimension*dimension; j ++)
		{

			if(start->values_[i] > start->values_[j]&&start->values_[i]!=0&&start->values_[j]!=0)cnt_ini ++;
			if(target->values_[i] > target->values_[j]&&target->values_[i]!=0&&target->values_[j]!=0)cnt_tar ++;
		}

	if(dimension%2==0)
	{
	    int y1,y2;
        y1=start->pos_/dimension;
        y2=target->pos_/dimension;
		int y = abs(y1 - y2);
		if(y%2==1)
		{
		    return (cnt_ini%2) != (cnt_tar%2);
		}
		else
		{

            return (cnt_ini%2) == (cnt_tar%2);
		}
	}
	else
	{
		return (cnt_ini%2) == (cnt_tar%2);
	}
}


/**
Print Path
**/
void Print(){
    State *link0,*link1,*link2;
    if(*target==*start){
        cout<<"³õÊ¼×´Ì¬:"<<endl;
        target->print();
        cout<<"ÖÕÖ¹×´Ì¬:"<<endl;
        target->print();
        return;
    }
    if((*target->parent_)==(*start)){
        cout<<"³õÊ¼×´Ì¬:"<<endl;
        start->print();
        cout<<"ÖÕÖ¹×´Ì¬:"<<endl;
        target->print();
        return;
    }
    link0=target->parent_->parent_;
    link1=target->parent_;
    link2=target;
    link2->parent_=NULL;
    while(link0!=NULL){
        link1->parent_=link2;
        link2=link1;
        link1=link0;
        link0=link0->parent_;
    }
    int i=0;
    link1->parent_=link2;
    while(link1!=NULL){
        if(i!=0&&link1->parent_!=NULL){
            cout<<"µÚ"<<i++<<"²½"<<endl;
        }
        if(i==0){
            i++;
            cout<<"³õÊ¼×´Ì¬"<<endl;
        }
        if(link1->parent_==NULL){
            cout<<"ÖÕÖ¹×´Ì¬"<<endl;
        }
        link1->print();
        link0=link1;
        link1=link1->parent_;
        //delete link0;
        //link0=NULL;
    }

}

/**
thread function. Now,you need change the pointer if you want to change algorithm
**/
void *func(void* a){
    start_t = clock();
    Astar* c=(Astar*)a;
    //IDAstar* c=(IDAstar*)a;
    //MHAstar* c=(MHAstar*)a;
    c->search();
    end_t = clock();
    return 0;
}

/**
 main function
**/

int main(){
    FILE* f=freopen("./input.txt", "r", stdin);
    freopen("./save/output_a.csv", "w", stdout);
    if(f==NULL){
        cout<<"failed"<<endl;
        return 0;
    }
    int dimension;
    int cnt=0;
    cout<<"ID,Time,Path Len,Node Num"<<endl;
    while(cin>>dimension){
        if(cnt==10){
            break;
        }
        start=new State(dimension);
        target=new State(dimension);
        for(int i=0;i<dimension*dimension;++i){
            cin>>start->values_[i];
            if(start->values_[i]==0){
                start->pos_=i;
            }
        }
        for(int i=0;i<dimension*dimension;++i){
            cin>>target->values_[i];
            if(target->values_[i]==0){
                target->pos_=i;
            }
        }
        /**
        different heuristic function for IDA_star and a_star
        **/
        //heuristic=new Manhattan();
        heuristic=new LinearConflict();
        //heuristic=new MisplacedTiles();
        /**
        A_star algorithm
        **/
        //wait for achieving
        Astar* algorithm;
        algorithm=new Astar(start,target);
        algorithm->h_function=heuristic;
        /**
        IDA_star algorithm
        **/
        //IDAstar* algorithm;
        //algorithm=new IDAstar(start,target);
        //algorithm->h_function=heuristic;
        /**
        MHA algorithm
        **/
        //MHAstar* algorithm;
        //algorithm=new MHAstar(start,target);
        /**
        time
        **/
        pthread_t tid;
        start_t= clock();
        if(isSolvable(start,target)){
            cout<<++cnt<<",";
            //cout<<"Case:"<<++cnt<<endl;
            //unlimited time for ruunig
            //algorithm->search();
            //limited time for ruunig
            pthread_create(&tid,NULL,func,algorithm);
            //set time for n ms
            Sleep(2000);
            if(target->g_==0){
                pthread_cancel(tid);
                //cout<<"no found"<<endl;
                //out of time
                //pthread_kill(tid, 0);
                if(end_t==0)
                    end_t = clock();
                dur = (double)(end_t - start_t);
                //printf("Use Time:%f s\n",(dur/CLOCKS_PER_SEC));
                cout<<(dur/CLOCKS_PER_SEC)<<",";
                cout<<"NaN"<<",";
                cout<<algorithm->exploration<<endl;
                continue;
            }
            //end_t = clock();
            dur = (double)(end_t - start_t);
            //printf("Use Time:%f s\n",(dur/CLOCKS_PER_SEC));
            //cout<<"Path length:"<<target->g_<<endl;
            //cout<<"node counting:"<<algorithm->exploration<<endl;
            //Print();
            /***excal***/
            cout<<(dur/CLOCKS_PER_SEC)<<",";
            cout<<target->g_<<",";
            cout<<algorithm->exploration<<endl;
            delete target;
            delete start;
            delete heuristic;
            delete algorithm;
        }else{
            //cout<<"no solution"<<endl;
            delete target;
            delete start;
            delete heuristic;
            delete algorithm;
        }
    }
    return 0;
}

/**
&&algorithm->search()
Test
3
1 2 3
4 5 6
7 8 0
1 2 3
4 5 6
7 8 0

3
1 2 3
4 5 0
7 8 6
1 2 3
4 5 6
7 8 0

4
1 2 3 4
5 10 6 7
14 0 11 8
9 13 15 12

1 2 3 4
5 6 7 8
9 10 11 12
13 14 15 0

4
1 2 3 4
6 7 8 0
5 10 11 12
9 13 14 15

1 2 3 4
5 6 7 8
9 10 11 12
13 14 15 0

4
8 13 0 6
1 15 9 14
3 4 5 11
7 2 10 12

1 2 3 4
5 6 7 8
9 10 11 12
13 14 15 0

IDA* + Manha:2005400

4
4 7 0 9
12 10 11 8
14 6 15 1
2 5 3 13

1 2 3 4
5 6 7 8
9 10 11 12
13 14 15 0

4
4 7 0 9
12 10 11 8
14 6 15 1
2 5 3 13

1 2 3 4
5 6 7 8
9 10 11 12
13 14 15 0
**/
