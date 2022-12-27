#include <bits/stdc++.h>
#include <cassert>
using namespace std;
int process(char c){
	switch(c){
		case 'A':{
			return 1;
		}
		case 'C':{
			return 2;
		}
		case 'G':{
			return 3;
		}
		case 'T':{
			return 4;
		}
		default:{
			assert(1 == 2);
		}
	}

}
int n, m, first;
struct node{
	int id, chaos;
	char s[105];
};
bool cmp(node u, node v){
	if(u.chaos == v.chaos){
		return u.id < v.id;
	}
	return u.chaos < v.chaos;
}
struct node a[1005];
int main(){
	while(cin >> n >> m){		
		for(int i = 0 ; i < m ; i ++){
			cin >> a[i].s;
			a[i].id = i;
            a[i].chaos = 0;
			for(int j = 0 ; j < n ; j ++){
				for(int k = j + 1 ; k < n ; k ++){
					if(process(a[i].s[j]) > process(a[i].s[k])){
						a[i].chaos ++;
					}
				}
			}
		}
		sort(a, a + m, cmp);
        if(first)cout << endl;
        first = 1;
		for(int i = 0 ; i < m ; i ++){
			cout << a[i].s << endl;
		}
		cout << "********************";
	}
	return 0;
}