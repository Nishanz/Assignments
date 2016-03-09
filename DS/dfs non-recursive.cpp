#include <bits/stdc++.h>
#define pb push_back
#define mp make_pair
#define f first
#define s second
#define endl "\n"
using namespace std;

vector< vector <pair<int,int> > > v(10000);
vector<bool> visited(10000,1);
vector<int> parent(10000);
vector<int> depth(10000,-1);
stack<int> mystack;

int a,b,w,n,flag=0,noc=0,m,non=0; //noc->Number of components non->no of nodes 

void insertg(int a, int b , int w);
void dfshead();
void dfs(int n);
int depthfindhead();
int depthfind(int n);

int main() 
{
	cin>>n>>m; //n->vertices(1->n) m->edges
	for(int i=1;i<=n;i++)visited[i]=false;
	for(int i=0;i<m;i++)
	{
	cin>>a>>b;
	insertg(a,b,1);
	}
	dfshead();
	cout<<noc<<endl;
	return 0;
} 

void insertg(int a, int b , int w)
{
	v[a].pb(mp(b,w));
	v[b].pb(mp(a,w)); 
	visited[a]=false;    
	visited[b]=false;     
}
void dfshead()
{
    for(int x=1;x<=n;x++)
      if(visited[x]==false){noc++;depth[x]=0;dfs(x);}
    depthfindhead();
}
void dfs(int n)
{
	int node;
	mystack.push(n);
	parent[n]=NULL;
	while(!mystack.empty())
	{
		node=mystack.top();
		mystack.pop();
		if(visited[node])continue;
		visited[node]=true;
		non++;
		//cout<<node<<endl;
		//what to do is here
		for(unsigned int i=0; i<v[node].size(); i++)
		{
			mystack.push(v[node][i].f);
			if(!visited[v[node][i].f])
			{
			parent[v[node][i].f]=node;
			//cout<<" parent of "<<v[node][i].f<<" is "<<node<<endl;
			}
		}
		
	}
	//cout<<endl;
	//cout<<non<<endl;
}
int depthfindhead()
{
	for(int x=1;x<=n;x++)
      if(depth[x]==-1){depthfind(x);}
      
    //for(int x=1;x<=n;x++)
    	//cout<<"Depth of "<<x<<" is :"<<depth[x]<<endl;
}
int depthfind(int n)
{
	if(depth[n]!=-1)return depth[n];
	else
	depth[n]=depthfind(parent[n])+1;
	
}



