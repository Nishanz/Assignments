#include <bits/stdc++.h>
#define ll long long int
using namespace std;

typedef struct node
{
	node* pnt;
	node* p[26];
	bool is[26];
}node;

node *start = (struct node *)malloc(sizeof(node));

void insertnodetrie(string s)
{
	node *it = (struct node *)malloc(sizeof(node));
	it=start;
	for(int i=0;i<s.length();i++)
	{
		if(!it->is[(int)(s[i]-'a')])
		{
			cout<<"Not found. Inserting .."<<s[i]<<endl;
			it->is[(int)(s[i]-'a')]=true;
			node *cont = (struct node *)malloc(sizeof(node));
			it->p[(int)(s[i]-'a')]=cont;
			cont->pnt=it;
			it=cont;
		}
		else
		{
			cout<<s[i]<<" Found! "<<endl;
			it=it->p[(int)(s[i]-'a')];
		}		
	}
}
void search(string s)
{
	node* it;
	it = start;
	for(int i=0;i<s.length();i++)
	{
		if(!it->is[(int)(s[i]-'a')])break;
		if(i==s.length()-1){cout<<"Found"<<endl; return;}
		it=it->p[(int)(s[i]-'a')];
	}
	cout<<"Not found"<<endl;
}

int main() {
	ll n,m;
	string s;
	cin>>n;
	for(int i=0;i<n;i++)
	{
		cin>>s;
		insertnodetrie(s);
	}
	cin>>m;
	for(int i=0;i<m;i++)
	{
		cin>>s;
		search(s);
	}
	
	return 0;
} 
