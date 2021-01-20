// Written by VSNelyubin, m80-206b-19, Original Code, do not steal!

#include <thread>

#include <mutex>

#include <condition_variable>

#include <string>

#include <fstream>

#include <iostream>

#include <cmath>

#include <vector>

double dabs(double a)
{
    if (a < 0) {
        return -a;
    }
    return a;
}

template <class T>
double dist(std::pair<T, T> a, std::pair<T, T> b)
{
    double rez = std::pow((double)a.first - (double)b.first, 2);
    rez += std::pow((double)a.second - (double)b.second, 2);
    return std::sqrt(rez);
}

template <class T>
class Figure {
public:
    std::pair<T, T>* cord;
    int verNum;
    Figure(int vern)
    {
        verNum = vern;
        cord = new std::pair<T, T>[vern];
    }
    ~Figure()
    {
        delete[] cord;
    }
};

template <class T>
bool checkEven(Figure<T>* ta)
{
    if (ta->verNum < 3) {
        return true;
    }
    double rez = dist(ta->cord[0], ta->cord[ta->verNum - 1]);
    for (int arei = 1; arei < ta->verNum; arei++) {
        if (rez != dist(ta->cord[arei], ta->cord[arei - 1])) {
            return false;
        }
    }
    rez = dist(ta->cord[0], ta->cord[ta->verNum - 2]);
    if (rez != dist(ta->cord[1], ta->cord[ta->verNum - 1])) {
        return false;
    }
    for (int arei = 2; arei < ta->verNum; arei++) {
        if (rez != dist(ta->cord[arei], ta->cord[arei - 2])) {
            return false;
        }
    }

    return true;
}

template <class T>
double figArea(Figure<T>* ta)
{
    if (ta->verNum < 3) {
        return 0;
    }
    double rez, res = 0;
    for (int arei = 2; arei < ta->verNum; arei++) {
        rez = ta->cord[0].first * (ta->cord[arei - 1].second - ta->cord[arei].second);
        rez += ta->cord[arei - 1].first * (ta->cord[arei].second - ta->cord[0].second);
        rez += ta->cord[arei].first * (ta->cord[0].second - ta->cord[arei - 1].second);
        res += dabs(rez / 2);
    }
    return res;
}

template <class T>
std::pair<double, double> finCenter(Figure<T>* t)
{
    std::pair<double, double> rez;
    rez.first = 0;
    rez.second = 0;
    for (int arei = 0; arei < t->verNum; arei++) {
        rez.first += (double)t->cord[arei].first;
        rez.second += (double)t->cord[arei].second;
    }
    rez.first = rez.first / (double)t->verNum;
    rez.second = rez.second / (double)t->verNum;
    return rez;
}

template <class T>
std::ostream& operator<<(std::ostream& out, Figure<T>& c)
{
    for (int arei = 0; arei < c.verNum; arei++) {
        out << c.cord[arei].first << " : " << c.cord[arei].second << "\n";
    }
    return out;
}

template <class T>
std::istream& operator>>(std::istream& out, Figure<T>& c)
{
    for (int arei = 0; arei < c.verNum; arei++) {
        out >> c.cord[arei].first >> c.cord[arei].second;
    }
    if (!checkEven(&c)) {
        std::cout << "figure sides are not even\n";
    }
    return out;
}

template <class T>
void prent(Figure<T>* c)
{
    std::cout << *c;
}

template <class T>
class pent : public Figure<T> { //XD public figure
public:
    pent()
        : Figure<T>(5)
    {
    }
};
template <class T>
class hex : public Figure<T> { //XD public figure
public:
    hex()
        : Figure<T>(6)
    {
    }
};
template <class T>
class oct : public Figure<T> { //XD public figure
public:
    oct()
        : Figure<T>(8)
    {
    }
};

template <class T>
void saev(Figure<T>* ta, std::ofstream& outf)
{
    if (ta->verNum == 5) {
        outf << 'p';
    }
    if (ta->verNum == 6) {
        outf << 'h';
    }
    if (ta->verNum == 8) {
        outf << 'o';
    }
    for (int i = 0; i < ta->verNum; i++) {
        outf << ' ' << ta->cord[i].first << ' ' << ta->cord[i].second;
    }
    outf << '\n';
}

template <typename T>
struct Processor {
    using value_type = T;
    virtual void process(std::shared_ptr<std::vector<Figure<T>*> > buffer) = 0;
};

template <typename T>
struct StreamProcessor : Processor<T> {
    using value_type = T;
    void process(std::shared_ptr<std::vector<Figure<T>*> > buffer) override;
};

template <typename T>
struct FileProcessor : Processor<T> {
    using value_type = T;
    void process(std::shared_ptr<std::vector<Figure<T>*> > buffer) override;

private:
    size_t cnt = 0;
};

template <typename T>
void StreamProcessor<T>::process(std::shared_ptr<std::vector<Figure<T>*> > buffer)
{
    for (int spi = 0; spi < buffer->size(); spi++) {
        std::cout << *(*buffer)[spi] << "\n";
    }
}

template <typename T>
void FileProcessor<T>::process(std::shared_ptr<std::vector<Figure<T>*> > buffer)
{
    std::cout << "print to file\n";
    std::ofstream fout;
    fout.open(std::to_string(cnt) + ".txt");
    ++cnt;
    if (!fout.is_open()) {
        std::cout << "can't open\n";
        return;
    }
    for (int spi = 0; spi < buffer->size(); spi++) {
        saev<int>((*buffer)[spi], fout);
    }
}

template <typename T>
struct Subscriber {
    using value_type = T;
    void operator()();
    std::vector<std::shared_ptr<Processor<T> > > processors;
    std::shared_ptr<std::vector<Figure<T>*> > buffer;
    std::mutex mtx;
    std::condition_variable cond_var;
    bool stop = false;
};

template <typename T>
void Subscriber<T>::operator()()
{
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cond_var.wait(lock, [&] { return (buffer != nullptr || stop); });
        if (stop) {
            break;
        }
        for (auto elem : processors) {
            elem->process(buffer);
        }
        buffer = nullptr;
        cond_var.notify_all();
    }
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "Wrong number of arguments. \n";
        return 0;
    }
    std::shared_ptr<std::vector<Figure<int>*> > arr = std::make_shared<std::vector<Figure<int>*> >();
    //std::vector<Figure<int>*> arr;
    pent<int>* a;
    hex<int>* b;
    oct<int>* c;
    int l, i = 0;
    char fgt;
    std::ofstream saveto;
    std::string fname;
    Subscriber<int> sub;
    sub.processors.push_back(std::make_shared<StreamProcessor<int> >());
    sub.processors.push_back(std::make_shared<FileProcessor<int> >());
    std::thread sub_thread(std::ref(sub));
    //	for(int i=0;i<std::atoi(argv[1]);i++){
    while (std::cin >> fgt) {
        std::unique_lock<std::mutex> locker(sub.mtx);
        if (fgt == 'p') {
            a = new pent<int>;
            std::cin >> *a;
            arr->push_back(a);
            i++;
        }
        else if (fgt == 'h') {
            b = new hex<int>;
            std::cin >> *b;
            arr->push_back(b);
            i++;
        }
        else if (fgt == 'o') {
            c = new oct<int>;
            std::cin >> *c;
            arr->push_back(c);
            i++;
        }
        else {
            //i--;
            std::cout << "please specify what you are doing: p - add pentagon, h - add hexagon, o - add octagon.\n";
        }
        if (i == std::atoi(argv[1])) {
            sub.buffer = arr;
            sub.cond_var.notify_all();
            sub.cond_var.wait(locker, [&]() { return sub.buffer == nullptr; });
            arr->clear();
	    i=0;
        }
    }
    sub.stop = true;
    sub.cond_var.notify_all();
    sub_thread.join();
    return 0;
}
