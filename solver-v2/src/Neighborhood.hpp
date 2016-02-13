#ifndef NEIGHBORHOOD_HPP
#define	NEIGHBORHOOD_HPP
#include "Move.hpp" 
#include "State.hpp"

class Neighborhood {
public:
    Neighborhood();
    virtual ~Neighborhood() = 0;
    virtual Move* next() = 0;
    virtual bool calculateDelta(Move* mv) = 0;
    virtual bool commitMove(Move* mv) = 0;
    virtual int compareMoves(Move* mv1, Move* mv2);
    virtual unsigned getSize() = 0;
    int numberOfEqualMoves = 0;

    virtual Move* nextRandom() = 0;


private:
protected:

};

#endif	/* NEIGHBORHOOD_HPP */

