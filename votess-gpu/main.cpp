#include <votess.hpp>

#include <json.hpp>
using nlohmann::json;

#include <param.hpp>
#include <generate_uniform_dataset.hpp>
#include <measure_execution_time.hpp>

int
main(void)
{
        {

        sycl::queue q;

        sycl::device device = q.get_device();
        std::cout << "Queue Information:\n";
        std::cout << " Device Name: "
                  << device.get_info<sycl::info::device::name>()
                  << "\n";
        std::cout << " Vendor: "
                  << device.get_info<sycl::info::device::vendor>()
                  << "\n";
        std::cout << " Device Type: ";
        switch (device.get_info<sycl::info::device::device_type>()) {
                case sycl::info::device_type::cpu:
                        std::cout << "CPU"; break;
                case sycl::info::device_type::gpu:
                        std::cout << "GPU"; break;
                case sycl::info::device_type::accelerator:
                        std::cout << "Accelerator"; break;
                default:
                        std::cout << "Unknown";
        }
        std::cout << "\n";
        std::cout << "  Max Compute Units: "
                  << device.get_info<sycl::info::device::max_compute_units>()
                  << "\n";
        std::cout << "  Global Memory: "
                  << device.get_info<sycl::info::device::global_mem_size>()
                    / (1024 * 1024)
                  << " MB\n";
        std::cout << "  Local Memory: "
                  << device.get_info<sycl::info::device::local_mem_size>()
                    / 1024
                  << " KB\n";
        std::cout << "        Max Work Group Size: "
                  << device.get_info<sycl::info::device::max_work_group_size>()
                  << "\n";

        }

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
                
                // to remove first run
                {
                        for (auto& v : vec) {
                                generate_uniform_dataset(v.begin(), v.size());
                        }
                        votess::vtargs vtargs;
                        vtargs["k"] = k;
                        vtargs["knn_grid_resolution"] = gr;
                        auto dnn = votess::tesellate<Ti, Tf>(
                                vec, vtargs, votess::device::gpu
                        );
                }

                for (I iter_i{0}; iter_i < imax; ++iter_i) {

                        std::cout << "[iter]: " << iter_i << std::endl;

                        for (auto& v : vec) {
                                generate_uniform_dataset(v.begin(), v.size());
                        }

                        auto time_votess_gpu = measure_execution_time([&]{
                                votess::vtargs vtargs;
                                vtargs["k"] = k;
                                vtargs["knn_grid_resolution"] = gr;
                                auto dnn = votess::tesellate<Ti, Tf>(
                                        vec, vtargs, votess::device::gpu
                                );
                        });

                        std::cout << "\t[votess][gpu] time: " 
                                  << time_votess_gpu 
                                  << std::endl;

                        j_data["votess (GPU)"]
                              ["run:" + std::to_string(n)]
                              .push_back(
                                time_votess_gpu
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
                            "profile__votess-gpu__" + std::string(timestamp)
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
