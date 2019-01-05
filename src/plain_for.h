/*
 * Copyright (c) 2018 Jonas Deyson
 *
 * This software is released under the MIT License.
 *
 * You should have received a copy of the MIT License
 * along with this program. If not, see <https://opensource.org/licenses/MIT>
 */

#ifndef PLAIN_FOR_H
#define PLAIN_FOR_H


/**
 * Encapsulates a multiple `for` construct.
 *
 * This class permits to emulate a multiple `for` like:
 * \code{.cpp}
 * for(int x = bx; x < ex; ++x)
 * for(int y = by; y < ey; ++y)
 * for(int z = bz; z < ez; ++z)
 * {...}
 * \endcode
 * using a single `while` construct:
 * \code{.cpp}
 * int x, y, z;
 * PlainFor<3> for3(bx, ex, by, ey, bz, ez);
 * while(for3.next(x, y, z))
 * {...}
 * \endcode
 */
template<int N>
class PlainFor
{
public:
    PlainFor() :
        beginX(0), endX(0), x(0)
    {}

    template<typename... Args>
    PlainFor(int bx, int ex, const Args&... args):
        beginX(bx), endX(ex), x(bx)
    {
        if(bx < ex) internalFor = PlainFor<N-1>(args...);
    }

    template<typename... Args>
    bool next(int& xi, Args&... args)
    {
        xi = x;
        if(!internalFor.next(args...))
        {
            if((xi = ++x) >= endX)
            {
                x = beginX;
                return false;
            }
            return internalFor.next(args...);
        }

        return true;
    }

private:
    int beginX, endX;
    int x;
    PlainFor<N-1> internalFor;
};



template<>
class PlainFor<1>
{
public:
    PlainFor() :
        beginX(0), endX(0), x(0)
    {}

    PlainFor(int bx, int ex):
        beginX(bx), endX(ex),
        x(bx)
    {}

    bool next(int& xi)
    {
        xi = x;

        if(x++ >= endX)
        {
            x = beginX;
            return false;
        }

        return true;
    }

private:
    int beginX, endX;
    int x;
};


#endif
