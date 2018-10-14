#include <iostream>
#include <algorithm>
#include <time.h>
#include <fstream>
#include <string>

using namespace std;

string link_converting( string link );
void file_random_double();

int main()
{
	ios_base :: sync_with_stdio(false);
	cin.tie(NULL);
	cout.tie(NULL);
	srand(time(NULL));

	file_random_double();

    return 0;
}

void file_random_double()
{
	cout << "file random double" << endl;
	cout << "---------------------------------------" << endl << endl;
	cout << "file's link: (ex: D:\\Desktop\\test.bin)" << endl;

	string link;
	getline(cin,link);

	string input = link_converting(link);
	fstream file_output;
	file_output.open(input, ios :: out | ios :: binary);

	if( ! file_output )
	{
		cout << "file FAIL" << endl;
		return;
	}

	cout << endl;
	cout << "the number of elements: " << endl;
	long n_ele;
	cin >> n_ele;

	//cout << "the number of (double) elements in a chunk: " << endl;
	long ele_chunk = 500000;
	//cin >> ele_chunk;

	double* temp_arr = new double[ele_chunk];
	for( int i = 0; i < ele_chunk; i ++ )
		temp_arr[i] = -1;

	double ele_1 = -1;
	double ele_2 = 1.2345;

	clock_t start = clock();

	for( long i = 0; i < n_ele; i ++)
	{
		ele_1 = (double) (rand() % 10000 + 1);
		temp_arr[i % ele_chunk] = ele_1 * ele_2;
		if( (i+1) % ele_chunk == 0 )
			file_output.write(reinterpret_cast<char*>(temp_arr),(8 * ele_chunk));
	}
	file_output.write(reinterpret_cast<char*>(temp_arr),(8 * (n_ele % ele_chunk)));

	clock_t finish = clock();
	double time = (double) (finish - start) / CLOCKS_PER_SEC;
	cout << endl;
	cout << "time: " << time << endl;

	return;
}

string link_converting( string link )
{
	for( int i = 0; i < link.length(); i ++ )
		if( link[i] == '\\')
			link[i] = '/';
	return link;
}


















