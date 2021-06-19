//#include <cmc_complex.h>
//#include <cmc_complex_stack.h>
//#include <cmc_complex_eval.h>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <functional>

namespace numbers
{
    class complex
    {
    private:
        double real_;
        double imag_;
    public:
        complex(double real = 0, double imag = 0) {
            real_ = real;
            imag_ = imag;
        }
        explicit complex(std::string s) {
            sscanf(s.c_str(), "(%lf,%lf)", &real_, &imag_);
        }
        std::string to_string() const {
            std::ostringstream ostr;
            std::setprecision(10);
            ostr << "(" << real_ << "," << imag_ << ")";
            return ostr.str();
        }
        double re() const {
            return real_;
        }
        double im() const {
            return imag_;
        }
        double abs2() const {
            return real_ * real_ + imag_ * imag_;
        }
        double abs() const {
            return sqrt(real_ * real_ + imag_ * imag_);
        }
        complex& operator +=(const complex& a) {
            real_ += a.real_;
            imag_ += a.imag_;
            return *this;
        }
        complex& operator -=(const complex& a) {
            real_ -= a.real_;
            imag_ -= a.imag_;
            return *this;
        }
        complex& operator *=(const complex& a) {
            double new_real = real_ * a.real_ - imag_ * a.imag_;
            double new_imag = real_ * a.imag_ + imag_ * a.real_;
            real_ = new_real;
            imag_ = new_imag;
            return *this;
        }
        complex& operator /=(const complex& a) {
            double new_real = (real_ * a.real_ + imag_ * a.imag_) / (a.real_ * a.real_ + a.imag_ * a.imag_);
            double new_imag = (imag_ * a.real_ - real_ * a.imag_) / (a.real_ * a.real_ + a.imag_ * a.imag_);
            real_ = new_real;
            imag_ = new_imag;
            return *this;
        }
        complex operator~() const {
            return complex(real_, -imag_);
        }
        complex operator-() const {
            return complex(-real_, -imag_);
        }
        
    };
    complex operator+(const complex&a, const complex&b) {
        complex tmp(a.re(), a.im());
        tmp += b;
        return tmp;
    }
    complex operator-(const complex&a, const complex&b) {
        complex tmp(a.re(), a.im());
        tmp -= b;
        return tmp;
    }
    complex operator*(const complex&a, const complex&b) {
        complex tmp(a.re(), a.im());
        tmp *= b;
        return tmp;
    }
    complex operator/(const complex&a, const complex&b) {
        complex tmp(a.re(), a.im());
        tmp /= b;
        return tmp;
    }

    class complex_stack
    {
    private:
        complex *stack;
        size_t len;
    public:
        complex_stack(size_t mem = 0) {
            stack = new complex[mem + 1];
            len = mem;
        }
        complex_stack(const complex_stack& a) {
            stack = new complex[a.size() + 1];
            len = a.size();
            for (size_t i = 0; i < len; i++) {
                stack[i] = a[i];
            }
        }
        ~complex_stack() {
            delete [] stack;
            len = 0;
        }
        complex operator[](size_t i) const {
            return stack[i];
        }
        complex* get_stack() const {
            return stack;
        }
        complex_stack& operator=(const complex_stack& a) {
            if (this == &a) return *this;
            if (len < a.size()) {
                delete [] stack;
                stack = new complex[a.size() + 1];
            }
            for (size_t i = 0; i < a.size(); i++) {
                stack[i] = a[i];
            }
            len = a.size();
            return *this;
        }
        complex operator+() const {
            return stack[len - 1];
        }
        complex_stack operator~() const {
            complex_stack res{len - 1};
            for (size_t i = 0; i < len - 1; i++) {
                res.get_stack()[i] = stack[i];
            }
            return res;
        }
        complex_stack operator<<(const complex& b) const {
            complex_stack res{len + 1};
            for (size_t i = 0; i < len; i++) {
                res.get_stack()[i] = stack[i];
            }
            res.get_stack()[len] = b;
            return res;
        }
        size_t size() const {
            return len;
        }
    };
    complex eval(const std::vector<std::string> &args, const complex &z) {
        complex_stack stack;
        std::map <char, std::function<void()>> oper {
            {'+', [&stack]() {
                complex z1 = +stack;
                stack = ~stack;
                complex z2 = +stack;
                stack = ~stack;
                stack = stack << (z1 + z2);
            }},
            {'-', [&stack]() {
                complex z2 = +stack;
                stack = ~stack;
                complex z1 = +stack;
                stack = ~stack;
                stack = stack << (z1 - z2);
            }},
            {'*', [&stack]() {
                complex z2 = +stack;
                stack = ~stack;
                complex z1 = +stack;
                stack = ~stack;
                stack = stack << (z1 * z2);
            }},
            {'/', [&stack]() {
                complex z2 = +stack;
                stack = ~stack;
                complex z1 = +stack;
                stack = ~stack;
                stack = stack << (z1 / z2);
            }},
            {'~', [&stack]() {
                complex z1 = +stack;
                stack = ~stack;
                stack = stack << ~z1;
            }},
            {'#', [&stack]() {
                complex z1 = +stack;
                stack = ~stack;
                stack = stack << -z1;
            }},
            {'!', [&stack]() {
                complex z1 = +stack;
                stack = stack << z1;
            }},
            {';', [&stack]() {
                stack = ~stack;
            }}
        };
        
        for (int i = 0; i < args.size(); i++) {
            if (args[i][0] == 'z') {
                stack = stack << z;
            }
            else if (args[i][0] == '(') {
                complex z1(args[i]);
                stack = stack << z1;
            }
            else {
                oper[args[i][0]]();
            }
        }
        return +stack;
    }
    
}

int main(int argc, char *argv[])
{
    int n;
    double r, phi = 0;
    std::vector<std::string> args;
    numbers::complex c(argv[1]);
    sscanf(argv[2], "%lf", &r);
    sscanf(argv[3], "%d", &n);
    for (int i = 4; i < argc; i++) {
        args.push_back(argv[i]);
    }
    double step = 2*M_PI/n;
    numbers::complex res{0,0};
    for (int i = 0; i < n; i++) {
        phi = i * 2*M_PI / n;
        double x1 = c.re() + r * cos(phi);
        double y1 = c.im() + r * sin(phi);
        double x2 = c.re() + r * cos(phi + step);
        double y2 = c.im() + r * sin(phi + step);
        double x3 = c.re() + r * cos(phi + step/2);
        double y3 = c.im() + r * sin(phi + step/2);
        numbers::complex z1(x1,y1), z2(x2,y2), z3(x3,y3);
        res += numbers::eval(args, z3) * (z2 - z1);
    }
    std::cout << res.to_string() << std::endl;
}
