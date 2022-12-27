#include <iostream>
#include <stack>
using namespace std;
stack<int> input, output;
int output_array[100005], output_pointer = 0;
int input_array[100005], input_pointer = 0, pointer = 0;
int main(){
    int tmp;
    while(true){
        cin >> tmp;
        if(tmp == -1){
            break;
        }
        input_array[input_pointer] = tmp;
        input_pointer ++;
    }
    while(true){
        cin >> tmp;
        if(tmp == -1){
            break;
        }
        output_array[output_pointer] = tmp;
        output_pointer ++;
        // output.push(tmp);
        // cout << "output add push " << + tmp << endl;
    }
    
    for(int i = output_pointer - 1 ; i >= 0 ; i --){
        output.push(output_array[i]);
    }
    // cout << "input end" << endl;

    while(true){
        if(output.empty()){
            cout << 0;
            return 0;
        }
        // if(output.empty)
        while(!input.empty() && !output.empty() && input.top() == output.top()){
            // cout << "delete " << input.top() << " " << output.top() << "\n";
            output.pop();
            input.pop();
        }
        if(pointer == input_pointer){
            if(output.empty()){
                cout << 1;
            }
            else{
                cout << 0;
            }
            return 0;
        }
        else{
            // cout << "push " << input_array[pointer] << "\n";
            input.push(input_array[pointer]);
            pointer ++ ;
        }
    }
    return 0;
}