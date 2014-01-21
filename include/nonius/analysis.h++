// Nonius - C++ benchmarking tool
//
// Written in 2013-2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Analysis module

#ifndef NONIUS_ANALYSIS_HPP
#define NONIUS_ANALYSIS_HPP

#include <nonius/clock.h++>
#include <nonius/detail/duration.h++>

#include <wheels/meta/invoke.h++>
#include <wheels/expand.h++>

#include <boost/math/distributions/normal.hpp>

#include <algorithm>
#include <functional>
#include <iterator>
#include <vector>
#include <array>
#include <random>
#include <numeric>
#include <tuple>
#include <cmath>
#include <utility>

namespace nonius {
    namespace stats {
        using sample = std::vector<double>;

        template <typename Iterator>
        double weighted_average_quantile(int k, int q, Iterator first, Iterator last) {
            int count = last - first;
            double idx = (count - 1) * k /(double) q;
            int j = (int)idx;
            double g = idx - j;
            std::nth_element(first, first+j, last);
            auto xj = first[j];
            if(g == 0) return xj;

            auto xj1 = *std::min_element(first+(j+1), last);
            return xj + g * (xj1 - xj);
        }

        struct outliers {
            int samples_seen = 0;
            int low_severe = 0;     // more than 3 times IQR below Q1
            int low_mild = 0;       // 1.5 to 3 times IQR below Q1
            int high_mild = 0;      // 1.5 to 3 times IQR above Q3
            int high_severe = 0;    // more than 3 times IQR above Q3
        };

        template <typename Iterator>
        outliers classify_outliers(Iterator first, Iterator last) {
            auto q1 = weighted_average_quantile(1, 4, first, last);
            auto q3 = weighted_average_quantile(3, 4, first, last);
            auto iqr = q3 - q1;
            auto los = q1 - (iqr * 3.);
            auto lom = q1 - (iqr * 1.5);
            auto him = q3 + (iqr * 1.5);
            auto his = q3 + (iqr * 3.);

            outliers o;
            for(; first != last; ++first) {
                auto&& t = *first;
                if(t < los) ++o.low_severe;
                else if(t < lom) ++o.low_mild;
                else if(t > his) ++o.high_severe;
                else if(t > him) ++o.high_mild;
                ++o.samples_seen;
            }
            return o;
        }

        template <typename Iterator>
        double mean(Iterator first, Iterator last) {
            int count = last - first;
            double sum = std::accumulate(first, last, 0.);
            return sum / count;
        }

        template <typename Iterator>
        double standard_deviation(Iterator first, Iterator last) {
            auto m = mean(first, last);
            double variance = std::accumulate(first, last, 0., [m](double a, double b) {
                        double diff = b - m;
                        return a + diff*diff;
                    }) / (last - first);
            return std::sqrt(variance);
        }

        struct sample_analysis {
            double mean;
            double standard_deviation;
            double outlier_variance;
        };

        template <typename... Estimators>
        using resamples = std::array<sample, sizeof...(Estimators)>;

        namespace detail {
            template <typename URng, typename Iterator, typename Estimator>
            void do_resample(int resamples, std::uniform_int_distribution<int>& dist, URng& rng, Iterator first, Iterator last, Estimator& estimator, sample& out) {
                int n = last - first;
                out.reserve(resamples);
                std::generate_n(std::back_inserter(out), resamples, [n, first, &estimator, &dist, &rng] {
                    std::vector<double> resampled;
                    resampled.reserve(n);
                    std::generate_n(std::back_inserter(resampled), n, [first, &dist, &rng] { return first[dist(rng)]; });
                    return estimator(resampled.begin(), resampled.end());
                });
            }
            template <int... I>
            struct indices {
                using next = indices<I..., sizeof...(I)>;
            };
            template <int N>
            struct make_seq {
                using type = typename make_seq<N-1>::next;
            };
            template <int N>
            using MakeSeq = wheels::meta::Invoke<make_seq<N>>;

            template <typename URng, typename Iterator, int... I, typename... Estimators>
            void do_resample(indices<I...>, int n_resamples, std::uniform_int_distribution<int>& dist, URng& rng, Iterator first, Iterator last, resamples<Estimators...>& out, Estimators&&... estimators) {
                WHEELS_EXPAND(do_resample(n_resamples, dist, rng, first, last, estimators, out[I]));
            }
        } // namespace detail
        // TODO replace estimator with boost.accumulators
        template <typename URng, typename Iterator, typename... Estimators>
        resamples<Estimators...> resample(URng&& rng, int n_resamples, Iterator first, Iterator last, Estimators&&... estimators) {
            int n = last - first;
            std::uniform_int_distribution<int> index_dist(0, n-1);

            resamples<Estimators...> results;
            detail::do_resample(detail::MakeSeq<sizeof...(Estimators)>{}, n_resamples, index_dist, rng, first, last, results, estimators...);

            return results;
        }

        template <typename Estimator, typename Iterator>
        sample jackknife(Estimator&& estimator, Iterator first, Iterator last) {
            int n = last - first;
            auto second = std::next(first);
            sample results;
            results.reserve(n);

            for(auto it = first; it != last; ++it) {
                std::iter_swap(it, first);
                results.push_back(estimator(second, last));
            }

            return results;
        }

        struct estimate {
            double point;
            double lower_bound;
            double upper_bound;
        };
        template <typename... Estimators>
        using estimates = std::array<estimate, sizeof...(Estimators)>;

        namespace detail {
            template <typename Iterator, typename Estimator>
            estimate do_bootstrap(double confidence_level, Iterator first, Iterator last, sample const& resample, Estimator&& estimator) {
                namespace bm = boost::math;

                int n_samples = last - first;

                double point = estimator(first, last);
                sample jack = jackknife(estimator, first, last);
                double jack_mean = mean(jack.begin(), jack.end());
                double sum_squares, sum_cubes;
                std::tie(sum_squares, sum_cubes) = std::accumulate(jack.begin(), jack.end(), std::make_pair(0., 0.), [jack_mean](std::pair<double, double> sqcb, double x) -> std::pair<double, double> {
                            auto d = jack_mean - x;
                            auto d2 = d * d;
                            auto d3 = d2 * d;
                            return { sqcb.first + d2, sqcb.second + d3 };
                        });

                double accel = sum_cubes / (6 * std::pow(sum_squares, 1.5));
                int n = resample.size();
                double prob_n = std::count_if(resample.begin(), resample.end(), [point](double x) { return x < point; }) /(double) n;
                double bias = bm::quantile(bm::normal{}, prob_n);
                double z1 = bm::quantile(bm::normal{}, (1. - confidence_level) / 2.);

                auto cumn = [n](double x) -> int { return std::lround(bm::cdf(bm::normal{}, x) * n); };
                auto a = [bias, accel](double b) { return bias + b / (1. - accel * b); };
                double b1 = bias + z1;
                double b2 = bias - z1;
                double a1 = a(b1);
                double a2 = a(b2);
                int lo = std::min(cumn(a1), n - 1);
                int hi = std::min(cumn(a1), n - 1);

                if(n_samples == 1) return { point, point, point };
                else return { point, resample[lo], resample[hi] };
            }

            template <typename Iterator, int... I, typename... Estimators>
            void do_bootstrap(indices<I...>, double confidence_level, Iterator first, Iterator last, resamples<Estimators...> const& resample, estimates<Estimators...>& out, Estimators&&... estimators) {
                WHEELS_EXPAND(out[I] = do_bootstrap(confidence_level, first, last, resample[I], estimators));
            }
        } // namespace detail

        template <typename Iterator, typename... Estimators>
        estimates<Estimators...> bootstrap(double confidence_level, Iterator first, Iterator last, resamples<Estimators...> const& resample, Estimators&&... estimators) {
            estimates<Estimators...> results;
            detail::do_resample(detail::MakeSeq<sizeof...(Estimators)>{}, confidence_level, first, last, resample, results, estimators...);
            return results;
        }
    } // namespace stats
} // namespace nonius

#endif // NONIUS_ANALYSIS_HPP

