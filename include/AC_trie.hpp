#ifndef AC_TRIE_HPP
#define AC_TRIE_HPP
#include<bitset>
#include<iostream>
#include<queue>
#include<map>
#include<string.h>
const int S=27;
template<int T>
struct AC_trie{
    int cnt=1;
    struct node{
        int son[S];
        int fail;
        int flag;
        bool ok;
        int& operator[](int p){
            return son[p];
        }
    }trie[T];
    int ans[T],in[T],_in[T];
    std::map<int,int> Map;
    bool vis[T];
    AC_trie(){
        for(int i=0;i<S;i++){
            trie[0][i]=1;
        }
    }
    void add(std::string str,int num){
        //std::cerr<<str<<'\n';
        int u=1;
        int ok=0;
        for(int i=0,v;i<str.size();i++){
            v=(str[i]=='0' ? 26 : str[i]-'a');ok+=v==26;
            if(!trie[u][v])trie[u][v]=++cnt;
            //cout<<u<<"->"<<trie[u][v]<<' '<<str[i]<<'\n';
            u=trie[u][v];
            if(ok==2)trie[u].ok=1;
        }
        trie[u].flag++;
        return ;
    }
    void get_fail(){
        std::queue<int> q;
        q.push(1);
        int u,Fail;
        while(!q.empty()){
            u=q.front();q.pop();
            Fail=trie[u].fail;
            for(int i=0,v;i<S;i++){
                v=trie[u][i];
                if(v){
                    trie[v].fail=trie[Fail][i];
                    _in[trie[v].fail]++;
                    q.push(v);
                }else{
                    trie[u][i]=trie[Fail][i];
                }
            }
            if(trie[Fail].ok){
                trie[u].flag+=trie[Fail].flag;
            }
        }
        return ;
    }
    int topo(std::string str){
        memset(ans,0,sizeof(ans));
        //std::cerr<<str<<'\n';
        int u=1;
        for(int i=0;i<str.size();i++){
            u=trie[u][(str[i]=='0' ? 26 : str[i]-'a')];
            ans[u]+=trie[u].flag;
        }
        int Fail,res=0;
        std::queue<int> q;
        for(int i=1;i<=cnt;i++){
            in[i]=_in[i];
            if(!in[i]){
                q.push(i);
            }
        }
        while(!q.empty()){
            u=q.front();q.pop();
            Fail=trie[u].fail;
            in[Fail]--;
            ans[Fail]+=ans[u];
            if(in[Fail]==0)q.push(Fail);
        }
        for(int i=1;i<=cnt;i++){
            res+=ans[i];
        }
        memset(vis,0,sizeof(vis));
        return res;
    }
    int query(std::string str){
        //memset(ans,0,sizeof(ans));
        //std::cerr<<str<<'\n';
        int u=1,res=0;
        for(int i=0;i<str.size();i++){
            u=trie[u][(str[i]=='0' ? 26 : str[i]-'a')];
            int id=u;
            res+=trie[id].flag;
        }
        return res;
    }
};
#endif
