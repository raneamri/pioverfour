#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <boost/multiprecision/cpp_dec_float.hpp>

using BigFloat = boost::multiprecision::cpp_dec_float_100;

const BigFloat PI = boost::math::constants::pi<BigFloat>();
const BigFloat LNTWO = log(BigFloat(2));

struct Score {
    int heads;
    int tails;
};

struct Summary {
    int heads;
    int tails;
    int tosses;
    long double fraction_heads;
};

struct FastCoin {
    std::mt19937& gen;
    uint32_t buffer = 0;
    int bits_left = 0;

    explicit FastCoin(std::mt19937& generator) : gen(generator) {}

    inline bool flip() {
        if (bits_left == 0) {
            buffer = gen();
            bits_left = 32;
        }

        bool bit = (buffer & 1u) != 0u;
        buffer >>= 1;
        --bits_left;
        return bit;
    }
};

std::pair<long long, long long> approximate_fraction(BigFloat x, long long max_den = 1000) {
    BigFloat value = x;
    long long a0 = static_cast<long long>(floor(value));

    long long p0 = 1, q0 = 0;
    long long p1 = a0, q1 = 1;

    value -= a0;

    while (value != 0) {
        value = 1 / value;
        long long a = static_cast<long long>(floor(value));

        long long p2 = a * p1 + p0;
        long long q2 = a * q1 + q0;

        if (q2 > max_den) break;

        p0 = p1; q0 = q1;
        p1 = p2; q1 = q2;

        value -= a;
    }

    return {p1, q1};
}

// computes LerchPhi(z, s, a) = sum_{k >= 0} z^k / (k + a)^s
BigFloat lerch_phi(const BigFloat& z, const BigFloat& s, const BigFloat& a,
                   BigFloat tolerance = BigFloat("1e-30"), int max_terms = 1000000) {
    if (a <= 0) {
        throw std::invalid_argument("a must be > 0");
    }

    BigFloat total = 0;
    BigFloat z_power = 1;

    for (int k = 0; k < max_terms; ++k) {
        const BigFloat denom = pow(BigFloat(k) + a, s);
        const BigFloat term = z_power / denom;
        total += term;

        z_power *= z;

        const BigFloat next_term_abs = boost::multiprecision::abs(z_power / pow(BigFloat(k + 1) + a, s));
        if (next_term_abs < tolerance) {
            break;
        }
    }

    return total;
}

Summary toss_coins_until_more_heads(std::mt19937& gen, int surplus, long double bias = 0.0L) {
    int heads = 0;
    int tails = 0;
    int tosses = 0;

    if (bias == 0.0L) {
        FastCoin coin(gen);

        while (heads < tails + surplus) {
            ++tosses;
            if (coin.flip()) {
                ++heads;
            } else {
                ++tails;
            }
        }
    } else {
        std::bernoulli_distribution coin(0.5 + bias);

        while (heads < tails + surplus) {
            ++tosses;
            if (coin(gen)) {
                ++heads;
            } else {
                ++tails;
            }
        }
    }

    long double fraction_heads = static_cast<long double>(heads) / tosses;

    return {heads, tails, tosses, fraction_heads};
}

BigFloat A(int s, BigFloat tolerance = BigFloat("1e-30"), int max_terms = 1000000) {
    if (s < 1) {
        throw std::invalid_argument("s must be >= 1");
    }

    BigFloat total = 0;

    int n = s;
    BigFloat term = pow(BigFloat(2), -(s + 1));

    for (int iter = 0; iter < max_terms; ++iter) {
        total += term;

        const int next_n = n + 2;

        const BigFloat ratio =
            BigFloat(n) * BigFloat(n) * BigFloat(n + 1) /
            (BigFloat(next_n) * BigFloat(n + s + 2) * BigFloat(n - s + 2));

        const BigFloat next_term = term * ratio;

        const BigFloat tail_estimate = BigFloat(next_n) * next_term;
        if (tail_estimate < tolerance) {
            break;
        }

        n = next_n;
        term = next_term;
    }

    return BigFloat("0.5") + total;
}

BigFloat b(int s) {
    if (s < 1) {
        throw std::invalid_argument("s must be >= 1");
    }

    if (s % 2 == 1) {
        // odd s = 2k+1
        int k = (s - 1) / 2;
        BigFloat sign = (k % 2 == 0) ? BigFloat(1) : BigFloat(-1);
        return sign * BigFloat(s); // coefficient of (pi/4)
    } else {
        // even s = 2k
        int k = s / 2;
        BigFloat sign = (k % 2 == 1) ? BigFloat(1) : BigFloat(-1);
        return sign * BigFloat(k); // coefficient of log(2)
    }
}

void simulate(int max_s, int tries) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::cout << std::setprecision(16);

    std::vector<BigFloat> expected(max_s + 1, BigFloat(0));
    std::vector<BigFloat> avgs(max_s + 1, BigFloat(0));
    std::vector<BigFloat> lerch(max_s + 1, BigFloat(0));

    for (int s = 1; s <= max_s; ++s) {
        expected[s] = A(s);
        lerch[s] = (BigFloat(s) / 2) * lerch_phi(BigFloat(-1), BigFloat(1), BigFloat(s) / 2);
        long double avg = 0.0L;

        for (long i = 0; i < tries; i++) {
            Summary result = toss_coins_until_more_heads(gen, s);

            avg = (avg * i + result.fraction_heads) / (i + 1);
        }

        avgs[s] = avg;

        std::cout << "s=" << s << " avg=" << avgs[s]
                  << " expected=" << expected[s]
                  << " lerch=" << lerch[s]
                  << " delta=" << boost::multiprecision::abs(BigFloat(avgs[s]) - lerch[s])
                  << std::endl;
    }

    std::cout << "\nsummary of convergence:" << std::endl;
    for (int s = 1; s <= max_s; ++s) {
        std::cout << "s=" << s << " avg=" << avgs[s]
                  << " expected=" << expected[s]
                  << " lerch=" << lerch[s]
                  << " delta=" << boost::multiprecision::abs(BigFloat(avgs[s]) - expected[s])
                  << std::endl;
    }
}

void solve(int max_s) {
    std::vector<BigFloat> expected(max_s + 1, BigFloat(0));

    for (int s = 1; s <= max_s; ++s) {
        expected[s] = A(s);
        BigFloat as, bs;
        bs = b(s);
        if (s % 2 == 1) {
            as = expected[s] - bs * (PI / 4);
        } else {
            as = expected[s] - bs * LNTWO;
        }
        auto [num, den] = approximate_fraction(as);

        std::cout << "s=" << s
                << " A(s)=" << expected[s]
                << " a=" << as
                << " ~ " << num << "/" << den
                << " b=" << bs
                << std::endl;
    }
}

void validate(int max_s) {
    std::vector<BigFloat> expected(max_s + 1, BigFloat(0));
    std::vector<BigFloat> lerch(max_s + 1, BigFloat(0));

    for (int s = 1; s <= max_s; ++s) {
        expected[s] = A(s);
        lerch[s] = lerch_phi(BigFloat(-1), BigFloat(1), BigFloat(s) / 2);
        BigFloat lerch_form = (BigFloat(s) / 2) * lerch[s];

        std::cout << "s=" << s
                << " A(s)=" << expected[s]
                << " LerchPhi=" << lerch_form
                << std::endl;
    }
}

int main() {
    std::cout << "Starting..." << std::endl;

    int max_s = 32;
    int tries = 1024;

    simulate(max_s, tries);

   return 0;
}