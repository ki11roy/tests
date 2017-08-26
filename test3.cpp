#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <set>
#include <unordered_map>

using namespace std;

namespace {

class TOrderbook {
public:
    void Insert(uint32_t Id, double Price)
    {
        Prices.insert(Price);
        PricesIds.insert(make_pair(Id, Price));
    }

    void Erase(uint32_t Id)
    {
        Prices.erase(Prices.find(PricesIds[Id]));
        PricesIds.erase(Id);
    }

    double GetHighest()
    {
        if (Prices.empty()) {
            return numeric_limits<double>::quiet_NaN();
        }
        return *Prices.crbegin();
    }

private:
    multiset<double> Prices;
    unordered_map<uint32_t, double> PricesIds;
};

struct TOperation {
    uint64_t Timestamp;
    uint32_t Id;
    double Price;
};

typedef char TOpType;

TOpType ParseOpLine(ifstream& input, TOperation& output) {
    TOpType result = '\0';

    if (!input.good()) {
        return result;
    }

    input >> output.Timestamp >> ws >> result >> ws >> output.Id >> ws;
    if (result == 'I') {
        input >> output.Price >> ws;
    }

    return result;
}

}

int main(int argc, char** argv) {
    ifstream input(argv[1]);
    TOrderbook book;

    uint64_t timestamp(0);
    uint64_t tradeTime(0);
    double weightSum(0);

    TOperation op;
    while (TOpType opType = ParseOpLine(input, op)) {
        double currentMax = book.GetHighest();

        if (opType == 'I') {
            book.Insert(op.Id, op.Price);
        } else {
            book.Erase(op.Id);
        }

        if (!std::isnan(currentMax)) {
            uint64_t elapsed = op.Timestamp - timestamp;
            tradeTime += elapsed;
            weightSum += elapsed * currentMax;
        }

        timestamp = op.Timestamp;
    }

    input.close();

    cout << weightSum / tradeTime << endl;
    return 0;
}

