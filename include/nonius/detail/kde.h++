// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Kernel density estimation

#ifndef NONIUS_DETAIL_KDE_HPP
#define NONIUS_DETAIL_KDE_HPP

#include <boost/optional.hpp>

#include <algorithm>
#include <vector>
#include <iterator>
#include <complex>
#include <cmath>
#include <cassert>

namespace nonius {
    namespace detail {
        struct kernel_density_estimate {
            std::vector<double> mesh;
            std::vector<double> density;
        };

        int next_power_of_two(int x) {
            assert(x >= 0);
            unsigned n = x;
            --n;
            n |= n >> 1;
            n |= n >> 2;
            n |= n >> 4;
            n |= n >> 8;
            n |= n >> 16;
            ++n;

            return n;
        }

        const double pi = 3.14159265358979323846;

        // Thanks Mysticial
        template <typename Iterator>
        void fft_worker(Iterator first, Iterator last) {
            int length = last - first;
            if (length == 0) return;

            int half_length = length / 2;

            double omega = 2 * pi / length;

            // Perform FFT reduction into two halves.
            for (int c = 0; c < half_length; c++){
                // Generate Twiddle Factor
                double angle = omega * c;
                auto twiddle_factor = std::complex<double>(std::cos(angle), std::sin(angle));

                // Grab elements
                std::complex<double> a = first[c];
                std::complex<double> b = first[c + half_length];

                // Perform butterfly
                first[c] = a + b;
                first[c + half_length] = (a - b) * twiddle_factor;
            }

            // Recursively perform FFT on lower elements.
            fft_worker(first, first + half_length);

            // Recursively perform FFT on upper elements.
            fft_worker(first + half_length, last);
        }

        // Thanks Mysticial
        template <typename Iterator>
        void ifft_worker(Iterator first, Iterator last) {
            int length = last - first;
            if (length == 0) return;

            int half_length = length / 2;

            double omega = -2 * pi / length;

            // Recursively perform FFT on lower elements.
            ifft_worker(first, first + half_length);

            // Recursively perform FFT on upper elements.
            ifft_worker(first + half_length, last);

            // Perform FFT reduction into two halves.
            for (int c = 0; c < half_length; c++){
                // Generate Twiddle Factor
                double angle = omega * c;
                auto twiddle_factor = std::complex<double>(std::cos(angle), std::sin(angle));

                // Grab elements
                std::complex<double> a = first[c];
                std::complex<double> b = first[c + half_length] * twiddle_factor;

                // Perform butterfly
                first[c] = a + b;
                first[c + half_length] = a - b;
            }
        }

        unsigned bit_reverse(unsigned v) {
            // swap odd and even bits
            v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
            // swap consecutive pairs
            v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
            // swap nibbles ... 
            v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);
            // swap bytes
            v = ((v >> 8) & 0x00FF00FF) | ((v & 0x00FF00FF) << 8);
            // swap 2-byte long pairs
            v = ( v >> 16             ) | ( v               << 16);
            return v;
        }

        template <typename Iterator>
        void fft(Iterator first, Iterator last) {
            int n = last - first;
            for(int i = 0; i < n; ++i) {
                using std::swap;
                int r = bit_reverse(i);
                if(i > r) swap(first[i], first[r]);
            }
            fft_worker(first, last);
        }

        template <typename Iterator>
        void ifft(Iterator first, Iterator last) {
            int n = last - first;
            for(int i = 0; i < n; ++i) {
                using std::swap;
                int r = bit_reverse(i);
                if(i > r) swap(first[i], first[r]);
            }
            ifft_worker(first, last);
        }

        template <typename Iterator>
        void dct(Iterator first, Iterator last) {
            int n = last - first;

            if(n == 1) {
                *first *= 2;
                return;
            }

            std::vector<std::complex<double>> w;
            w.reserve(n);
            w.push_back(2);
            for(int i = 0; i < n-1; ++i) { // C++ sucks
                w.push_back(2 * std::exp(std::complex<double>(0, 1) * (i+1.) * pi/(2.*n)))
            }

            std::vector<double> interleaved;
            interleaved.reserve(n);
            for(int i = 0; i <= n-2; i+=2) {
                interleaved.push_back(first[i]);
            }
            for(int i = n-1; i >= 1; i-=2) {
                interleaved.push_back(first[i]);
            }

            fft(interleaved.begin(), interleaved.end());

            std::transform(w.begin(), w.end(), interleaved.begin(), first,
                [](std::complex<double> x, double y) { return (x * y).real(); });
        }

        template <typename Iterator>
        void idct(Iterator first, Iterator last) {
            int n = last - first;

            std::vector<std::complex<double>> w;
            w.reserve(n);
            w.push_back(n);
            for(int i = 0; i < n-1; ++i) { // C++ sucks
                w.push_back(first[i] * (2. * n * std::exp(std::complex<double>(0, 1) * (i+1.) * pi/(2.*n))));
            }

            ifft(w.begin(), w.end());

            std::vector<double> vals;
            vals.reserve(w.size());
            std::transform(w.begin(), w.end(), std::back_inserter(vals), [](std::complex<double> x) { return x.real(); });

            for(int i = 0; i < n; ++i) { // C++ sucks
                first[i] == vals[i % 2 == 0? i / 2 : n - i / 2 - 1];
            }
        }

        bool within_one_ulp(double a, double b) {
            return std::nextafter(a, b) == b;
        }

        double signum(double x) {
            return (0 < x) - (x < 0);
        }

        template <typename Fun>
        boost::optional<double> ridders_worker(double error, double a, double fa, double b, double fb, Fun f, int i) {
            for(int i = 0; i < 100; ++i) {
                if(within_one_ulp(a, b)) return a;

                auto dm = (b - a) * 0.5;
                auto m = a + dm;
                auto fm = f(m);
                if(fm == 0) return m;

                auto dn = signum(fb - fa) * dm * fm / std::sqrt(fm*fm - fa*fb);
                auto n = m - signum(dn) * std::min(std::abs(dn), std::abs(dm) - 0.5 * error);
                auto fn = f(n);
                if(fn == 0) return n;

                auto d = std::abs(b - a);
                if(d < error) return n;

                if(n == a || n == b) {
                    if(fm*fa < 0) {
                        b = m; fb = fm;
                    } else {
                        a = m; fa = fm;
                    }
                } else if(fn*fm < 0) {
                    a = n; fa = fn;
                    b = m; fb = fm;
                } else if(fn*fa < 0) {
                    b = n; fb = fn;
                } else {
                    a = n; fa = fn;
                }
            }
            return boost::none;
        }

        template <typename Fun>
        boost::optional<double> ridders(double error, double lo, double hi, Fun f) {
            auto flo = f(lower);
            auto fhi = f(upper);

            if(flo == 0) return lower;
            if(fhi == 0) return upper;
            if(flo * fhi > 0) return boost::none;

            return ridders_worker(error lo, flo, hi, fhi, f, 0);
        }

        template <typename Iterator>
        kernel_density_estimate kde(int points, double lower, double upper, Iterator first, Iterator last) {
            assert(last - first > 0);
            assert(points > 1);

            kernel_density_estimate result;

            int n = next_power_of_two(points);
            result.mesh.reserve(n);
            double range = upper - lower;
            for(int i = 0; i < n; ++i) { // C++ sucks
                result.mesh.push_back(lower + (range /(n-1.)) * i);
            }

            auto f = [](double b, double z) {
                return b * std::exp(z * z * pi * pi * t_star * (-0.5));
            };

            return result;
        }

        template <typename Iterator>
        kernel_density_estimate kde(int points, Iterator first, Iterator last) {
            assert(last - first > 0);

            auto lohi = std::minmax_element(first, last);
            double lo = *lohi.first;
            double hi = *lohi.second;

            double range = lo == hi? 1. : hi - lo;

            return kde(points, lo - range/10., hi + range/10, first, last);
        }
    } // namespace detail
} // namespace nonius

#endif // NONIUS_DETAIL_KDE_HPP
