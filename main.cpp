#include <iostream>
#include "mpi.h"
#include "buff_parser.h"
#include "MPI_utility.h"
#include <chrono>

using namespace std;

int main(int argc, char **argv) {

    // getting the file name
    char* fname = argv[1];
    char* pbuff;

    /*__________________________ READ ____________________________*/
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

    //identify the output
    std::vector<record> signal_output;
    std::vector<record> noise_output;
    double sum = 0.0;
    double sum_square = 0.0;
    double sum_cubic = 0.0;
    double sum_quadruple = 0.0;
    std::vector<double> return_output;

    SCRUB(input, signal_output, noise_output,return_output, sum, sum_square, sum_cubic, sum_quadruple);

//    cout << input.size() << endl;
//    cout << signal_output.size() << endl;
//    cout << noise_output.size() << endl;
//    cout << sum << "," << sum_square << "," << sum_cubic << "," << sum_quadruple << endl;
//    cout << return_output.size() << endl;
//
    /*______________ Calculate the input of JB test ______________*/
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

    //save the output to char array
    MPI_Offset size_signal, size_noise;
    char* signal_buff;
    char* noise_buff;

    size_signal = set_buff(signal_buff,signal_output);
    size_noise = set_buff(noise_buff,noise_output);

    std::cout << "buffer is:" << signal_buff << "!!!!!!!!" <<std::endl;

    std::cout<< "size of signal: " << size_signal << endl;
    std::cout<< "size of noise: " << size_noise << endl;

    char fname_signal[] = "signal.txt"; // set the output file_name
    char fname_noise[] = "noise.txt"; // set the output file_name
    // write outputs for noise and signal
    ret = mpi_write(fname_signal, signal_buff, size_signal, node_rank, node_size); // write file //


    return 0;
}


