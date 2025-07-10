[TOC]

# About Kariba

Kariba is a supporting C++ library for BHJet.

BHJet is a semi-analytical, multi-zone jet model designed for modelling steady-state SEDs of jets launched from accreting black holes. The key features of the model are:

1. It is applicable across the entire black hole mass scale, from black hole X-ray binaries (both low and high mass) to active galactic nuclei of any class (from low-luminosity AGN to flat spectrum radio quasars)

2. It is designed to be more comparable than other codes to GRMHD simulations and/or RMHD semi-analytical solutions.

The model is fairly complex and fitting it to data is not always straightforward. As such, it is highly recommended to read this file carefully before running the code. It takes little time and will save you a lot of headaches later on. All the physics of the model, the assumptions going into it, as well as some applications, are discussed in depth in Lucchini et al. 2022 [DOI:10.1093/mnras/stac2904](https://doi.org/10.1093/mnras/stac2904), and references therein.

If you have any questions, or find issues or bugs, please feel free to open a ticket in the Issues section of the repository.


## Details

### Particle distributions

These classes are designed to treat both non-relativistic and relativistic particle distributions. This requires the particle distributions to be written in units of momentum, and the number density to be particles per unit volume, per unit momentum. The class also automatically initializes and calculates the distribution in Lorentz factor space - the Lorentz factor is calculated from the particle momentum, and the corresponding number density is written in units of particles per unit volume, per unit Lorentz factor. One important point in using all of these classes is that, before calculating the number densities with the .set\_ndens() member, users need to set every other relevant quantity (temperature, non-thermal slopes, etc) needs to be set explicitely by the user. Then, the normalisation is set by calling the .set_norm(n) member function. This method requires knowing n, the total number density per unit volume of particles, in advance. _The set\_ndens() method can only be called after this is done_. The functions used to integrate the particle distributions are friend members; this allows the functions to access the protected and private members of the class, and to also have the correct input parameters (a double and a void pointer) for integrating with the GSL libraries. For all derived classes, the constructor only requires passing the size of the arrays to be used. The classes that share this structure are:

- Particles: this is the prototype class for all distributions; it containes basic methods to manipulate and test arrays that are common and shared between all distributions, as well as a generalised class destructor.

- Thermal: this distribution follows a Maxwell-Juttner distribution in momentum space, and can treat both relativistic temperatures (> 511 keV) and non-relativistic temperatures down to ~1 keV. Below this threshold, the normalization of the M-J distribution diverges due to numerical errors, and the number density array returns only nan. This class does not contain methods to solve the continuity equation, as it makes no sense to do so if one assumes the distribution is thermalized in the first place.

- Powerlaw: this distribution follows a N0*p^(-s)*exp(-p/pmax) exponentially cutoff power-law distribution in momentum space; for large values of p, it reduces to a standard power-law distribution in Lorentz factor space as well.

- Bknpower: this distribution follows a smoothly broken power-law distribution in momentum space; for large values of p, it reduces to a standard broken power-law distribution in Lorentz factor (e.g. Ghisellini et al. 2009) space as well.

- Kappa: this distribution follows a k-distribution in Lorentz factor space (e.g. Develaar et al. 2017), which is then converted in the appropriate distribution in momentum space; the k- distribution roughly mimics a Maxwell-Juttner distribution with a power-law tail of slope k+1, where k is the index that defines the Kappa distribution.

- Mixed: this distribution follows a hybrid thermal/non-thermal distribution, as traditionally done in the agnjet model (e.g. Markoff et al. 2001, 2005). The ratio of non-thermal to thermal particles is set by the .set\_plfrac(f) method; a fraction f is assumed to be non-thermal and follows the same distribution and methods as the Powerlaw class, while the remaining fraction 1-f is thermal (and is therefore treated identically to the Thermal.hpp class).

The classes powerlaw, bknpower, kappa and mixed all have methods to both set the maximum momentum of the particles, and solve the steady-state continuity equation, for a given set of physical conditions in the source. The .set\_p() method allows one to set the maximum momentum of the distribution by comparing the acceleration and cooling time-scales (including adiabatic cooling, synchrotron cooling, and inverse Compton cooling, and neglectic Klein-Nishina effects). Alternatively, it is possible to simply specify a desired maximum Lorentz factor in each distribution. The .cooling\_steadystate() method allows one to solve the continuity equation assuming continuous injection of particles, and knowing the adiabatic and radiative timescales loss terms in the source (neglecting Klein-Nishina effects as the .set\_p() method).

### Radiative mechanisms

These classes are designed to calculate the emission of spectral components commonly found in the SEDs of high energy sources. These can either be related to particle distributions described above (e.g. cyclosynchrotron, inverse Compton), but that need not be the case (e.g. black body, accretion disk). Each class always uses two different sets of arrays; one in the comoving frame of the source (en_phot, num_phot), and one in the observer frame (en_phot_obs, num_phot_obs), automatically accounting for viewing angle and Doppler boosting effects, but not for cosmological redshift. The energy of the photons is always expressed in erg, and the luminosity for each energy bin is expressed in erg/s/Hz. _Like the case of the particle distributions, the constructors for each object only require the desired size of the arrays, and every physical quantity (magnetic field, frequency intervals, Thomson optical depths, etc) needs to be set explicitely with the setter functions by the user before calculating the spectra_.

- Radiation: this is the prototype class for all the spectral components treated; it containes basic methods to manipulate and test arrays that are common and shared between all classes. Note that before calculating the spectra one needs to set the geometry of the source (assumed to be homogeneous); the only exception to this is the ShSDisk class, which assumes a Shakura-Sunyaev type disk and therefore sets the geometry internally. It is also possible to include the presence of both an approaching and receding source (effectively, a counterjet) with different Lorentz factors; the class does so by extending the size of the _obs arrays.

- BBody: this class calculates the emission from a thermalized, optically thick source emitting black body radiation of given temperature (in Kelvin or keV) and luminosity (in erg/s). It is also possible to return the energy density seen by an observer standing at rest, at a distance d from the source.

- ShSDisk: this class treats a truncated, optically thick, geometrically thin, Shakura-Sunyaev type disk. The disk is assumed to be truncated at a distance Rin (expressed in Rg); at this distance, it is possible to either define the temperature and calculated the corresponding luminosity, or vice versa, through the constructor. The scale height H/R of the disk is assumed to be max(0.1,L), where L is the luminosity in Eddington units, H the disk height, and R the disk radius. H/R is therefore constant throughout the disk, and the farther away one moves from the central engine, the thicker the disk gets in units of Rg. This behavior physically roughly mimics the Shakura-Sunyaev model (Shakura and Sunyaev 1973): for low (<10% Eddington) accretion rates H/R is driven by the viscosity alpha parameter (whose value is typically 0.1), but for higher accretion rates radiation pressure can start puffing up the disk. Note that this is NOT a self-consistent treatement of a slim disk model.

- Cyclosyn: this class calculates the cyclosynchrotron emission from a population of particles in both the relativistic and non-relativistic regime.  The emissivity for the non-relativistic regime is the phenomenlogical treatement of Petrosian (1981), while in the relativistic regime the treatement is that of Bloumethal and Gould (1970). Before running the calculations, one needs to specify the magnetic field with the .set_bfield() method. The absorption coefficient is calculated by integrating by parts - therefore, one needs to know the differential of the particle distribution. In order to calculate the spectrum one needs to call the .cycsyn_spectrum() method, which requires knowledge of the minimum and maximum Lorentz factor of the particle distribution, as well as both the electron distribution and its differential in Lorentz factor units. The latter two need to be gsl_spline objects.

- Compton: this class calculates the inverse Compton emission from a population of particles in both the relativistic and non-relativistic regime. In both cases, the calculations are found in Bloumethal and Gould (1970). The code accounts both for Klein-Nishina effects as well as multiple scatters, and is optimized for optical depths of up to ~a few in order to probe X-ray coronae of accreting black holes. There are two important notes on using this class in the multiple scatter regime. Frist, this class is the most computationally expensive of the library, especially in the case of multiple scatters. Second, the code automatically recognizes when the photon to be scattered has more energy than the electron doing the scattering. Therefore specifying the exact number of scatters physically happening is not necessary; typically, using more than ~15 scatters slows down the code without any change to the spectrum. Different seed fields can be used. It is possible to calculate SSC emission, using the en_phot and num_phot arrays from the Cyclosyn class, or to scatter black body photons (described by an energy density in erg/cm and a temperature in keV), or to scatter disk photons in a lamp-post geometry (described by a disk temperature in Kelvin, an inner and outer radius in Rg, a scale height h, at a distance z -in Rg- from the disk).


## Examples

The examples section of the repository contains three examples to familiarise users with Kariba, the CompPS spectra generated to compute
the radiative transfer corrections in Kariba, and a set of Python scripts to replicate figures 3,4,6 and 7 of Lucchini et al. 2022.

The examples detail the following

### particles

This code highlights how to set up each particle distribution needs to be set up, and also highlights how the three most complex
particle distributions in Kariba (Bknpower, Kappa, Mixed) compare to each other for a near-identical set of parameters. It can
also be used to replicate figure 1 of the model paper. The classes used are Thermal, Mixed, Kappa and Bknpowerlaw.

### corona

This code highlights how to set up the an accretion disk+spherical corona model in Kariba, and compares the thermal Comptonisation
for a range of temperatures and optical depths, and compares the output in Kariba and CompPS. This is also shown in figure 2 of
the model paper. The classes used are Thermal, ShSDisk and Compton.

### singlezone

This code highlights how to set up the simplest iteration possible of a single zone blazar emission model, neglecting cooling as
well as external photon fields. Only synchrotron and single-scattering synchrotron-self Compton emission from a spherical region
are calculated. It replicates the resultes of the EHT MWL campaign of M87, and the default parameters are the same as Model 2 in
that paper. The classes used are Powerlaw, Cyclosyn and Compton.

## BHJet model

The BHJet model is the original main application, with Kariba as a support library. Here, it serves as the main model upon one can build their own model.


### Parameters

The free parameters of the model are:

 - mbh: mass of the black hole. Always frozen before the fit, bhjet should NOT be used to estimate BH masses from SED modelling.
 - Incl: viewing angle of the jet. Sets Doppler factor for the various regions. Typically frozen before the fit.
 - dkpc: distance from source in kpc. Always frozen before the fit.
 - redshfit: self explanatory. Only used for modelling AGN and set before the fit.
 - jetrat: amount of power injected at the base of the jet. Increasing values increase the normalisation of the synchrotron flux, and the importance of Comptonisation. Measured in Eddington units.
 - r_0: radius of the nozzle/corona, described by an outflowing, magnetized cylinder of radius r_0 and height 2r_0. Decreasing values increase the optical depth of the nozzle. Measured in units of r_g.
 - z_diss: location of non-thermal particle injection region. Sets optically thick to thin break, overall normalization and Compton dominance of non-thermal component, and for high accretion rate AGN sets the EC target field (BLR or BLR+torus). Decreasing values move the synchrotron thick-to-thin break to higher frequencies. Measured in units of r_g.
 - z_acc: if velsw > 1, sets location of the jet acceleration. Sets the jet speed and dependency of magnetic field with distance; smaller values correspond to faster dissipation before z_acc (see velsw below). Measured in r_g like zdiss.
 - z_max: maximum length over which jet calculations are carried out, in units of Rg. Always frozen to large values in order to get a flat radio spectrum.
 - t_e: temperature of relativistic electrons in the nozzle/corona, expressed in keV.
 - f_nth: Fraction of thermal particles accelerated into power-law tail. Typical values range between 0.01 and 1. Sets the relative importance of thermal to non-thermal emission in the compact jet. Typically frozen before the fit.
 - f_pl: reduces particle temperature and percentage of accelerated particles along the jet after z_diss, resulting in an inverted radio spectrum. Set to 0 for a standard flat spectrum, increase for a more inverted spectrum.
 - pspec: slope of non-thermal particle distribution. Sets the slope of the non-thermal synchrotron and inverse Compton components.
 - f_heat: imitates shock heating; at z=z_diss bumps Te by a fixed factor f_heat, increasing values increase the radiative efficiency of the jet after z_diss. Set to 1 and frozen unless necessary.
 - f_b: sets the effective adiabatic cooling timescale, defined as t_ad = r/f_b*c. This in turn sets the location of the cooling break in the radiating particle distribution. Kept free if the SED shows a cooling break, otherwise set to 0.1 and frozen.
 - f_sc: If < 0.1, sets the maximum energy of non-thermal particles by parametrizing the acceleration timescale (and therefore acceleration efficiency). If > 10, sets the maximum Lorenz factor of the non-thermal particles. Either way, sets the energy up to which the non-thermal synchrotron and inverse Compton spectra extend to. Values between 0.1 and 10 are non-physical and should be disregarded.
 - p_beta: plasma beta (ratio between lepton and magnetic field energy density) at the base. If velsw = 0 or 1 this sets (and freezes) the equipartition value throughout the jet as well as the pair content, if velsw > 1 it only sets the pair content and has to be frozen to a value high enough to have pair content ~approx unity. This is because the assumptions going into the calculation of the magnetic field when velsw>1 only hold if lepton energy density <<< (cold) proton energy density. Additionally, if velsw > 1, this can be set to 0 to always enforce one proton per electron. Can be used to tune the optical depth of the jet nozzle for a given jet power. _See the main paper for a discussion on valid values of p_beta for all model flavours._
 - sig_acc: only used if velsw > 1, sets the value for magnetization. Higher values decrease the Compton dominance and increase the peak frequencies of the non-thermal synchrotron and inverse Compton components. Typically frozen to ~0.01-1 unless required by the data.
 - l_disk: sets the luminosity of the Shakura-Sunyaev disk in Eddington units, and the corresponding temperature is computed from knowing this luminosity and Rin. If set to a negative value, the code still includes the disk as a seed photon field for IC scattering, but it is not included in the output of the model. This is in case users want to use a more complex disk model (e.g. diskir).
 - r_in: inner radius of the disk.
 - r_out: outer radius of the disk. Only has a minimal contribution to the SED, typically frozen. rout<=rin disables the disk.
 - compar1: first parameter of the external photon field, depending on the value of compsw
 - compar2: second parameter of the external photon field, depending on the value of compsw
 - compar3: third parameter of the external photon field, depending on the value of compsw
 - compsw: sets the external photon fields to be used in the IC calculation. 0 only includes SSC (+disk IC, if present) emission, 1 adds a uniform external black body contribution (e.g. a host galaxy), 2 adds the Broad Line Region and Torus of an AGN and ties their luminosity to that of the disk. If compsw = 1, compar1 is the temperature in Kelvin, compar2 is the total black-body luminosity, and compar3 is the BB energy density. If compsw = 2, compar1 is the fraction of disk photons reprocessed by the BLR, compar2 the fraction of disk photons reprocessed by the torus, and compar3 is not used.
 - velsw: This sets the jet velocity profile used by the code. If velsw = 0 or 1 the jet is pressure-driven and mildly relativistic, otherwise the jet base is highly magnetized and the jet is magnetically driven. In this case, the value of velsw is also the final Lorentz factor of the jet, achieved at a distance z_acc (see above).
 - infosw: information switch; the higher the value, the more info the code returns. 0 simply stores a total flux value in the photspec array; 1 also prints the total emission to, as well as contribution from each radiative component (e.g. thermal synchrotron, non-thermal synchrotron, thermal Comptonization, non-thermal IC emission, external fields, disk) to a file; 2 also prints the emission and particle distribution in each zone, which will be automatically plotted by Plot.py; 3, 4 and 5 print increasing amounts of information to the terminal.
 - EBLsw: switch to activate extragalactic background light (EBL) attenuation. If EBLsw = 0 does not use EBL attenuation, and 1 uses the fiducial model by Gilmore et al. (2012).

Generally, no more than ~9 parameters should be fitted at the same time due to model degeneracies; depending on the application, many parameters can be frozen to a reasonable educated guess.

### Notes

There are two important notes for running the code with acceptable performance:

1. The jet is arbitrarily divided into 100 zones (controlled by the integer variable nz) which do not interact with each other in any way. As the calculations move from the base of the jet to zmax, the grid which defines each zone changes. Up to 1000 Rg, the grid advances in steps of 2*r, where r is the radius of the jet at that particular distance. Outwards, the grid uses logarithmic steps instead. The reason for this choice (discussed in Connors et al. 2018) is that it prevents the final spectrum from being resolution-dependent. On top of that, the code runtime increases linearly with the zone count, so anything above ~70 will simply slow the code down for no gain. Note that particularly large values of zmax and/or small values of r0 can result in the number of zones being insufficient to cover the entire jet length; this is why nz is set slightly higher than the 70 segments necessary to optimize performance in most applications.

2. The bulk of the code runtime is caused by the inverse Compton calculation, in particular for cases of moderate to high optical depth (>~0.05, when multiple scatters are considered) and/or when many zones in the jet produce bright IC emission. The code uses two adjustments to improve the efficiency of the radiation calculations: a) the frequency grid over which the emission of each zone (both inverse Compton and synchrotron) is updated dynamically after calculating the appropriate scale frequencies, and b) the inverse Compton itself is only calculated when it's expected to be bright enough to contribute meaningfully to the SED through the Compton_check function. Depending on the source and model parameters, this reduces the code run time by a factor of ~3-50. _It is extremely important to double check whether Compton_check is being too aggressive in neglecting zones to compute IC or not! You can do this simply by forcing Compton_check to always return true, thus calculating the IC emission for every zone, and comparing the result with the standard prescriptions in the function._

### Plotting


If you want to plot the output, a simple python script is provided, `Plot.py`.

### ISIS

If you are using ISIS [Houck and Denicola, 2000](https://ui.adsabs.harvard.edu/abs/2000ASPC..216..591H)

add the following lines to your `.isisrc` file:

```
append_to_isis_load_path(path+"path/to/your/agnjet/folder");
append_to_isis_module_path(path+"path/to/your/agnjet/folder");
```

(on some systems if gsl libs aren't on obvious path you may need to explicitly include with -I and -L, but try this first!)
