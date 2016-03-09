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
#define sz(a) (ll)(a.size())
#define endl '\n'
#define fa(i,a,b) for(ll i=a; i<b; ++i)
#define fd(i,a,b) for(ll i=a; i>b; --i)
#define rep(i,a) for(ll i=0; i<a; ++i)
#define in1(a) cin>>a
#define in2(a,b) cin>>a>>b
#define in3(a,b,c) cin>>a>>b>>c
#define ina(a,n); rep(i,n)cin>>a[i];
#define outs1(a) cout<<a<<" "
#define outs2(a,b) cout<<a<<" "<<b<<" "
#define outs3(a,b,c) cout<<a<<" "<<b<<" "<<c<<" "
#define outas(a,n); rep(i,n)cout<<a[i]<<" ";
#define outn(a) cout<<a<<endl;
#define outan(a,n); rep(i,n)cout<<a[i]<<endl;
#define all(v) v.begin(),v.end()
#define mset(a,b) memset(a,b,sizeof(a))
#define sortv(a) sort(a.begin(),a.end())
#define sorta(a,n) sort(a,a+n);
#define tst()  int t; cin>>t; while(t--)
#define ___ ios_base::sync_with_stdio(false);cin.tie(NULL);
#define INF 9999999

ll n,d[1000][1000],p[1000][1000],m,a,b,c;

void adjmatinit()
{
	in1(n);
	rep(i,n)rep(j,n)d[i][j]=INF;
	rep(i,n)d[i][i]=0;
	rep(i,n)rep(j,n)p[i][j]=j;
	in1(m);
	rep(i,m){in3(a,b,c);d[a][b]=c;}
}
void floydwarshall()
{
        
	rep(k,n)rep(i,n)rep(j,n)if(d[i][k]+d[k][j]<d[i][j]){d[i][j]=d[i][k]+d[k][j];p[i][j]=k;}
	rep(i,n)rep(j,n)if(d[i][j]<INF){while (p[i][j] != p[i][p[i][j]]) p[i][j] = p[i][p[i][j]]; }
}
void printmat()
{
	rep(i,n){rep(j,n){if(d[i][j]!=9999999)outs1(d[i][j]);else outs1("INF");}cout<<endl;}
}

int main()
{
  
  adjmatinit();
  floydwarshall();
  printmat();
    
  return 0;
} 
