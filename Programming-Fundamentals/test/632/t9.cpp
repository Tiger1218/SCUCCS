// Wrong Answer
#include <iostream>
#include <cmath>
using namespace std;
int m, n, k, subs;
int main(){
    cin >> m >> n >> k;
    if(k > n){
        cout << "Input Error";
        return 0;
    }
    for(int i = k ; i <= m ; i ++){
        if(i == m){
            subs += n;
            // cout << subs << endl;
        }
        if(i == m - 1){
            subs += 2 * (n - 1) * n;
            // cout << subs << endl;
        }
        if(i < m - 1){
            subs += 2 * n * (n - 1) * pow(n, m - i - 1);
            subs += (m - i - 1) * (n - 1) * (n - 1) * pow(n, m - i - 2);    
        }
    }
    // cout << subs << endl;
    cout << pow(n, m) - subs;
    return 0;
}