
#ifndef EQVIOLATION_HPP
#define	EQVIOLATION_HPP
#include <memory>
#include "Constants.hpp"
#include <unordered_map>
//#include "IntegerVariable.hpp"
#include "Invariant.hpp"
//#include "DependencyDigraph.hpp" 
//#include "DependencyDigraph.hpp"
//#include <utility>
//struct elem {
//	int index;
//	double coeff;
//	~elem() {};
//};
class EQviolation : public Invariant {
   
        
protected:
    
//    std::vector<IntegerVariable*> VariablePointers;
    //    double oldValue = 0;
//    std::vector<std::pair<int, int>> VariableChange;
    std::vector<int> VariableChange;
    int RHS;
    invariant LHS;
//    std::shared_ptr<DependencyDigraph> DDG;
    
    
public:
    
//    Sum(std::vector<IntegerVariable*>& vars, std::vector<int>& c, unsigned id);
    /// Purely used for testing
    EQviolation(invariant invar, int RHS);
//    Sum(std::unordered_map<int,coefType> map, unsigned id);
//    Sum(std::unordered_map<int,coefType> map,  std::shared_ptr<DependencyDigraph> DDG);
//    EQviolation(std::unordered_map<int,coefType> map);
    
    

    ~EQviolation();

//    Sum& operator=(const Sum &a);

//    void rollback() {
//        // Skal den overhovedet bruges?
//    }
//
//    void initialize() {
//        // er det hurtigere at løbe igennem invarianter end at løbe gennem updates?
//    }

    bool calculateDelta();
//    void initialize();
    
//    void usedByConstraint(int constraint, int priority);
//    void usedByObjective(int constraint, int priority);

    void proposeChange(int variableNumber, int changeInValue);
    void updateValue();


//    void addChange(std::vector<int> variableNumbers, std::vector<int> changes);

//    void addChangeSwap(int variableNumber1, int variableNumber2);


    bool test();
};
#endif	/* EQVIOLATION_HPP */

