#ifndef RENDERCRAFT_CORE_SAMPLER_H
#define RENDERCRAFT_CORE_SAMPLER_H

#include <Core/core.h>
#include <Core/rand-gen.h>
#include <random>

namespace rdcraft {
class Sampler {
  public:
    Real get() {
      return dis(gen);
    }
    template <int Dim>
    Vector<Real, Dim> sample() {
      Vector<Real, Dim> ret;
      for (int i = 0; i < Dim; i++)
        ret[i] = dis(gen);
      return ret;
    }
    Real sample() {
      return dis(gen);
    }

  private:
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;
};

struct DiscreteDistribution {
  explicit DiscreteDistribution(int n)
    : probabilities(n) {
  }
  void buildFromWeights(const std::vector<Real>& weights) {
    probabilities = weights;
    auto n = probabilities.size();
    alias.resize(n);
    std::vector<Real> scaledProbabilities = probabilities;
    for (int i = 0; i < n; ++i)
      scaledProbabilities[i] *= n;
    std::vector<int> small, large;
    for (int i = 0; i < n; ++i) {
      if (scaledProbabilities[i] < 1.0)
        small.push_back(i);
      else
        large.push_back(i);
    }

    while (!small.empty() && !large.empty()) {
      int less = small.back();
      small.pop_back();
      int more = large.back();
      large.pop_back();

      alias[less] = more;
      scaledProbabilities[more] =
          scaledProbabilities[more] + scaledProbabilities[less] - 1.0;

      if (scaledProbabilities[more] < 1.0)
        small.push_back(more);
      else
        large.push_back(more);
    }

    while (!large.empty()) {
      int curr = large.back();
      large.pop_back();
      scaledProbabilities[curr] = 1.0;
    }

    while (!small.empty()) {
      int curr = small.back();
      small.pop_back();
      scaledProbabilities[curr] = 1.0;
    }
  }

  struct SampleRecord {
    int idx;
    Real pdf;
  };
  SampleRecord sample() const {
    int idx = randomInt(0, probabilities.size() - 1);
    Real pdf = probabilities[idx] * probabilities.size();
    if (randomReal() < pdf)
      return {idx, pdf};
    else
      return {alias[idx], pdf};
  }
  Real prob(int idx) const {
    return probabilities[idx] * probabilities.size();
  }
  std::vector<Real> probabilities;
  std::vector<int> alias;
};
}

#endif