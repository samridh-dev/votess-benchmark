#ifndef PARAM_HPP
#define PARAM_HPP

/* ------------------------------------------------------------------------- */
// Type Parameters
using I = std::size_t;
using Tf = float;
using Ti = int;
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
// Votess parameters
constexpr I k   = 64;
constexpr I gr  = 32;
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
// Benchmark Parameters
constexpr I imax = 1e0;           // Maximum number of iterations per step
constexpr I N0   = 1e3;           // Initial dataset size
constexpr I N1   = 1e6;           // Final   dataset size

constexpr I counts = 1e3;         // Total number of data points to collect

constexpr I dN     = N1 / counts; // Initial dataset step (increases over time)

/* ------------------------------------------------------------------------- */

#endif PARAM_HPP
