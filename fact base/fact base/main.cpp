
#include <iostream>
#include <string>
#include <windows.h>
#include <algorithm>

using namespace std;

int main(){

	const int N = 7;
	int myints[N];
	for(int i=0; i<N; i++) myints[i] = i+1;

	cout << "The "<< N <<"! possible permutations with " << N <<" elements:\n";

	sort(myints, myints+N);

	do {
		for(int i=0; i<N; i++) cout << myints[i] << " ";
		cout << endl;
	} while ( next_permutation(myints, myints+N) );

	char str[64];
	cin >> str;

	return 0;
}
