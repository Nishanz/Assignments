
//By aMeYa PrAbHu
#include <bits/stdc++.h>
using namespace std;

#define ll long long int
typedef pair<ll,ll> pii;
typedef vector<ll> vi;
typedef vector< pii > vpii;

#define mp make_pair
#define pb push_back
#define mt make_tuple
#define F first
#define S second
#define sz(x) (ll)(x.size())
#define endl '\n'
#define fa(i,x,y) for(ll i=x; i<y; ++i)
#define fd(i,x,y) for(ll i=x-1; i>=y; --i)
#define rep(i,x) for(ll i=0; i<x; ++i)
#define in1(x) cin>>x
#define in2(x,y) cin>>x>>y
#define in3(x,y,z) cin>>x>>y>>z
#define ina(x,n); rep(i,n)cin>>x[i];
#define outs1(x) cout<<x<<" "
#define outs2(x,y) cout<<x<<" "<<y<<" "
#define outs3(x,y,z) cout<<x<<" "<<y<<" "<<z<<" "
#define outas(x,n); rep(i,n)cout<<x[i]<<" ";
#define outn(a) cout<<a<<endl;
#define outan(a,n); rep(i,n)cout<<a[i]<<endl;
#define all(x) x.begin(),x.end()
#define ___ ios_base::sync_with_stdio(false);cin.tie(NULL);
#define mset(x,y) memset(x,y,sizeof(x))
#define sortv(x) sort(x.begin(),x.end())
#define sorta(x,y) sort(x,x+y);
#define tst()  int t; cin>>t; while(t--)
#define tr(c,i) for(typeof((c).begin() i = (c).begin(); i != (c).end(); i++) 
#define present(c,x) ((c).find(x) != (c).end())
#define INF 1e15
#define N 100100   

inline ll pwr(ll x,ll y)
{
    ll z=1;
    for(;y;y>>=1,x=x*x)if(y&1)z=z*x;
    return z;
}

ll a[1000000],k,seg[1000100],n,m,p,val,w;

ll opans_minmax(ll c,ll b, ll q)
{
	if(q&01)
	{
		return c|b;
	}
	else
	{
		return c^b;
	}
}

ll opans(ll a, ll b)
{
        return a whatever b ;
        //remove the b from the 
}
// node is 1 (i.e start ) at the initialization of build_tree .. always build_tree(1,1,n,b);
void build_tree( ll node ,ll l ,ll r ,ll minmax ) 
{
	if (l == r) 
	{
		seg[node] = a[l];
	} 
	else 
	{
		ll mid = (l+r)/2;
		minmax--;
		 build_tree(node<<1, l, mid, minmax);
		 build_tree((node<<1)+1, mid + 1, r, minmax);
		 minmax++;
		seg[node] = opans(seg[node<<1] , seg[(node<<1) + 1], minmax);
	}
}

void update( ll node ,ll l ,ll r ,ll minmax ) 
{
	if (l == r) 
	{
		seg[node] = a[l];
	} 
	else 
	{
		ll mid = (l+r)/2;
		minmax--;
		if(p<=mid)
		{
		        update(node<<1, l, mid, minmax);
		}
		else
		{ 
		        update((node<<1)+1, mid + 1, r, minmax);
		}
		 minmax++;
		seg[node] = opans(seg[node<<1] , seg[(node<<1) + 1], minmax);
	}
}

ll query (ll node, ll ql, ll qr, ll cl, ll cr, ll minmax) 
{
	if(cl >= qr || cr <= ql) 
	{
		return 0; // think abut it
	} 
	else if(cl >= ql && cr <= qr)
	{
		return seg[node];
	} 
	else 
	{
	        minmax--;
		ll mid = (cl + cr)/2;
		return opans(query(1<<node, ql, qr, cl, mid,minmax), query(1<<node + 1,  ql, qr, mid, cr,minmax),minmax+1);
	}
}
int main()
{
  ios_base::sync_with_stdio(false);
  cin.tie(NULL);
  ll n;
  cin>>n;
  build_segtree(n);
  disptree(n);
  return 0;
}








 
