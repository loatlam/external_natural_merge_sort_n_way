#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <time.h>
#include <iomanip>

using namespace std;

void Run();
void external_merge( string input_link, string buffer_folder_link, int n_way, long ele_chunk );
void split( string input_link, string buffer_folder_link, int n_way, long ele_chunk);
void merge_buffer( string buffer_folder_link, int n_way, long ele_chunk);
string link_converting( string link );
long long get_file_size( string link );
long long get_file_size( fstream& file );
bool check_file_empty( string file_link );
bool check_file_empty( fstream& file );
void file_read_vector( fstream& file, vector<double>& vector_empty, long ele_chunk );
void file_read_queue( fstream& file, queue<double>& queue_empty, long ele_chunk );
void file_write_vector( fstream& file, vector<double>& vec );
void file_write_queue( fstream& file, queue<double>& que );

//
// this function reads file to console terminal;
// input: file's link;
// output: file's content in console terminal;
void file_read_console();

//
// THIS FUNCTION IS FOR TESTING;
// input: input_link, buffer_FOLDER_link, n_way;
// output: TIME of the the external_merge.
void Run_time_calculating();

//
// MONO - FUNCTION // FIXED FUNCTION // NOT FOR TESTING // DO NOT INITIATE THIS UNLESS ITS CODE IS FULLY UNDERSTOOD;
// Run_time_estimating():
// input test: 100M, 200M, 300M, ..., 500M; (elements) (double)
// buffers (n way): 10, 20, 30, 40, 50, 60, 70, 80, 90, 100; (ways)
// each test runs through all the buffers
// output is a final_table;
void The_whole_set_run_time_calculating( long ele_chunk = 500000 );


int main()
{
	ios_base :: sync_with_stdio(false);
	cin.tie(NULL);
	cout.tie(NULL);

	// mono - function // not for testing // print a table in output folder of this project;
	The_whole_set_run_time_calculating(500000);

	// testing function;
	//Run_time_calculating();

	// read file's contents to console function;
	//file_read_console();

	// instruction interface;
	//Run();
    return 0;
}

void Run()
{
	cout << "This is an external merge and external merge time calculating program (use for testing if needed)" << endl << endl;
	cout << "This console terminal won't do any thing but giving instruction :)" << endl << endl;
	cout << "Plese read the instruction and then go to the project - main() function to run this program." << endl;
	cout << "-------------------------------------------------------------" << endl << endl;

	cout << "Instruction: " << endl << endl;
	cout << "there're 3 functions in the main(): " << endl << endl;
	cout << "1/ the first function is not for use. (The_whole_set_run_time_calculating())" << endl << endl;
	cout << "2/ Run_time_calculating(): \t sort file with external merge, calculate the time and then print it to console terminal." << endl << endl;
	cout << "3/ file_read_console(): \t read file's content to console terminal." << endl << endl;
	cout << "-------------------------------------------------------------" << endl << endl;

	cout << "The function menu is above the main()." << endl << endl;
	cout << "Functions' source code is below the main()." << endl << endl;
	cout << "-------------------------------------------------------------" << endl << endl;

	cout << "Thank you for reading ^^" << endl << endl;

	cout << "PS: while running other functions, remember to put this Run() function to comment for obviousness." << endl << endl;
	cout << "-------------------------------------------------------------" << endl << endl;

	cout << "press any key (character) then Enter to exit." << endl;
	string pause;
	cin >> pause;
}

void external_merge( string input_link, string buffer_folder_link, int n_way, long ele_chunk )
{
	split(input_link,buffer_folder_link,n_way,ele_chunk);
	merge_buffer(buffer_folder_link,n_way,ele_chunk);
}

void split( string input_link, string buffer_folder_link, int n_way, long ele_chunk)
{
	buffer_folder_link = link_converting(buffer_folder_link);
	buffer_folder_link.push_back('/');

	// generate 2 * n_way buffers (for split and future merge_buffer);
	for( int i = 1; i <= 2 * n_way; i ++ )
	{
		string buffer_link = buffer_folder_link;
		buffer_link.append(to_string(i));
		buffer_link.append(".bin");

		fstream buffer;
		buffer.open(buffer_link, ios :: out | ios :: binary);
		buffer.close();
	}

	input_link = link_converting(input_link);
	fstream input;
	input.open(input_link, ios :: in | ios :: binary);

	long file_n_ele = get_file_size(input) / 8;
	long loop = file_n_ele / ele_chunk;
	long remaining = file_n_ele % ele_chunk;
	if( remaining != 0 )
		loop ++;

	vector<fstream> buffer (n_way);
	for( int i = 1; i <= n_way; i ++ )
	{
		string buffer_link = buffer_folder_link;
		buffer_link.append(to_string(i));
		buffer_link.append(".bin");

		buffer[i - 1].open(buffer_link, ios :: out | ios :: binary);
	}

	int current_buffer = 1;
	double last_ele = -1;
	vector<double> temp_arr;
	vector<vector<double>> temp_buffer_matrix (n_way);

	while( loop != 0 )
	{
		file_read_vector(input,temp_arr,ele_chunk);
		for( long i = 0; i < temp_arr.size(); i ++ )
		{
			if( last_ele > temp_arr[i] )
			{
				double end_run = -1;
				temp_buffer_matrix[current_buffer - 1].push_back(end_run);
				current_buffer ++;
				if( current_buffer > n_way )
					current_buffer = 1;
			}
			temp_buffer_matrix[current_buffer - 1].push_back(temp_arr[i]);
			last_ele = temp_arr[i];
		}

		for( int i = 1; i <= n_way; i ++ )
			if( ! temp_buffer_matrix[i - 1].empty() )
				file_write_vector(buffer[i - 1],temp_buffer_matrix[i - 1]);

		loop --;
	}

	for( int i = 1; i <= n_way; i ++ )
		buffer[i - 1].close();
	input.close();
}

void merge_buffer( string buffer_folder_link, int n_way, long ele_chunk)
{
	buffer_folder_link = link_converting(buffer_folder_link);
	buffer_folder_link.push_back('/');

	string buffer_1 = buffer_folder_link;
	buffer_1.append("1.bin");
	string buffer_2 = buffer_folder_link;
	buffer_2.append("2.bin");
	string buffer_2_n = buffer_folder_link;
	buffer_2_n.append(to_string(2 + n_way));
	buffer_2_n.append(".bin");

	int current_buffer = 1;
	int current_end = n_way;
	int current_write = 1 + n_way;
	int current_write_end = 2 * n_way;

	while( ! (check_file_empty(buffer_2) && check_file_empty(buffer_2_n)) )
	{
		if( ! check_file_empty(buffer_1) )
		{
			current_buffer = 1;
			current_end = n_way;
			current_write = 1 + n_way;
			current_write_end = 2 * n_way;
		}
		else
		{
			current_buffer = 1 + n_way;
			current_end = 2 * n_way;
			current_write = 1;
			current_write_end = n_way;
		}

		int default_write = current_write;

		vector<fstream> buffer_arr (2 * n_way);
		for( int i = current_buffer; i <= current_end; i ++ )
		{
			string buffer_link = buffer_folder_link;
			buffer_link.append(to_string(i));
			buffer_link.append(".bin");

			buffer_arr[i - 1].open(buffer_link, ios :: in | ios :: binary);
		}

		vector<fstream> write_arr (2 * n_way);
		for( int i = current_write; i <= current_write_end; i ++ )
		{
			string write_link = buffer_folder_link;
			write_link.append(to_string(i));
			write_link.append(".bin");

			write_arr[i - 1].open(write_link, ios :: out | ios :: binary);
		}

		vector<int> loop (2 * n_way,0);
		for( int i = current_buffer; i <= current_end; i ++ )
		{
			long buffer_n_ele = get_file_size(buffer_arr[i - 1]) / 8;
			long buffer_loop = buffer_n_ele / ele_chunk;
			long remaining = buffer_n_ele % ele_chunk;
			if( remaining != 0 )
				buffer_loop ++;

			loop[i - 1] = buffer_loop + 1;
		}

		vector<queue<double>> temp_buffer_matrix (2 * n_way);
		for( int i = current_buffer; i <= current_end; i ++ )
		{
			file_read_queue(buffer_arr[i - 1],temp_buffer_matrix[i - 1],ele_chunk);
			loop[i - 1] --;
		}

		int count_all_buffer_finish = 0;

		priority_queue<double, vector<double>, greater<double>> data;
		vector<vector<double>> temp_write_matrix (2 * n_way);
		vector<double> last_ele_arr (2 * n_way, - 1);

		while( count_all_buffer_finish != n_way )
		{
			for( int i = current_buffer; i <= current_end; i ++ )
			{
				if( loop[i - 1] != 0 )
				{
					data.push(temp_buffer_matrix[i - 1].front());
					last_ele_arr[i - 1] = temp_buffer_matrix[i - 1].front();
					temp_buffer_matrix[i - 1].pop();
					if( temp_buffer_matrix[i - 1].empty() )
					{
						if( loop[i - 1] == 1 )
							loop[i - 1] = 0;
						else
						{
							file_read_queue(buffer_arr[i - 1],temp_buffer_matrix[i - 1],ele_chunk);
							loop[i - 1] --;
						}
					}
				}
				// else do nothing;
			}

			while( ! data.empty() )
			{
				temp_write_matrix[current_write - 1].push_back(data.top());
				if( temp_write_matrix[current_write - 1].size() >= ele_chunk )
					file_write_vector(write_arr[current_write - 1],temp_write_matrix[current_write - 1]);
				int buffer_update = -1;
				for( int i = current_buffer; i <= current_end; i ++ )
				{
					if( last_ele_arr[i - 1] == data.top() )
					{
						buffer_update = i;
						break;
					}
				}
				data.pop();

				if( loop[buffer_update - 1] != 0 )
				{
					if( temp_buffer_matrix[buffer_update - 1].front() != -1 )
					{
						data.push(temp_buffer_matrix[buffer_update - 1].front());
						last_ele_arr[buffer_update - 1] = temp_buffer_matrix[buffer_update - 1].front();
						temp_buffer_matrix[buffer_update - 1].pop();
						if( temp_buffer_matrix[buffer_update - 1].empty() )
						{
							if( loop[buffer_update - 1] == 1 )
								loop[buffer_update - 1] = 0;
							else
							{
								file_read_queue(buffer_arr[buffer_update - 1],temp_buffer_matrix[buffer_update - 1],ele_chunk);
								loop[buffer_update - 1] --;
							}
						}
					}
					else
					{
						double end_run = -1;
						last_ele_arr[buffer_update - 1] = end_run;
						temp_buffer_matrix[buffer_update - 1].pop();
						if( temp_buffer_matrix[buffer_update - 1].empty() )
						{
							if( loop[buffer_update - 1] == 1 )
								loop[buffer_update - 1] = 0;
							else
							{
								file_read_queue(buffer_arr[buffer_update - 1],temp_buffer_matrix[buffer_update - 1],ele_chunk);
								loop[buffer_update - 1] --;
							}
						}
					}
				}
				// else do nothing;
			}
			double end_run = -1;
			temp_write_matrix[current_write - 1].push_back(end_run);

			current_write ++;
			if( current_write > current_write_end )
				current_write = default_write;

			count_all_buffer_finish = 0;
			for( int i = current_buffer; i <= current_end; i ++ )
				if( loop[i - 1] == 0)
				count_all_buffer_finish ++;
		}

		current_write = default_write;
		for( int i = current_write; i <= current_write_end; i ++ )
			if( ! temp_write_matrix[i - 1].empty() )
				file_write_vector(write_arr[i - 1],temp_write_matrix[i - 1]);

		for( int i = current_buffer; i <= current_end; i ++ )
			buffer_arr[i - 1].close();
		for( int i = current_write; i <= current_write_end; i ++ )
			write_arr[i - 1].close();

		for( int i = current_buffer; i <= current_end; i ++ ) // delete current buffers' content for future buffer write;
		{
			string buffer_link = buffer_folder_link;
			buffer_link.append(to_string(i));
			buffer_link.append(".bin");

			fstream buffer;
			buffer.open(buffer_link, ios :: out | ios :: binary | ios :: trunc);
			buffer.close();
		}
	}

	for( int i = 1; i <= 2 * n_way; i ++ )
	{
		string buffer_link = buffer_folder_link;
		buffer_link.append(to_string(i));
		buffer_link.append(".bin");

		fstream buffer;
		buffer.open(buffer_link, ios :: out | ios :: binary | ios :: trunc);
		buffer.close();
	}
}

string link_converting( string link )
{
	for( int i = 0; i < link.length(); i ++ )
		if( link[i] == '\\')
			link[i] = '/';
	return link;
}

long long get_file_size( string link )
{
	fstream file;
	file.open(link, ios :: in | ios :: binary);

	file.seekg(0, ios :: end);
	long long file_size = file.tellg();
	file.close();
	return file_size;
}

long long get_file_size( fstream& file )
{
	long long current_file_pos = file.tellg();
	file.seekg(0, ios :: end);
	long long file_size = file.tellg();
	file.seekg(current_file_pos);
	return file_size;
}

bool check_file_empty( string file_link )
{
	fstream file;
	file.open(file_link, ios :: in | ios :: binary);

	file.seekg(0 , ios :: end);
	long long ending = file.tellg();
	file.close();
	if( ending == 0 )
		return 1;
	return 0;
}

bool check_file_empty( fstream& file )
{
	if( get_file_size(file) == 0 )
		return 1;
	return 0;
}

void file_read_vector( fstream& file, vector<double>& vector_empty, long ele_chunk )
{
	vector_empty.clear();
	double* temp_arr = new double [ele_chunk];
	for( long i = 0; i < ele_chunk; i ++ )
		temp_arr[i] = -100;
	file.read(reinterpret_cast<char*>(temp_arr),(8 * ele_chunk));
	for( long i = 0; i < ele_chunk; i ++ )
		if( temp_arr[i] != -100 )
			vector_empty.push_back(temp_arr[i]);
	delete[] temp_arr;
}

void file_read_queue( fstream& file, queue<double>& queue_empty, long ele_chunk )
{
	while( queue_empty.size() != 0 )
		queue_empty.pop();
	double* temp_arr = new double [ele_chunk];
	for( long i = 0; i < ele_chunk; i ++ )
		temp_arr[i] = -100;
	file.read(reinterpret_cast<char*>(temp_arr),(8 * ele_chunk));
	for( long i = 0; i < ele_chunk; i ++ )
		if( temp_arr[i] != -100 )
			queue_empty.push(temp_arr[i]);
	delete[] temp_arr;
}

void file_write_vector( fstream& file, vector<double>& vec )
{
	double* temp_arr = new double[vec.size()];
	for( long i = 0; i < vec.size(); i ++ )
		temp_arr[i] = vec[i];
	file.write(reinterpret_cast<char*>(temp_arr),(8 * vec.size()));
	vec.clear();
	delete[] temp_arr;
}

void file_write_queue( fstream& file, queue<double>& que )
{
	long queue_size = que.size();
	double* temp_arr = new double[queue_size];
	for( long i = 0; i < queue_size; i ++ )
	{
		temp_arr[i] = que.front();
		que.pop();
	}
	file.write(reinterpret_cast<char*>(temp_arr),(8 * queue_size));
	delete[] temp_arr;
}

//
// this function reads file to console terminal;
// input: file's link;
// output: file's content in console terminal;
void file_read_console()
{
	long ele_chunk = 1000000;

	cout << "reading file's content to console terminal" << endl;
	cout << "------------------------------------------------" << endl << endl;

	string file_link;
	cout << "file's link: (ex: D:\\folder\\file.bin) (copied - pasted) " << endl;
	getline(cin,file_link);

	// output manipulation;
	cout << endl;
	// output manipulation;

	file_link = link_converting(file_link);

	long n_ele = get_file_size(file_link) / 8;
	long loop = n_ele / ele_chunk;
	long remaining = n_ele % ele_chunk;
	if( remaining != 0 )
		loop ++;

	fstream file;
	file.open(file_link, ios :: in | ios :: binary);

	while( loop != 0 )
	{
		vector<double> temp_arr;
		file_read_vector(file,temp_arr,ele_chunk);
		for( long i = 0; i < temp_arr.size(); i ++ )
			cout << temp_arr[i] << " ";
		loop --;
	}
	file.close();

	// output manipulation;
	cout << endl;
	// output manipulation;
}

//
// THIS FUNCTION IS FOR TESTING;
// input: input_link, buffer_FOLDER_link, n_way;
// output: TIME of the the external_merge.
void Run_time_calculating()
{
	cout << "run time calculating" << endl;
	cout << "-------------------------------------------" << endl << endl;

	cout << "instruction: " << endl << endl;
	cout << "1/ input the input link (the file need to be sorted): (ex: D:\\inputs\\input.bin) (copied - pasted )" << endl << endl;
	cout << "2/ input the buffer FOLDER link (where buffers are generated): (ex: D:\\buffers) (copied - pasted )" << endl << endl;
	cout << "3/ input the number of ways (n ways) that will be buffers: (ex: 100)" << endl << endl;
	cout << "4/ input the number of elements (ele_chunk) read in at one time (1000000 for the fastest read write speed (tested), but memory should be considered)"
		<< endl << endl;
	cout << "THE OUTPUT: In the end, after finishing merge, the time (output) will be printed to console terminal." << endl << endl;
	cout << "THE RESULT FILE: The final result (the file's been merged) is in the buffer FOLDER, it's the only remaining one, either 1.bin or (1 + n_way).bin.";
	cout << endl << endl;
	cout << "DELETE BUFFERS: buffers (in the buffer FOLDER) should be deleted for future testing transparency." << endl;
	cout << "-------------------------------------------" << endl << endl;

	string input_link;
	string buffer_folder_link;
	int n_way = 0;
	long ele_chunk = 1000000; // default // which is the fastest read write speed due to testing;

	cout << "input the input link: " << endl;
	getline(cin,input_link);
	cout << "input the buffer FOLDER link: " << endl;
	getline(cin,buffer_folder_link);
	cout << "input the number of ways: " << endl;
	cin >> n_way;
	cout << "input the number of elements: " << endl;
	cin >> ele_chunk;
	cout << endl;

	clock_t start = clock();

	external_merge(input_link,buffer_folder_link,n_way,ele_chunk);

	clock_t finish = clock();
	double time = (double) (finish - start) / CLOCKS_PER_SEC;

	cout << "time: " << time << endl;
}

//
// MONO - FUNCTION // FIXED FUNCTION // NOT FOR TESTING // DO NOT INITIATE THIS UNLESS ITS CODE IS FULLY UNDERSTOOD;
// Run_time_estimating():
// input test: 100M, 200M, 300M, ..., 500M; (elements) (double)
// buffers (n way): 10, 20, 30, 40, 50, 60, 70, 80, 90, 100; (ways)
// each test runs through all the buffers
// output is a final_table;
void The_whole_set_run_time_calculating( long ele_chunk )
{
	string output_link = "outputs/final_table.txt";
	fstream output;
	output.open(output_link, ios :: out);

	if( ! output )
	{
		cout << "output FAIL" << endl;
		return;
	}

	string buffer_folder_link = "buffers";

	// output manipulation
	output << setw(13) << "size/n_way";
	output << setw(13) << 10;
	output << setw(13) << 20;
	output << setw(13) << 30;
	output << setw(13) << 40;
	output << setw(13) << 50;
	output << setw(13) << 60;
	output << setw(13) << 70;
	output << setw(13) << 80;
	output << setw(13) << 90;
	output << setw(13) << 100;
	output << setw(13) << "average";
	output << endl << endl;

	cout << setw(13) << "size/n_way";
	cout << setw(13) << 10;
	cout << setw(13) << 20;
	cout << setw(13) << 30;
	cout << setw(13) << 40;
	cout << setw(13) << 50;
	cout << setw(13) << 60;
	cout << setw(13) << 70;
	cout << setw(13) << 80;
	cout << setw(13) << 90;
	cout << setw(13) << 100;
	cout << setw(13) << "average";
	cout << endl << endl;
	// output manipulation

	float average_time_size = 0;
	vector<float> average_time_n_way(10,0);

	for( int i = 1; i <= 5; i ++ )
	{															   // Only this link is changable;
		string input_folder = "D:/Desktop/external_merge/inputs/"; // FIXED LINK, if the input folder's different, this link must be changed to be adapted;
		string input_link = input_folder;
		input_link.append(to_string(i));
		input_link.append(".bin");

		// output manipulation
		string data_size = to_string(i);
		data_size.append("00M");

		cout << setw(13) << data_size;
		output << setw(13) << data_size;

		// output manipulation

		for( int j = 1; j <= 10; j ++)
		{
			int n_way = j * 10;

			clock_t start = clock();

			external_merge(input_link,buffer_folder_link,n_way,ele_chunk);

			clock_t finish = clock();
			double time = (double) (finish - start) / CLOCKS_PER_SEC;

			// output manipulation
			output << setw(13) << time;
			cout << setw(13) << time;
			// output manipulation

			average_time_size += time;
			average_time_n_way[j-1] += time;
		}
		average_time_size /= 10;

		// output manipulation
		output << setw(13) << average_time_size;
		cout << setw(13) << average_time_size;

		output << endl << endl;
		cout << endl << endl;
		// output manipulation

		average_time_size = 0;
	}

	for( int i = 0; i < average_time_n_way.size(); i ++ )
		average_time_n_way[i] /= 5;

	// output manipulation
	output << setw(13) << "average";
	cout << setw(13) << "average";
	// output manipulation

	// output manipulation
	for( int i = 0; i < average_time_n_way.size(); i ++ )
	{
		output << setw(13) << average_time_n_way[i];
		cout << setw(13) << average_time_n_way[i];
	}
	output << endl << endl;
	cout << endl << endl;
	// output manipulation

	// after getting the final_table output, truncate all buffers to release memory space;
	for( int i = 1; i <= 200; i ++ )
	{
		string buffer_link = buffer_folder_link;
		buffer_link.push_back('/');
		buffer_link.append(to_string(i));
		buffer_link.append(".bin");

		fstream buffer;
		buffer.open(buffer_link, ios :: out | ios :: binary | ios :: trunc);
		buffer.close();
	}
}



















