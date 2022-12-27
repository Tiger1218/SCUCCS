#include <iostream>
using namespace std;
int a[100000], sizes;
void process(int n){
    while(n != 1){
        a[sizes] = n;
        sizes ++ ;
        if(n % 2 == 1){
            n = n * 3 + 1;
        }
        else{
            n = n / 2;
        }
    }
}
int main(){
    int n;
    cin >> n;
    process(n);
    cout << 1;
    for(int i = sizes - 1 ; i >= 0 ; i --){
        cout << " " << a[i];
    }
    return 0;
}