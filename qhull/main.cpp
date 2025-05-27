#include "libqhullcpp/Qhull.h"
#include "libqhullcpp/QhullPoint.h"
#include "libqhullcpp/QhullVertex.h"
#include "libqhullcpp/QhullVertexSet.h"

#include <iostream>
#include <fstream>

#include <json.hpp>
using nlohmann::json;

#include <param.hpp>
#include <generate_uniform_dataset.hpp>
#include <measure_execution_time.hpp>

int
main(void)
{

        std::vector<I> N;

        I num_points = (N1 - N0) / dN + 1;
        for (int i = 0; i < num_points; ++i) {
                N.push_back(static_cast<I>(std::round( std::pow(
                        10,
                        std::log10(N0) + i * (std::log10(N1) - std::log10(N0))
                        / (num_points - 1)
                ))));
        }

        json j_data;
        j_data["metadata"]["iterations"] = imax;
        j_data["metadata"]["N"]  =  N;
        j_data["metadata"]["k"]  =  k;
        j_data["metadata"]["gr"] = gr;

        for (auto n : N) {

                std::cout << "[N]" << n << std::endl;

                std::vector<std::array<Tf, 3>> vec(n);

                for (I iter_i{0}; iter_i < imax; ++iter_i) {

                        std::cout << "[iter]: " << iter_i << std::endl;

                        for (auto& v : vec) {
                                generate_uniform_dataset(v.begin(), v.size());
                        }

                        auto time_qhull = measure_execution_time([&]{

                                const int dim         = 3;
                                const I   npts  = static_cast<I>(vec.size());

                                std::vector<double> pts;
                                pts.reserve(npts * dim);
                                for (auto const &p : vec) {
                                pts.push_back(static_cast<double>(p[0]));
                                pts.push_back(static_cast<double>(p[1]));
                                pts.push_back(static_cast<double>(p[2]));
                                }

                                orgQhull::Qhull qh;
                                qh.runQhull("", dim, npts, pts.data(), "d Qt");

                        });

                        std::cout << "\t[qhull]      time: " 
                                  << time_qhull       
                                  << std::endl;

                        j_data["qhull"]
                              ["run:" + std::to_string(n)]
                              .push_back(
                                time_qhull
                        );

                }
        }

        std::cout << j_data.dump(8) << std::endl;

        std::time_t now = std::time(nullptr);
        char timestamp[20];
        std::strftime(
                timestamp, sizeof(timestamp),
                "%Y%m%d%H%M%S", std::localtime(&now)
        );

        std::string fname = "data/"
                            "profile__qhull__" + std::string(timestamp)
                          + ".json";

        std::cout << "JSON file to write to: " << fname << std::endl;

        std::ofstream fp(fname);
        if (!fp.is_open()) {
                std::cerr << "\033[31mError:\033[0m "
                          << "Unable to open file: " << fname << std::endl;
                std::abort();
        }

        fp << j_data.dump(8);
        fp.close();

}
