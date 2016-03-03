#include "mpi.h"
#include "buff_parser.h"
#include <string>
#include <cstring>
#include <iostream>
#include <sstream>

#ifndef BDIF_A_MPI_UTILITY_H
#define BDIF_A_MPI_UTILITY_H

// helper function to convert double to string - this function doesnt work well
// as 3.0000 will be cast to 0
std::string double_to_string(double input)
{
    std::string result;          // string which will contain the result
    std::stringstream convert;   // stream used for the conversion
    convert << input;      // insert the textual representation of 'Number' in the characters in the stream
    result = convert.str();
    return result;
}

// convert vector to a char array, as MPI could only write char array
MPI_Offset set_buff(char* &p, std::vector<record> input)
{
    MPI_Offset size_buff = 0;
    for (long i=0; i < input.size(); i++) {
        size_buff += input[i].time.length();
        size_buff += input[i].price.length();
        size_buff += input[i].volume.length();
        size_buff += 1;
    }

    // overridden the buffer - reserve the space
    p = new char[size_buff];
    char* tmp = p;

    // convert the vector of records to char array
//    for (long i=0; i < input.size(); i++) {
//
//        std::string tmp_buff = input[i].time + "," + std::to_string(input[i].price) + "," + std::to_string(input[i].volume);
//        strcpy(tmp, tmp_buff.c_str());
//        tmp += tmp_buff.length();
//        *tmp = '\n';
//        tmp = tmp + 1;
//    }
    for (auto &i : input)
    {
        std::string tmp_buff = i.time + i.price +i.volume;
        strcpy(tmp, tmp_buff.c_str());
        tmp += tmp_buff.length();
        *tmp = '\n';
        tmp = tmp + 1;
    }

    return size_buff;
}

// MPI read utility function, given a file name, output char array buffer,
// given how many nodes, and the corresponding node

int mpi_read(char * fname, char * &pbuff, int node_size, int node_rank)
{
    // MPI read function

    MPI_File fh;
    MPI_Status status;
    MPI_Offset sz;
    MPI_Offset start_point, read_count, interval_count;

    // MPI_COMM_WORLD const
    // testing if the output to be 0

    int open_code = MPI_File_open(MPI_COMM_WORLD,fname,MPI_MODE_RDONLY, MPI_INFO_NULL, &fh); // read the file
    int size_code = MPI_File_get_size(fh, &sz); // get the file size

//    MPI_Comm_size(MPI_COMM_WORLD, &node_size); // get the node size
//    MPI_Comm_rank(MPI_COMM_WORLD, &node_rank); // get the each node rank

    // update the list of parse position
    interval_count = sz / node_size; // how big is each interval
    start_point = interval_count * node_rank;
    read_count = node_rank == node_size - 1 ? sz-(node_size-1) * interval_count : interval_count; // partition the read count

//    std::cout << "starting piont :" << start_point << std::endl;
//    std::cout << "node_size :" << node_size << std::endl;
//    std::cout << "read count :" << read_count << std::endl;

    pbuff = new char[read_count + 1]; // reason is to manually add a stopping criteria - very important

    int ret = MPI_File_read_at(fh,start_point,(void *)pbuff, read_count, MPI_BYTE, &status);

    MPI_File_close(&fh);
    // Testing of MPI read file properly
    if (ret) {
        std::cout << "mpi read error code:" << ret;
        delete[] pbuff;
        return ret;
    }

    pbuff[read_count] = 0; // 0 is an integer, so any char wont equal to this

    return 0;
    // function processing buffer


}


// input list: file name, write_buff ( current vectors changed to buffer -> to be written), current buff size, current rank number, total node numbers)
int mpi_write(char * fname, char* &pbuff, long long current_size, int rank, int nodes) {
    int ret;
    MPI_File fh;
    MPI_Status status;

    MPI_Offset *total_size = new MPI_Offset[nodes]; // number of nodes, store the size of all nodes


    // combine all the nodes size information to be used to share across
    MPI_Allgather(&current_size, 1, MPI_LONG, total_size, 1, MPI_LONG, MPI_COMM_WORLD);

//    std::cout << nodes << std::endl;

    for (int i = 1; i < nodes; i++) {
        total_size[i] += total_size[i-1]; // update each node results
    }
    for (int i = 0; i < nodes; i++) {
        std::cout << "rank number:" << i << std::endl;
        std::cout << "rank size" << total_size[i] << std::endl;
    }

//    std::cout << "total size is:" << total_size[0] << std::endl;

    // use MPI file open command, if exists, directly overwrite, o/w create a new
    ret = MPI_File_open(MPI_COMM_WORLD, fname, MPI_MODE_WRONLY|MPI_MODE_CREATE, MPI_INFO_NULL, &fh);

    //getting the file size
    ret = MPI_File_set_size(fh, 0);
    if (ret) {
        std::cout << "file open error" << std::endl;
        return 0;
    }

    ret = MPI_File_write_at_all(fh, total_size[rank] - current_size, (void*)pbuff, current_size, MPI_BYTE, &status);

    if (ret) {
        std::cout << "file write error" << std::endl;
        return 0;
    }

    MPI_File_close(&fh);
    return 0;
}

#endif //BDIF_A_MPI_UTILITY_H
