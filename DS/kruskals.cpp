//By aMeYa PrAbHu
#include <bits/stdc++.h>
#define ll long long int
#define mp make_pair
#define pb push_back
#define rep(n) for(ll i=0;i<n;i++)
using namespace std;

vector<pair<ll,pair<ll,ll> > > ve;
bool visited[100000];
vector< vector<ll> > mst(100000);
ll w,n,a,b;

int main()
{
  ios_base::sync_with_stdio(false);
  cin.tie(NULL);
  cin>>n;
  w=0;
  rep(n)
  {
  	cin>>w>>a>>b;
  	ve.pb(mp(w,mp(a,b)));
  	visited[a]=false;
  	visited[b]=false;
  }
  sort(ve.begin(),ve.end());
  rep(n)
  {
  	if(
  	{
  		visited[(ve[i].second).second]=1;
  		visited[(ve[i].second).first]=1;
  		w+=ve[i].first;
  		mst[(ve[i].second).first].pb((ve[i].second).second);
  		mst[(ve[i].second).second].pb((ve[i].second).first);
  	}
  }
  rep(n)
  {
  	for(int j=0;j<mst[i].size();j++)
  	{
  		cout<<i<<" "<< mst[i][j]<<" ";
  	}
  	cout<<endl;
  }
  return 0;
}

  
