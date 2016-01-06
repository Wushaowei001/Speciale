
#include "FlipObjNE.hpp"
//#include "State.hpp"

FlipObjNE::FlipObjNE(std::shared_ptr<Model> model, std::shared_ptr<State> st) {
    this->model = model;
    this->state = st;
}

//FlipObjNE::FlipObjNE(const FlipObjNE& orig) {
//}

FlipObjNE::~FlipObjNE() {
    //    std::cout << "Destructing FlipObjNE" << std::endl;
}
Move* FlipObjNE::next() {
    Variable* iv = model->getEvaluationVariableNr(moveCounter);
    moveCounter++;
    //    Move* mv = new Move(iv, (1 - iv->getCurrentValue()) - iv->getCurrentValue());
    Move* mv = new Move(iv, (1 - iv->getCurrentValue()) - iv->getCurrentValue());
    //    mv->deltaVector.resize(model->getPriorityVectors().size());
    mv->deltaVector.resize(model->getPriorityVectors().size(), 0);
    return mv;
}

bool FlipObjNE::hasNext() {
    if (moveCounter < model->getEvaluationVariables().size()) {
        return true;
    } else {
        moveCounter = 0;
        
        return false;
    }
}

Move* FlipObjNE::nextRandom() {
    Variable* iv = model->getEvaluationVariableNr(Random::Integer(0, (int) model->getEvaluationVariables().size() - 1));
    randomCounter++;
    //    Move* mv = new Move(iv, (1 - iv->getCurrentValue()) - iv->getCurrentValue());
    Move* mv = new Move(iv, (1 - iv->getCurrentValue()) - iv->getCurrentValue());
    //    mv->deltaVector.resize(model->getPriorityVectors().size());
    mv->deltaVector.resize(model->getPriorityVectors().size(), 0);
    return mv;
}

bool FlipObjNE::hasNextRandom() {
    if (randomCounter < randomMovesWanted) {
        return true;
    } else {
        randomCounter = 0;
        return false;
    }
}

void FlipObjNE::setRandomCounter(unsigned numberOfRandomMoves) {
    randomMovesWanted = numberOfRandomMoves;
}

bool FlipObjNE::calculateDelta(Move* mv) {

    std::vector<int>& change = mv->getDeltaVector();

    Variable* variable = mv->var;
    propagation_queue& queue = model->getPropagationQueue(variable);
    updateVector& update = model->getUpdate(variable);

    for (updateType& invar : update) {
        invar->proposeChange(variable->getID(), mv->getVariableChange());
    }
    bool legal = true;
    for (updateType invar : queue) {

        legal = invar->calculateDeltaValue();
        if (!legal) {
            break;
        }
        if (invar->getDeltaValue() != 0) {
            for (updateType inv : model->getUpdate(invar)) {
                inv->proposeChange(invar->getVariableID(), invar->getDeltaValue());
            }
        }
    }
    if (!legal) {
        for (updateType invar : queue) {
            invar->calculateDeltaValue();
        }
    } else {
        for (unsigned i = 0; i < change.size(); i++) {
            change[i] = model->getEvaluationInvariantNr(i)->getDeltaValue();
        }
    }


    return legal;
}

bool FlipObjNE::commitMove(Move* mv) {
    moveCounter = 0;
    Variable* var = mv->getVar();
    std::vector<int>& evaluation = state->getEvaluation();

    // Skal genberegne!!!!!
    bool legal = calculateDelta(mv);
    if (!legal) {
        return false;
    }
    var->setCurrentValue(1 - var->getCurrentValue());

    propagation_queue queue = model->getPropagationQueue(var);
    for (updateType invar : queue) {

        invar->updateValue();
        if (invar->representConstraint()) {
            if (invar->getCurrentValue() == 0) {
                if (invar->getInvariantPointers().back()->inViolatedConstraints()) {
//                    std::unordered_map<unsigned, invariant>& vioCons = model->getViolatedConstraints();
                    model->removeViolatedConstraint(invar->getInvariantPointers().back());

                }
            } else {
                if (!invar->inViolatedConstraints()) {
//                    std::unordered_map<unsigned, invariant>& vioCons = model->getViolatedConstraints();
                    model->addViolatedConstraint(invar->getInvariantPointers().back());


                }
            }
        }
    }
    for (unsigned i = 0; i < model->getEvaluationInvariants().size(); i++) {
        evaluation[i] = model->getEvaluationInvariantNr(i)->getCurrentValue();
    }

    return true;
}

//void FlipObjNE::makeMove(Move* mv, std::shared_ptr<State> st) {
//    commitMove(mv, st);
//}