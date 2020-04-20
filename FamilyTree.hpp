//
// Created by user on 12/04/2020.
//

#pragma once

#include <iostream>
#include <vector>
#include <set>
#include <sstream>
#include <map>
using namespace std;

enum position {
    self, father_pos, mother_pos
};

struct relation_data {
    bool valid;
    int depth;
    position pos;
};

struct node
{
    string name;
    node *father;
    node *mother;

    node(string name){
        this->name = name;
        father = mother = NULL;
    }
};

namespace family{
    class Tree{
    private:
        /*Private variables*/
        node *root = NULL;

        /*Private methods*/
        void freeTree(node *root);
        node* getChild(string who,bool found, node *root);
        node* search(string who, node *root);
        relation_data search(string who,int currentDepth, position pos, node *root);
        node* search(relation_data data ,int currentDepth, position pos ,node *root);
        vector<string> split(const string& s, char delimiter);
        relation_data getRelationData(vector<string> relation);
        string relationDataToString(relation_data data);

    public:
        Tree(string root);
        ~Tree();

        Tree& addFather(string to, string name);
        Tree& addMother(string to, string name);

        void display();
        string relation(string who);
        string find(string relation);
        void remove(string name);
    };
}
