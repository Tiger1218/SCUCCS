#include <iostream>
#include <climits>
using namespace std;
int a[1000];
int main(){
    int tmp, pointer = 0, stored = INT_MAX;
    while(true){
        cin >> tmp;
        if(tmp == -1){
            break;
        }
        if(tmp != stored){
            stored = tmp;
            a[pointer] = tmp;
            pointer ++ ;
        }
    }
    for(int i = 0 ; i < pointer ; i ++){
        cout << a[i] << " ";
    }
    return 0;
}