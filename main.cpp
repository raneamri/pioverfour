#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <cmath>

#define PIOVERFOUR (M_PI / 4)
#define LNTWO (M_LN2)
#define VERBOSE 1

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

Score toss_coins(int n, std::mt19937& gen) {
    int heads = 0;
    std::bernoulli_distribution coin(0.5);

    for (int i = 0; i < n; ++i) {
        heads += static_cast<int>(coin(gen));
    }

    return {heads, n - heads};
}

Summary toss_coins_until_more_heads(std::mt19937& gen, int surplus) {
    int heads = 0;
    int tails = 0;
    int tosses = 0;
    std::bernoulli_distribution coin(0.5);

    while (heads < tails + surplus) {
        ++tosses;
        if (coin(gen)) {
            ++heads;
        } else {
            ++tails;
        }
    }

    long double fraction_heads = static_cast<long double>(heads) / tosses;

    return {heads, tails, tosses, fraction_heads};
}

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());

    int tries = pow(2, 16);
    long double avg = 0.0L;
    int surplus = 2;

    for (long i = 0; i < tries; i++) {
        Summary result = toss_coins_until_more_heads(gen, surplus);

        avg = (avg * (i+1) + result.fraction_heads) / ((i+1) + 1);

        if (VERBOSE && i % 1000 == 0) {
            std::cout << "trial=" << i
                      << " avg=" << avg
                      << " target=" << ((surplus == 1) ? PIOVERFOUR : LNTWO)
                      << " delta=" << abs(avg - ((surplus == 1) ? PIOVERFOUR : LNTWO)) << '\n';
        }
    }

    std::cout << "final delta=" << abs(avg - ((surplus == 1) ? PIOVERFOUR : LNTWO)) << std::endl;

   return 0;
}