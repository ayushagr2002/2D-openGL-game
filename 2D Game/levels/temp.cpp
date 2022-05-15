#include <bits/stdc++.h>
using namespace std;

int main() {
    srand(time(0));
    for(int i=0;i<25;i++){
        for(int j=0;j<25;j++) {
            if(rand()%13 == 6)
            {
                cout << "0 ";
            }
            else {
                cout << "1 ";
            }
        }
        cout << "\n";
    }
}