// $Id: couplage.h 1083 2013-02-14 11:31:55Z boman $

#ifndef COUPLAGE_H
#define COUPLAGE_H

#if defined(WIN32)
#ifdef couplage_EXPORTS
#define COUPLAGE_API __declspec(dllexport)
#else
#define COUPLAGE_API __declspec(dllimport)
#endif
#else
#define COUPLAGE_API
#endif

#include <gmm.h>
#include <vector>
#include <string>

#ifndef SWIG
void COUPLAGE_API save(gmm::row_matrix<gmm::wsvector<double> > &mat, std::string const &fname);
void COUPLAGE_API save(std::vector<double> &vec, std::string const &fname);
#endif

#endif