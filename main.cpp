#include <iostream>
#include "mpi.h"
#include "buff_parser.h"
#include "MPI_utility.h"
#include <chrono>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>

using namespace std;


//int main()
//{
//    double b = 3.0000;
//    std::string s;
//    // convert double b to string s
//    { std::stringstream ss;
//        ss << b;
////        ss << "\n";
//        s = ss.str();
//    }
//
//    std::string s2;
//    s2 = std::to_string(b);
//
//    // do something here with s
//    std::cout << "3.0000 length is  :" << s.length() << endl;
//    std::cout << "3.0000 converted to :" << s << std::endl;
//    std::cout << "3.0000 length is  :" << s2.length() << endl;
//    std::cout << "3.0000 converted to :" << s2 << std::endl;
//}


int main(int argc, char **argv) {


    // getting the file name
    char* fname = argv[1];
    char* pbuff;

    /*__________________________ READ ____________________________*/
    // log time to start reading

    clock_t begin_reading = clock();

    // initialize MPI
    MPI_Init(&argc,&argv);

    // MPI read function

    MPI_File fh;
    MPI_Offset start_point, read_count, interval_count;
    int node_size, node_rank;
    MPI_Status status;

    MPI_Comm_size(MPI_COMM_WORLD, &node_size); // get the node size
    MPI_Comm_rank(MPI_COMM_WORLD, &node_rank); // get the each node rank

    int ret = mpi_read(fname,pbuff,node_size,node_rank); // read file

    std::vector<record> input = buff_record(pbuff);
    clock_t finish_reading = clock(); // finish the reading

    //identify the output
    std::vector<record> signal_output;
    std::vector<record> noise_output;
    double sum = 0.0;
    double sum_square = 0.0;
    double sum_cubic = 0.0;
    double sum_quadruple = 0.0;
    std::vector<double> return_output;

    clock_t begin_srub = clock();

    SCRUB(input, signal_output, noise_output,return_output, sum, sum_square, sum_cubic, sum_quadruple);

    clock_t finish_srub = clock();

    /* preliminary print */
//    cout << input.size() << endl;
//    cout << signal_output.size() << endl;
//    cout << noise_output.size() << endl;
//    cout << sum << "," << sum_square << "," << sum_cubic << "," << sum_quadruple << endl;
//    cout << return_output.size() << endl;


    /* ____________________ Generate the output ____________________ */

    clock_t begin_write = clock();

    //save the output to char array
    MPI_Offset size_signal, size_noise;
    char* signal_buff;
    char* noise_buff;

    size_signal = set_buff(signal_buff,signal_output);
    size_noise = set_buff(noise_buff,noise_output);

//    std::cout<< "size of signal: " << size_signal << endl;
//    std::cout<< "size of noise: " << size_noise << endl;

    char fname_signal[] = "signal.txt"; // set the output file_name
    char fname_noise[] = "noise.txt"; // set the output file_name

    // write outputs for noise and signal
    ret = mpi_write(fname_signal, signal_buff, size_signal, node_rank, node_size); // write file //
    ret = mpi_write(fname_noise, noise_buff, size_noise, node_rank, node_size); // write file //

    clock_t finish_write = clock();

    /*______________ Normality test ______________*/
    clock_t begin_test = clock();

    int k = 1; // here there is only regressor, namely price
    long n = return_output.size();
    double mean_price = sum/n;
    double mean_square_diff = sum_square - 2 * mean_price * sum - n * pow(mean_price,2);
    double mean_cubic_diff = sum_cubic - 3  * sum_square * mean_price + 3 * pow(mean_price,2) * sum - pow(mean_price,3);
    double mean_quadruple_diff = sum_quadruple - 4 * mean_price * sum_cubic + 6 * pow(mean_price,2) * sum_square - 4 * pow(mean_price, 3) * sum + n * pow(mean_price, 4);


    double s = (mean_cubic_diff/n) / pow(mean_square_diff/n,3/2.0);
    double c = (mean_quadruple_diff/n) / pow(mean_square_diff/n,2);

    double JB_score = (n-k+1)/6 *(pow(s,2) + 1/4.0*pow(c-3,2));
    cout << "JB score is " << JB_score << endl;

    // reference is, for large sample
    //0.10(level)- 4.61, 0.05(level)-5.99, 0.01(level)- 9.21
    std::cout << "Normality test results: " << std::endl;
    if (JB_score > 9.21)
    {
        cout << " Conclusion: The return from signal doesnt follow normal distribution based on JB test" << endl;
    }
    else
    {
        cout << " Conclusion: The return from signal follows normal distribution based on JB test based on 99% confidence interval" << endl;

    }
    clock_t finish_test = clock();


    // generate the log file
    ofstream log_file;
    log_file.open("log.txt");
    stringstream ss;
    ss << "begin to read data: " << begin_reading << endl
       << "Time to finish reading: " << finish_reading << endl
       << "number of record of trading data is: " << input.size()  << endl
       << "total size of byte is: " << size_noise + size_signal << endl
       << "It took " << float(finish_reading - begin_reading)/CLOCKS_PER_SEC << " secs to read the data." << endl << endl

       << "begin to scrub data: " << begin_srub << endl
       << "Time to finish scrubbing: " << finish_srub << endl
       << "number of record of signal is: " << signal_output.size()  << endl
       << "size of signal: " << size_signal << " in bytes" << endl
       << "number of record of noise is: " << noise_output.size()  << endl
       << "size of noise: " << size_noise  << " in bytes" << endl
       << "It took " << float(finish_srub - begin_srub)/CLOCKS_PER_SEC << " secs to scrub the data." << endl << endl

       << "begin to write data: " << begin_write << endl
       << "Time to finish writing:" << finish_write << endl
       << "It took " << float(finish_write - begin_write)/CLOCKS_PER_SEC << " secs to write the data." << endl << endl

       << "Time to start normality test: " << begin_test << endl
       << "Time to finish nornality test: " << finish_test << endl
       << "It took " << float(finish_test - begin_test)/CLOCKS_PER_SEC << " secs to do normality test."  << endl;

    log_file << ss.str();
    log_file.close();

    return 0;
}


