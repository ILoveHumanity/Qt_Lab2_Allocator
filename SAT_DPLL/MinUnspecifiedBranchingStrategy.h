#ifndef MINUNSPECIFIEDBRANCHINGSTRATEGY_H
#define MINUNSPECIFIEDBRANCHINGSTRATEGY_H

#include "BranchingStrategy.h"

///@brief Стратегия выбора столбца с минимальным количеством '-'.
class MinUnspecifiedBranchingStrategy : public BranchingStrategy
{
public:
    MinUnspecifiedBranchingStrategy() = default;
    ~MinUnspecifiedBranchingStrategy() = default;

    ///@brief Выбирает индекс столбца для ветвления с минимальным количеством '-'.
    ///@param equation Текущее состояние булева уравнения.
    ///@return Индекс столбца с нуля или -1, если свободных столбцов нет.
    int ChooseColForBranching(BoolEquation& equation);
};

#endif // MINUNSPECIFIEDBRANCHINGSTRATEGY_H
