#ifndef LINEAR_HPP
#define	LINEAR_HPP
#include "Sum.hpp"
#include "Invariant.hpp"
#include "Constraint.hpp"
#include "Constants.hpp"
#include "LEQviolation.hpp"
#include "EQviolation.hpp"

class Linear : public Constraint {
protected:
    //    Sum* lhs;
    coefType rhs;
    int relation; // 0 is equal, 1 is LQ
    InvariantContainer invars;

public:


    /// Used to create the original (given by user) constraints

    Linear(std::vector<int> coefficients, std::vector<Variable*>& variables, coefType ub, int relation, unsigned priority) {
        //    Linear(Sum* lhs, int ub, int relation) : Constraint() {
        this->relation = relation;

        this->variables = variables;
        for (unsigned i = 0; i < variables.size(); i++) {
            //            int id = variables.at(i)->getID();
            //            this->coefficients[id] = coefficients.at(i);
            std::pair<int, coefType> coef(variables.at(i)->getID(), coefficients.at(i));
            this->coefficients.insert(coef);
        }
        rhs = ub;
        type = LINEAR;
        arguments.push_back(relation);
        arguments.push_back(ub);
        this->priority = priority;
        if (relation == EQ) {
            bool allUnit = true;
            for (int c : coefficients) {
                //            for (std::unordered_map<int, coefType>::iterator it = this->coefficients.begin(); it != this->coefficients.end(); ++it) {

                if (c != 1) {
                    if (c != -1) {
                        allUnit = false;
                        break;
                    }
                }
            }
            if (allUnit) {
                functional = true;
            }
        }
    }

    ~Linear() {
        //        delete lhs;

        coefficients.clear();
    }

    InvariantContainer& createInvariants() {
            int value = 0;
            //        InvariantContainer invars;
            std::unordered_map<int, coefType>& coef = this->getCoefficients();
            //                std::shared_ptr<Sum> sumInvariant = std::make_shared<Sum>(coef, model->getDDG());
            //                std::shared_ptr<Sum> sumInvariant = std::make_shared<Sum>(cons->getVariables(), coef);
            Sum* sumInvariant = new Sum(coef);
            //                    sumInvariant->invariantID = model->getInvariants().size();
            variableContainer variables;
            InvariantContainer invariants;
            for (Variable* iv : this->getVariables()) {
                unsigned id = iv->getID();
                if (!iv->isDef()) {

                    value += coef.at(id) * iv->getCurrentValue();
                    variables.push_back(iv);
                } else {
                    value += coef.at(id) * iv->getOneway()->getCurrentValue();
                    invariants.push_back(iv->getOneway());
                }

            }
            sumInvariant->setVariablePointers(variables);
            sumInvariant->setInvariantPointers(invariants);
            assert(variables.size() + invariants.size() == coef.size());

            sumInvariant->setValue(value);
            invars.push_back(sumInvariant);
            if (this->getPriority() != OBJ) {

                if (relation == LQ) {
                    LEQviolation* leq = new LEQviolation(sumInvariant, rhs);
                    if (value <= rhs) {
                        leq->setValue(0);
                    } else {
    //                    leq->setValue(1);
                        leq->setValue(value -rhs);
                    }
                    invars.push_back(leq);
                } else if (relation == EQ) {
                    EQviolation* eq = new EQviolation(sumInvariant, rhs);
                    if (value == rhs) {
                        eq->setValue(0);
                    } else {
                        eq->setValue(std::abs(value - rhs));
                        //                    eq->setValue(1);
                    }
                    invars.push_back(eq);
                } else {
                    std::cout << "Only LEQ and EQ linear constraint are implemented" << std::endl;
                }

            }

            assert(invars.size()>0);
            return invars;
        }

    bool canBeMadeOneway() {
        if(!functional){
            return false;
        }
        if (this->isOneway()) {
            return false;
        }
        //    if (cons->getPriority() == OBJ) {
        //        return false;
        //    }
        if (this->getArgument(0) == LQ) {
            return false;
        }
        //        std::cout <<  this->getArgument(1) << " ";
        for (auto it = coefficients.begin(); it != coefficients.end(); it++) {
            double coef = it->second;
            if (coef == 1 || coef == -1) {
                //                debug;
                //                return false; 
                //                return false; 
            } else {
                return false;
            }
        }
        // Find best iv to define (currently the one not defining other variables)
        bool canBeMadeOneway = false;
        Variable* bestVar;
        unsigned prevEqual = 0;
//        unsigned prio = 0;
        unsigned equalCounter = 0;
        unsigned defined = std::numeric_limits<unsigned int>::max();
        unsigned constraintsApplyingToiv = std::numeric_limits<unsigned>::max();

        for (Variable* iv : this->getVariables()) {
            if (iv->isFixed()) {
                continue;
            }
            if (iv->isDef()) {
                continue;
            }
            // Will not make cycles 
            //                                if(iv->getDefining()>0){
            //                                    continue;
            //                                }
            if (defined > iv->getDefining()) {
                bestVar = iv;
                canBeMadeOneway = true;
                constraintsApplyingToiv = iv->getDegree();
//                prio = iv->getSerachPriority();
                prevEqual = 1;
                //            defined = defining.at(iv->getID());
                defined = iv->getDefining();
                continue;
            } else if (defined == iv->getDefining()) {
                //                if (iv->isIntegerVariable() > bestVar->isIntegerVariable()) {
                //                    bestVar = iv;
                //                    canBeMadeOneway = true;
                //                    constraintsApplyingToiv = iv->usedIn;
                //                    prio = iv->getSerachPriority();
                //                    prevEqual = 1;
                //                    defined = iv->getDefining();

                //                } else 
                //                    if (iv->isIntegerVariable() == bestVar->isIntegerVariable()) {
                //
                //
                //                    if (iv->getSerachPriority() > prio) {
                //                        bestVar = iv;
                //                        canBeMadeOneway = true;
                //                        constraintsApplyingToiv = iv->usedIn;
                //                        prio = iv->getSerachPriority();
                //                        prevEqual = 1;
                //                        defined = iv->getDefining();
                //                    } else 
//                if (iv->getSerachPriority() == prio) {


                    if (iv->getDegree() < constraintsApplyingToiv) {
                        bestVar = iv;
                        canBeMadeOneway = true;
                        constraintsApplyingToiv = iv->getDegree();
//                        prio = iv->getSerachPriority();
                        prevEqual = 1;
                        defined = iv->getDefining();
                    } else if (iv->getDegree() == constraintsApplyingToiv) {
                        int choose = Random::Integer(prevEqual);
                        prevEqual++;
                        //            ties++
                        equalCounter++;
                        if (choose == 0) {
                            bestVar = iv;
                            canBeMadeOneway = true;
                            constraintsApplyingToiv = iv->getDegree();
//                            prio = iv->getSerachPriority();
                            defined = iv->getDefining();
                        }
                    }
//                }
                //        if (defining.at(iv->getID()) > 0) {
                //            return false;
                //        }
            }
        }

        if (canBeMadeOneway) {
            defining = bestVar;
            bestVar->increaseDefining();
        }
        return canBeMadeOneway;
        //        if (canBeMadeOneway) {
        ////            makeOneway(bestVar);
        //            return true;
        //        }
        //
        //        return false;
    }

    invariant makeOneway() {
        Variable* var = defining;
        assert(priority != OBJ);
        variableContainer& oldVars = this->getVariables();
        std::unordered_map<int, coefType> coefficients = this->getCoefficients();
        std::unordered_map<int, coefType> newCoefficients;
        double coeff = coefficients.at(var->getID());
        //    std::cout << cons->getArgument(1) << "  "<< coeff<< std::endl;

        coefType value = -this->getArgument(1);
        assert(coeff != 0);
        if (coeff == -1) {
            for (auto it = coefficients.begin(); it != coefficients.end(); ++it) {
                std::pair<int, coefType> coef(it->first, it->second);
                newCoefficients.insert(coef); //[it->first] = it->second;
            }
        } else {

            for (auto it = coefficients.begin(); it != coefficients.end(); ++it) {
                std::pair<int, coefType> coef(it->first, it->second / (-coeff));
                newCoefficients.insert(coef);
            }

            value = value / (-coeff);
        }
        if (coeff < -1 || 1 < coeff) {
            std::cout << coeff << std::endl;
            debug;
        }
        newCoefficients.erase(var->getID());
        //    std::shared_ptr<Sum> sumInvariant = std::make_shared<Sum>(newCoefficients);
        Sum* sumInvariant = new Sum(newCoefficients);
        //        InvariantContainer invariants;
        //        variableContainer vars;
        variableContainer varsAndInvars;

        sumInvariant->setStartValue(value);
        //    std::cout << value << " ";

        for (Variable* oldiv : oldVars) {
            if (oldiv != var) {
                unsigned id = oldiv->getID();
                value += newCoefficients.at(id) * (oldiv->getCurrentValue()*1.0);
                oldiv->increaseDefining();
                varsAndInvars.push_back(oldiv);
                //                if (var->isDef()) {
                //                    invariants.push_back(oldiv->getOneway());
                //                } else {
                //                    vars.push_back(oldiv);
                //                }
            }
        }

        sumInvariant->setVariablePointers(varsAndInvars);
        //        sumInvariant->setVariablePointers(vars);
        //        sumInvariant->setInvariantPointers(invariants);
        //        assert(vars.size() + invariants.size() == newCoefficients.size());
        //        sumInvariant->invariantID = model->getInvariants().size();
        //        model->addInvariant(sumInvariant);
        sumInvariant->setVariableID(var->getID());

        //        DDG->addInvariant(sumInvariant, vars, invars);
        //    model->addInvariantToDDG(sumInvariant, vars);
        //    DDG->addInvariant(sumInvariant, vars);
        assert(value == (int) value);
        assert(var->getCurrentValue() >= var->getLowerBound());
        sumInvariant->setValue(value);
        sumInvariant->setVariable(var);
        assert(var == defining);
            if (value != var->getCurrentValue()) {
                std::cout << value << " vs " << var->getCurrentValue() << std::endl;
                for(Variable* var : variables){
                    std::cout <<  var->getCurrentValue() << " "; 
                }
                std::cout <<  " = 1"<<  std::endl; 
            }
        var->setDefinedBy(sumInvariant, this);
        //                this->setInvariant(sumInvariant);
        onewayInvariant = sumInvariant;
        //            sumInvariant->variableID = iv->getID();
        sumInvariant->setBounds(var->getLowerBound(), var->getUpperBound());
        //        if (!sumInvariant->test()) {
        //
        //        }
        this->isOneway(true);
        assert(this->isOneway());
        return sumInvariant;


    }


    bool testCons() {
        //        int currentValue = 0;
        //        invariant invar = getInvariant();
        //
        //        //        for (std::shared_ptr<Invariant> invar : getInvariant()) {
        //        currentValue += invar->getCurrentValue();
        //        //        }
        //        if (currentValue <= rhs) {
        //            if (Violation != 0) {
        //                std::cout << "failed test Linear" << std::endl;
        //                std::cout << "Violation " << Violation << " lhs " << invar->getCurrentValue() << " rhs " << rhs << std::endl;
        //                return false;
        //            }
        //        } else {
        //            if (Violation != 1) {
        //                std::cout << "failed test Linear" << std::endl;
        //                std::cout << "Violation " << Violation << " lhs " << invar->getCurrentValue() << " rhs " << rhs << std::endl;
        //                return false;
        //            }
        //        }
        return true;
    }

    //    bool testObj() {
    //
    //        int currentValue = 0;
    //        std::shared_ptr<Invariant> invar = getInvariant();
    //
    //        //        for (std::shared_ptr<Invariant> invar : getInvariant()) {
    //        currentValue += invar->getCurrentValue();
    //        //        }
    //        if (ViolationDegree != currentValue - rhs) {
    //            std::cout << "failed test Linear obj" << std::endl;
    //            std::cout << "ViolationDegree " << ViolationDegree << " lhs " << invar->getCurrentValue() << " rhs " << rhs << std::endl;
    //            return false;
    //
    //        }
    //        return true;
    //    }

};

#endif	/* LINEAR_HPP */

