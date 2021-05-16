// Copyright (c) 2021
//
// Author: Zhou Yating
//
// Lisence: GPL
//
// File: AI/State.h
/**********************************************************************************************************************************/
#ifndef AI_STATE_H_
#define AI_STATE_H_

#include<vector>
#include<iostream>
using namespace std;
class State{
public:
//public function
    //construction fuction and destroyed fuction
    State(int dimension);
    State(const State& s);
    State& operator= (const State&);
    ~State(){};
    void print();
    //other function
    bool operator<(const State& s) const{ return f_==s.f_ ? g_>s.g_ : f_>s.f_; }
    bool operator==(const State& s) const{ return values_==s.values_;}


//public variable
    int f_,g_; //heuristic value and path length
    int pos_; // the position of slider;
    int hash_; //hash value for current state; the default is -1;
    int dimension_; // for n puzzle, the dimension is equal to n;
    std::vector<int> values_; //the state values for
    State *parent_; //the pointer is to point its parent state;

private:

};

State::State(int dimension):dimension_(dimension){
    values_=std::vector<int>(dimension*dimension);
    parent_=0;
    hash_=-1;
    f_=0;
    g_=0;
    pos_=0;
}

void State::print(){
    //cout<<"values:"<<endl;
    for(int i=0;i<dimension_*dimension_;++i){
        cout<<values_[i]<<" ";
        if(i%dimension_==dimension_-1)
            cout<<endl;
    }
    //cout<<"g,f:"<<g_<<","<<f_<<endl;
    //cout<<"pos"<<pos_<<endl;
}
State::State(const State& s){
    dimension_=s.dimension_;
    values_=s.values_;
    parent_=s.parent_;
    hash_=s.hash_;
    f_=s.f_;
    g_=s.g_;
    pos_=s.pos_;
}

State& State::operator=(const State& s){
    dimension_=s.dimension_;
    values_=s.values_;
    parent_=s.parent_;
    hash_=-s.hash_;
    f_=s.f_;
    g_=s.g_;
    pos_=s.pos_;
    return *this;
}

#endif
