#ifndef _DIST_CHI_SQUARE_H_
#define _DIST_CHI_SQUARE_H_

double chisq_P (const double index, const double degree)
{
  return gsl_cdf_gamma_P (x, nu / 2, 2.0);
}

double chisq_Q (const double x, const double nu)
{
  return gsl_cdf_gamma_Q (x, nu / 2, 2.0);
}

#endif