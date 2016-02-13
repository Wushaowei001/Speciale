#include "SwapNeighborhood.hpp"

SwapNeighborhood::SwapNeighborhood(std::shared_ptr<Storage> model, std::shared_ptr<State> st) {
    this->model = model;
    this->state = st;
    std::cout << "Still uses constraints, not invariants" << std::endl;
    debug;
    exit(1);
}

SwapNeighborhood::~SwapNeighborhood() {
}

Move* SwapNeighborhood::next() {
    if (moveCounter < model->getMask().size() - 1) {
        Variable* iv;
        Variable* iv2;
        if (moveCounter2 == model->getMask().size() - 1) {
            iv2 = model->getMaskAt(moveCounter2);
            moveCounter2 = 0;
            iv = model->getMaskAt(moveCounter);
            moveCounter++;
        } else {
            iv = model->getMaskAt(moveCounter);
            iv2 = model->getMaskAt(moveCounter2);
            moveCounter2++;
        }
        while (iv->getCurrentValue() == iv2->getCurrentValue() && moveCounter != model->getMask().size() - 1) {
            if (moveCounter2 == model->getMask().size() - 1) {
                iv2 = model->getMaskAt(moveCounter2);
                moveCounter2 = 0;
                iv = model->getMaskAt(moveCounter);
                moveCounter++;
            } else {
                iv = model->getMaskAt(moveCounter);
                iv2 = model->getMaskAt(moveCounter2);
                moveCounter2++;
            }
        }
        std::vector<Variable*> vars;
        vars.push_back(iv);
        vars.push_back(iv2);

        std::vector<int> delta;
        delta.push_back(iv2->getCurrentValue() - iv->getCurrentValue());
        delta.push_back(iv->getCurrentValue() - iv2->getCurrentValue());
        Move* mv = new Move(vars, delta);
        mv->deltaVector.resize(state->getEvaluation().size(), 0);
        return mv;
    } else {
        moveCounter = 0;
        moveCounter2 = 1;
        return NULL;
    }
}


unsigned SwapNeighborhood::getSize() {
    return model->getMask().size() * (model->getMask().size() - 1) / 2;
}

Move * SwapNeighborhood::nextRandom() {
    Variable* iv1 = model->getMaskAt(Random::Integer(0, (int) model->getMask().size() - 1));
    Variable* iv2 = model->getMaskAt(Random::Integer(0, (int) model->getMask().size() - 1));
    while (iv1->getCurrentValue() == iv2->getCurrentValue() && model->getMask().size() > 1) {
        iv1 = model->getMaskAt(Random::Integer(0, (int) model->getMask().size() - 1));
        iv2 = model->getMaskAt(Random::Integer(0, (int) model->getMask().size() - 1));
    }
    std::vector<Variable*> vars;
    vars.push_back(iv1);
    vars.push_back(iv2);
    std::vector<int> delta;
    delta.push_back(iv2->getCurrentValue() - iv1->getCurrentValue());
    delta.push_back(iv1->getCurrentValue() - iv2->getCurrentValue());
    Move* mv = new Move(vars, delta); //    mv->deltaVector.resize(state->getEvaluation().size());
    mv->deltaVector.resize(state->getEvaluation().size(), 0);
    return mv;
}
bool SwapNeighborhood::calculateDelta(Move * mv) {

    std::vector<int>& change = mv->getDeltaVector();
    for (unsigned i = 0; i < change.size(); i++) {
        model->getEvaluationInvariantNr(i)->calculateDelta();
    }
    std::vector<Variable*>& variables = mv->getVars();
    propagation_queue& queue1 = model->getPropagationQueue(variables.at(0));
    propagation_queue& queue2 = model->getPropagationQueue(variables.at(1));


    // #########################################################
    // Create queue by comparison
    // #########################################################
    std::vector<updateType> queue;
    propagation_queue::iterator iter1 = queue1.begin();
    propagation_queue::iterator iter2 = queue2.begin();
    while (iter1 != queue1.end() || iter2 != queue2.end()) {


        if ((*iter1)->getTimestamp()> (*iter2)->getTimestamp()) {
            queue.push_back((*iter1));
            iter1++;
        } else if ((*iter1)->getTimestamp() == (*iter2)->getTimestamp()) {
            queue.push_back((*iter1));
            iter1++;
            iter2++;
        } else {
            queue.push_back(*iter2);
            iter2++;
        }
    }
    while (iter1 != queue1.end()) {
        queue.push_back(*iter1);
        iter1++;
    }
    while (iter2 != queue2.end()) {
        queue.push_back(*iter2);
        iter2++;
    }

    
    updateVector& update1 = model->getUpdate(variables.at(0));
    updateVector& update2 = model->getUpdate(variables.at(1));
    for (updateType& invar : update1) {
        invar->proposeChange(variables.at(0)->getID(), mv->getVariableChanges().at(0));
    }
    for (updateType& invar : update2) {
        invar->proposeChange(variables.at(1)->getID(), mv->getVariableChanges().at(1));
    }
    bool legal = true;

    for (updateType invar : queue) {

        legal = invar->calculateDelta();
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
            invar->calculateDelta();
        }
    } else {
        for (unsigned i = 0; i < model->getEvaluationInvariants().size(); i++) {
            change[i] = model->getEvaluationInvariants().at(i)->getDeltaValue();
        }
    }
    return legal;
}

bool SwapNeighborhood::commitMove(Move * mv) {
    moveCounter = 0;
    moveCounter2 = 0;
    Variable* var1 = mv->getVars().at(0);
    Variable* var2 = mv->getVars().at(1);
    std::vector<int>& evaluation = state->getEvaluation();
    // Skal genberegne!!!!!
    bool legal = calculateDelta(mv);
    if (!legal) {
        return false;
    }
    var1->setCurrentValue(1 - var1->getCurrentValue());
    var2->setCurrentValue(1 - var2->getCurrentValue());

    propagation_queue& queue1 = model->getPropagationQueue(var1);
    propagation_queue& queue2 = model->getPropagationQueue(var2);

    // #########################################################
    // Create queue by comparison
    // #########################################################
    std::vector<updateType> queue;
    propagation_queue::iterator iter1 = queue1.begin();
    propagation_queue::iterator iter2 = queue2.begin();
    while (iter1 != queue1.end() || iter2 != queue2.end()) {


        if ((*iter1)->getTimestamp()> (*iter2)->getTimestamp()) {
            queue.push_back((*iter1));
            iter1++;
        } else if ((*iter1)->getTimestamp() == (*iter2)->getTimestamp()) {
            queue.push_back((*iter1));
            iter1++;
            iter2++;
        } else {
            queue.push_back(*iter2);
            iter2++;
        }
    }
    while (iter1 != queue1.end()) {
        queue.push_back(*iter1);
        iter1++;
    }
    while (iter2 != queue2.end()) {
        queue.push_back(*iter2);
        iter2++;
    }
    for (updateType invar : queue) {
        invar->updateValue();
        if (invar->representConstraint()) {
            if (invar->getCurrentValue() == 0) {
                if (invar->getInvariantPointers().back()->inViolatedConstraints()) {
                    model->removeViolatedConstraint(invar->getInvariantPointers().back());
                }
            } else {
                if (!invar->getInvariantPointers().back()->inViolatedConstraints()) {
                    model->addViolatedConstraint(invar->getInvariantPointers().back());
                }
            }
        }

    }
    for (unsigned i = 0; i < model->getEvaluationInvariants().size(); i++) {
        evaluation[i] = model->getEvaluationInvariants().at(i)->getCurrentValue();
    }

    return true;
}