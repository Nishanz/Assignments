ll par[N];

//1) if v is not the root, then par[v] shows the parent of v.  
//2) if v is the root, it shows negative number of nodes in group. 
// Hence, acting like a 2 in 1 array

ll root(ll v)
{
    return par[v] < 0 ? v : (par[v] = root(par[v]));
}

void merge(ll x,ll y)
{
    if((x = root(x)) == (y = root(y))     
        return ;
	if(par[y] < par[x])	// balancing the height of the tree
		swap(x, y);
	par[x] += par[y];
	par[y] = x;
}
