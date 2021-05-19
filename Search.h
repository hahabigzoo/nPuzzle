// Copyright (c) 2021
//
// Author: Zhou Yating
//
// Lisence: GPL
//
// File: AI/Search.h
/**********************************************************************************************************************************/
#ifndef AI_SEARCH_H_
#define AI_SEARCH_H_

#include<iostream>
#include<cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <omp.h>
#include <vector>
#include <queue>
#include <map>
using namespace std;




class State;
class Heuristic;
class SearchBase{
public:
    SearchBase(){}
    virtual bool search()=0; // the search fuction for n puzzle
    State *start,*end; //the start state and the end state
};




/**
IDAstar
**/


class IDAstar: public SearchBase{
public:
    IDAstar(State *start, State *end);
    int dfs(State* cur,int bound);
    Heuristic* h_function;
    State *start,*end;
    int exploration=0;
    bool search();
    bool isgoal(State* cur){
        return (*cur)==(*end);
    }
    int MOVE_X[4] ={0,0,-1,1};
    int MOVE_Y[4] ={1,-1,0,0};
    State* successor(State* cur,int i);

};

State* IDAstar::successor(State* cur,int i){
    int x,y;
    int dimension=cur->dimension_;
    x=cur->pos_%dimension;
    y=cur->pos_/dimension;
    x+=MOVE_X[i];
    y+=MOVE_Y[i];
    if(x<0||y<0||x>=dimension||y>=dimension)
        return 0;
    State* node=new State(dimension);
    node->values_=cur->values_;
    node->dimension_=dimension;
    node->pos_=y*dimension+x;
    node->values_[cur->pos_]=cur->values_[node->pos_];
    node->values_[node->pos_]=0;
    node->parent_=cur;
    node->g_=cur->g_+1;
    if((cur->parent_!=0)&&((*node)==(*(cur->parent_))))
    {
        delete node;
        return 0;
    }
    return node;
}

IDAstar::IDAstar(State *start, State *end):start(start),end(end){}

bool IDAstar::search(){
    int bound=h_function->get_h(start,end);
    while(true){
        bound=dfs(start,bound);
        if(bound==100000000){
            return false;
        }
        if(bound==-1){
            return true;
        }
    }

}

int IDAstar::dfs(State* cur,int bound){
    exploration+=1;
    cur->f_=cur->g_+h_function->get_h(cur,end);
    if(cur->f_ > bound)
        return cur->f_;
    if(isgoal(cur)){
        (*end)=(*cur);
        delete cur;
        cur=NULL;
        return -1;
    }

    int result=100000000;
    for(int i=0;i<4;i++){
        State *next=successor(cur,i);
        if(next!=0){
            result=min(result,dfs(next,bound));
        }
        if(result==-1)
            return -1;
        delete next;
    }
    return result;
}

/****************************************************************************************************************************************/

/**
Astar

Main function
g(sstart) = 0; all other g-values are infinite; OPEN = {sstart};
ComputePath();
publish solution;

ComputePath function
while(sgoal is not expanded)
remove s with the smallest [f(s) = g(s)+h(s)] from OPEN;
insert s into CLOSED;
for every successor s¡¯ of s such that s¡¯ not in CLOSED
if g(s¡¯) > g(s) + c(s,s¡¯)
g(s¡¯) = g(s) + c(s,s¡¯);
insert s¡¯ into OPEN;
**/



class Astar: public SearchBase{
    struct cmp{
        bool operator()(State* a,State* b){
            return a->f_ == b->f_? a->g_>b->g_ : a->f_ > b->f_;
        }
    };
    struct Compare
    {
      bool operator()(const State* x, const State* k) const{
        int dimension=x->dimension_;
        for(int i=0;i<dimension*dimension;++i){
            if(x->values_[i]==k->values_[i])
                continue;
            return x->values_[i] < k->values_[i];
        }
        return x->values_[dimension*dimension-1] < k->values_[dimension*dimension-1];
      }
    };
    typedef priority_queue<State*, vector<State*>, cmp> QUEUE;
    typedef map<State*,int,Compare> CloseList;
public:
    Astar(State *start, State *end);
    ~Astar();
    Heuristic* h_function;
    State *start,*end;
    int exploration=0;
    QUEUE que;
    CloseList closed;


    bool search();
    void expand(State* cur);
    bool isgoal(State* cur){
        if((*cur)==(*end))
        {
            (*end)=(*cur);
            delete cur;
            cur=NULL;
            return true;
        }
        return false;
    }
    int MOVE_X[4] ={0,0,-1,1};
    int MOVE_Y[4] ={1,-1,0,0};
    State* successor(State* cur,int i);

};

Astar::~Astar(){

}

State* Astar::successor(State* cur,int i){
    int x,y;
    int dimension=cur->dimension_;
    x=cur->pos_%dimension;
    y=cur->pos_/dimension;
    x+=MOVE_X[i];
    y+=MOVE_Y[i];
    if(x<0||y<0||x>=dimension||y>=dimension)
        return 0;
    State* node=new State(dimension);
    node->values_=cur->values_;
    node->dimension_=dimension;
    node->pos_=y*dimension+x;
    node->values_[cur->pos_]=cur->values_[node->pos_];
    node->values_[node->pos_]=0;
    node->parent_=cur;
    node->g_=cur->g_+1;
    return node;
}

Astar::Astar(State *start, State *end):start(start),end(end){}

void Astar::expand(State* cur){
    exploration+=1;
    State* node=NULL;
    for(int i=0;i<4;i++){
        node=successor(cur,i);
        if(node==NULL){
            continue;
        }
        if(closed.count(node) == 0){
            closed[node]=1;
            node->f_=node->g_+h_function->get_h(node,end);
            que.push(node);
        }else{
            delete node;
            node=NULL;
        }
    }
}

bool Astar::search(){
    State* cur=NULL;
    cur=start;
    cur->f_=cur->g_+h_function->get_h(cur,end);
    que.push(start);
    closed[cur]=1;
    while(!que.empty()){
        cur=que.top();
        que.pop();
        if(isgoal(cur)){
            return true;
        }
        expand(cur);
    }
    return false;

}





/******************************************************************************************************************************************/
/**
IMHA* ALGORITHM FROM
Aine, Sandip, et al. "Multi-heuristic a." The International Journal of Robotics Research 35.1-3 (2016): 224-243.
**/

class MHAstar{
    struct cmp{
        bool operator()(State* a,State* b){
            return a->f_ == b->f_? a->g_>b->g_ : a->f_ > b->f_;
        }
    };
    struct Compare
    {
      bool operator()(const State* x, const State* k) const{
        int dimension=x->dimension_;
        for(int i=0;i<dimension*dimension;++i){
            if(x->values_[i]==k->values_[i])
                continue;
            return x->values_[i] < k->values_[i];
        }
        return x->values_[dimension*dimension-1] < k->values_[dimension*dimension-1];
      }
    };
    typedef priority_queue<State*, vector<State*>, cmp> QUEUE;
    typedef map<State*,int,Compare> CloseList;
public:
    MHAstar(State *start, State *end);
    ~MHAstar();
    /**
    const var
    **/
    int MOVE_X[4] ={0,0,-1,1};
    int MOVE_Y[4] ={1,-1,0,0};
    /**
    local var
    **/
    vector<Heuristic*> hs;
    State *start,*end;
    int exploration=0;
    /**
    function
    **/
    bool search();
    bool isgoal(State* cur){
        if((*cur)==(*end))
        {
            (*end)=(*cur);
            delete cur;
            cur=NULL;
            return true;
        }
        return false;
    }
    State* successor(State* cur,int i);
    int key(State* cur,int i){
        return cur->g_+w1*hs[i]->get_h(cur,end);
    }
    void expand(State* cur,int i);
    /**
    data struct
    **/

    vector<QUEUE> qhs;
    vector<CloseList> chs;
    int w1,w2;
    int n;
};

MHAstar::MHAstar(State *start, State *end):start(start),end(end){
    n=5;
    w1=20;
    w2=20;
    qhs=vector<QUEUE>(n);
    chs=vector<CloseList>(n);
    hs=vector<Heuristic*>(n);
    hs[0]=new LinearConflict();
    hs[1]=new MisplacedTiles();
    for(int i=2;i<n;i++){
        hs[i]=new MHeuristic();
    }

}

MHAstar::~MHAstar(){
    for(int i=0;i<n;i++){
        delete hs[i];
        for(auto &it : chs[i]){
            if(it.first!=NULL){
                delete it.first;
            }
        }
    }
}

State* MHAstar::successor(State* cur,int i){
    int x,y;
    int dimension=cur->dimension_;
    x=cur->pos_%dimension;
    y=cur->pos_/dimension;
    x+=MOVE_X[i];
    y+=MOVE_Y[i];
    if(x<0||y<0||x>=dimension||y>=dimension)
        return 0;
    State* node=new State(dimension);
    node->values_=cur->values_;
    node->dimension_=dimension;
    node->pos_=y*dimension+x;
    node->values_[cur->pos_]=cur->values_[node->pos_];
    node->values_[node->pos_]=0;
    node->parent_=cur;
    node->g_=cur->g_+1;
    return node;
}

void MHAstar::expand(State* cur,int k){
    exploration+=1;
    State* node=NULL;
    for(int i=0;i<4;i++){
        node=successor(cur,i);
        if(node==NULL){
            continue;
        }
        if(chs[k].count(node) == 0){
            chs[k][node]=1;
            node->f_=key(node,k);
            qhs[k].push(node);
        }else{
            delete node;
            node=NULL;
        }
    }

}

bool MHAstar::search(){
    State* cur=NULL;
    State* cur1=NULL;
    for(int i=0;i<n;i++){
        cur =new State(*start);
        cur->f_=key(cur,i);
        qhs[i].push(cur);
        chs[i][cur]=1;
    }
    while(!qhs[0].empty()){
        cur=qhs[0].top();
        for(int i=1;i<n;i++){
            cur1=qhs[i].top();
            if(cur1->f_<=w2*cur->f_){
                if(isgoal(cur1)){
                    return true;
                }
                qhs[i].pop();
                expand(cur1,i);
            }else{
                if(isgoal(cur)){
                    return true;
                }
                qhs[0].pop();
                expand(cur,0);
            }

        }
    }
    return false;
}


/**
Weighted A* Search: expands states in the order of f =
g+¦Åh values, ¦Å > 1 = bias towards states that are closer to
goal
**/
class WAstar: public SearchBase{
    struct cmp{
        bool operator()(State* a,State* b){
            return a->f_ == b->f_? a->g_>b->g_ : a->f_ > b->f_;
        }
    };
    struct Compare
    {
      bool operator()(const State* x, const State* k) const{
        int dimension=x->dimension_;
        for(int i=0;i<dimension*dimension;++i){
            if(x->values_[i]==k->values_[i])
                continue;
            return x->values_[i] < k->values_[i];
        }
        return x->values_[dimension*dimension-1] < k->values_[dimension*dimension-1];
      }
    };
    typedef priority_queue<State*, vector<State*>, cmp> QUEUE;
    typedef map<State*,int,Compare> CloseList;
public:
    WAstar(State *start, State *end );
    ~WAstar();
    Heuristic* h_function;
    State *start,*end;
    int exploration=0;
    int sigma=2;
    QUEUE que;
    CloseList closed;


    bool search();
    void expand(State* cur);
    bool isgoal(State* cur){
        if((*cur)==(*end))
        {
            (*end)=(*cur);
            delete cur;
            cur=NULL;
            return true;
        }
        return false;
    }
    int MOVE_X[4] ={0,0,-1,1};
    int MOVE_Y[4] ={1,-1,0,0};
    State* successor(State* cur,int i);

};

WAstar::~WAstar(){

}

State* WAstar::successor(State* cur,int i){
    int x,y;
    int dimension=cur->dimension_;
    x=cur->pos_%dimension;
    y=cur->pos_/dimension;
    x+=MOVE_X[i];
    y+=MOVE_Y[i];
    if(x<0||y<0||x>=dimension||y>=dimension)
        return 0;
    State* node=new State(dimension);
    node->values_=cur->values_;
    node->dimension_=dimension;
    node->pos_=y*dimension+x;
    node->values_[cur->pos_]=cur->values_[node->pos_];
    node->values_[node->pos_]=0;
    node->parent_=cur;
    node->g_=cur->g_+1;
    return node;
}

WAstar::WAstar(State *start, State *end):start(start),end(end){}

void WAstar::expand(State* cur){
    exploration+=1;
    State* node=NULL;
    for(int i=0;i<4;i++){
        node=successor(cur,i);
        if(node==NULL){
            continue;
        }
        if(closed.count(node) == 0){
            closed[node]=1;
            node->f_=node->g_+sigma*h_function->get_h(node,end);
            que.push(node);
        }else{
            delete node;
            node=NULL;
        }
    }
}

bool WAstar::search(){
    State* cur=NULL;
    cur=start;
    cur->f_=cur->g_+sigma*h_function->get_h(cur,end);
    que.push(start);
    closed[cur]=1;
    while(!que.empty()){
        cur=que.top();
        que.pop();
        if(isgoal(cur)){
            return true;
        }
        expand(cur);
    }
    return false;

}
#endif

