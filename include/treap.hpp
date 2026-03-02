#ifndef TREAP_H
#define TREAP_H
#include<random>
#include<time.h>
template<typename T,int N>
struct treap{
    std::mt19937 rd;
    int root,idx;
    struct node{
        int l,r;
        T val;
        int key;
        int size;
    }tr[N];
	int tag[N];
	bool is_tag=0;
    treap(){
        srand(time(0));
        rd.seed(rand()^time(0));
    }
    int newnode(T v){
        tr[++idx].key=rd();
        tr[idx].size=1;
        tr[idx].val=v;
        return idx;
    }
    int copynode(int id){
    	tr[++idx]=tr[id];
    	return idx;
	}
    void pushup(int id){
        tr[id].size=tr[tr[id].l].size+tr[tr[id].r].size+1;
        return ;
    }
    void split_by_val(int id,T v,int &x,int &y){//by val
        if(!id){
            x=y=0;
            return ;
        }
        if(is_tag)pushdown(id); 
        if(tr[id].val<=v){
            x=id;
            split_by_val(tr[x].r,v,tr[x].r,y);
        }else{
            y=id;
            split_by_val(tr[y].l,v,x,tr[y].l);
        }
        pushup(id);
        return ;
    }
    void split_by_rk(int id,int rk,int &x,int &y){//by rk
    	if(!id){
    		x=y=0;
    		return ;
		}
		if(is_tag)pushdown(id);
		if(tr[tr[id].l].size<rk){
			x=id;
			split_by_rk(tr[x].r,rk-tr[tr[id].l].size-1,tr[x].r,y);
		}else{
			y=id;
			split_by_rk(tr[y].l,rk,x,tr[y].l);
		}
		pushup(id);
		return ;
	}
    int merge(int x,int y){
        if(!x or !y){
            return x+y;
        } 
    	if(is_tag)pushdown(x);
    	if(is_tag)pushdown(y); 
        if(tr[x].key<=tr[y].key){
            tr[x].r=merge(tr[x].r,y);
            pushup(x);
            return x;
        }else{
            tr[y].l=merge(x,tr[y].l);
            pushup(y);
            return y;
        }
    }
    void insert(T v){
        int a,b,c;
        c=newnode(v);
        split_by_val(root,v,a,b);
        root=merge(merge(a,c),b);
        return ;
    }
    void insert_next(T v){
    	int a;
		a=newnode(v);
		root=merge(root,a);
		return ; 
	}
    void del_one(T v){
        int a,b,c;
        split_by_val(root,v,a,b);
        split_by_val(a,v-1,a,c);
        c=merge(tr[c].l,tr[c].r);
        root=merge(merge(a,c),b);
        return ;
    }
    void del_all(T v){
        int a,b,c;
        split_by_val(root,v,a,b);
        split_by_val(a,v-1,a,c);
        root=merge(a,b);
        return ;    
    }
    int get_rank(T v){
        int x,y;
        split_by_val(root,v-1,x,y);
        int rk=tr[x].size+1;
        root=merge(x,y);
        return rk;
    }
    int get_rank_k_id(int id,int k){
        if(k<=tr[tr[id].l].size){
            return get_rank_k_id(tr[id].l,k);
        }else if(k==tr[tr[id].l].size+1){
            return id;
        }else{
            return get_rank_k_id(tr[id].r,k-tr[tr[id].l].size-1);
        }
    }
    T get_rank_k(int k){
        return tr[get_rank_k_id(root,k)].val;
    }
    T get_rank_k(int id,int k){
        return tr[get_rank_k_id(id,k)].val;
    }
    int get_pre_increasing_id(T v){
        int x,y;
        split_by_val(root,v-1,x,y);
        int res=get_rank_k_id(x,tr[x].size);
        root=merge(x,y);
        return res;
    }
    T get_pre_increasing(T v){
        return tr[get_pre_increasing_id(v)].val;
    }
    int get_pre_nondecreasing_id(T v){
        int x,y,z;
        split_by_val(root,v,x,z);
        split_by_val(x,v-1,x,y);
        if(tr[y].size>1){
            root=merge(merge(x,y),z);
            return y;
        }else{
            int res=get_rank_k_id(x,tr[x].size);
            root=merge(merge(x,y),z);
            return res;
        }
    }
    T get_pre_nondecreasing(T v){
        return tr[get_pre_nondecreasing_id(v)].val;
    }

    int get_suc_increasing_id(T v){
        int x,y;
        split_by_val(root,v,x,y);
        int res=get_rank_k_id(y,1);
        root=merge(x,y);
        return res;
    }
    T get_suc_increasing(T v){
        return tr[get_suc_increasing_id(v)].val;
    }
    int get_suc_nondecreasing_id(T v){
        int x,y,z;
        split_by_val(root,v,x,z);
        split_by_val(x,v-1,x,y);
        if(tr[y].size>1){
            root=merge(merge(x,y),z);
            return y;
        }else{
            int res=get_rank_k_id(z,1);
            root=merge(merge(x,y),z);
            return res;
        }
    }
    T get_suc_nondecreasing(T v){
        return tr[get_suc_nondecreasing_id(v)].val;
    }
    void pushdown(int id);
    void add_tag(int l,int r);
    void dfs(int id);
};
//reverse tag
#ifdef REVERSE_TREAP
template<typename T,int N>
void treap<T,N>::pushdown(int id){
    if(!id or !tag[id]) return;
    tag[id]=0;
    swap(tr[id].l, tr[id].r);
    if(tr[id].l)tag[tr[id].l]^=1;
    if(tr[id].r)tag[tr[id].r]^=1;
    return ;
}
template<typename T,int N>
void treap<T,N>::add_tag(int l,int r){
	int lr,mr,rr;
	split_by_rk(root,r,lr,rr);
	split_by_rk(lr,l-1,lr,mr);
	tag[mr]^=1;
	root=merge(merge(lr,mr),rr);
	return ;
}
#endif
template<typename T,int N>
void treap<T,N>::dfs(int id){
	if(!id)return ;
	pushdown(id);
	if(tr[id].l)dfs(tr[id].l);
	cout<<tr[id].val<<' ';
	if(tr[id].r)dfs(tr[id].r);
	return ;
}
#endif
