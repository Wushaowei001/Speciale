#ifndef GENERALSOLVER_HPP
#define	GENERALSOLVER_HPP
#include <cmath>
#include <algorithm>
#include "LSSpace.hpp"
#include <assert.h>
#include <gecode/driver.hh>
#include <gecode/int.hh>
//#include <gecode/minimodel.hh>
#include <limits>
#include "IntegerVariable.hpp"
//#include "Invariant.hpp"
//#include "Constraint.hpp"
#include "Clock.hpp"
//#include "getRSS.hpp"
//#include "Random.hpp"
//using namespace Gecode;

class GeneralSolver : public Gecode::Space, private LSSpace {
    friend class Test;
private:
    //    Gecode::IntVarArray* solutions;
    Gecode::IntVarArray IntVars;
    //    std::vector<Gecode::IntVarArgs*> test;
public:

    GeneralSolver() {
        //        Random::Seed(SEED);

    }

    ~GeneralSolver() {
        //        std::cout << "Destructing GS" << std::endl;
    }

    GeneralSolver& operator=(const GeneralSolver &a) {
        this->IntVars = a.IntVars;
        //        std::cout << "hest " << std::endl;
        return *this;

    }

    void branch(Gecode::Home home, vector<IntegerVariable*>* vars, Gecode::IntVarBranch var, Gecode::IntValBranch val) {

        Gecode::branch(home, IntVars, var, val);
        //        std::cout << IntVars.size() << std::endl;

    }

    void linear(GeneralSolver& s, std::vector<int>* coefficients, vector<IntegerVariable*>* variables, Gecode::IntRelType irt, int ub, Gecode::IntConLevel icl, int type) { // 0 = obj, 1 = soft, 2 = hard 
        if (type == OBJ) {
            // add to obj fnc
            LSSpace::linear(coefficients, variables, 0, ub, type);

        } else if (type== SOFT) {
            
        } else {
            Gecode::IntArgs c(coefficients->size());
            Gecode::IntVarArgs x(coefficients->size());
            for (unsigned j = 0; j < coefficients->size(); j++) {
                c[j] = coefficients->at(j);
                x[j] = *(variables->at(j)->getVariablePointer());
            }
            //        std::cout << "før gecode" << std::endl;

            Gecode::linear(s, c, x, irt, ub, icl);

            // What should be given to LSSpace
            if (irt == Gecode::IRT_EQ) {
                LSSpace::linear(coefficients, variables, 0, ub, type);
            } else if (irt == Gecode::IRT_LQ) {
                LSSpace::linear(coefficients, variables, 1, ub, type);
            }
        }
    }

    //    void SetValues(Gecode::IntVarArray vars) {
    //        for (int i = 0; i < vars.size(); i++) {
    //            assert(vars[i].assigned());
    //            this->IntVarVector.at(i)->setCurrentValue(vars[i].val());
    //        }
    //    }

    std::vector<IntegerVariable*>* createIntVars(unsigned numberOfVariables, int lb, int ub) {
        // Given to gecode space
        Gecode::IntVarArray Vars(*this, numberOfVariables, lb, ub);

        // Looks bad but does not copy the variables. Points to two different places in memory but the variables in the two arrays have the same memory address. 
        IntVars = Vars;
        // Given to LS space


        return LSSpace::addIntVariablesToState(&IntVars);
        //        return LSSpace::addIntVariablesToLS(IntVars);
        //        return vars;
    }

    Gecode::BoolVarArray createBoolVars(int amount, int lb, int ub) {
        // Given to gecode space
        Gecode::BoolVarArray vars(*this, amount, lb, ub);

        // Given to LS space
        return vars;
    }

    /// Constructor for cloning s

    GeneralSolver(bool share, GeneralSolver & s) :
    Gecode::Space(share, s) {
        //        std::cout << "this is not a line " << IntVars.size() << std::endl;
        IntVars.update(*this, share, s.IntVars);
        // remember to update your main variables!
        //		model = s.model;

    }

    // Copy during cloning

    virtual Space * copy(bool share) {
        //        std::cout << "copy" << std::endl;
        return new GeneralSolver(share, *this);
    }

    void print(std::ostream & os) const {
        os << IntVars << std::endl;
    }

    void print(std::vector<IntegerVariable> IntegerVariables) {
        std::cout << "{";
        for (unsigned i = 0; i < IntegerVariables.size(); i++) {
            std::cout << *(IntegerVariables[i].getVariablePointer()) << ", ";
        }
        std::cout << std::endl;
    }


    //    void InitialSolution(Gecode::InstanceOptions opt) {

    /// Uses Gecode to find initial solution
    /// returns a new GeneralSolver with the initial solution the old (the one this method is called from) is not updated with new solution. 

    GeneralSolver* InitialSolution(Gecode::Search::Options* so) {
        //	// This is to stop the search at the time limit imposed
        //        Gecode::Search::Options so;
        //        print(std::cout);

        //        startTimer(so);


        printSpaceStatus();
        this->print(std::cout);
        std::cout << "after status" << std::endl;
        GeneralSolver* s;

        //        Gecode::Search::Cutoff* c = Gecode::Search::CutoffConstant::constant(1);
        //        c->constant(1);
        //        std::cout <<  so.stop << std::endl;

        //        so.stop = new Gecode::Search::NodeStop(8000000);
        //        so.threads =1;
        //        so->stop->node(800000);
        //        so.stop->fail(10000);
        //        so.stop->time(300000);

        so->c_d = IntVars.size();
        so->a_d = so->c_d - 1;

        std::cout << "clone distance " << so->a_d << std::endl;
        std::cout << "Clone commit distance " << so->c_d << std::endl;
        try {
            std::clock_t GecodeClock = std::clock();

            //            std::cout << "try" << std::endl;
            Gecode::DFS<GeneralSolver> e(this, *so);
            std::cout << "Still searching for solution" << std::endl;
            s = e.next();
            //            Gecode::Search::TimeStop
            //            std::cout << " print" << std::endl;
            if (!e.stopped()) {
                //                std::cout << "e did not stop" << std::endl;
                if (s != NULL) {
                    //                    std::cout << "s not null" << std::endl;
                    if (!s->failed()) {
                        Gecode::Search::Statistics stat = e.statistics();

                        print_stats(stat);
                    }
                }
            }
            //            std::cout <<  s.
            //            s = e.next();
            std::cout << "Gecode found solution after " << (std::clock() - Clock::globalClock) / (double) CLOCKS_PER_SEC << std::endl;

            if (e.stopped()) {
                cout << "WARNING: solver stopped, solution is not optimal!\n";

                if (so->stop->stop(e.statistics(), *so)) {
                    //                    cout << "\t Solver stopped because of TIME LIMIT!\n";
                    //                    cout << "\t Solver stopped because of  NODE LIMIT!\n";
                    std::cout << "\t Number of nodes expanded: " << e.statistics().node << std::endl;
                    std::cout << "\t Number of failed nodes: " << e.statistics().fail << std::endl;
                    std::cout << "\t Number of restarts: " << e.statistics().restart << std::endl;
                    double time = (std::clock() - GecodeClock) / (double) CLOCKS_PER_SEC;
                    std::cout << "\t Time spend searching for solution: " << time << " seconds" << std::endl;
                }
            }


        } catch (Gecode::Exception e) {
            std::cerr << "Gecode exception: " << e.what() << std::endl;
            //        return 1;
        }

        int timesRelaxed = 0;
        std::cout << "stabel " << s->stable() << std::endl;
        //                while (s == NULL) {
        //        s = relax(so, this, timesRelaxed);
        //        timesRelaxed++;
        //                }
        assert(s != NULL);
        assert(!s->failed());

        return s;
        //
        //        this->print(cout);

    }
    /// relaxes the space (reduce the number of constraints). Used when Gecode cant find a solution in time.
    /// Only works for binary

    GeneralSolver* relax(Gecode::Search::Options* so, GeneralSolver* GS, int timesRelaxed) {
        GeneralSolver* relaxed = new GeneralSolver();
        relaxed->st = this->st;
        //        Gecode::IntVarArray Vars(*this, IntVars.size(), 0, 1);
        //        IntVars = Vars;

        relaxed->IntVars = IntVars;
        if (timesRelaxed > 0 || st->getSoftConstraints()->size() == 0) {
            // Maybe split this into more methods. eg relax half of const, relax top ten with most variables, relax in topological order. 
            simpleRelax(relaxed);
        } else {
            // Do not give soft to gecode
        }


        return relaxed;
        // Brug operator = til at lave det gamle space til det nye? Ellers skal det hele løses med det nye og gamle pointer skal opdateres. 
        // Hvordan fungere det så med at slette? 


    }

    void simpleRelax(GeneralSolver* relaxed) {
        std::cout << "do nothing" << std::endl;
        std::vector<Constraint*>* hardConstraints = st->getHardConstraints();
        std::vector<Invariant*>* invariants = st->getInvariants();
        for (int i = 0; i < invariants->size(); i++) {
//            Constraint* cons = hardConstraints->at(i);
            Invariant* invar = invariants->at(i);
            std::vector<IntegerVariable*>* integerVariables = invar->VariablePointers;
            Gecode::IntArgs c(integerVariables->size());
            Gecode::IntVarArgs x(integerVariables->size());
            for (unsigned j = 0; j < integerVariables->size(); j++) {
//                c[j] = HVORDAN FÅR JEG DEN?;
                x[j] = *(integerVariables->at(j)->getVariablePointer());
            }
            Constraint* cons = hardConstraints->at(invar->usedInObjectiveNr);
            // irt (eq, lq), ub, og IntegerConsisdencyLevel. Hvor skal jeg få dem fra?
            
            
            
            //        std::cout << "før gecode" << std::endl;

            Gecode::linear(relaxed, c, x, irt, ub, icl);
                    //            st->getInvariants()->at(0).

        }
    }

    void initializeLS(GeneralSolver* GS) {
        SetValues(GS->IntVars);

        initializeInvariants();

        initializeConstraints();

        initializeObjective();
        delete GS;
    }

    int getInitialValue() {
        return this->initialValue;
    }

    void optimizeSolution(int time) {
        LSSpace::optimizeSolution(time);

    }
    // Only for testing

    Gecode::IntVarArray* getIntVars() {
        return &IntVars;
    }

    void printCurrent() {
        LSSpace::printCurrent();
    }


private:



    //    void initializeInvariants() {
    //        for (int i = 0; i < LSSpace::Invariants.size(); i++) {
    //            LSSpace::Invariants.at(i).initialize();
    //        }
    //    }

    void printSpaceStatus() {
        std::cout << "in status" << std::endl;

        std::cout << this->status() << std::endl;

        std::cout << "can it find status? " << std::endl;
        Gecode::SpaceStatus status = this->status();
        if (status == Gecode::SS_FAILED) {
            cout << "Status: " << this->status() << " the space is failed at root."
                    << endl;
            std::cout << "No feasible solution" << std::endl;
            exit(1);
        } else if (status == Gecode::SS_SOLVED) {
            cout << "Status: " << this->status()
                    << " the space is not failed but the space has no brancher left."
                    << endl;
        } else if (status == Gecode::SS_BRANCH) {
            cout << "Status: " << this->status()
                    << " the space is not failed and we need to start branching."
                    << endl;
        }

    }

    //    void startTimer(Gecode::Search::Options so) {
    //        //        Gecode::Search::TimeStop* ts = new Gecode::Search::TimeStop(opt.time());
    //        Gecode::Search::TimeStop* ts = new Gecode::Search::TimeStop(1);
    //        so.stop = ts;
    //        Gecode::Search::Statistics stat;
    //        //
    //        // Let's start the Timer
    //        Gecode::Support::Timer t;
    //        t.start();
    //        
    //        //        delete ts;
    //    }

    void print_stats(Gecode::Search::Statistics & stat) {
        cout << "\tfail: " << stat.fail << endl;
        cout << "\tnodes: " << stat.node << endl;
        cout << "\tpropagators: " << stat.propagate << endl; // see page 145 MPG
        cout << "\tdepth: " << stat.depth << endl;
        cout << "\trestarts: " << stat.restart << endl;
        cout << "\tnogoods: " << stat.nogood << endl;
    }



};

#endif	/* GENERALSOLVER_HPP */

