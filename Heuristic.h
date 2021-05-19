// Copyright (c) 2021
//
// Author: Zhou Yating
//
// Lisence: GPL
//
// File: AI/Heuristic.h
/**********************************************************************************************************************************/
#ifndef AI_HEURISTIC_H
#define AI_HEURISTIC_H
#include <omp.h>
#include <stdlib.h>
#include <time.h>
int abs(int a){
    return a>0? a:-a;
}

class Heuristic{
public:
    virtual int get_h(State* cur,State* target)=0;
};

/**
BFS
**/
class NULLHeuristic:public Heuristic{
public:
    int get_h(State* cur,State* target);
};
int NULLHeuristic::get_h(State* cur,State* target){
    return 0;
}
/**
Manhattan distance:

function heuristic(node) =
    dx = abs(node.x - goal.x)
    dy = abs(node.y - goal.y)
    return D * (dx + dy)
**/

class Manhattan:public Heuristic
{
private:
    /* data */
public:
    Manhattan(/* args */);
    ~Manhattan();
    int get_h(State* cur,State* target);
};

Manhattan::Manhattan(/* args */)
{
}

Manhattan::~Manhattan()
{
}

Manhattan::get_h(State* cur,State* target){
    int distance=0;
    int dimention=cur->dimension_;
    int x1,y1,x2,y2;
    //#pragma omp parallel for num_threads(32)
    for(int i=0;i<dimention*dimention;++i){
        x1=i%dimention;y1=i/dimention;
        if(cur->values_[i]==0)
            continue;
        for(int j=0;j<dimention*dimention;j++){
            if(cur->values_[i]==target->values_[j]){
                x2=j%dimention;y2=j/dimention;
                distance+=(abs(x1-x2)+abs(y1-y2));
            }
        }
    }
    return distance;
}

/**
Misplaced Tiles distance:

**/

class MisplacedTiles:public Heuristic{
public:
    int get_h(State* cur,State* target);
};
int MisplacedTiles::get_h(State* cur,State* target){
    int distance=0;
    int dimention=cur->dimension_;
    for(int i=0;i<dimention*dimention;++i){
        if(cur->values_[i]==0||cur->values_[i]==target->values_[i])
        {
            continue;
        }
        distance+=1;
    }
    return distance;
}

/**
Linear Conflict distance:
Misplaced Tiles distance + Linear Conflict
**/

class LinearConflict:public Heuristic{
public:
    int get_h(State* cur,State* target);
    bool conflict(int x1,int y1,int x1s,int y1s,int x2,int y2,int x2s,int y2s){
        if(x1==x1s&&x2==x2s&&x1==x2){
            return (y1>y2&&y1s<y2s)||(y1<y2&&y1s>y2s);
        }
        if(y1==y1s&&y2==y2s&&y1==y2){
            return (x1>x2&&x1s<x2s)||(x1<x2&&x1s>x2s);
        }
        return false;
    }
};
int LinearConflict::get_h(State* cur,State* target){
    int distance=0;
    int dimention=cur->dimension_;
    int* loc = new int[(dimention*dimention)];
    int x1,y1,x2,y2;
    for(int i=0;i<dimention*dimention;++i){
        x1=i%dimention;y1=i/dimention;
        if(cur->values_[i]==0)
            continue;
        for(int j=0;j<dimention*dimention;j++){
            loc[target->values_[j]]=j;
            if(cur->values_[i]==target->values_[j]){
                x2=j%dimention;y2=j/dimention;
                distance+=(abs(x1-x2)+abs(y1-y2));
            }
        }
    }
    int x1s,y1s,x2s,y2s;
    for(int i=0;i<dimention*dimention;i++){
        x1=i%dimention;y1=i/dimention;
        int v=cur->values_[i];
        x2=loc[v]%dimention;y2=loc[v]/dimention;
        if(x1==x2||y1==y2&&cur->values_[i]!=0){
            for(int j=i+1;j<dimention*dimention;j++){
                x1s=j%dimention;y1s=j/dimention;
                int k=target->values_[j];
                x2s=loc[k]%dimention;y2s=loc[k]/dimention;
                if(target->values_[j]!=0&&conflict(x1,y1,x2,y2,x1s,y1s,x2s,y2s)){
                    distance+=2;
                }
            }
        }
    }
    delete[] loc;
    return distance;
}

class MHeuristic:public Heuristic{
public:
    MHeuristic(){
        r1=rand() % 5+ 1;
        r2=rand() % 5+ 1;
        r3=rand() % 5+ 1;
        h1=new Manhattan();
        h2=new LinearConflict();
        h3=new MisplacedTiles();
    }
    ~MHeuristic(){
        delete h1;
        delete h2;
        delete h3;
    }
    int get_h(State* cur,State* target){
        return r1*h1->get_h(cur,target)+r2*h2->get_h(cur,target)+r3*h3->get_h(cur,target);
    }
    /**
    var
    **/
    int r1,r2,r3;
    Heuristic *h1,*h2,*h3;
};



class DoubleHeuristic:public Heuristic{
public:
    DoubleHeuristic(){
        h1=new LinearConflict();
        h2=new MisplacedTiles();
    }
    ~DoubleHeuristic(){
        delete h1;
        delete h2;
    }
    int get_h(State* cur,State* target){
        return h1->get_h(cur,target)+h2->get_h(cur,target);
    }
    /**
    var
    **/
    Heuristic *h1,*h2;
};

/**
Diagonal distance: h(x,y) = max(abs(x-xgoal), abs(y-ygoal))
**/
class Diagonal:public Heuristic{
public:
    int get_h(State* cur,State* target);
};
int Diagonal::get_h(State* cur,State* target){
    int distance=0;
    int dimention=cur->dimension_;
    int x1,y1,x2,y2;
    //#pragma omp parallel for num_threads(32)
    for(int i=0;i<dimention*dimention;++i){
        x1=i%dimention;y1=i/dimention;
        if(cur->values_[i]==0)
            continue;
        for(int j=0;j<dimention*dimention;j++){
            if(cur->values_[i]==target->values_[j]){
                x2=j%dimention;y2=j/dimention;
                distance+=max(abs(x1-x2),abs(y1-y2));
            }
        }
    }
    return distance;
}
#endif
