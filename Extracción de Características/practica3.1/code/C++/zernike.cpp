#include "zernike.h"
#include <cmath>
#include <cassert>
#include <iostream>
// No incluir más gsl_sf_fact.h
// #include <gsl/gsl_sf_fact.h>

#define PI 3.14159265358979323846264338328
#define MAX_D 15
#define MAX_Z 72
#define MAX_LUT 240

// Definir función de factorial
double factorial(int n) {
    if (n == 0) return 1.0;
    double result = 1.0;
    for (int i = 1; i <= n; ++i) {
        result *= i;
    }
    return result;
}

void mb_Znl(const std::vector<double>& X, const std::vector<double>& Y, const std::vector<double>& P, double D, double m10_m00,
            double m01_m00, double R, double psum, std::vector<double>& zvalues) {
    static double LUT[MAX_LUT];
    static int n_s[MAX_Z], l_s[MAX_Z];
    static char init_lut = 0;

    double x, y, p;
    int i, m, theZ, theLUT, numZ = 0;
    int n = 0, l = 0;

    std::vector<std::complex<double>> sum(MAX_Z);
    std::complex<double> Vnl;

    assert(D == MAX_D);

    if (!init_lut) {
        theZ = 0;
        theLUT = 0;
        for (n = 0; n <= MAX_D; n++) {
            for (l = 0; l <= n; l++) {
                if ((n - l) % 2 == 0) {
                    for (m = 0; m <= (n - l) / 2; m++) {
                        LUT[theLUT] = pow(-1.0, m) * (factorial(n - m) / (factorial(m) * factorial((n - 2 * m + l) / 2) *
                                factorial((n - 2 * m - l) / 2)));
                        theLUT++;
                    }
                    n_s[theZ] = n;
                    l_s[theZ] = l;
                    theZ++;
                }
            }
        }
        init_lut = 1;
    }

    for (n = 0; n <= D; n++) {
        for (l = 0; l <= n; l++) {
            if ((n - l) % 2 == 0) {
                sum[numZ] = std::complex<double>(0.0, 0.0);
                numZ++;
            }
        }
    }

    for (i = 0; i < X.size(); i++) {
        x = (X[i] - m10_m00) / R;
        y = (Y[i] - m01_m00) / R;
        double sqr_x2y2 = sqrt(x * x + y * y);
        if (sqr_x2y2 > 1.0) continue;

        p = P[i] / psum;
        double atan2yx = atan2(y, x);
        theLUT = 0;
        for (theZ = 0; theZ < numZ; theZ++) {
            n = n_s[theZ];
            l = l_s[theZ];
            Vnl = std::complex<double>(0.0, 0.0);
            for (m = 0; m <= (n - l) / 2; m++) {
                Vnl += (std::polar(1.0, l * atan2yx) * LUT[theLUT] * pow(sqr_x2y2, (n - 2 * m)));
                theLUT++;
            }
            sum[theZ] += std::conj(Vnl) * p;
        }
    }

    zvalues.resize(numZ);
    for (theZ = 0; theZ < numZ; theZ++) {
        sum[theZ] *= ((n_s[theZ] + 1) / PI);
        zvalues[theZ] = std::abs(sum[theZ]);
    }
}
