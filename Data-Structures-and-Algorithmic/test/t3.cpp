// #include <iostream>
// #include <cstdio>
// #include <climits>
#include <bits/stdc++.h>
using namespace std;
struct node{
    int l, r, value, max, min;
};
struct node n[100005];
int N, counts, a, b, c, ans = 1, value_a[100005], pointer_a;
void checks(int node){
    if(n[node].l == 0 && n[node].r == 0){
        n[node].max = n[node].value;
        n[node].min = n[node].value;
    }
    if(n[node].l != 0){
        checks(n[node].l);
        if(n[n[node].l].max > n[node].value){
            ans = 0;
        }
        else{
            n[node].min = min(n[node].value, n[n[node].l].min);
        }
    }
    if(n[node].r != 0){
        checks(n[node].r);
        if(n[n[node].r].min < n[node].value){
            ans = 0;
        }
        else{
            n[node].max = max(n[node].value, n[n[node].r].max);
        }
    }
    n[node].min = min(n[node].value, n[node].min);
    n[node].max = max(n[node].value, n[node].max);
}
int main(){
    while(cin >> a){
        if(counts % 3 == 0){
            n[counts / 3 + 1].l = a;
            n[counts / 3 + 1].min = INT_MAX;
            n[counts / 3 + 1].max = -INT_MAX;
        }
        else if(counts % 3 == 1){
            n[counts / 3 + 1].r = a;
        }
        else{
            n[counts / 3 + 1].value = a;
            value_a[pointer_a] = a;
            pointer_a ++ ;
        }
        counts ++;
    }
    // cout << counts;
    N = counts / 3;
    // cout << N << endl;
    checks(1);
    for(int i = 1 ; i <= N ; i ++){
        // cout << n[i].l << " " << n[i].r << " " << n[i].value << " " << n[i].min << " " << n[i].max << " " << endl;
    }
    if(ans == 1){
        sort(value_a + 0, value_a + pointer_a);
        for(int i = 0 ; i < pointer_a ; i ++){
            cout << value_a[i] << " ";
        }
    }
    else{
        cout << endl;
    }
    return 0;   
}