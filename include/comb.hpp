#ifndef COMB_HPP
#define COMB_HPP
int pw(int a,int b,int p){
	if(!a)return 0;
	int res=1;
	while(b>0){
		if(b&1)res=1ll*res*a%p;
		a=1ll*a*a%p;
		b>>=1; 
	}
	return res;
} 
int inv(int a,int p){
	return pw(a,p-2,p);
}
template<int N,int mod=1000000007>
struct FAC{
private:
	int val[N];
public:
	FAC(){
		val[0]=1;
		for(int i=1;i<N;i++){
			val[i]=1ll*val[i-1]*i%mod;
		}
	}
	int operator[](const int p)const{
		return val[p];
	}
};
template<int N,int mod=1000000007>
struct INV_FAC{
private:
	int val[N];
public:
	INV_FAC(){
		int fac=1;
		for(int i=2;i<N;i++){
			fac=1ll*fac*i%mod;
		}
		val[N-1]=inv(fac,mod);
		for(int i=N-2;i>=1;i--){
			val[i]=1ll*val[i+1]*(i+1)%mod;
		}
		val[0]=1;
		return ;
	}
	int operator[](const int p)const{
		return val[p];
	}
};
template<int N,int mod=1000000007>
struct DOUBLE_FAC{
private:
	int val[N];
public:
	DOUBLE_FAC(){
		val[0]=val[1]=1;
		for(int i=2;i<N;i++){
			val[i]=1ll*val[i-2]*i%mod;
		}
	}
	int operator[](const int p)const{
		return val[p];
	}
};
template<int N,int mod=1000000007>
struct INV_DOUBLE_FAC{
private:
	int val[N];
public:
	// 修正构造函数名为 INV_DOUBLE_FAC
	INV_DOUBLE_FAC(){
		// 1. 先正向计算双阶乘 n!!
		val[0] = val[1] = 1;
		for(int i = 2; i < N; i++) {
			val[i] = 1ll * val[i-2] * i % mod;
		}

		// 2. 计算最后两项的逆元 (分别是奇数项和偶数项序列的末尾)
		// 假设外部已经定义了 inv(a, p) 函数
		if (N > 0) val[N-1] = inv(val[N-1], mod);
		if (N > 1) val[N-2] = inv(val[N-2], mod);

		// 3. 反向递推计算所有逆元
		// 公式: inv((i-2)!!) = inv(i!!) * i
		for(int i = N - 1; i >= 2; i--) {
			val[i-2] = 1ll * val[i] * i % mod;
		}
	}
	
	int operator[](const int p)const{
		return val[p];
	}
};
template<int N,int mod=1000000007>
struct INV{
private:
	int val[N];
public:
	INV(){
		val[0]=val[1]=1;
		for(int i=2;i<N;i++){
			val[i]=mod-1ll*mod/i*val[mod%i]%mod;
		} 
		return ;
	}
	int operator[](const int p)const{
		return val[p];
	}
};
template<int N,int mod=1000000007>
struct comb{
	FAC<N,mod> fac;
	INV_FAC<N,mod> inv_fac;
	int operator()(int n,int m)const{
		if(n<m)return 0;
		return 1ll*fac[n]*inv_fac[m]%mod*inv_fac[n-m]%mod;
	}
};
template<int N,int mod=1000000007>
struct arrange{
	FAC<N,mod> fac;
	INV_FAC<N,mod> inv_fac;
	int operator()(const int n,const int m)const{
		if(n<m)return 0;
		return 1ll*fac[n]*inv_fac[n-m]%mod;
	}
};
template<int N,int mod=1000000007>
struct derange{
private:
	int val[N];
public:
	derange(){
		val[0]=1;
		val[1]=0;
		for(int i=2;i<N;i++){
			val[i]=1ll*(i-1)*(val[i-1]+val[i-2])%mod;
		}
	}
	int operator[](const int p)const{
		return val[p];
	}
};
template<int N,int M,int mod=1000000007>
struct stirling_fir{
private:
    int val[N][M];
public:
    stirling_fir(){
        val[0][0]=1;
        for(int i=1;i<N;i++){
            for(int j=1;j<M;j++){
                val[i][j]=(val[i-1][j-1]+1ll*val[i-1][j]*(i-1)%mod)%mod;
            }
        }
    }
    int operator()(const int n,const int m){
        return val[n][m];
    }
};
template<int N, int mod=1000000007>
struct stirling_sec_general_term {
    comb<N, mod> C; 
    
    stirling_sec_general_term() {}

    int operator()(const int n, const int m) const {
        if (m < 0 || m > n) return 0;
        int res = 0, p = 1;
        for(int k = 0; k <= m; k++) {
            int term = 1ll * C(m, k) * pw(m - k, n, mod) % mod;
            if(p == 1) res = (res + term) % mod;
            else res = (res - term + mod) % mod;
            p *= -1;
        }
        res = 1ll * res * C.inv_fac[m] % mod;
        return res; 
    }
};

template<int N, int mod=1000000007>
struct stirling_sec_dp {
    int val[N][N];
    stirling_sec_dp() {
        memset(val, 0, sizeof(val));
        val[0][0] = 1;
        for(int i = 1; i < N; i++) {
            for(int j = 1; j <= i; j++) {
                val[i][j] = (val[i-1][j-1] + 1ll * j * val[i-1][j]) % mod;
            }
        }
    }

    int operator()(int n, int m) const {
        if (m < 0 || m > n) return 0;
        return val[n][m];
    }
};
const int CHECK_WHO=6000;
template<int N, int mod=1000000007>
struct stirling_sec : public std::conditional<
    (N > CHECK_WHO),                       
    stirling_sec_general_term<N, mod>, 
    stirling_sec_dp<N, mod>           
>::type {
    
    using Base = typename std::conditional<
        (N > CHECK_WHO),
        stirling_sec_general_term<N, mod>,
        stirling_sec_dp<N, mod>
    >::type;

    stirling_sec() : Base() {}
};
template<int N,int M,int mod=1000000007>
struct int_ptt{
private:
	int val[N][M];
public:
	int_ptt(){
		val[0][0]=1;
		for(int i=1;i<N;i++){
			val[i][1]=1;
			for(int j=2;j<=i and j<M;j++){
				val[i][j]=(val[i-1][j-1]+val[i-j][j])%mod;
			}
		}
	}
	int operator()(const int n,const int m)const{
		if(m>n)return 0;
		return val[n][m];
	} 
};
#endif 