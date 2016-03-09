#include <bits/stdc++.h>
#define pb push_back
#define mp make_pair
#define f first
#define s second
#define gray 1
#define white 0
#define black 2
using namespace std;

vector< vector <pair<int,int> > > v(100000);
vector<bool> visited(100000);
int par[100000];

int a,b,w,n;
void insertg(int a, int b , int w)
{
	v[a].pb(mp(b,w));
	v[b].pb(mp(a,w));
	visited[a]=white;
	visited[b]=white;
	
}
void dfs(int n)
{
	visited[n]=gray;
	//Write what to do here.
	for(unsigned int i=0;i<v[n].size();i++)
	{
		if(visited[v[n][i].f]!=white)
		{
		    dfs(v[n][i].f);
		    par[v[n][i].f]=n;
		}
	}
	visited[n]=black;
	return;
}


int main() {
	
	cin>>n;
	for(int i=0;i<n;i++)
	{
	cin>>a>>b;
	insertg(a,b,1);
	}
	dfs(0);
	return 0;
} 
