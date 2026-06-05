/*
#include "Allocator.h"
#include <assert.h>
#include <new>
#include <iostream>

// @see https://github.com/endurodave/Allocator

// On VisualStudio, to disable the debug heap for faster performance when using
// the debugger use this option:
// Debugging > Environment _NO_DEBUG_HEAP=1

class MyClass
{
    DECLARE_ALLOCATOR
    // remaining class definition
};
IMPLEMENT_ALLOCATOR(MyClass, 0, 0)

// Heap blocks mode unlimited with 100 byte blocks
Allocator allocatorHeapBlocks(100);

// Heap pool mode with 20, 100 byte blocks
Allocator allocatorHeapPool(100, 20);

// Static pool mode with 20, 100 byte blocks
char staticMemoryPool[100 * 20];
Allocator allocatorStaticPool(100, 20, staticMemoryPool);

// Static pool mode with 20 MyClass sized blocks using template
AllocatorPool<MyClass, 20> allocatorStaticPool2;

// Benchmark allocators
static const int MAX_BLOCKS = 10000;
static const int MAX_BLOCK_SIZE = 4096;
void* memoryPtrs[MAX_BLOCKS];
void* memoryPtrs2[MAX_BLOCKS];
AllocatorPool<char[MAX_BLOCK_SIZE], MAX_BLOCKS*2> allocatorStaticPoolBenchmark;
Allocator allocatorHeapBlocksBenchmark(MAX_BLOCK_SIZE);

static void out_of_memory()
{
    // new-handler function called by Allocator when pool is out of memory
    assert(0);
}

typedef void* (*AllocFunc)(int size);
typedef void (*DeallocFunc)(void* ptr);
void Benchmark(const char* name, AllocFunc allocFunc, DeallocFunc deallocFunc);
void* AllocHeap(int size);
void DeallocHeap(void* ptr);
void* AllocStaticPool(int size);
void DeallocStaticPool(void* ptr);
void* AllocHeapBlocks(int size);
void DeallocHeapBlocks(void* ptr);

//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
int main(void)
{
    std::set_new_handler(out_of_memory);

    // Allocate MyClass using fixed block allocator
    MyClass* myClass = new MyClass();
    delete myClass;

    // Allocate 100 bytes in fixed block allocator, then deallocate
    void* memory1 = allocatorHeapBlocks.Allocate(100);
    allocatorHeapBlocks.Deallocate(memory1);

    void* memory2 = allocatorHeapBlocks.Allocate(100);
    allocatorHeapBlocks.Deallocate(memory2);

    void* memory3 = allocatorHeapPool.Allocate(100);
    allocatorHeapPool.Deallocate(memory3);

    void* memory4 = allocatorStaticPool.Allocate(100);
    allocatorStaticPool.Deallocate(memory4);

    void* memory5 = allocatorStaticPool2.Allocate(sizeof(MyClass));
    allocatorStaticPool2.Deallocate(memory5);

    Benchmark("Heap (Run 1)", AllocHeap, DeallocHeap);
    Benchmark("Heap (Run 2)", AllocHeap, DeallocHeap);
    Benchmark("Heap (Run 3)", AllocHeap, DeallocHeap);
    Benchmark("Static Pool (Run 1)", AllocStaticPool, DeallocStaticPool);
    Benchmark("Static Pool (Run 2)", AllocStaticPool, DeallocStaticPool);
    Benchmark("Static Pool (Run 3)", AllocStaticPool, DeallocStaticPool);
    Benchmark("Heap Blocks (Run 1)", AllocHeapBlocks, DeallocHeapBlocks);
    Benchmark("Heap Blocks (Run 2)", AllocHeapBlocks, DeallocHeapBlocks);
    Benchmark("Heap Blocks (Run 3)", AllocHeapBlocks, DeallocHeapBlocks);
    return 0;
}

//------------------------------------------------------------------------------
// AllocHeap
//------------------------------------------------------------------------------
void* AllocHeap(int size)
{
    return new CHAR[size];
}

//------------------------------------------------------------------------------
// DeallocHeap
//------------------------------------------------------------------------------
void DeallocHeap(void* ptr)
{
    delete [] ptr;
}

//------------------------------------------------------------------------------
// AllocStaticPool
//------------------------------------------------------------------------------
void* AllocStaticPool(int size)
{
    return allocatorStaticPoolBenchmark.Allocate(size);
}

//------------------------------------------------------------------------------
// DeallocStaticPool
//------------------------------------------------------------------------------
void DeallocStaticPool(void* ptr)
{
    allocatorStaticPoolBenchmark.Deallocate(ptr);
}

//------------------------------------------------------------------------------
// AllocHeapBlocks
//------------------------------------------------------------------------------
void* AllocHeapBlocks(int size)
{
    return allocatorHeapBlocksBenchmark.Allocate(size);
}

//------------------------------------------------------------------------------
// DeallocHeapBlocks
//------------------------------------------------------------------------------
void DeallocHeapBlocks(void* ptr)
{
    allocatorHeapBlocksBenchmark.Deallocate(ptr);
}

//------------------------------------------------------------------------------
// Benchmark
//------------------------------------------------------------------------------
void Benchmark(const char* name, AllocFunc allocFunc, DeallocFunc deallocFunc)
{
#if WIN32
    LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds, TotalElapsedMicroseconds= {0};
    LARGE_INTEGER Frequency;

    SetProcessPriorityBoost(GetCurrentProcess(), true);

    QueryPerformanceFrequency(&Frequency);

    // Allocate MAX_BLOCKS blocks MAX_BLOCK_SIZE / 2 sized blocks
    QueryPerformanceCounter(&StartingTime);
    for (int i=0; i<MAX_BLOCKS; i++)
        memoryPtrs[i] = allocFunc(MAX_BLOCK_SIZE / 2);
    QueryPerformanceCounter(&EndingTime);
    ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
    ElapsedMicroseconds.QuadPart *= 1000000;
    ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
    std::cout << name << " allocate time: " << ElapsedMicroseconds.QuadPart << std::endl;
    TotalElapsedMicroseconds.QuadPart += ElapsedMicroseconds.QuadPart;

    // Deallocate MAX_BLOCKS blocks (every other one)
    QueryPerformanceCounter(&StartingTime);
    for (int i=0; i<MAX_BLOCKS; i+=2)
        deallocFunc(memoryPtrs[i]);
    QueryPerformanceCounter(&EndingTime);
    ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
    ElapsedMicroseconds.QuadPart *= 1000000;
    ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
    std::cout << name << " deallocate time: " << ElapsedMicroseconds.QuadPart << std::endl;
    TotalElapsedMicroseconds.QuadPart += ElapsedMicroseconds.QuadPart;

    // Allocate MAX_BLOCKS blocks MAX_BLOCK_SIZE sized blocks
    QueryPerformanceCounter(&StartingTime);
    for (int i=0; i<MAX_BLOCKS; i++)
        memoryPtrs2[i] = allocFunc(MAX_BLOCK_SIZE);
    QueryPerformanceCounter(&EndingTime);
    ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
    ElapsedMicroseconds.QuadPart *= 1000000;
    ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
    std::cout << name << " allocate time: " << ElapsedMicroseconds.QuadPart << std::endl;
    TotalElapsedMicroseconds.QuadPart += ElapsedMicroseconds.QuadPart;

    // Deallocate MAX_BLOCKS blocks (every other one)
    QueryPerformanceCounter(&StartingTime);
    for (int i=1; i<MAX_BLOCKS; i+=2)
        deallocFunc(memoryPtrs[i]);
    QueryPerformanceCounter(&EndingTime);
    ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
    ElapsedMicroseconds.QuadPart *= 1000000;
    ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
    std::cout << name << " deallocate time: " << ElapsedMicroseconds.QuadPart << std::endl;
    TotalElapsedMicroseconds.QuadPart += ElapsedMicroseconds.QuadPart;

    // Deallocate MAX_BLOCKS blocks
    QueryPerformanceCounter(&StartingTime);
    for (int i=MAX_BLOCKS-1; i>=0; i--)
        deallocFunc(memoryPtrs2[i]);
    QueryPerformanceCounter(&EndingTime);
    ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
    ElapsedMicroseconds.QuadPart *= 1000000;
    ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
    std::cout << name << " deallocate time: " << ElapsedMicroseconds.QuadPart << std::endl;
    TotalElapsedMicroseconds.QuadPart += ElapsedMicroseconds.QuadPart;

    std::cout << name << " TOTAL TIME: " << TotalElapsedMicroseconds.QuadPart << std::endl;

    SetProcessPriorityBoost(GetCurrentProcess(), false);
#endif
}
*/



#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QStack>
#include <string>
#include <cstring>
#include <stack>
#include <ostream>
#include "NodeBoolTree.h"
#include "boolinterval.h"
#include "boolequation.h"
#include "BBV.h"
//#include "Allocator.h"

#include "memory"
#include "MinUnspecifiedBranchingStrategy.h"
#include "FirstBranchingStrategy.h"

int main(int argc, char *argv[])
{
	QStringList full_file_list;
	QList<QStringList> Elements;
	std::string filepath;
	QStringList inputs;
	//std::cout << "Input file path...\n";
	//std::cin >> filepath;
	// Hardcode input
	//	filepath = "sat_ex_2.pla";
	//filepath = "Sat_ex11_3.pla";
    filepath = "../Qt_Lab2_Allocator/SAT_DPLL/SatExamples/Sat_ex30_3.pla";
	QFile file(QString::fromUtf8(filepath.c_str()));

	//считываем весь файл
	if ((file.exists()) && (file.open(QIODevice::ReadOnly))) {
		while (!file.atEnd()) {
			full_file_list << file.readLine().replace("\r\n", "");
		}

		int cnfSize = full_file_list.length();
		BoolInterval **CNF = new BoolInterval*[cnfSize];
		int rangInterval = -1; // error

		if (cnfSize) {
			rangInterval = full_file_list[0].toUtf8().trimmed().length();
		}

		for (int i = 0; i < cnfSize; i++) { // Заполняем массив
			QString strv = full_file_list[i];
			CNF[i] = new BoolInterval(strv.toUtf8().trimmed().data());
		}

		QString rootvec = "";
		QString rootdnc = "";

		//Строим интервал в которм все компоненты принимают значение '-',
		//который представляет собой корень уравнения, пока пустой.
		//В процессе поиска корня, компоненты интервала буду заменены на конкретные значения.

		for (int i = 0; i < rangInterval; i++) {
			rootvec += "0";
			rootdnc += "1";
		}

		QByteArray v = rootvec.toUtf8();

		BBV vec(v.data());
		QByteArray d = rootdnc.toUtf8();
		BBV dnc(d.data());

		// Создаем пустой корень уравнения;
		BoolInterval *root = new BoolInterval(vec, dnc);

		BoolEquation *boolequation = new BoolEquation(CNF, root, cnfSize, cnfSize, vec);


        //boolequation->SetBranchingStrategy(std::make_shared<MinUnspecifiedBranchingStrategy>());
        boolequation->SetBranchingStrategy(std::make_shared<FirstBranchingStrategy>());

		// Алгоритм поиска корня. Работаем всегда с верхушкой стека.
		// Шаг 1. Правила выполняются? Нет - Ветвление Шаг 5. Да - Упрощаем Шаг 2.
		// Шаг 2. Строки закончились? Нет - Шаг1, Да - Корень найден? Да - Успех КОНЕЦ, Нет - Шаг 3.
		// Шаг 3. Кол-во узлов в стеке > 1? Нет - Корня нет КОНЕЦ, Да - Шаг 4.
		// Шаг 4. Текущий узел выталкиваем из стека, попадаем в новый узел. У нового узла lt rt отличны от NULL? Нет - Шаг 1. Да - Шаг 3.
		// Шаг 5. Выбор компоненты ветвления, создание двух новых узлов, добавление их в стек сначала с 1 потом с 0. Шаг 1.

		// Алгоритм CheckRules.
		// Цикл по строкам КНФ.
		// 1. Проверка правила 2. Выполнилось? Да - Корня нет, Нет - Идем дальше.
		// 2. Проверка правила 1. Выполнилось? Да - Упрощаем, Нет - Идем дальше.

		// Создаем стек под узлы булева дерева
		// QStack<NodeBoolTree> BoolTree;

		bool rootIsFinded = false;
		stack<NodeBoolTree *> BoolTree;
		NodeBoolTree *startNode = new NodeBoolTree(boolequation);
		BoolTree.push(startNode);

		do {
			NodeBoolTree *currentNode(BoolTree.top());

			if (currentNode->lt == nullptr &&
					currentNode->rt == nullptr) { // Если вернулись в обработанный узел
				BoolEquation *currentEquation = currentNode->eq;
				bool flag = true;

				// Цикл для упрощения по правилам.
				while (flag) {
					int a = currentEquation->CheckRules(); // Проверка выполнения правил

					switch (a) {
						case 0: { // Корня нет.
							BoolTree.pop();
							flag = false;
							break;
						}

						case 1: { // Правило выполнилось, корень найден или продолжаем упрощать.
							if (currentEquation->count == 0 ||
									currentEquation->mask.getWeight() ==
									currentEquation->mask.getSize()) { // Если кончились строки или столбцы, корень найден.
								flag = false;
								rootIsFinded =
									true; // Полагаем, что корень найден, выполняем проверку корня

								for (int i = 0; i < cnfSize; i++) {

									if (!CNF[i]->isEqualComponent(*currentEquation->root)) {
										rootIsFinded = false;//Корень не найден. Продолжаем искать дальше.
										BoolTree.pop();
										break;
									}
								}
							}

							break;
						}

						case 2: { // Правила не выполнились, ветвление.
							// Ветвление, создание новых узлов.

							int indexBranching = currentEquation->ChooseColForBranching();

							BoolEquation *Equation0 = new BoolEquation(*currentEquation);
							BoolEquation *Equation1 = new BoolEquation(*currentEquation);

							Equation0->Simplify(indexBranching, '0');
							Equation1->Simplify(indexBranching, '1');

							NodeBoolTree *Node0 = new NodeBoolTree(Equation0);
							NodeBoolTree *Node1 = new NodeBoolTree(Equation1);

							currentNode->lt = Node0;
							currentNode->rt = Node1;

							BoolTree.push(Node1);
							BoolTree.push(Node0);

							flag = false;
							break;
						}
					}
				}
			} else {
				BoolTree.pop();
			}

		} while (BoolTree.size() > 1 && !rootIsFinded);

		if (rootIsFinded) {
			cout << "Root is:\n ";
			BoolInterval *finded_root = BoolTree.top()->eq->root;
			cout << string(*finded_root);
		} else {
			cout << "Root is not exists!";
		}

	} else {
		std::cout << "File does not exists.\n";
	}

	return 0;

}

