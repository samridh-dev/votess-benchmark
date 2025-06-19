#include <voro++.hh>
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

        std::ifstream fp0("param.json");
        json j;
        fp0 >> j;
        const auto k    { j.at("k"    ).get<I>()              };
        const auto gr   { j.at("gr"   ).get<I>()              };
        const auto imax { j.at("imax" ).get<I>()              };
        const auto N    { j.at("N"    ).get<std::vector<I>>() };

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

                        auto time_voro = measure_execution_time([&]{

                                using namespace voro;

                                std::vector<std::vector<Ti>> nn;
                                nn.reserve(n);

                                container con(
                                        0, 1, 0, 1, 0, 1,
                                        24, 24, 24,
                                        false, false, false,
                                        n
                                );

                                for (I i{0}; i < n; ++i) {
                                        con.put(
                                                i,
                                                vec[i][0],
                                                vec[i][1],
                                                vec[i][2]
                                        );
                                }

                                c_loop_all cl(con);
                                voronoicell_neighbor c;
                                if (cl.start()) do if (con.compute_cell(c, cl)) 
                                {

                                        std::vector<Ti> nni;
                                        std::vector<Ti> fnni;

                                        std::vector<double> fareas;
                                        double x, y, z;

                                        cl.pos(x, y, z);
                                        c.neighbors(nni);
                                        c.face_areas(fareas);

                                        for (I i{0}; i < fareas.size(); i++) {
                                                if (fareas[i] >= tolerance) {
                                                        fnni.push_back(nni[i]);
                                                }
                                        }

                                        nn.push_back(fnni);

                                } while(cl.inc());
                 
                        });

                        std::cout << "\t[voro++]      time: " 
                                  << time_voro       
                                  << std::endl;

                        j_data["voro++"]
                              ["run:" + std::to_string(n)]
                              .push_back(
                                time_voro
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
                            "profile__voropp__" + std::string(timestamp)
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
