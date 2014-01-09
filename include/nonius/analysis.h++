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

#include <wheels/fun/result_of.h++>

#include <algorithm>
#include <functional>
#include <iterator>
#include <vector>
#include <random>
#include <utility>

namespace nonius {
    namespace detail {
        template <typename Iterator>
        using IteratorValue = typename std::iterator_traits<Iterator>::value_type;
    } // namespace detail

    namespace stats {
        template <typename Iterator>
        detail::IteratorValue<Iterator> weighted_average_quantile(int k, int q, Iterator first, Iterator last) {
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
            auto los = q1 - (iqr * 3.0);
            auto lom = q1 - (iqr * 1.5);
            auto him = q3 + (iqr * 1.5);
            auto his = q3 + (iqr * 3.0);

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
        detail::IteratorValue<Iterator> mean(Iterator first, Iterator last) {
            int count = last - first;
            auto sum = std::accumulate(first, last, detail::IteratorValue<Iterator>{});
            return sum /(double) count;
        }

        template <typename T>
        struct sample_analysis {
            T mean;
            T standard_deviation;
            T outlier_variance;
        };

        // TODO replace estimator with boost.accumulators
        template <typename URng, typename Iterator, typename Estimator>
        std::vector<wheels::fun::ResultOf<Estimator(Iterator,Iterator)>> resample(URng&& rng, int resamples, Iterator first, Iterator last, Estimator&& estimator) {
            int n_samples = last-first;
            std::uniform_int_distribution<int> index_dist(0, n_samples-1);

            std::vector<wheels::fun::ResultOf<Estimator(Iterator, Iterator)>> results;
            results.reserve(resamples);

            std::generate_n(std::back_inserter(results), resamples, [n_samples, first, &estimator, &index_dist, &rng]{
                std::vector<detail::IteratorValue<Iterator>> resampled;
                resampled.reserve(n_samples);
                std::generate_n(std::back_inserter(resampled), n_samples, [first, &index_dist, &rng]{ return first[index_dist(rng)]; });
                return estimator(resampled.begin(), resampled.end());
            });

            return results;
        }

        template <typename T>
        struct estimate {
            T point;
            T lower_bound;
            T upper_bound;
            double confidence_level;
        };
    } // namespace stats
} // namespace nonius

#endif // NONIUS_ANALYSIS_HPP

