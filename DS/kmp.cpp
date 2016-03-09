 #include <bits/stdc++.h>
using namespace std;

string pat,txt;
int lps[1000000];

void findlps();
void KMPSearch();

int main() {
    
	cin>>txt;
	cin>>pat;
	KMPSearch();
	return 0;
}

void KMPSearch()
{
    int m = pat.length();
    int n = txt.length();
 
    // create lps[] that will hold the longest prefix suffix values for pattern
    int j  = 0;  // index for pat[]
 
    // Preprocess the pattern (calculate lps[] array)
    findlps();
 
    int i = 0;  // index for txt[]
    while (i < n)
    {
      if (pat[j] == txt[i])
      {
        j++;
        i++;
      }
 
      if (j == m)
      {
        cout<<"Found pattern at index "<<i-j<<endl;
        j = lps[j-1];
      }
 
      // mismatch after j matches
      else if (i < n && pat[j] != txt[i])
      {
        // Do not match lps[0..lps[j-1]] characters,
        // they will match anyway
        if (j != 0)
         j = lps[j-1];
        else
         i = i+1;
      }
    }
    
}


void findlps()
{
    int len = 0;  // length of the previous longest prefix suffix
 
    lps[0] = 0; // lps[0] is always 0
    int i = 1;
 
    // the loop calculates lps[i] for i = 1 to pat.length()-1
    while (i < pat.length())
    {
       if (pat[i] == pat[len])
       {
         len++;
         lps[i] = len;
         i++;
       }
       else // (pat[i] != pat[len])
       {
         if (len != 0)
         {
           // This is tricky. Consider the example AAACAAAA and i = 7.
           len = lps[len-1];
 
           // Also, note that we do not increment i here
         }
         else // if (len == 0)
         {
           lps[i] = 0;
           i++;
         }
       }
    }
}
