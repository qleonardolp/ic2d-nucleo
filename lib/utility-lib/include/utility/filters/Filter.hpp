#ifndef FILTER_HPP
#define FILTER_HPP

namespace utility {

class Filter
{
public:

    Filter() {}

    virtual ~Filter() {}

    virtual double process(double input, double h) = 0;

    virtual void clean() = 0;

};
}

#endif // FILTER_HPP