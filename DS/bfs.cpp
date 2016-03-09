#include <bits/stdc++.h>
#define pb push_back
#define mp make_pair
#define f first
#define s second
using namespace std;

vector< vector <pair<int,int> > > v(10000);
vector<bool> visited(10000);
queue<int> mystack;

int a,b,w,n;
void insertg(int a, int b , int w)
{
	v[a].pb(mp(b,w));
	v[b].pb(mp(a,w));
	visited[a]=false;
	visited[b]=false;
	
}
//replace queue by stack and .front() by .top() to get BFS
void bfs(int n)
{
	int node;
	mystack.push(n);
	
	while(!mystack.empty())
	{
		node=mystack.front();
		mystack.pop();
		if(visited[node])continue;
		visited[node]=true;
		cout<<node<<endl;
		for(unsigned int i=0; i<v[node].size(); i++)
		{
			mystack.push(v[node][i].f);
		}
		
	}
}


int main() {
	
	cin>>n;
	for(int i=0;i<n;i++)
	{
	cin>>a>>b;
	insertg(a,b,1);
	}
	bfs(0);
	return 0;
} 
 
