

#include <stdio.h>
#include <string.h>

int linear_search(int *a, int sz, int elem){

    for(int i=0;i<sz;i++){
        if(a[i] == elem){
            return i;
        }



    }

    return -1;




}


void reverse_arr(int *arr, int sz) {
    for (int i = 0; i < sz / 2; i++) {
        int middle = arr[i];
        arr[i] = arr[sz - i - 1];
        arr[sz - i - 1] = middle;
    }
}

int main() {
    // Q0: Hello World
    printf("Hello World\n");

   

    return 0;
}










