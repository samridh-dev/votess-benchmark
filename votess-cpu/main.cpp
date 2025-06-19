#include <param.hpp>

#include <votess.hpp>

#include <json.hpp>
using nlohmann::json;

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

                        auto time_votess_cpu = measure_execution_time([&]{
                                votess::vtargs vtargs;
                                vtargs["k"] = k;
                                vtargs["knn_grid_resolution"] = gr;
                                auto dnn = votess::tesellate<Ti, Tf>(
                                        vec, vtargs, votess::device::cpu
                                );
                        });

                        std::cout << "\t[votess][cpu] time: " 
                                  << time_votess_cpu 
                                  << std::endl;

                        j_data["votess (CPU)"]
                              ["run:" + std::to_string(n)]
                              .push_back(
                                time_votess_cpu
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
                            "profile__votess-cpu__" + std::string(timestamp)
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
