#pragma once

#include <gsl/gsl_const_cgsm.h>
#include <gsl/gsl_const_num.h>
#include <gsl/gsl_math.h>

namespace kariba {

namespace constants {
    constexpr double kpc = 1e3 * GSL_CONST_CGSM_PARSEC;
    constexpr double cee = GSL_CONST_CGSM_SPEED_OF_LIGHT;
    constexpr double cee_cee = cee * cee;
    constexpr double emgm = GSL_CONST_CGSM_MASS_ELECTRON;
    constexpr double pmgm = GSL_CONST_CGSM_MASS_PROTON;
    constexpr double kboltz = GSL_CONST_CGSM_BOLTZMANN;
    constexpr double kboltz_kev2erg = 1.6022e-9;    //!< Boltzman constant in keV/erg
    constexpr double gr_to_kev = 5.6095883571872e+29;
    constexpr double me_kev = 511.0;
    constexpr double emerg = GSL_CONST_CGSM_MASS_ELECTRON * GSL_CONST_CGSM_SPEED_OF_LIGHT *
                             GSL_CONST_CGSM_SPEED_OF_LIGHT;
    constexpr double pi = M_PI;
    constexpr double charg = 4.8e-10;
    constexpr double sigtom = GSL_CONST_CGSM_THOMSON_CROSS_SECTION;
    constexpr double herg = GSL_CONST_CGSM_PLANCKS_CONSTANT_H;
    constexpr double hkev = GSL_CONST_CGSM_PLANCKS_CONSTANT_H * 6.2415e8;
    constexpr double mjy = 1.e-26;
    constexpr double re0 = 2.81794e-13;
    constexpr double gconst = GSL_CONST_CGSM_GRAVITATIONAL_CONSTANT;
    constexpr double sbconst = GSL_CONST_CGSM_STEFAN_BOLTZMANN_CONSTANT;
    constexpr double aconst = 7.56e-15;
    constexpr double msun = GSL_CONST_CGSM_SOLAR_MASS;
    constexpr double erg = 6.24e11;               //!< 1 erg = 6.24e11 eV
    constexpr double mprotTeV = 938.272046e-6;    //!< mass of proton in TeV/c^2
    constexpr double mpionTeV = 139.57e-6;        //!< mass of pion in TeV/c^2
    constexpr double Kpp = 0.5;     //!< Inelasticity Kpp. Here is considered constant.
    constexpr double Kpi = 0.17;    //!< fraction of E_kinetic of proton transferred to neutrinos
    constexpr double hbar = herg / (2.0 * pi);    //!< h bar
    constexpr double barn = 1.0e-24;
    constexpr double mbarn = 1.e-3 * barn;
    constexpr double sigmapp = 3.43e-26;    //!< pp cross section in cm2

}    // namespace constants

}    // namespace kariba
