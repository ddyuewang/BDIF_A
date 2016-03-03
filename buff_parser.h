#include <vector>
#include <string>
#include <iostream>
#include <deque>
#include <cmath>

#ifndef BDIF_A_BUFF_PARSER_H
#define BDIF_A_BUFF_PARSER_H

using string = std::string;

// struct to store the record
struct record
{
    std::string time;
    std::string price;
    std::string volume;
    string prit() {
        return time+","+price+","+volume;
    }
};

int bug_number = 23460;
// declare function signature
int test_time_neighbor(long curr, std::deque<record> neighbor);
bool test_price_neighbor(double curr, double prev, double next);
int update_JB_factor(std::vector<record> & signal_output, std::vector<double> & return_output,
                     double & new_price, double & sum, double & sum_square,
                     double & sum_cubic, double & sum_quadruple);

std::vector<record> buff_record(char buff[], int rank)
{
    int idx = 0;
    std::vector<record> record_vec; // result

    char * tracker = buff; // init the iterator of buff

    /* the first line, it might be broken based on the cut
       throw it, parse the data to the next line */

    while( (*tracker) !=  0 && (*tracker) != '\n')
    { tracker++ ;
    }
    tracker++;
    //std::cout << (*tracker) << std::endl;

    /* deal with the actual block buffer*/
    while((*tracker) != 0 )
    {
        //std::cout << "rank:" <<  rank << " idx == " << ++idx << std::endl;
        std::string tmp_time, tmp_price, tmp_volume;

        // get the time
        while((*tracker) != ',' && (*tracker) != 0 )
        {
            tmp_time += *tracker;
            tracker++;
        }
        if((*tracker) ==0)
        {break;}
        tracker++;

        // get the price - use stod to cast from string to float
        while((*tracker) != ',' && (*tracker) != 0)
        {
            tmp_price += *tracker;
            tracker++;
        }
        if((*tracker) ==0)
        {break;}
        tracker++;

        // get the volume
        while((*tracker) != '\n' && (*tracker) != 0)
        {
            tmp_volume += *tracker;
            tracker++;
        }

        if((*tracker) ==0)
        {break;}
        tracker++;

        // append the data to the vector
        record tmp_rec;
        tmp_rec.time = tmp_time;
//        tmp_rec.price = std::stod(tmp_price);
//        tmp_rec.volume = std::stol(tmp_volume);
        tmp_rec.price = tmp_price;
        tmp_rec.volume = tmp_volume;

        record_vec.push_back(tmp_rec);

    }
    std::cout << "Rank:" << rank << " buff_rec returned\n";
    return record_vec;
}

// a helper function to convert a time string to two parts － output those two things
int time_converter(std::string time_stamp, long & date, long & time)
{
    const char * tracker = time_stamp.c_str();

    long length = time_stamp.length();
    int counter = 0;

    std::string tmp_date, tmp_hour, tmp_min, tmp_sec, tmp_dec;
    long date_;
    double time_;

    // get the the date
    while((*tracker) != ':')
    {
        tmp_date += *tracker;
        tracker++;
        counter++;
    }
    tracker++;
    counter++;

    // get the hour
    while((*tracker) != ':')
    {
        tmp_hour += *tracker;
        tracker++;
        counter++;
    }
    tracker++;
    counter++;


    // get the min
    while((*tracker) != ':')
    {
        tmp_min += *tracker;
        tracker++;
        counter++;
    }
    tracker++;
    counter++;


    // get the sec
    while((*tracker) != '.')
    {
        tmp_sec += *tracker;
        tracker++;
        counter++;
    }
    tracker++;
    counter++;


    // get the tmp_sec
    while(counter != length)
    {
        tmp_dec += *tracker;
        tracker++;
        counter++;
    }
    double dec_ = std::stod(tmp_dec);
    while (dec_ > 1)
    {
        dec_ = dec_/10;
    }

    date = std::stol(tmp_date);
    time = std::stod(tmp_hour) * 3600 + std::stod(tmp_min) * 60 + std::stod(tmp_sec) + dec_;


    return 0;
}

// output theorem－ keep track the sum, sum^2, sum^3, sum^4 for JB

/*std::vector<record> signal_output;
std::vector<record> noise_output;
std::vector<double> return_output;
output sum, sum_square,sum_cubic, sum_quadruple
double sum;
double sum_square;
double sum_cubic;
double sum_quadruple;*/


/*_________________ implementation of main SCRUB function _________________*/
int SCRUB(int rank, std::vector<record> buff, std::vector<record> & signal_output,
          std::vector<record> & noise_output, std::vector<double> & return_output,
          double & sum, double & sum_square, double & sum_cubic, double & sum_quadruple)
{
    std::vector<record>::iterator buff_iter; // current buffer
    std::deque<record> neighb_front; // front 10 observations
    std::deque<record> neighb_back; // back 10 observations

    /* __________ initialize the neigb_front & neigb_back ____________ */
    std::vector<record>::iterator tmp_front, tmp_back;
    buff_iter = buff.begin();

    tmp_front = buff_iter;
    tmp_back = buff_iter;
    tmp_back++;

    // counter for neighborhood
    int counter_front = 10;
    int counter_back = 10;

    // initialize the front and end of sliding windows
    while(counter_front != 0 && tmp_front != buff.begin())
    {
        neighb_front.push_back(*tmp_front);
        //std::cout << (*tmp_front).time << std::endl;
        counter_front--;
        tmp_front--;
    }

    while(counter_back != 0 && tmp_back != buff.end())
    {
        neighb_back.push_back(*tmp_back);
        //std::cout << (*tmp_back).time << std::endl;
        counter_back--;
        tmp_back++;
    }


    // current record, front neighb, back neighb
    // need a neighborhood test, given a current data, front and back neighbour hood

    long date_prev, date_current, date_next;
    long time_prev, time_current, time_next;
    double price_prev, price_after;
    bool front_boundary, back_boundary;
    front_boundary = false;
    back_boundary = false;

    std::cout << "Rank" << rank << " Inited\n";
    /*________________ Finish the initialization __________________*/

    record current_record = *buff_iter; //
    int iddxx = 0;
// start of the while counter for the current element
    int temp_iter = 10;
//    while(buff_iter != buff.end() && temp_iter !=0) {
    while(buff_iter != buff.end()) {

            temp_iter--;
        iddxx++;
        if (rank ==2 && iddxx > bug_number) {
            std::cout << "rank:" << rank << " idx:" << iddxx << " out of " << buff.size()<< std::endl;
            std::cout << buff_iter->prit() << "\n";
            std::cout << "front size is:" << neighb_front.size() << std::endl;
            std::cout << "back size is:" << neighb_back.size() << std::endl;

        }

        time_converter(current_record.time, date_current, time_current);

        if (neighb_front.size() != 0) {
            time_converter(neighb_front[0].time, date_prev, time_prev);
            price_prev = std::stod(neighb_front[0].price);
        }
        else {
            front_boundary = true;
            price_prev = 0.0;
        }

        if (neighb_back.size() != 0) {
            time_converter(neighb_back[0].time, date_next, time_next);
            price_after = std::stod(neighb_back[0].price);
        }
        else {
            back_boundary = true;
            price_after = 0.0;
        }

        /*___________________ price and volume filter_____________________*/
        // testing from the price and volume - concurrently update the current to the correct pos

//        std::cout << "current price:" << current_record.price << std::endl;
        if (!test_price_neighbor(std::stod(current_record.price), price_prev, price_after) || std::stol(current_record.volume) < 0) {

            if (rank ==2 && iddxx > bug_number) {
            std::cout << "whats happening:" << std::endl;}

            noise_output.push_back(current_record);
//            std::cout << "passed price is:" << current_record.price << std::endl;

            if (neighb_back.size() != 0) {
                current_record = neighb_back[0]; // update the buff to next position
                neighb_back.pop_front(); // get out the the first element
                buff_iter++;
            }
            else
            {
                return 0;
            }

            if (tmp_back != buff.end()) { // when its not reaching the end
                neighb_back.push_back(*tmp_back); // update the later sliding windows
                tmp_back++;
            }

            continue;

        }
//        /*___________________ date time filter ______________________*/
//        // first layer testing: if current date diff from the prev and current date - bad data

        if (rank ==2 && iddxx > bug_number) {
            std::cout << "size of the front queue is: " << neighb_front.size() << std::endl;
            std::cout << "size of the back queue is: " << neighb_back.size() << std::endl;

            if (neighb_front.size() != 0) {
                std::cout << "print the queue informtion:" << std::endl;
                std::cout << "print out the front queue : " << neighb_front.size() << std::endl;
                for (std::deque<record>::iterator it = neighb_front.begin(); it != neighb_front.end(); ++it) {
                    std::cout << it->time << "|" << it->price << "|" << it->volume << "||||||||" ;
                }
            }
            if (neighb_back.size() != 0) {
                std::cout << std::endl << "---------------" << std::endl;
                std::cout << "print out the back queue : " << neighb_back.size() << std::endl;
                for (std::deque<record>::iterator it = neighb_back.begin(); it != neighb_back.end(); ++it) {
                    std::cout << it->time << "|" << it->price << "|" << it->volume << "||||||||" ;
                }
            }
        }

//        if (neighb_front.size() != 0) {
//            std::cout << "print the queue informtion:" << std::endl;
//            std::cout << "print out the front queue : " << neighb_front.size() << std::endl;
//            for (std::deque<record>::iterator it = neighb_front.begin(); it != neighb_front.end(); ++it) {
//                std::cout << it->time << "|" << it->price << "|" << it->volume << "||||||||" ;
//            }
//        }
//        if (neighb_back.size() != 0) {
//            std::cout << std::endl << "---------------" << std::endl;
//            std::cout << "print out the back queue : " << neighb_back.size() << std::endl;
//            for (std::deque<record>::iterator it = neighb_back.begin(); it != neighb_back.end(); ++it) {
//                std::cout << it->time << "|" << it->price << "|" << it->volume << "||||||||" ;
//            }
//        }

        /* check the front boundary case */
        if (rank ==2 && iddxx > bug_number) {
            std::cout << "front boundary is:" << front_boundary << std::endl;
        }

        /*_______________________________________ FRONT BOUNDARY _______________________________________*/
        if (front_boundary) {
            
            if (rank ==2 && iddxx > bug_number) {
                std::cout << "front boundary case" <<  std::endl;
            }
            if (date_current == date_next && std::abs(time_next - time_current) < 5) {

                if (rank ==2 && iddxx > bug_number) {
                std::cout << "front boundary: current = next, lag < 5s" << std::endl;}
                
//                std::cout << date_next << "|" << date_current << std::endl;
//                std::cout << time_next << "|" << time_current << std::endl;

                double current_price = std::stod(current_record.price);
                update_JB_factor(signal_output, return_output, current_price, sum, sum_square,
                                     sum_cubic, sum_quadruple);

                signal_output.push_back(current_record);
                neighb_front.push_front(current_record); // no need to pop the neighb_front, as its empty
                neighb_back.push_back(*tmp_back); // update the later sliding windows
                tmp_back++;

                current_record = neighb_back[0]; // update the buff to next position
                neighb_back.pop_front(); // get out the the first element
                buff_iter++;

//                if (neighb_back.size() != 0) {
//                    current_record = neighb_back[0]; // update the buff to next position
//                    neighb_back.pop_front(); // get out the the first element
//                    buff_iter++;
//                }
//                else
//                {
//                    break;
//                }

                front_boundary = false;
                continue;
            }
            else {
                noise_output.push_back(current_record);
                if (rank ==2 && iddxx > bug_number) {
                    std::cout << "front boundary: current != next or lag > 5s" << std::endl;
                }
//                std::cout << date_next << "|" << date_current << std::endl;
//                std::cout << time_next << "|" << time_current << std::endl;

                // update the current cursor
                neighb_back.push_back(*tmp_back); // update the later sliding windows
                tmp_back++;
                current_record = neighb_back[0]; // update the buff to next position
                neighb_back.pop_front(); // get out the the first element
                buff_iter++;

//                if (neighb_back.size() != 0) {
////                    std::cout << " neighb_back is not zero " << std::endl;
//                    current_record = neighb_back[0]; // update the buff to next position
//                    neighb_back.pop_front(); // get out the the first element
//                    buff_iter++;
//                }
//                else
//                {
//                    break;
//                }
                front_boundary = false;
                continue;
            }
        }
        /*_______________________________________ BACK BOUNDARY _______________________________________*/
        /* check the back boundary case */
        if (rank ==2 && iddxx > bug_number) {
            std::cout << "back boundary is:" << back_boundary << std::endl;
        }
        if (back_boundary) {
            if (rank ==2 && iddxx > bug_number) {
            std::cout << "back boundary case" << std::endl;}
            if (date_current == date_prev && std::abs(time_next - time_current) < 5) {
                if (rank ==2 && iddxx > bug_number) {
                std::cout << "back boundary: current = prev or lag <5s" << std::endl;}
//                std::cout << date_prev << "|" << date_current << std::endl;
//                std::cout << time_prev << "|" << time_current << std::endl;
                double current_price = std::stod(current_record.price);
                update_JB_factor(signal_output, return_output, current_price, sum, sum_square,
                                 sum_cubic, sum_quadruple);
                signal_output.push_back(current_record);
            }
            else {
                //finish the program
                noise_output.push_back(current_record);
                if (rank ==2 && iddxx > bug_number) {
                std::cout << "back boundary: current != prev or lag >5s" << std::endl;}
//                std::cout << date_prev << "|" << date_current << std::endl;
//                std::cout << time_prev << "|" << time_current << std::endl;

            }
            return 0;
        }


        if (rank ==2 && iddxx > bug_number) {
        std::cout << " if boundary boundary condition being met or not :" << (!front_boundary && !back_boundary) << std::endl;}

        // regular case handling
        /*_______________________________________ REGULAR CASE _______________________________________*/

        if (!front_boundary && !back_boundary) {
            if (rank ==2 && iddxx > bug_number) {
            std::cout << "regular boundary case" << std::endl;}
            if (date_current != date_prev && date_current != date_next) {
                if (rank ==2 && iddxx > bug_number) {
                std::cout << "regular boundary case: each not equal" << std::endl;}
                noise_output.push_back(current_record);
                // update the current cursor
                if (neighb_back.size() != 0) {
                    current_record = neighb_back[0]; // update the buff to next position
                    neighb_back.pop_front(); // get out the the first element
                    buff_iter++;
                }
                else
                {
                    buff_iter = buff.end();
                    return 0;
                }

                if (tmp_back != buff.end()) { // when its not reaching the end
                    neighb_back.push_back(*tmp_back); // update the later sliding windows
                    tmp_back++;
                }

                continue;

            }
            else if (date_current != date_prev) {
                if (rank ==2 && iddxx > bug_number) {
                std::cout << "regular boundary case: current !=prev" << std::endl;}
                // neighborhood check for the forward data
                if (test_time_neighbor(time_current, neighb_back)) {
                    if (rank ==2 && iddxx > bug_number) {
                    std::cout << "regular boundary case: current !=prev: time within" << std::endl;}
//                    std::cout << time_next << "|" << time_current << std::endl;

                    double current_price = std::stod(current_record.price);
                    update_JB_factor(signal_output, return_output, current_price, sum, sum_square,
                                     sum_cubic, sum_quadruple);
                    signal_output.push_back(current_record);

                    // update the front
                    if (neighb_front.size() != 0) {
                        neighb_front.pop_back(); // get out the the first element
                    }
                    neighb_front.push_front(current_record);


                    // update the current record and cursor
                    if (neighb_back.size() != 0) {
                        current_record = neighb_back[0]; // update the buff to next position
                        neighb_back.pop_front(); // get out the the first element
                        buff_iter++;
                    }
                    else
                    {
                        buff_iter = buff.end();
                        return 0;
                    }

                    // update the back
                    if (tmp_back != buff.end()) { // when its not reaching the end
                        neighb_back.push_back(*tmp_back); // update the later sliding windows
                        tmp_back++;

                    }

                    continue;
                }
                else {
                    if (rank ==2 && iddxx > bug_number) {
                    std::cout << "regular boundary case: current !=prev: time not" << std::endl;}
//                    std::cout << time_next << "|" << time_current << std::endl;

                    noise_output.push_back(current_record);


                    if (neighb_back.size() != 0) {
                        current_record = neighb_back[0]; // update the buff to next position
                        neighb_back.pop_front(); // get out the the first element
                        buff_iter++;
                    }
                    else
                    {
                        buff_iter = buff.end();
                        return 0;
                    }

                    if (tmp_back != buff.end()) { // when its not reaching the end
                        neighb_back.push_back(*tmp_back); // update the later sliding windows
                        tmp_back++;
                    }
                    continue;
                }

            }
            else if (date_current != date_next) {
                if (rank ==2 && iddxx > bug_number) {
                std::cout << "regular boundary case: current !=next" << std::endl;}
                // neighborhood check for the backward data
                if (test_time_neighbor(time_current, neighb_back)) {
                    if (rank ==2 && iddxx > bug_number) {
                    std::cout << "regular boundary case: current !=next: time within" << std::endl;}
//                    std::cout << time_prev << "|" << time_current << std::endl;

                    double current_price = std::stod(current_record.price);
                    update_JB_factor(signal_output, return_output, current_price, sum, sum_square,
                                     sum_cubic, sum_quadruple);
                    signal_output.push_back(current_record);

                    // update the front
                    if (neighb_front.size() != 0) {
                        neighb_front.pop_back(); // get out the the first element
                    }
                    neighb_front.push_front(current_record);


                    // update the current record and back
                    if (neighb_back.size() != 0) {
                        current_record = neighb_back[0]; // update the buff to next position
                        neighb_back.pop_front(); // get out the the first element
                        buff_iter++;
                    }
                    else
                    {
                        buff_iter = buff.end();
                        return 0;
                    }

                    // update the back
                    if (tmp_back != buff.end()) { // when its not reaching the end
                        neighb_back.push_back(*tmp_back); // update the later sliding windows
                        tmp_back++;
                    }
                    continue;

                }
                else {
                    if (rank ==2 && iddxx > bug_number) {
                    std::cout << "regular boundary case: current !=next: time within" << std::endl;}
//                    std::cout << time_prev << "|" << time_current << std::endl;
                    noise_output.push_back(current_record);

                    if (neighb_back.size() != 0) {
                        current_record = neighb_back[0]; // update the buff to next position
                        neighb_back.pop_front(); // get out the the first element
                        buff_iter++;
                    }
                    else
                    {
                        buff_iter = buff.end();
                        return 0;
                    }

                    if (tmp_back != buff.end()) { // when its not reaching the end
                        neighb_back.push_back(*tmp_back); // update the later sliding windows
                        tmp_back++;

                    }

                    continue;
                }
            }
            // handle the case where current date = prev date = next date
            else
            {
                if (rank ==2 && iddxx > bug_number) {std::cout << "regular boundary case: current=next=previous" << std::endl;}
                if((std::abs(time_prev - time_current) < 5) && (std::abs(time_next - time_current) < 5))
                {
                    if (rank ==2 && iddxx > bug_number) {
                        std::cout << "regular boundary case: current=next=previous: lag within 5" << std::endl;}
//                    std::cout << time_prev << "|" << time_current << std::endl;

                    double current_price = std::stod(current_record.price);
                    update_JB_factor(signal_output, return_output, current_price, sum, sum_square,
                                     sum_cubic, sum_quadruple);
                    signal_output.push_back(current_record);

                    // update the front
                    if (neighb_front.size() != 0) {
                        neighb_front.pop_back(); // get out the the first element
                    }
                    neighb_front.push_front(current_record);

                    // update the current record and cursor
                    if (neighb_back.size() != 0) {
                        current_record = neighb_back[0]; // update the buff to next position
                        neighb_back.pop_front(); // get out the the first element
                        buff_iter++;
                    }
                    else
                    {
                        buff_iter = buff.end();
                        return 0;
                    }

                    // update the back
                    if (tmp_back != buff.end()) { // when its not reaching the end
                        neighb_back.push_back(*tmp_back); // update the later sliding windows
                        tmp_back++;
                    }
                    continue;
                }
                else
                {
                    if (rank ==2 && iddxx > bug_number) {
                        std::cout << "regular boundary case: current=next=previous: lag > 5" << std::endl;}
//                    std::cout << time_prev << "|" << time_current << std::endl;

                    noise_output.push_back(current_record);

                    if (neighb_back.size() != 0) {
                        current_record = neighb_back[0]; // update the buff to next position
                        neighb_back.pop_front(); // get out the the first element
                        buff_iter++;
                    }
                    else
                    {
                        buff_iter = buff.end();
                        return 0;
                    }

                    if (tmp_back != buff.end()) { // when its not reaching the end
                        neighb_back.push_back(*tmp_back); // update the later sliding windows
                        tmp_back++;
                    }

                    continue;
                }
            }
        }
    }
    return 0;
}

//  time test how close is it to its neighbor.
int test_time_neighbor(long curr, std::deque<record> neighbor)
{
    long total_vote = neighbor.size();
    int pos = 0;
    for (std::deque<record>::iterator it = neighbor.begin(); it!= neighbor.end(); it++)
    {
        long tmp_date, tmp_time;
        time_converter(it->time,tmp_date,tmp_time);
        if(std::abs(tmp_time - curr) < 20)
        {
            pos++;
        }
    }

    if (double(pos)/total_vote > 0.7)
        {return 0;}
    else
        {return 1;}
}

//  price test how close is it to its neighbor.
bool test_price_neighbor(double curr, double prev, double next)
{
    if (curr == 0.0f)
    {
        return false;
    }
    else {
        if (prev != 0.0 && next != 0.0) {
            return (std::fabs(curr / prev - 1.0) < 0.25) && (std::fabs(next / curr - 1.0) < 0.25);
        }
        else if (prev == 0.0) {
            return std::fabs(next / curr - 1.0) < 0.25;
        }
        else  {
            return std::fabs(curr / prev - 1.0) < 0.25;
        }
    }
}

// function to generate the return series and update the corresponding unit
int update_JB_factor(std::vector<record> & signal_output, std::vector<double> & return_output,
                     double & new_price, double & sum, double & sum_square,
                     double & sum_cubic, double & sum_quadruple)
{
    if (!signal_output.empty())
    {
        double current_return = new_price/std::stod(signal_output.back().price) - 1.0f;

        if (current_return != 0.0) {
            sum += double(current_return);
            sum_square += pow(double(current_return), 2);
            sum_cubic += pow(double(current_return), 3);
            sum_quadruple += pow(double(current_return), 4);
            return_output.push_back(current_return);
        }


    }
    else // here, the return moments are all zeros - no need to add 0.0^moment
    {
        return_output.push_back(0.0);
    }
    return 0;

}

#endif //BDIF_A_BUFF_PARSER_H
