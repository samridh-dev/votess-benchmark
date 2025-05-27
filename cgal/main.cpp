#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Delaunay_triangulation_cell_base_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>

#include <iostream>
#include <fstream>

#include <json.hpp>
using nlohmann::json;

#include <param.hpp>
#include <generate_uniform_dataset.hpp>
#include <measure_execution_time.hpp>

typedef CGAL::Exact_predicates_inexact_constructions_kernel     K;
typedef CGAL::Triangulation_vertex_base_with_info_3<unsigned,K> Vb;
typedef CGAL::Delaunay_triangulation_cell_base_3<K>             Cb;
typedef CGAL::Triangulation_data_structure_3<Vb,Cb>             Tds;
typedef CGAL::Delaunay_triangulation_3<K,Tds>                   Delaunay;
typedef Delaunay::Point                                         Point;


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

                {
                        for (auto& v : vec) {
                                generate_uniform_dataset(v.begin(), v.size());
                        }

                        std::vector<std::pair<Point,unsigned>> pts;
                        pts.reserve(n);
                        for(unsigned i=0; i<n; ++i){
                                const auto& a = vec[i];
                                pts.emplace_back(Point(a[0], a[1], a[2]), i);
                        }

                        Delaunay dt(pts.begin(), pts.end());

                }

                for (I iter_i{0}; iter_i < imax; ++iter_i) {

                        std::cout << "[iter]: " << iter_i << std::endl;

                        for (auto& v : vec) {
                                generate_uniform_dataset(v.begin(), v.size());
                        }

                        std::vector<std::pair<Point,unsigned>> pts;
                        pts.reserve(n);
                        for(unsigned i=0; i<n; ++i){
                                const auto& a = vec[i];
                                pts.emplace_back(Point(a[0], a[1], a[2]), i);
                        }

                        std::vector<std::vector<unsigned>> neighbors(n);

                        auto time_cgal = measure_execution_time([&]{
                                Delaunay dt(pts.begin(), pts.end());

                                for(auto vit = dt.finite_vertices_begin(); 
                                         vit != dt.finite_vertices_end();
                                         ++vit) {

                                        auto vh = vit;
                                        unsigned i = vh->info();

                                        std::vector<Delaunay::Vertex_handle> inc;
                                        
                                        dt.incident_vertices(
                                                        vh,
                                                        std::back_inserter(inc)
                                        );

                                        for(auto w: inc) {
                                            if(dt.is_infinite(w)) continue;
                                            neighbors[i].push_back(w->info());
                                        }

                                }

                        });

                        std::cout << "\t[CGAL]      time: " 
                                  << time_cgal       
                                  << std::endl;

                        j_data["CGAL"]
                              ["run:" + std::to_string(n)]
                              .push_back(
                                time_cgal
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
                            "profile__cgal__" + std::string(timestamp)
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
