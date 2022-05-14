#include "InitialConditions.h"

double psi(int t) {
    return 5000.0-t/2.0;
}

double phi(int x) {
    return x/2.0+5000.0;
}
