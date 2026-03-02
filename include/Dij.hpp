#ifndef DIJ
#define DIJ
#include<vector>
#include<queue>
template<int N>
struct dij{
    struct node{
        int to,dst;
        bool operator<(const node &nd)const{
            return dst>nd.dst;
        }
    };
    int n,m;
    int dst[N];
    bool vis[N];
    std::vector<node> a[N];
    void init(int _n,int _m){
        n=_n;m=_m;
        return ;
    }
    void add(int u,int v,int w){
        a[u].push_back({v,w});
        a[v].push_back({u,w});
        return ;
    }
    int dist(int s,int t){
        memset(dst,0x7f,sizeof(dst));
        memset(vis,0,sizeof(vis));
        dst[s]=0;
        std::priority_queue<node> q;
        q.push({s,0});
        while(!q.empty()){
            node d=q.top();q.pop();
            int id=d.to;
            vis[id]=1;
            for(auto i:a[id]){
                if(i.dst+d.dst<dst[i.to]){
                    dst[i.to]=i.dst+d.dst;
                    if(!vis[i.to]){
                        q.push({i.to,dst[i.to]});
                    }
                }
            }
        }
        return dst[t];
    }
};
#endif