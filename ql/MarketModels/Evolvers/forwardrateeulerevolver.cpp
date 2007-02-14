/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Ferdinando Ametrano
 Copyright (C) 2006 Mark Joshi

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/reference/license.html>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/
#include <ql/MarketModels/Evolvers/forwardrateeulerevolver.hpp>
#include <ql/MarketModels/duffsdeviceinnerproduct.hpp>

namespace QuantLib {

    ForwardRateEulerEvolver::ForwardRateEulerEvolver(
                           const boost::shared_ptr<MarketModel>& marketModel,
                           const BrownianGeneratorFactory& factory,
                           const std::vector<Size>& numeraires,
                           Size initialStep)
    : marketModel_(marketModel),
      numeraires_(numeraires),
      initialStep_(initialStep),
      n_(marketModel->numberOfRates()), F_(marketModel_->numberOfFactors()),
      curveState_(marketModel->evolution().rateTimes()),
      forwards_(marketModel->initialRates()),
      displacements_(marketModel->displacements()),
      logForwards_(n_), initialLogForwards_(n_), drifts1_(n_),
      initialDrifts_(n_), brownians_(F_), correlatedBrownians_(n_),
      alive_(marketModel->evolution().firstAliveRate())
    {
        checkCompatibility(marketModel->evolution(), numeraires);

        Size steps = marketModel->evolution().numberOfSteps();

        generator_ = factory.create(F_, steps-initialStep_);

        currentStep_ = initialStep_;

        calculators_.reserve(steps);
        fixedDrifts_.reserve(steps);
        for (Size j=0; j<steps; ++j) {
            const Matrix& A = marketModel_->pseudoRoot(j);
            calculators_.push_back(LMMDriftCalculator(A,
                                                   displacements_,
                                                   marketModel->evolution().rateTaus(),
                                                   numeraires[j],
                                                   alive_[j]));
            std::vector<Real> fixed(n_);
            for (Size k=0; k<n_; ++k) {
                Real variance =
                    std::inner_product(A.row_begin(k), A.row_end(k),
                                       A.row_begin(k), 0.0);
                fixed[k] = -0.5*variance;
            }
            fixedDrifts_.push_back(fixed);
        }

        setForwards(marketModel_->initialRates());
    }

    const std::vector<Size>& ForwardRateEulerEvolver::numeraires() const {
        return numeraires_;
    }

    void ForwardRateEulerEvolver::setForwards(const std::vector<Real>& forwards)
    {
        QL_REQUIRE(forwards.size()==n_,
                   "mismatch between forwards and rateTimes");
        for (Size i=0; i<n_; ++i) 
            initialLogForwards_[i] = std::log(forwards[i] +
                                              displacements_[i]);
        calculators_[initialStep_].compute(forwards, initialDrifts_);
    }

    void ForwardRateEulerEvolver::setInitialState(const CurveState& cs) {
        setForwards(cs.forwardRates());
    }

    Real ForwardRateEulerEvolver::startNewPath() {
        currentStep_ = initialStep_;
        std::copy(initialLogForwards_.begin(), initialLogForwards_.end(),
                  logForwards_.begin());
        return generator_->nextPath();
    }

    Real ForwardRateEulerEvolver::advanceStep()
    {
        // we're going from T1 to T2

        // a) compute drifts D1 at T1;
        if (currentStep_ > initialStep_) {
            calculators_[currentStep_].compute(forwards_, drifts1_);
        } else {
            std::copy(initialDrifts_.begin(), initialDrifts_.end(),
                      drifts1_.begin());
        }

        // b) evolve forwards up to T2 using D1;
        Real weight = generator_->nextStep(brownians_);
        const Matrix& A = marketModel_->pseudoRoot(currentStep_);
        const std::vector<Real>& fixedDrift = fixedDrifts_[currentStep_];

        Size alive = alive_[currentStep_];
        for (Size i=alive; i<n_; i++) {
            logForwards_[i] += drifts1_[i] + fixedDrift[i];
            logForwards_[i] +=
                std::inner_product(A.row_begin(i), A.row_end(i),
                                   brownians_.begin(), 0.0);
            forwards_[i] = std::exp(logForwards_[i]) - displacements_[i];
        }

        // same as PC evolver with two steps dropped

        // c) update curve state
        curveState_.setOnForwardRates(forwards_);

        ++currentStep_;

        return weight;
    }

    Size ForwardRateEulerEvolver::currentStep() const {
        return currentStep_;
    }

    const CurveState& ForwardRateEulerEvolver::currentState() const {
        return curveState_;
    }

}
