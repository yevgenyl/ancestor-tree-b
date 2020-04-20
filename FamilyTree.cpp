#include <iostream>
#include <vector>
#include "FamilyTree.hpp"

using namespace std;
using namespace family;

/*
* Possible exceptions:
* 1) deleteRootException - root can't be deleted.
* 2) relationNotFoundException - relation doesn't exist.
* 3) badRelationException - relation syntax is incorrect.
* 4) alreadyExistException - when trying to add father/mother to someone who already exist.
* 5) personNotFoundException - when trying to add father/mother to someone who doesn't exist.
*/

class deleteRootException: public exception
{
    virtual const char* what() const throw()
    {
        return "Root can't be deleted!";
    }
} deleteRootException;

class relationNotFoundException: public exception
{
    virtual const char* what() const throw()
    {
        return "Unable to find relation.";
    }
} relationNotFoundException;

class badRelationException: public exception
{
    virtual const char* what() const throw()
    {
        return "Given string violates relation syntax rules. please check your string.";
    }
} badRelationException;

class alreadyExistException: public exception
{
    virtual const char* what() const throw()
    {
        return "Father/mother already exist!";
    }
} alreadyExistException;

class personNotFoundException: public exception
{
    virtual const char* what() const throw()
    {
        return "Person not found!";
    }
} personNotFoundException;

/*Outline constructor - creates new tree data structure with youngest person as root*/
Tree::Tree(string root){
    this->root = new node(root);
}

/*Outline destructor - destruct the Tree*/
Tree::~Tree(){
    freeTree(this->root); // Free all dynamic memory that was allocated bt Tree nodes.
}

/*
* freeTree - Free all dynamic memory that was allocated by tree nodes.
* param root: The tree root.
*/
void Tree::freeTree(node *root)
{
    if(root != NULL)
    {
        freeTree(root->father);
        freeTree(root->mother);
        delete root;
    }
}

/*
* getChild - get child of a given person (by name).
* param 1: who - the person who's child need to be found.
* param 2: found - true/false flag to stop recursive function.
* param 3: root = the root of the Tree.
* return value: node* - if node found Or NULL in case of no child at all.
*/
node* Tree::getChild(string who,bool found, node *root){
    if(root != NULL){
        if(found){
            return root;
        }else{
            bool flag = false;
            if(root->father != NULL) {
               flag = root->father->name.compare(who) == 0;
            }
            node *found = flag ? getChild(who,flag,root) : getChild(who,flag,root->father);
            if(found == NULL){
                if(root->mother != NULL) {
                    flag = root->mother->name.compare(who) == 0;
                }
                found = flag ? getChild(who,flag,root) : getChild(who,flag,root->mother);
            }
            return found;
        }
    }else{
        return NULL;
    }
}

/*
* search - get person's node by given name.
* param 1: who - the person who need to be found.
* param 2: root = the root of the Tree.
* return value: node* - if node found Or NULL in case of no matching.
*/
node* Tree::search(string who,node *root){
    if(root != NULL){
        if(root->name.compare(who) == 0){
            return root;
        }else{
            node *found = search(who,root->father);
            if(found == NULL){
                found = search(who,root->mother);
            }
            return found;
        }
    }else{
        return NULL;
    }
}

/*
* search - search person by given name and get relation information object in return (relation_data).
* param 1: who - the person who need to be found.
* param 2: currentDepth = the current depth (recuresive iteration).
* param 3: pos = self/father/mother.
* param 4: root = Tree root.
* return value: relation_data - an object which contains relation information: depth, sex and validation flag.
*/
relation_data Tree::search(string who,int currentDepth, position pos, node *root){
    relation_data data;
    if(root != NULL){
        if(root->name.compare(who) == 0){
            data.valid = true;
            data.depth = currentDepth;
            data.pos = pos;
            return data;
        }else{
            relation_data found = search(who ,++currentDepth , father_pos, root->father);
            if(!found.valid){
                found = search(who ,currentDepth , mother_pos, root->mother);
            }
            return found;
        }
    }else{
        data.valid = false;
        return data;
    }
}

/*
* search - search person by relation_data object.
* param 1: data - relation data object.
* param 2: currentDepth = the current depth (recuresive iteration).
* param 3: pos = self/father/mother.
* param 4: root = Tree root.
* return value: node* - if person was found by the specified data object. NULL in case of no match.
*/
node* Tree::search(relation_data data ,int currentDepth, position pos ,node *root){
    if(root != NULL && currentDepth <= data.depth){
        if(data.depth == currentDepth && data.pos == pos){
            return root;
        }else{
            node *found = search(data,++currentDepth,father_pos,root->father);
            if(found == NULL){
                found = search(data,currentDepth,mother_pos,root->mother);
            }
            return found;
        }
    }else{
        return NULL;
    }
}

/*
* split - splits a string by given delimiter.
* param 1: s - string input to split.
* param 2: delimiter - specified character.
* return value: vector of string splited by the specified delimiter.
*/
vector<string> Tree::split(const string& s, char delimiter){
    vector<string> words;
    string word = "";
    istringstream stream(s);
    while(getline(stream,word,delimiter)){
        words.push_back(word);
    }
    return words;
}

/*
* getRelationData - creates relation_data object by splitted string of relation (Example: "great-great-grandfather").
* param 1: relation - a vector of splitted words which represents realtion.
* return value: data - relation_data object.
*/
relation_data Tree::getRelationData(vector<string> relation){
    relation_data data;
    int size = relation.size();
    set<string> valid_words = {"me", "father", "mother", "grandfather", "grandmother", "great"};
    if(size < 1)
        data.valid = false;
    if(size == 1){
        string word = relation.at(0);
        if(valid_words.find(word) != valid_words.end()){
            if(word.compare("me") == 0) {
                data.valid = true;
                data.pos = self;
                data.depth = 0;
            }else if(word.compare("mother") == 0){
                data.valid = true;
                data.pos = mother_pos;
                data.depth = 1;
            } else if (word.compare("father") == 0){
                data.valid = true;
                data.pos = father_pos;
                data.depth = 1;
            } else if (word.compare("grandfather") == 0){
                data.valid = true;
                data.pos = father_pos;
                data.depth = 2;
            }else if (word.compare("grandmother") == 0){
                data.valid = true;
                data.pos = mother_pos;
                data.depth = 2;
            }else{
                data.valid = false;
            }
        }else{
            data.valid = false;
        }
    } else{
       //more than one word
       data.valid = true;
       unsigned int counter = 0;
        string lastMember = relation.at(size-1);
        for(string s : relation){
            if(valid_words.find(s) == valid_words.end() || ((s.compare("great") != 0)&&(counter < size-1)) ||
                    ((lastMember.compare("grandfather") != 0) && (lastMember.compare("grandmother") != 0))){
                data.valid = false;
                break;
            } else{
                counter++;
            }
        }
        if(data.valid){
            data.valid = true;
            data.depth = counter + 1;
            data.pos = lastMember.compare("grandfather") == 0 ? father_pos : mother_pos;
        }
    }
    return data;
}

/*
* relationDataToString - the inverse function of 'getRelationData'. the function constructs a string from realtion_data object.
* param 1: data - relation_data object.
* return value: a string which describes a relation (Example: "great-great-grandmother").
*/
string Tree::relationDataToString(relation_data data){
    string to_return = "";
    if(data.valid){
        if(data.depth == 0){
            to_return = "me";
        } else if (data.depth == 1){
            if(data.pos == father_pos){
                to_return = "father";
            }else{
                to_return = "mother";
            }
        }else if (data.depth == 2){
            if(data.pos == father_pos){
                to_return = "grandfather";
            }else{
                to_return = "grandmother";
            }
        }else{
            for(int i = 0; i < data.depth-2; i++){
                to_return += "great-";
            }
            if(data.pos == father_pos){
                to_return += "grandfather";
            }else{
                to_return += "grandmother";
            }
        }
    }
    return to_return;
}

/*
* addFather - a function which adds father to child who already exist.
* param 1: to - someone to add father to.
* return value: a reference to the Tree object.
*/
Tree& Tree::addFather(string to, string name){
    node *son = search(to,this->root);
    if(son == NULL){
        throw personNotFoundException;
    }else{
        if(son->father == NULL){
            node *f = new node(name);
            son->father = f;
        }else{
            throw alreadyExistException;
        }
    }
    return *this;
}

/*
* addMother - a function which adds mother to child who already exist.
* param 1: to - someone to add mother to.
* return value: a reference to the Tree object.
*/
Tree& Tree::addMother(string to, string name){
    node *son = search(to,this->root);
    if(son == NULL){
        throw personNotFoundException;
    }else{
        if(son->mother == NULL){
            node *m = new node(name);
            son->mother = m;
        }else{
            throw alreadyExistException;
        }
    }
    return *this;
}

/*
* Helper function of display().
* printPreOrder - prints Tree preorder and writes relevant relation info.
* param 1: child - a child node of given father/mother node.
* param 2: next - given father/mother.
* param 3: pos - is father or mother.
*/
void printPreOrder(node *child, node *next, position pos){
    if(next == NULL)
        return;
        string relationType = pos == father_pos ? "father" : "mother";
        if(child == NULL){
            cout << next->name << endl;
        }else {
            cout << child->name+"'s "+relationType + ": " + next->name << endl;
        }
        printPreOrder(next,next->father,father_pos);
        printPreOrder(next,next->mother,mother_pos);
}

/*
* display - prints the tree.
*/
void Tree::display(){
    printPreOrder(NULL,this->root,father_pos);
}

/*
* relation - get relation information (father/mother...granfather..).
* param 1: who - a name of person.
* return value: string which represents a relation (Example: "me" or "father" ..).
*/
string Tree::relation(string who){
    relation_data data = search(who,0,father_pos,this->root);
    string to_return = "";
    if(data.valid) {
        to_return = relationDataToString(data);
    }else{
        to_return = "unrelated";
    }
    return to_return;
}

/*
* find - search person's name by given relation type (Example: returns the name of root if given relation is "me").
* param 1: relation - a relation type (Example: "grandfather").
* return value: string (name).
*/
string Tree::find(string relation){
    string to_return = "";
    vector<string> relationVector = split(relation,'-');
    if(relationVector.empty()){
        throw badRelationException;
    }else{
        relation_data data = getRelationData(relationVector);
        if(data.valid){
            node *found = Tree::search(data,0,self,this->root);
            if(found != NULL){
                to_return = found->name;
            }else{
                throw relationNotFoundException;
            }
        } else{
            throw badRelationException;
        }
    }
    return to_return;
}

/*
* remove - removes a person and all lower depth relations (of the specified person).
* param 1: name - person's name.
*/
void Tree::remove(string name){
    node *child = getChild(name, false, this->root);
    bool deleteFather = false;
    if(child != NULL) {
        if(child->father != NULL) {
            if (child->father->name.compare(name) == 0)
                deleteFather = true;
        }
        freeTree(this->search(name, this->root));
        deleteFather ? child->father = NULL : child->mother = NULL;
    } else{
        throw deleteRootException;
    }
}
