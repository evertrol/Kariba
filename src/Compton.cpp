#include <cmath>
#include <iostream>

#include <gsl/gsl_integration.h>
#include <gsl/gsl_spline2d.h>

#include "kariba/Compton.hpp"
#include "kariba/Radiation.hpp"
#include "kariba/constants.hpp"

namespace kariba {

//! radiative transfer tables vs tau and temperature. The limits are set an
//! epsilon off the physical limits tested (20-2500 kev, tau 0.05 to 3) to avoid
//! occasional weird GSL interoplation bugs.
static double Te_table[15] = {19.999, 30.,  50.,  70.,  90.,   120.,  150.,    200.,
                              280.,   400., 600., 900., 1300., 1800., 2500.001};
static double tau_table[15] = {0.04999, 0.07, 0.1,  0.19, 0.27, 0.38, 0.54,  0.76,
                               0.94,    1.08, 1.34, 1.75, 2.1,  2.6,  3.0001};

static double esc_table_sph[225] = {
    0.02,  0.07,  0.17,  0.27,  0.33, 0.4,  0.45, 0.45, 0.55, 0.55, 0.54, 0.51, 0.48, 0.55, 0.55,
    0.02,  0.07,  0.18,  0.275, 0.33, 0.4,  0.45, 0.47, 0.55, 0.53, 0.56, 0.51, 0.5,  0.55, 0.55,
    0.02,  0.07,  0.18,  0.275, 0.34, 0.4,  0.45, 0.47, 0.55, 0.54, 0.52, 0.51, 0.53, 0.55, 0.58,
    0.025, 0.085, 0.18,  0.275, 0.35, 0.4,  0.45, 0.47, 0.55, 0.54, 0.59, 0.57, 0.5,  0.55, 0.6,
    0.028, 0.085, 0.2,   0.275, 0.35, 0.4,  0.45, 0.49, 0.54, 0.54, 0.6,  0.59, 0.53, 0.55, 0.65,
    0.035, 0.09,  0.21,  0.29,  0.35, 0.4,  0.45, 0.49, 0.55, 0.57, 0.58, 0.6,  0.53, 0.55, 0.7,
    0.045, 0.12,  0.23,  0.3,   0.35, 0.41, 0.42, 0.49, 0.54, 0.57, 0.58, 0.58, 0.5,  0.55, 0.7,
    0.055, 0.135, 0.23,  0.3,   0.35, 0.4,  0.42, 0.49, 0.52, 0.57, 0.59, 0.61, 0.58, 0.57, 0.85,
    0.07,  0.14,  0.235, 0.3,   0.35, 0.4,  0.41, 0.49, 0.5,  0.57, 0.58, 0.61, 0.61, 0.57, 0.85,
    0.08,  0.145, 0.235, 0.3,   0.35, 0.39, 0.4,  0.49, 0.5,  0.57, 0.58, 0.63, 0.7,  0.6,  0.85,
    0.09,  0.15,  0.245, 0.3,   0.35, 0.4,  0.4,  0.45, 0.49, 0.55, 0.62, 0.64, 0.73, 0.6,  0.85,
    0.105, 0.17,  0.26,  0.31,  0.35, 0.4,  0.4,  0.45, 0.47, 0.56, 0.62, 0.65, 0.78, 0.65, 0.85,
    0.115, 0.19,  0.28,  0.32,  0.35, 0.4,  0.4,  0.45, 0.44, 0.5,  0.67, 0.68, 0.8,  0.7,  1.,
    0.14,  0.215, 0.3,   0.34,  0.38, 0.4,  0.4,  0.45, 0.42, 0.48, 0.68, 0.73, 0.85, 0.8,  1.,
    0.16,  0.23,  0.31,  0.35,  0.38, 0.4,  0.4,  0.45, 0.42, 0.48, 0.67, 0.78, 0.9,  0.9,  1.,
};

static double esc_table_cyl[225] = {
    0.02,  0.06,  0.22,  0.3,  0.5,  0.5,  0.57, 0.7,  0.75, 0.75, 0.75, 0.77, 0.75, 0.76, 0.7,
    0.02,  0.06,  0.19,  0.28, 0.4,  0.5,  0.57, 0.65, 0.65, 0.75, 0.7,  0.75, 0.7,  0.7,  0.75,
    0.017, 0.04,  0.17,  0.27, 0.32, 0.5,  0.57, 0.6,  0.6,  0.7,  0.65, 0.65, 0.73, 0.67, 0.7,
    0.01,  0.04,  0.12,  0.17, 0.23, 0.35, 0.4,  0.55, 0.5,  0.55, 0.55, 0.65, 0.7,  0.67, 0.68,
    0.01,  0.04,  0.1,   0.17, 0.19, 0.28, 0.32, 0.4,  0.45, 0.5,  0.5,  0.5,  0.68, 0.66, 0.68,
    0.015, 0.04,  0.09,  0.17, 0.19, 0.25, 0.25, 0.35, 0.45, 0.4,  0.45, 0.5,  0.64, 0.64, 0.68,
    0.02,  0.05,  0.09,  0.15, 0.17, 0.22, 0.24, 0.3,  0.35, 0.35, 0.45, 0.4,  0.62, 0.61, 0.64,
    0.02,  0.05,  0.095, 0.15, 0.17, 0.22, 0.22, 0.25, 0.3,  0.35, 0.4,  0.38, 0.4,  0.4,  0.6,
    0.024, 0.05,  0.095, 0.14, 0.16, 0.21, 0.21, 0.25, 0.27, 0.32, 0.4,  0.42, 0.4,  0.4,  0.5,
    0.025, 0.055, 0.1,   0.15, 0.17, 0.21, 0.21, 0.25, 0.27, 0.3,  0.35, 0.4,  0.35, 0.4,  0.43,
    0.03,  0.065, 0.11,  0.15, 0.18, 0.21, 0.21, 0.25, 0.27, 0.3,  0.35, 0.4,  0.4,  0.45, 0.47,
    0.045, 0.085, 0.135, 0.17, 0.19, 0.22, 0.24, 0.27, 0.3,  0.35, 0.4,  0.45, 0.5,  0.5,  0.55,
    0.06,  0.1,   0.16,  0.2,  0.22, 0.25, 0.26, 0.29, 0.33, 0.35, 0.45, 0.5,  0.5,  0.55, 0.65,
    0.09,  0.14,  0.2,   0.23, 0.26, 0.29, 0.29, 0.33, 0.4,  0.4,  0.48, 0.55, 0.6,  0.65, 0.75,
    0.11,  0.165, 0.23,  0.26, 0.29, 0.31, 0.33, 0.36, 0.4,  0.45, 0.52, 0.6,  0.65, 0.75, 0.8,
};

Compton::~Compton() {
    gsl_spline2d_free(esc_p_sph);
    gsl_spline2d_free(esc_p_cyl);
    gsl_interp_accel_free(acc_Te);
    gsl_interp_accel_free(acc_tau);

    gsl_spline_free(seed_ph);
    gsl_interp_accel_free(acc_seed);

    gsl_spline_free(iter_ph);
    gsl_interp_accel_free(acc_iter);
}

Compton::Compton(size_t size, size_t seed_size)
    : Radiation(size), seed_energ(seed_size, 0.0), seed_urad(seed_size, 0.0), iter_urad(size, 0.0) {
    en_phot_obs.resize(en_phot_obs.size() * 2, 0.0);
    num_phot_obs.resize(num_phot_obs.size() * 2, 0.0);

    Niter = 20;
    ypar = 0;
    escape_corr = 1.;

    counterjet = false;

    acc_tau = gsl_interp_accel_alloc();
    acc_Te = gsl_interp_accel_alloc();
    esc_p_sph = gsl_spline2d_alloc(gsl_interp2d_bicubic, 15, 15);
    esc_p_cyl = gsl_spline2d_alloc(gsl_interp2d_bicubic, 15, 15);

    gsl_spline2d_init(esc_p_sph, Te_table, tau_table, esc_table_sph, 15, 15);
    gsl_spline2d_init(esc_p_cyl, Te_table, tau_table, esc_table_cyl, 15, 15);

    seed_ph = gsl_spline_alloc(gsl_interp_steffen, seed_energ.size());
    acc_seed = gsl_interp_accel_alloc();

    iter_ph = gsl_spline_alloc(gsl_interp_steffen, en_phot.size());
    acc_iter = gsl_interp_accel_alloc();
}

//! This function is the kernel of eq 2.48 in Blumenthal & Gould(1970),
//! represents the scattered photon spectrum for a given electron and includes
//! the Klein-Nishina cross section.
double comfnc(double ein, void* pars) {
    ComfncParams* params = static_cast<ComfncParams*>(pars);
    double game = params->game;
    double e1 = params->e1;
    gsl_spline* phodis = params->phodis;
    gsl_interp_accel* acc_phodis = params->acc_phodis;

    double einit, utst, biggam, q;
    double phonum;
    double tm1, tm2, tm3;
    double btst, eg4;

    einit = std::exp(ein);
    btst = einit / (game * constants::emerg);
    eg4 = 4. * einit * game;
    utst = eg4 / (constants::emerg + eg4);

    if ((e1 < btst && (btst - e1) / btst >= 4.e-8) || (e1 > utst && (e1 - utst) / utst >= 4.e-8)) {
        return 0.;
    } else {
        biggam = eg4 / constants::emerg;
        q = e1 / (biggam * (1. - e1));
        phonum = gsl_spline_eval(phodis, einit, acc_phodis);
        tm1 = 2. * q * std::log(q);
        tm2 = (1. + 2. * q) * (1. - q);
        tm3 = 0.5 * (std::pow(biggam * q, 2.) * (1. - q)) / (1. + biggam * q);
        return (tm1 + tm2 + tm3) * std::pow(10., phonum);
    }
}

//! This function is the integral of comfnc above over the total seed photon
//! distribution
double comint(double gam, void* pars) {
    ComintParams* params = static_cast<ComintParams*>(pars);
    double eph = (params->eph);
    double ephmin = (params->ephmin);
    double ephmax = (params->ephmax);
    gsl_spline* eldis = (params->eldis);
    gsl_interp_accel* acc_eldis = (params->acc_eldis);
    gsl_spline* phodis = (params->phodis);
    gsl_interp_accel* acc_phodis = (params->acc_phodis);

    double game, econst, blim, ulim, e1, den;
    double result, error;

    game = exp(gam);
    e1 = eph / (game * constants::emerg);
    econst = 2. * constants::pi * constants::re0 * constants::re0 * constants::cee;
    blim = std::log(std::max(eph / (4. * game * (game - eph / constants::emerg)), ephmin));
    ulim = std::log(std::min(eph, ephmax));

    if (ulim <= blim) {
        return 0;
    } else {
        gsl_integration_workspace* w2;
        w2 = gsl_integration_workspace_alloc(100);
        gsl_function F2;
        auto F2params = ComfncParams{game, e1, phodis, acc_phodis};
        F2.function = &comfnc;
        F2.params = &F2params;
        gsl_integration_qag(&F2, blim, ulim, 1e0, 1e0, 100, 2, w2, &result, &error);
        gsl_integration_workspace_free(w2);

        den = gsl_spline_eval(eldis, game, acc_eldis);
        return econst * den * result / game;
    }
}

//! This integrates the individual electron spectrum from comint over the total
//! electron distribution
double Compton::comintegral(size_t it, double blim, double ulim, double enphot, double enphmin,
                            double enphmax, gsl_spline* eldis, gsl_interp_accel* acc_eldis) {
    double result, error;
    gsl_integration_workspace* w1;
    w1 = gsl_integration_workspace_alloc(100);

    gsl_function F1;
    auto F1params = ComintParams{enphot, enphmin, enphmax, eldis, acc_eldis, seed_ph, acc_seed};
    auto F1params_it = ComintParams{enphot, enphmin, enphmax, eldis, acc_eldis, iter_ph, acc_iter};
    F1.function = &comint;
    if (it == 0) {
        F1.params = &F1params;
    } else {
        F1.params = &F1params_it;
    }
    // NOTE: in some regimes, using a key of 2 in the gsl_integral_qag line
    // instead of 1 makes for smoother integrals. Not important for the final
    // spectrum, but it makes for better-looking and more accurate plots
    gsl_integration_qag(&F1, blim, ulim, 1e0, 1e0, 100, 2, w1, &result, &error);
    gsl_integration_workspace_free(w1);

    return result;
}

//! This calculates the final spectrum in all frequency bins, including multiple
//! scatters. Note: the reason the Doppler boosting is a factor of 2 instead of 3
//! is because the calculations are done for a conical jet in the Lind&BLanford
//! 1985 prescription
void Compton::compton_spectrum(double gmin, double gmax, gsl_spline* eldis,
                               gsl_interp_accel* acc_eldis) {
    double blim, ulim, com;
    double dopfac_cj;
    double ephmin, ephmax;

    ephmin = seed_energ.front();    //[0];
    ephmax = seed_energ.back();     //[seed_size - 1];

    dopfac_cj = dopfac * (1. - beta * cos(angle)) / (1. + beta * cos(angle));

    size_t size = en_phot.size();
    for (size_t it = 0; it < Niter; it++) {
        for (size_t i = 0; i < size; i++) {
            blim = std::log(std::max(gmin, en_phot[i] / constants::emerg));
            ulim = std::log(gmax);
            if (blim >= ulim) {
                com = 1e-100;
            } else {
                com = comintegral(it, blim, ulim, en_phot[i], ephmin, ephmax, eldis, acc_eldis);
            }
            num_phot[i] = num_phot[i] + com * vol * en_phot[i] * constants::herg;
            en_phot_obs[i] = en_phot[i] * dopfac;
            num_phot_obs[i] = num_phot[i] * std::pow(dopfac, dopnum);
            if (counterjet == true) {
                en_phot_obs[i + size] = en_phot[i] * dopfac_cj;
                num_phot_obs[i + size] = num_phot[i] * std::pow(dopfac_cj, dopnum);
            } else {
                en_phot_obs[i + size] = 0.0;
                num_phot_obs[i + size] = 0.0;
            }
            if (com == 0) {
                iter_urad[i] = -50;
            } else {
                iter_urad[i] = std::log10(escape_corr * com * vol /
                                          (constants::pi * std::pow(r, 2.) * constants::cee));
            }
        }
        ephmin = en_phot.front();    // [0];
        ephmax = en_phot.back();     //[size - 1];
        gsl_spline_init(iter_ph, en_phot.data(), iter_urad.data(), size);
    }
}

//! Method to include cyclosynchrotron array from Cyclosyn.hh to the seed field.
//! The two input arrays should be get_energ() and get_nphot() methods from
//! Cyclosyn.hh. If the seed_urad array wasn't empty, the contribution is
//! automatically added to the existing one. note: the second condition has a <=
//! sign to dodge numerical errors when the seed field energy density is
//! extremely low, which can result in negative values/nan for the seed_urad
void Compton::cyclosyn_seed(const std::vector<double>& seed_arr,
                            const std::vector<double>& seed_lum) {
    // to do: asssert input and member sizes are the same
    for (size_t i = 0; i < seed_arr.size(); i++) {
        seed_energ[i] = seed_arr[i];
        if (seed_urad[i] != 0) {
            seed_urad[i] = std::log10(std::pow(10., seed_urad[i]) +
                                      seed_lum[i] / (constants::cee * constants::herg *
                                                     seed_energ[i] * constants::pi * r * r));
        } else if (seed_lum[i] /
                       (constants::cee * constants::herg * seed_energ[i] * constants::pi * r * r) <=
                   0) {
            seed_urad[i] = -100;
        } else {
            seed_urad[i] = std::log10(seed_lum[i] / (constants::cee * constants::herg *
                                                     seed_energ[i] * constants::pi * r * r));
        }
    }
    gsl_spline_init(seed_ph, seed_energ.data(), seed_urad.data(), seed_energ.size());
}

//! Method to include black body to seed field for IC;
//! Note: Urad and Tbb need to be passed in the co-moving frame, the function
//! does NOT account for beaming
void Compton::bb_seed_k(const std::vector<double>& seed_arr, double Urad, double Tbb) {
    double ulim, bbfield;

    ulim = 1e2 * Tbb * constants::kboltz;
    // seed_freq_array(seed_arr);
    seed_energ = seed_arr;

    for (size_t i = 0; i < seed_arr.size(); i++) {
        if (seed_energ[i] < ulim) {
            bbfield = (2. * Urad * std::pow(seed_energ[i] / constants::herg, 2.)) /
                      (constants::herg * std::pow(constants::cee, 2.) * constants::sbconst *
                       std::pow(Tbb, 4) * (exp(seed_energ[i] / (constants::kboltz * Tbb)) - 1.));
        } else {
            bbfield = 1.e-100;
        }
        if (seed_urad[i] != 0) {
            seed_urad[i] = std::log10(std::pow(10., seed_urad[i]) + bbfield);
        } else if (bbfield > 0) {
            seed_urad[i] = std::log10(bbfield);
        } else {
            seed_urad[i] = -100;
        }
    }
    gsl_spline_init(seed_ph, seed_energ.data(), seed_urad.data(), seed_energ.size());
}

void Compton::bb_seed_kev(const std::vector<double>& seed_arr, double Urad, double Tbb) {
    double ulim, bbfield;

    ulim = 1e2 * Tbb * constants::kboltz_kev2erg;
    // seed_freq_array(seed_arr);
    seed_energ = seed_arr;

    for (size_t i = 0; i < seed_arr.size(); i++) {
        if (seed_energ[i] < ulim) {
            bbfield = (2. * Urad * std::pow(seed_energ[i] / constants::herg, 2.)) /
                      (constants::herg * std::pow(constants::cee, 2.) * constants::sbconst *
                       std::pow(Tbb * constants::kboltz_kev2erg / constants::kboltz, 4) *
                       (exp(seed_energ[i] / (Tbb * constants::kboltz_kev2erg)) - 1.));
        } else {
            bbfield = 1.e-100;
        }
        if (seed_urad[i] != 0) {
            seed_urad[i] = std::log10(std::pow(10., seed_urad[i]) + bbfield);
        } else if (bbfield > 0) {
            seed_urad[i] = std::log10(bbfield);
        } else {
            seed_urad[i] = -100;
        }
    }
    gsl_spline_init(seed_ph, seed_energ.data(), seed_urad.data(), seed_energ.size());
}

//! Calculates incident photon field for a Shakura-Sunyaev disk with aspect ratio
//! H, inner temperature Tin, inner radius Rin, at a distance z from the disk,
//! taking beaming into account
double disk_integral(double alfa, void* pars) {
    DiskIcParams* params = static_cast<DiskIcParams*>(pars);
    double gamma = params->gamma;
    double beta = params->beta;
    double tin = params->tin;
    double rin = params->rin;
    double rout = params->rout;
    double h = params->h;
    double z = params->z;
    double nu = params->nu;

    double delta = 1. / (gamma - beta * cos(alfa));
    double a, b, x, y, r, T_eff, nu_eff, fac, Urad;

    a = z - (h * rout * rin) / (2. * (rout - rin));
    b = 1. / tan(alfa) + (h * rout) / (2. * (rout - rin));

    x = a / b;
    y = x / tan(alfa) + z;

    r = std::pow(std::pow(x, 2.) + std::pow(y, 2.), 1. / 2.);
    T_eff = delta * tin * std::pow(rin / r, 0.75);
    nu_eff = delta * nu;
    fac = (constants::herg * nu_eff) / (constants::kboltz * T_eff);

    Urad = 4. * constants::pi * std::pow(nu_eff, 2.) /
           (constants::herg * std::pow(constants::cee, 3.) * (exp(fac) - 1.));

    return Urad;
}

void Compton::shsdisk_seed(const std::vector<double>& seed_arr, double tin, double rin, double rout,
                           double h, double z) {
    double ulim, blim, nulim, Gamma, result, error, diskfield;

    Gamma = 1. / std::pow((1. - std::pow(beta, 2.)), 1. / 2.);

    blim = atan(rin / z);
    if (z < h * rout / 2.) {
        ulim = constants::pi / 2. + atan((h * rout / 2. - z) / rout);
    } else {
        ulim = atan(rout / (z - h * rout / 2.));
    }
    nulim = 1e1 * tin * constants::kboltz;
    // seed_freq_array(seed_arr);
    seed_energ = seed_arr;

    for (size_t i = 0; i < seed_energ.size(); i++) {
        if (seed_energ[i] < nulim) {
            gsl_integration_workspace* w1;
            w1 = gsl_integration_workspace_alloc(100);
            gsl_function F;
            auto Fparams =
                DiskIcParams{Gamma, beta, tin, rin, rout, h, z, seed_energ[i] / constants::herg};
            F.function = &disk_integral;
            F.params = &Fparams;
            gsl_integration_qag(&F, blim, ulim, 0, 1e-5, 100, 2, w1, &result, &error);
            gsl_integration_workspace_free(w1);

            diskfield = result;
        } else {
            diskfield = 1.e-100;
        }
        if (seed_urad[i] != 0) {
            seed_urad[i] = std::log10(std::pow(10., seed_urad[i]) + diskfield);
        } else if (diskfield > 0) {
            seed_urad[i] = std::log10(diskfield);
        } else {
            seed_urad[i] = -100;
        }
    }
    gsl_spline_init(seed_ph, seed_energ.data(), seed_urad.data(), seed_energ.size());
}

//! Method to estimate the number of scatterings the electrons go through;
//! scatters are repeated until the photons reach the same typical energy as the
//! electrons Input parameters are initial scale frequency of photons in Hz and
//! temperature of scattering electrons in erg note: this assumes that a) the
//! final energy of the photons is set only by the electrons and b) that the
//! photons have one scale energy only; it could be incorrect if scattering
//! multiple photon fields (e.g. disk and cyclosynchrotron)
void Compton::set_niter(double nu0, double Te) {
    double x0, xf, k;

    x0 = constants::herg * nu0 / constants::emerg;
    xf = Te / constants::emerg;
    k = std::log(xf / x0);
    Niter = static_cast<size_t>(k + 0.5);
}

void Compton::set_niter(size_t n) { Niter = n; }

//! Sets optical depth for given number density of emitting region (assuming
//! radius is set correctly), and compton-Y for a given electron average Lorentz
//! factor. In some cases not covered by the radiative transfer tables,
//! escape_corr reverts to the constructor default value of 1.
void Compton::set_tau(double n, double Te) {
    double theta;

    theta = Te * constants::kboltz_kev2erg / constants::emerg;
    tau = n * r * constants::sigtom;
    ypar = std::max(tau * tau, tau) * (std::pow(4.0 * theta, 1) + std::pow(4.0 * theta, 2));
    rphot = 1. / (n * constants::sigtom);

    // set up the radiative transfer correction (first two ifs), and handle out
    // of range cases (all other ifs) if it's too low, avoid any problems by
    // only doing one scattering if tau is too high, assume the value for tau =3
    // (which is wrong!) and yell at the user if the temperature is too low or
    // high, revert to the 20 or 2500 kev case and yell at the user
    if (geometry == "sphere" && tau >= 0.05 && tau <= 3. && Te >= 20. && Te <= 2500.) {
        escape_corr = gsl_spline2d_eval(esc_p_sph, Te, tau, acc_Te, acc_tau);
    } else if (geometry == "cylinder" && tau >= 0.05 && tau <= 3. && Te >= 20. && Te <= 2500.) {
        escape_corr = gsl_spline2d_eval(esc_p_cyl, Te, tau, acc_Te, acc_tau);
    } else if (tau < 0.05) {
        Niter = 1;
    } else if (tau > 3. && Te >= 20. && Te <= 2500.) {
        std::cout << "Optical depth too high, assuming it's 3 for IC calculation!" << std::endl;
        std::cout << "Don't trust the output spectrum slope and change parameters!" << std::endl;
        escape_corr = gsl_spline2d_eval(esc_p_cyl, Te, 3., acc_Te, acc_tau);
    } else if (Te < 20.) {
        std::cout << "Temperature too low, assuming it's 20 kev for IC calculation!" << std::endl;
        std::cout << "Don't trust the output spectrum slope and change parameters!" << std::endl;
        escape_corr = gsl_spline2d_eval(esc_p_cyl, 20., tau, acc_Te, acc_tau);
    } else if (Te > 2500.) {
        std::cout << "Temperature too high, assuming it's 2500 kev for IC "
                     "calculation!"
                  << std::endl;
        std::cout << "Don't trust the output spectrum slope and change parameters!" << std::endl;
        escape_corr = gsl_spline2d_eval(esc_p_cyl, 2500., tau, acc_Te, acc_tau);
    }
    // set up the photopshere correction if optical depth greater than 1
    if (geometry == "sphere" && tau > 1.) {
        vol = (4. / 3.) * constants::pi * (std::pow(r, 3.) - std::pow(r - rphot, 3.));
    } else if (geometry == "cylinder" && tau > 1.) {
        vol = constants::pi * z * (std::pow(r, 2.) - std::pow(r - rphot, 2.));
    }
}

void Compton::set_tau(double _tau) { tau = _tau; }

//! Method to set up the frequency array over desired range
void Compton::set_frequency(double numin, double numax) {
    double nuinc =
        (std::log10(numax) - std::log10(numin)) / static_cast<double>(en_phot.size() - 1);

    for (size_t i = 0; i < en_phot.size(); i++) {
        en_phot[i] =
            std::pow(10., std::log10(numin) + static_cast<double>(i) * nuinc) * constants::herg;
    }
}

//! This method is to hard-code an escape term, e.g. to implement a different
//! geometry
void Compton::set_escape(double escape) { escape_corr = escape; }

//! Method to set up seed frequency array
void Compton::seed_freq_array(const std::vector<double>& seed_arr) {
    // to do: asssert input and member sizes are the same
    for (size_t i = 0; i < seed_arr.size(); i++) {
        seed_energ[i] = seed_arr[i];
    }
}

//! This resets the energy density in case different photon fields want to be
//! calculated separately to see the contribution of each
void Compton::reset() {
    std::fill(seed_urad.begin(), seed_urad.end(), 0);
    std::fill(seed_energ.begin(), seed_energ.end(), 0);
    std::fill(num_phot.begin(), num_phot.end(), 0);
    std::fill(num_phot_obs.begin(), num_phot_obs.end(), 0);
}

void Compton::urad_test() {
    for (size_t i = 0; i < seed_energ.size(); i++) {
        std::cout << seed_energ[i] / constants::herg << " " << seed_urad[i] << " " << iter_urad[i]
                  << std::endl;
    }
}

void Compton::test() {
    std::cout << "Optical depth: " << tau << " Compton-Y: " << ypar << " r: " << r << " z: " << z
              << " angle: " << angle << " speed: " << beta << " delta: " << dopfac << std::endl;
    std::cout << "Number of scatters: " << Niter << std::endl;
}

}    // namespace kariba
