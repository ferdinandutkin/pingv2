
#include "echo.h"



#include <chrono>
#include <iostream>
#include <numeric>
#include <array>



using  namespace std::literals::chrono_literals;


int main(int argc, char** argv) {

    if (argc > 1) {
        wsa init;
        icmpfile file;

       

        try {
            host_entry host = {argv[1]};


            for (int i{}; i < host.ip_count; i++) {

                std::string payload = "test payload";
                std::cout << "Pinging " << host <<
                    " [" << host[i] << "] with " << payload.size() << " bytes of data" << std::endl << std::endl;


                std::array<long long, 4> time;

                for (int j{}; j < 4; j++) {

                    auto reply = file.send_echo(host[i], payload, 1000ms);


                    std::cout << "Reply from " << reply.address << ": "
                        << "bytes = " << reply.data_size << "  "
                        << "time = " << reply.time.count() << "ms "
                        << "TTL = " << reply.TTL << std::endl;

                    time[j] = reply.time.count();

                }
                std::cout << "Average time = " << std::accumulate(time.begin(), time.end(), 0.0) / 4 << "ms" << std::endl << std::endl;

            }


        }

        catch (std::exception e) {
            std::cerr << e.what();

        }

    }


}