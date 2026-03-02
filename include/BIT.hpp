#ifndef BIT_HPP
#define BIT_HPP
template<int T>
struct BIT{
    int c[T];
    void clear(){
        memset(c,0,sizeof(c));
        /*
        for(int i=1;i<=n;i++){
            c[i]=0;
        }
        */
    }
    inline int lowbit(int x){
        return x&-x;
    }
    void add(int id,int x){
        if(id==0){
            c[0]+=x;
            return ;
        }
        for(;id<=T/*n*/;id+=lowbit(id)){
            c[id]+=x;
        }
        return ;
    }
    int sum(int id){
        int res=c[0];
        for(;id;id-=lowbit(id)){
            res+=c[id];
        }
        return res;
    }
    int ans(int l,int r){
        return sum(r)-sum(l-1);
    }
};
#endif