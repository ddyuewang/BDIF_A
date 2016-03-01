//
// Created by Yue Wang on 2/29/16.
//

//int main()
//{
//    vector<int> vec1 { 10, 20, 30, 40, 50, 60, 70, 80, 90};
//    vector<string> vec2 { "10", "20", "30", "40", "50", "60", "70", "80", "90"};
//    std::vector<int>:: iterator it;
//    std::vector<string>:: iterator it1;
//
//    it = vec1.begin();
//    it1 = vec2.begin();
//
//    cout << *(++it1) << endl;
//
//    it1++;
//    bool test1 = it1 == vec2.begin();
//    cout << test1 << endl;
//    cout << *it1 << endl;
////    it1--;
////    bool test2 = it1 == vec2.begin();
////    cout << test2 << endl;
//
////    std::vector<string>::iterator tmp;
////    tmp = it1;
////    tmp++;
////    std::cout << *tmp << endl;
////    std::cout << *it1 << endl;
////    tmp++;
////    std::cout << *tmp << endl;
//
//    cout << abs(5-2) <<endl;
//    return 0;
//}

//int main()
//{
////    long old_precision = std::cout.precision();
////    cout << old_precision << endl;
//    cout.precision(15);
//    char test[] = "20140804:10:00:00.961882,1331.45,647567\n20140804:10:00:00.282531,1428.00,343729\n20140804:10:00:00.369841,1270.41,277335\n\0";
//    vector<record> res = buff_record(test);
//    std::cout <<  res[0].time << endl;
//    std::cout <<  time_converter(res[0].time) << endl;
////    for (auto i = res.begin(); i != res.end(); ++i)
////        std::cout << i->price << "|" << i->time << "|" << i-> volume << '\n';
//
//    return 0;
//}

/*__________________________ WRITE ____________________________*/

//    int MPI_File_write_at(MPI_File fh, MPI_Offset offset, const void *buf,
//                          int count, MPI_Datatype datatype, MPI_Status *status);
//
//    // initialize MPI
//    // MPI_Init(&argc,&argv); // only once required
//
//    // MPI read function
//    MPI_File fh;
//    MPI_Offset sz;
//    MPI_Offset start_point, read_count, interval_count;
//    int node_size, node_rank;
//    MPI_Status status;
//
//    // need gather function to connect the uneven piece of different blocks
//
//    // testing if the output to be 0
//
//    MPI_File_open(MPI_COMM_WORLD,"write.txt",MPI_MODE_WRONLY, MPI_INFO_NULL, &fh); // read the file
//    MPI_File_set_size(fh, 0); // set the file size
//
//    MPI_Comm_size(MPI_COMM_WORLD, &node_size); // get the node size
//    MPI_Comm_rank(MPI_COMM_WORLD, &node_rank); // get the each node rank
//
//    // update the list of parse position
//    interval_count = sz / node_size; // how big is each interval
//    start_point = interval_count * node_rank;
//    read_count = node_rank == node_size - 1 ? sz-(node_size-1) * interval_count : interval_count;
//
//    char * pbuff = new char[read_count + 1]; // reason is to manually add a stopping criteria
//    MPI_File_read_at(fh,start_point,(void *)pbuff, read_count, MPI_BYTE, &status);
//    pbuff[read_count] = 0; // 0 is an integer, so any char wont equal to this
//
//    // function processing buffer
//    cout << pbuff << endl;


/*__________________________ READ ____________________________*/