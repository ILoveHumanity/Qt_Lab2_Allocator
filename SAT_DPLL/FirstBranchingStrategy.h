#ifndef FIRSTBRANCHINGSTRATEGY_H
#define FIRSTBRANCHINGSTRATEGY_H

#include "BranchingStrategy.h"

///@brief Стратегия выбора столбца с минимальным количеством '-'.
class FirstBranchingStrategy : public BranchingStrategy
{
public:
    FirstBranchingStrategy() = default;
    ~FirstBranchingStrategy() = default;

    ///@brief Выбирает индекс первого доступного столбца для ветвления.
    ///@param equation Текущее состояние булева уравнения.
    ///@return Индекс столбца с нуля или -1, если свободных столбцов нет.
    int ChooseColForBranching(BoolEquation& equation);
};

#endif // FIRSTBRANCHINGSTRATEGY_H
