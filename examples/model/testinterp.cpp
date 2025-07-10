#include <iostream>
#include <vector>

void jetinterp(std::vector<double> &ear, std::vector<double> &energ, std::vector<double> &phot,
               std::vector<double> &photar, size_t ne, size_t newne) {
    size_t i, iplus1, j, jplus1;
    double emid, phflux;
    bool bracket = true;

    j = 0;
    for (i = 0; i < newne; i++) {
        // Middle of bin
        iplus1 = i + 1;
        emid = (ear[i] + ear[iplus1]) / 2.;
        // Linear search if we don't bracket yet
        if (!bracket) {
            j = 1;
            bracket = true;
        }
        while (j <= ne && energ[j] < emid) {
            j++;
        }

        jplus1 = j;
        if (j == 0) {
            bracket = false;
        } else {
            j = j - 1;
        }

        if (j < 1 || j > ne) {
            photar[i] = 0.;
        } else {
            // ph/cm^2/s/keV
            phflux =
                phot[j] + (phot[jplus1] - phot[j]) * (emid - energ[j]) / (energ[jplus1] - energ[j]);
            photar[i] = phflux * (ear[iplus1] - ear[i]);
        }
    }
}

void jetinterp2(std::vector<double> &ear, std::vector<double> &energ, std::vector<double> &phot,
                std::vector<double> &photar, size_t ne, size_t newne) {
    int i, iplus1, j, jplus1;
    double emid, phflux;

    j = 0;
    for (i = 0; i < newne; i++) {
        // Middle of bin
        iplus1 = i + 1;
        emid = (ear[i] + ear[iplus1]) / 2.;
        // Linear search if we don't bracket yet
        if (j == -1) {
            j = 1;
        }
        while (j <= ne && energ[j] < emid) {
            j++;
        }

        jplus1 = j;
        j = j - 1;

        if (j < 1 || j > ne) {
            photar[i] = 0.;
        } else {
            // ph/cm^2/s/keV
            phflux =
                phot[j] + (phot[jplus1] - phot[j]) * (emid - energ[j]) / (energ[jplus1] - energ[j]);
            photar[i] = phflux * (ear[iplus1] - ear[i]);
        }
    }
}

int main() {

    std::vector<double> ear{0, 1, 2, 3, 4, 5, 6, 7};
    std::vector<double> energ(ear.size(), 0.0);
    std::vector<double> phot(ear.size() / 2, 0.0);
    std::vector<double> photarr(ear.size() / 2, 0.0);

    for (size_t i = 0; i < ear.size(); i++) {
        energ[i] = exp(ear[i]);
    }

    jetinterp(ear, energ, phot, photarr, ear.size(), phot.size());

    for (const auto &item : photarr) {
        std::cout << item << ",  ";
    }
    std::cout << "\n";
}
