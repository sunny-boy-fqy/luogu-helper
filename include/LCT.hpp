#ifndef LCT_HPP
#define LCT_HPP
#include<algorithm>
#include<iostream>

// #define MAX_FOR_LCT
// #define MIN_FOR_LCT

template<int N,int mod=1000000007>
struct LCT{
    #define mul(x,c) x=1ll*x*c%mod
    #define add(x,c) x=(x+c+mod)%mod
    #define exist if(!x)return 
    #define ls ch[x][0]
    #define rs ch[x][1]
    
    int cnt=0;
    int fa[N];
    int ch[N][2];
    int val[N];
    int sum[N];
    
    #ifdef MAX_FOR_LCT
    int mx[N];
    int mx_id[N]; 
    #endif
    
    #ifdef MIN_FOR_LCT
    int mn[N];
    int mn_id[N]; 
    #endif
    
    int tag_add[N],tag_mul[N];
    bool tag_rev[N];
    
    int sz[N]; 
    int si[N];
    int all_sz[N];

    int newnode(int v){
        val[++cnt]=v;
        sum[cnt]=v;
        
        #ifdef MAX_FOR_LCT
        mx[cnt]=v;
        mx_id[cnt]=cnt; 
        #endif
        
        #ifdef MIN_FOR_LCT
        mn[cnt]=v;
        mn_id[cnt]=cnt; 
        #endif
        
        fa[cnt]=ch[cnt][0]=ch[cnt][1]=0;
        tag_add[cnt]=tag_rev[cnt]=0;
        tag_mul[cnt]=1;
        
        sz[cnt]=1;
        si[cnt]=0;    
        all_sz[cnt]=1; 
        return cnt;
    }
    inline void delnode(int x){
        splay(x);
        fa[ls]=fa[rs]=0;
        return ;
    }
    inline void pushadd(int x,int c){
        exist;
        add(sum[x],1ll*c*sz[x]%mod);
        add(val[x],c);
        add(tag_add[x],c);

        #ifdef MAX_FOR_LCT
        add(mx[x], c);
        #endif

        #ifdef MIN_FOR_LCT
        add(mn[x], c);
        #endif
        
        return ;
    }

    inline void pushmul(int x,int c){
        exist;
        mul(sum[x],c);
        mul(val[x],c);
        mul(tag_mul[x],c);
        mul(tag_add[x],c);

        #ifdef MAX_FOR_LCT
        mul(mx[x], c);
        #endif

        #ifdef MIN_FOR_LCT
        mul(mn[x], c);
        #endif

        return ;
    }

    inline void pushrev(int x){
        exist;
        std::swap(ls,rs);
        tag_rev[x]^=1;
        return ;
    }

    inline void pushdown(int x){
        exist;
        if(tag_mul[x]!=1){
            pushmul(ls,tag_mul[x]);
            pushmul(rs,tag_mul[x]);
            tag_mul[x]=1;
        }
        if(tag_add[x]){
            pushadd(ls,tag_add[x]);
            pushadd(rs,tag_add[x]);
            tag_add[x]=0;
        }
        if(tag_rev[x]){
            pushrev(ls);
            pushrev(rs);
            tag_rev[x]=0;
        }
        return ;
    }
    inline void pushup_max_min(int x){
        #ifdef MAX_FOR_LCT
        mx[x] = val[x];
        mx_id[x] = x;
        if(ls && mx[ls] > mx[x]) { 
            mx[x] = mx[ls];
            mx_id[x] = mx_id[ls];
        }
        if(rs && mx[rs] > mx[x]) { 
            mx[x] = mx[rs];
            mx_id[x] = mx_id[rs];
        }
        #endif

        #ifdef MIN_FOR_LCT
        mn[x] = val[x];
        mn_id[x] = x;
        if(ls && mn[ls] < mn[x]) { 
            mn[x] = mn[ls];
            mn_id[x] = mn_id[ls];
        }
        if(rs && mn[rs] < mn[x]) { 
            mn[x] = mn[rs];
            mn_id[x] = mn_id[rs];
        }
        #endif
    }
    inline void pushup_val(int x);

    inline void pushup_sz(int x){
        sz[x] = sz[ls] + sz[rs] + 1;
        all_sz[x] = all_sz[ls] + all_sz[rs] + 1 + si[x];
        return ;
    }
    inline void pushup(int x){
        pushup_sz(x);
        pushup_val(x);
        pushup_max_min(x);
        return ;
    }
    void pushall(int x){
        if(!is_root(x))pushall(fa[x]);
        pushdown(x);
        return ;
    }
    bool is_root(int x){
        return fa[x]==0||(ch[fa[x]][0]!=x&&ch[fa[x]][1]!=x);
    }
    bool dir(int x){
        return x==ch[fa[x]][1];
    }
    inline void rotate(int x){
        exist;
        if(is_root(x))return ;
        int y=fa[x],z=fa[y];
        int d=dir(x),fd=dir(y);
        if(!is_root(y))ch[z][fd]=x;
        ch[y][d]=ch[x][!d];
        if(ch[y][d])fa[ch[y][d]]=y;
        ch[x][!d]=y;
        fa[y]=x;
        fa[x]=z;
        pushup(y);
        pushup(x);
        return ;
    }
    inline void splay(int x){
        pushall(x);
        while(!is_root(x)){
            int y=fa[x],z=fa[y];
            if(!is_root(y)){
                if(dir(x)==dir(y)){
                    rotate(y);
                }else{
                    rotate(x);
                }
            }
            rotate(x);
        }
        return ;
    }
    
    inline int access(int x){
        int p=0;
        while(x){
            splay(x);
            if(rs) si[x] += all_sz[rs];
            if(p) si[x] -= all_sz[p];
            
            ch[x][1]=p;
            pushup(x);
            p=x;
            x=fa[x];
        }
        return p;
    }
    inline void makeroot(int x){
        access(x);
        splay(x);
        pushrev(x);
        return ;
    }
    inline void split(int x,int y){//y is root
        makeroot(x);
        access(y);
        splay(y);
        return ;
    }
    inline int getroot(int x){
        access(x);
        splay(x);
        while(ls){
            pushdown(x);
            x=ls;
        }
        splay(x);
        return x;
    }
    bool is_connected(int x,int y){
        return getroot(x)==getroot(y);
    }
    
    inline bool link(int x,int y){//y is parent
        makeroot(x);
        if(getroot(y)==x)return 0;
        access(y); 
        splay(y);
        
        fa[x]=y;
        si[y] += all_sz[x]; // x 成为 y 的虚儿子
        pushup(y);
        return 1;
    }
    
    inline bool cut(int x,int y){
        if(!is_connected(x,y))return 0;
        split(x,y);
        if(fa[x]!=y||rs)return 0;
        fa[x]=ch[y][0]=0;
        pushup(y);
        return 1;
    }
    inline void modify(int x,int v){
        splay(x);
        val[x]=v;
        pushup(x);
        return ;
    }
    inline int query_sum(int x,int y){
        if(!is_connected(x,y))return 0; 
        split(x,y);
        return sum[y];
    }

    #ifdef MAX_FOR_LCT
    inline int query_max(int x,int y){
        if(!is_connected(x,y))return 0;
        split(x,y);
        return mx[y];
    }
    inline int query_max_id(int x, int y){
        if(!is_connected(x,y)) return 0;
        split(x, y);
        return mx_id[y];
    }
    #endif

    #ifdef MIN_FOR_LCT
    inline int query_min(int x, int y){
        if(!is_connected(x,y))return 0;
        split(x, y);
        return mn[y];
    }
    inline int query_min_id(int x, int y){
        if(!is_connected(x,y)) return 0;
        split(x, y);
        return mn_id[y];
    }
    #endif

    inline int query_subtree_sz(int root, int u) {
        makeroot(root); 
        access(u); 
        splay(u);
        return si[u] + 1;
    }

    void prT(int x){
        exist;
        prT(ls);
        std::cerr<<x<<"(val:"<<val[x]<<", sz:"<<sz[x]<<", all_sz:"<<all_sz[x]<<") ";
        prT(rs);
        return ;
    }
    inline void prT_splay(int x){
        exist;
        x=getroot(x);
        std::cerr<<"root:"<<x<<'\n';
        prT(x);
        std::cerr<<'\n';
        return ;
    }
    inline void add_range(int x,int y,int v){
        split(x,y);
        pushadd(y,v);
        return ;
    }
    inline void mul_range(int x,int y,int v){
        split(x,y);
        pushmul(y,v);
        return ;
    }
};
#endif