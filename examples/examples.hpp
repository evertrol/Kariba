#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fenv.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdarg.h>
#include <string>
#include <vector>

#ifdef _OPENMP
#include <omp.h>
#endif

#include <gsl/gsl_const_cgsm.h>
#include <gsl/gsl_const_num.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_spline.h>

void plot_write(size_t size, const std::vector<double>& en, const std::vector<double>& lum,
                const std::string& path, double redshift);
void plot_write(size_t size, const std::vector<double>& p, const std::vector<double>& g,
                const std::vector<double>& pdens, const std::vector<double>& gdens,
                const std::string& path);

void sum_zones(size_t size_in, size_t size_out, std::vector<double>& input_en,
               std::vector<double>& input_lum, std::vector<double>& en, std::vector<double>& lum);
void sum_ext(size_t size_in, size_t size_out, const std::vector<double>& input_en,
             const std::vector<double>& input_lum, std::vector<double>& en,
             std::vector<double>& lum);
double integrate_lum(size_t size, double numin, double numax, const std::vector<double>& input_en,
                     const std::vector<double>& input_lum);
double photon_index(size_t size, double numin, double numax, const std::vector<double>& input_en,
                    const std::vector<double>& input_lum);

void clean_file(const std::string& path, bool check);
void read_params(const std::string& path, std::vector<double>& pars);
