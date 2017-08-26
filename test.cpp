#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <iostream>
#include <iomanip>
class OrderBook
{
public:
  void AddOrder(double price, unsigned int id)
  {
    orders.insert(std::make_pair(price, id));
  }
  void RemoveOrder(unsigned int id)
  {
    for(auto it = orders.begin(); it != orders.end(); ++it)
    {
      if (it->second == id)
      {
        orders.erase(it);
        break;
      }
    }
  }
  bool GetMaxPrice(double& max)
  {
    if (orders.empty())
    {
      return false;
    }
    max = orders.begin()->first;
    return true;
  }
private:
  class Compare
  {
  public:
    bool operator() (const double& first, const double& second) const
    {
      return first > second;
    }
  };
  //mulitmap для сортировки заказов по цене
  //переопределено сравнение чтобы масимальный элемент был первым
  std::multimap<double, unsigned int, Compare> orders;
};
class OrderAnalyzer
{
  struct Order
  {
    size_t time;
    unsigned int id;
    double price;
  };
public:
  OrderAnalyzer() : m_beg_time(0), m_per_max(0), m_all_time(0)
  {
  }
  void ProcessOrder(const std::string& str)
  {
    std::stringstream ss(str);
    Order order;
    ss >> order.time;
    char op;
    ss >> op;
    ss >> order.id;
    switch (op)
    {
    case 'I':
      ss >> order.price;
      InsertOrder(order);
      break;
    case 'E':
      ErraseOrder(order);
      break;
    }
  }
  //получить взвешенную по времени максимальную цену
  double GetAverageMax()
  {
    return m_per_max/m_all_time;
  }
private:
  //добавляем заказ в книгу
  void InsertOrder(Order& order)
  {
    double max_price;
    if (m_book.GetMaxPrice(max_price))
    {
      if (max_price < order.price)
      {
        //закрываем период когда появляется новая максимальная цена
        ClosePeriod( order.time, max_price);
      }
      
    }
    else
    {
      //начинаем новый период
      m_beg_time = order.time;
    }
    m_book.AddOrder(order.price, order.id);
  }
  
  //удаляем заказ из книги
  void ErraseOrder(Order& order)
  {
    double max_price_old;
    double max_price_new;
    if (m_book.GetMaxPrice(max_price_old))
    {
      m_book.RemoveOrder(order.id);
      if (m_book.GetMaxPrice(max_price_new))
      {
        if (max_price_new != max_price_old)
        {
          //закрываем период если маскимальная цена изменилась после удаления заказа
          ClosePeriod(order.time, max_price_old);
        }
      }
      else
      {
        //закрываем период если в книге не осталось заказов
        //чтобы не учитывать время простоя
        ClosePeriod(order.time, max_price_old);
      }
    }
  }
  //закрыть период
  //обновить начало периода, общее время без учета простоя, обновить сумму максимумов
  void ClosePeriod(size_t end, double max)
  {
    UpdateAverageMax(m_beg_time, end, max);
    m_all_time += end - m_beg_time;
    m_beg_time = end;
  }
  void UpdateAverageMax(size_t beg, size_t end, double max)
  {
    m_per_max += (end - beg) * max;
  }
  OrderBook m_book;//список всех заказов
  size_t m_beg_time;//начальное время периода
  size_t m_all_time;//Общее время без учета простоев
  double m_per_max;//сумма максимумов умнуженных на длину периода
};
int main()
{
  std::cout << "input file name : ";
  std::string file_name;
  std::cin >> file_name;
  std::cout << file_name << "\n";
  std::ifstream in(file_name);
  if (!in.good())
  {
    std::cerr << "file is not good";
    return 0;
  }
  try
  {
    OrderAnalyzer analyzer;
    std::string str;
    while(in.good())
    {
      std::getline(in, str);
      analyzer.ProcessOrder(str);
    }
    double res = analyzer.GetAverageMax();
    std::cout << "average max : " << std::fixed << std::setprecision (2) << res << std::endl;
  }
  catch(std::ios_base::failure& exc)
  {
    std::cerr << "input file name : " << exc.what();
    return 0;
  }
  return 0;
}
