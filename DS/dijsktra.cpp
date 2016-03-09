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
#define INF 99999999999
#define N 100100
ll a,b,w,n,m;

struct comp
{
	bool operator() (const pii &a, const pii &b)
	{
		return a.second > b.second;
	}
};

vector< vpii > v(100010);
vector<bool> visited(100010);

priority_queue< pii, vector< pii >, comp > q;
ll d[N];
ll sze,wgt,nxt,node;

void ioctrl();
void insertg(int a, int b , int w);
void dijsktra(ll s);
void out();


int main()
{
 ___
	ioctrl();
	return 0;
} 
  
void ioctrl()
{
	in2(n,m);
	rep(i,m)
	{
		in3(a,b,w);
		insertg(a,b,w);
	}
	dijsktra(1);
	out();
}

void insertg(int a, int b , int w)
{
	v[a].pb(mp(b,w));
	v[b].pb(mp(a,w));
	visited[a]=false;
	visited[b]=false;
}

void dijsktra(ll s)
{
	rep(i,n+1) d[i] = INF;
	d[s] = 0;
	q.push(mp(s,0));
	
	while(!q.empty())
	{
		node=q.top().first;
		q.pop();
		if(visited[node])continue;
		visited[node]=true;
		//cout<<node<<endl;
		sze=v[node].size();
		rep(i,sze)
		{
			 nxt = v[node][i].first;
			 wgt = v[node][i].second;
			  if(!visited[nxt] && d[node]+wgt < d[nxt]) 
			  {
			  	d[nxt] = d[node] + wgt;
				q.push(mp(nxt, d[nxt]));
			  }
		}
		
	}
}

void out()
{
	outas(d,n+1);
}


 
