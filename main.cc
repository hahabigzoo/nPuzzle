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
Heuristic* heuristics[]={new NULLHeuristic(), new MisplacedTiles(),new Diagonal(),new Manhattan(),new LinearConflict()};//different heuristic function for IDA_star and a_star
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

void run(Astar* algorithm){
        end_t=0;
        pthread_t tid;
        start_t= clock();
        if(isSolvable(start,target)){
            //cout<<"Case:"<<++cnt<<endl;
            //limited time for ruunig
            pthread_create(&tid,NULL,func,algorithm);
            //set time for n ms
            Sleep(60000);
            if(target->g_==0){
                //out of time
                pthread_cancel(tid);
                cout<<"no found"<<endl;
                if(end_t==0)
                    end_t = clock();
                dur = (double)(end_t - start_t);
                printf("Use Time:%f s\n",(dur/CLOCKS_PER_SEC));
                cout<<(dur/CLOCKS_PER_SEC)<<",";
                cout<<"NaN"<<",";
                cout<<algorithm->exploration<<endl;
                return;
            }
            dur = (double)(end_t - start_t);
            //printf("Use Time:%f s\n",(dur/CLOCKS_PER_SEC));
            //cout<<"Path length:"<<target->g_<<endl;
            //cout<<"node counting:"<<algorithm->exploration<<endl;
            Print();
        }
}



/**
 main function
**/

int main(){
    FILE* f=freopen("./input_3.txt", "r", stdin);
    //freopen("./output_3.csv", "w", stdout);
    if(f==NULL){
        cout<<"failed"<<endl;
        return 0;
    }
    int dimension;
    int cnt=0;
    //cout<<"ID,Time,Path Len,Max Node,Node Num"<<endl;
    while(cin>>dimension){
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
        A_star algorithm
        **/
        Astar* algorithm;
        algorithm=new Astar(start,target);
        /**
        IDA_star algorithm
        **/
        //IDAstar* algorithm;
        //algorithm=new IDAstar(start,target);
        /**
        different heuristic function for IDA_star and a_star
        heuristics={ NULL, MisplacedTiles, Diagonal, Manhattan, LinearConflict}
        **/
        algorithm->h_function=heuristics[4];
        /**
        MHA algorithm
        **/
        //MHAstar* algorithm;
        //algorithm=new MHAstar(start,target);
        /**
        time
        **/
        end_t=0;
        start_t= clock();
        if(isSolvable(start,target)){
            //cout<<++cnt<<",";
            //cout<<"Case:"<<++cnt<<endl;
            //unlimited time for ruunig
            algorithm->search();
            end_t = clock();
            dur = (double)(end_t - start_t);
            //printf("Use Time:%f s\n",(dur/CLOCKS_PER_SEC));
            //cout<<"Path length:"<<target->g_<<endl;
            //cout<<"node counting:"<<algorithm->exploration<<endl;
            Print();
            /***excal***/

            /**
            cout<<(dur/CLOCKS_PER_SEC)<<",";
            cout<<target->g_<<",";
            cout<<algorithm->que.size()<<",";
            cout<<algorithm->exploration<<endl;
            **/
            delete target;
            delete start;
            delete algorithm;
        }else{
            cout<<"no solution"<<endl;
            delete target;
            delete start;
            delete algorithm;
        }
    }
    return 0;
}
