#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector> 
#include <algorithm>
#include <iomanip>

using namespace std;

int frequency_table[1000]={0};

struct FPNode
{
    int name;
    int count;
    FPNode *parent = NULL;
    FPNode *link = NULL;
    vector<FPNode *> children;
};

struct Header_table
{
    int name;
    int count;
    FPNode *link = NULL;
};

bool Compare(int a, int b) {
    return (frequency_table[a] > frequency_table[b]);
}

vector<Header_table *> construct_Header_table(int largest_item, float min_support)
{
    int *item = new int[largest_item+1];
    for(int i = 0; i <= largest_item; i++)
        item[i] = i;
    sort(item,item+largest_item+1,Compare);
    /*for(int i = 0;i <= largest_item; i++)
        cout << item[i] << " ";
    cout << endl;*/
    vector<Header_table *> header_table;
    for(int i = 0; i <= largest_item; i++)
    {
        if(frequency_table[item[i]] < min_support)
            break;
        Header_table *tmp = new Header_table();
        tmp->name = item[i];
        tmp->count = frequency_table[item[i]];
        tmp->link = NULL;
        header_table.push_back(tmp);
    }
    return header_table;
}

FPNode *construct_FPTree(vector<Header_table *> *header_table, vector<vector<int>> data)
{
    FPNode *root = new FPNode();
    root->name = -1;
    root->count = 0;
    /*root->parent = NULL;
    root->link = NULL;
    cout << 123 << endl;*/
    for(int i = 0; i < data.size(); i++)
    {
        FPNode *cur_Node = root;
        for(int j = 0; j < data.at(i).size(); j++)
        {
            bool check = false;
            for(int k = 0; k < cur_Node->children.size(); k++)
                if(data.at(i).at(j) == cur_Node->children.at(k)->name)
                {
                    check = true;
                    cur_Node->children.at(k)->count ++;
                    cur_Node = cur_Node->children.at(k);
                    break;
                }
            if(!check)
            {
                FPNode *tmp = new FPNode();
                tmp->name = data.at(i).at(j);
                tmp->count = 1;
                tmp->parent = cur_Node;
                cur_Node->children.push_back(tmp);
                cur_Node = tmp;
                for(int k = 0; k < header_table->size(); k++)
                {
                    if(header_table->at(k)->name == data.at(i).at(j))
                    {
                        if(header_table->at(k)->link == NULL)
                            header_table->at(k)->link = cur_Node;
                        else
                        {
                            tmp = header_table->at(k)->link;
                            while(tmp->link != NULL)
                                tmp = tmp->link;
                            tmp->link = cur_Node;
                        }
                        break;
                    }
                }
            }
        }
    }
    return root;
}

vector<vector<int>> construct_ConditionalFPTree(FPNode *head) {
    vector<vector<int>> data;
    while (head != NULL) {
        FPNode *cur_Node = head;
        vector<int> tx;
        while (cur_Node->parent->name != -1) {
            tx.push_back(cur_Node->parent->name);
            cur_Node = cur_Node->parent;
        }
        if (tx.size() == 0) {
            head = head->link;
            continue;
        }
        for (int i = 0; i < head->count; ++i) {
            data.push_back(tx);
        }
        head = head->link;
    }
    return data;
}

void Mining_FPTree(vector<Header_table *> *header_table, float min_support,vector<int> *prefix,vector<vector<int>> *frequent_pattern,int largest_item)
{
    for(int i = header_table->size() - 1; i >= 0; i--)
    {
        vector<int> tmp;
        for(int j = 0;j < prefix->size(); j++)
            tmp.push_back(prefix->at(j));
        tmp.push_back(header_table->at(i)->name);
        sort(tmp.begin(),tmp.end());
        frequent_pattern->push_back(tmp);
        //cout << "link : " << header_table->at(i)->link->name << endl;
        vector<vector<int>> data = construct_ConditionalFPTree(header_table->at(i)->link);
        int new_largest_item = -1;
        /*cout << "old\n";
        for(int j = 0; j <= largest_item; j++)
            cout << j << " : " << frequency_table[j] << endl;*/
        for(int j = 0; j <= largest_item; j++)
            frequency_table[j] = 0;
        //cout <<"size : " << data->size() << endl; 
        for(int j = 0; j < data.size(); j++)
            for(int k = 0; k < data.at(j).size(); k++)
            {
                frequency_table[data.at(j).at(k)]++;
                if(data.at(j).at(k) > new_largest_item)
                    new_largest_item = data.at(j).at(k);
            }
        /*cout << "new\n";
        for(int j = 0; j <= new_largest_item; j++)
            cout << j << " : " << frequency_table[j] << endl;*/
        vector<Header_table *> new_header_table = construct_Header_table(new_largest_item,min_support);
        //cout << new_header_table.size() << endl;
        if(new_header_table.size() != 0)
        {
            FPNode *ConditionalFPTree = construct_FPTree(&new_header_table,data);
            Mining_FPTree(&new_header_table,min_support,&tmp,frequent_pattern,new_largest_item);
        }
    }
}


int main(int argc, char *argv[])
{
    float min_support = stof(argv[1]);
    int transaction_count = 0;
    int largest_item = -1;
    vector<vector<int>> data;
    int num;
    fstream input,output;
    string str,item;
    input.open(argv[2], ios::in);
    while(getline(input,str))
    {
        vector<int> tmp;
        while(1)
        {
            if(str.find(",") == -1)
            {
                num = stoi(str);
                tmp.push_back(num);
                frequency_table[num]++;
                if(num > largest_item)
                    largest_item = num;
                break;
            }
            item = str.substr(0,str.find(","));
            str = str.substr(str.find(",")+1,str.length());
            num = stoi(item);
            tmp.push_back(num);
            frequency_table[num]++;
            if(num > largest_item)
                largest_item = num;
        }
        data.push_back(tmp);
        transaction_count ++;
    }
    /*for(int i = 0; i <= largest_item; i++)
        cout << i << " : " << frequency_table[i] << endl;*/
    min_support = min_support * transaction_count;
    //cout << "min_support : " << min_support << endl;
    for(vector<vector<int>>::iterator tx = data.begin(); tx != data.end();)
    {
        for(vector<int>::iterator it = tx->begin(); it != tx->end();)
            if(frequency_table[*it] < min_support)
                tx->erase(it);
            else
                it++;
        if(tx->empty())
            data.erase(tx);
        else{
            sort(tx->begin(),tx->end(),Compare);
            tx++;
        }
    }
    /*for(int i = 0; i < data.size(); i++)
    {
        for(int j = 0; j < data[i].size(); j++)
            cout << data[i][j] << " ";
        cout << endl;
    }*/
    vector<Header_table *> header_table = construct_Header_table(largest_item,min_support);
    /*for(int i = 0; i < header_table.size(); i++)
        cout << header_table[i]->name << " " << header_table[i]->count << " " << endl;*/
    FPNode *TreeRoot = construct_FPTree(&header_table,data);
    /*for(int i = 0; i < TreeRoot->children.size(); i++)
        cout << TreeRoot->children[i]->name << " " << TreeRoot->children[i]->count << " " << endl;*/
    vector<vector<int>> frequent_pattern;
    vector<int> prefix;
    Mining_FPTree(&header_table,min_support,&prefix,&frequent_pattern,largest_item);

    /*for(int i = 0; i < frequent_pattern.size(); i++)
    {
        for(int j = 0; j < frequent_pattern[i].size(); j++)
            cout << frequent_pattern[i][j] << " ";
        cout << endl;
    }*/

    output.open(argv[3], ios::out);
    for(int i = 0; i < frequent_pattern.size(); i++)
    {
        float pattern_count = 0;
        for(int k = 0; k < data.size(); k++){
            int j = 0;
            for(int l = 0; l < data[k].size(); l++)
            {
                if(data[k][l] == frequent_pattern[i][j])
                {
                    j++;
                    l = -1;
                }
                if(j == frequent_pattern[i].size())
                {
                    pattern_count++;
                    break;
                }
            }
        }
        float ans_sup = pattern_count / transaction_count;
        for(int j = 0; j < frequent_pattern[i].size(); j++)
        {
            if(j == frequent_pattern[i].size() - 1)
                output << frequent_pattern[i][j];
            else
                output << frequent_pattern[i][j] << ",";
        }
        output << ":" << fixed << setprecision(4) << ans_sup << endl;
    }
}