#include "Move.hpp"
#include "Storage.hpp"
#include "Constants.hpp"
#include <memory>
#include "Neighborhood.hpp"
//#include "Random.hpp"
//#include "boost/random/mersenne_twister.hpp"
//#include "boost/random/uniform_int_distribution.hpp"
#include "Random.hpp"
#include "State.hpp"
#ifndef FLIPNEIGHBORHOOD_HPP
#define	FLIPNEIGHBORHOOD_HPP

class FlipNeighborhood : public Neighborhood {
public:
    std::shared_ptr<Storage> model;
    std::shared_ptr<State> state;
    FlipNeighborhood(std::shared_ptr<Storage> model, std::shared_ptr<State> st);
    FlipNeighborhood(const FlipNeighborhood& orig);
    ~FlipNeighborhood();

    //    template<typename returnType>
    //    void randomWalk(std::shared_ptr<State> st);
    //    template<typename returnType>
    //    bool bestImprovement(std::shared_ptr<State> st);
    //    template<typename returnType>
    //    bool firstImprovement(Move* mv, Model* st);

    //    void makeMove(Move* mv, std::shared_ptr<State> st);
//    bool hasNext();
    Move* next();
    bool commitMove(Move* mv);
    bool calculateDelta(Move* mv);
    Move* nextRandom();
//    bool hasNextRandom();
//    void setRandomCounter(unsigned numberOfRandomMoves);
    unsigned getSize();


private:
    unsigned testCounter =0;
    unsigned moveCounter = 0;
    unsigned randomCounter = 0;
    unsigned randomMovesWanted = 0;
    unsigned moveCounterStart = 0;
    bool flag = false;
    unsigned  size;
    //    template<typename returnType>
    //    std::pair<int, int> calculateDeltaChange(Move* mv, std::shared_ptr<State> st);

};

#endif	/* FLIPNEIGHBORHOOD_HPP */

