#ifndef NBASICS_H
#define NBASICS_H

#include "BasicTypes.h"
#include <vector>

namespace NBasics
{

/** Remove an element from a container */
template<typename T, typename ContainerT>
bool ContainerRemoveOne(ContainerT& Container, const T& kElement)
{
    for (auto Iter = Container.begin(); Iter != Container.end(); Iter++)
    {
        if (*Iter == kElement)
        {
            Container.erase(Iter);
            return true;
        }
    }
    return false;
}

template<typename T>
inline bool VectorRemoveOne(std::vector<T>& Vector, const T& kElement)
{
    return ContainerRemoveOne< T, std::vector<T> >(Vector, kElement);
}

template<typename T>
inline bool ListRemoveOne(std::list<T>& List, const T& kElement)
{
    return ContainerRemoveOne< T, std::list<T> >(List, kElement);
}

/** Remove all occurrences of an element from a container. Returns the number of elements that were removed. */
template<typename T, typename ContainerT>
int ContainerRemoveAll(ContainerT& Container, const T& kElement)
{
    int NumRemoved = 0;

    for (auto Iter = Container.begin(); Iter != Container.end(); Iter++)
    {
        if (*Iter == kElement)
        {
            Iter = Container.erase(Iter);
            NumRemoved++;
        }
    }

    return NumRemoved;
}

template<typename T>
inline int VectorRemoveAll(std::vector<T>& Vector, const T& kElement)
{
    return ContainerRemoveAll< T, std::vector<T> >(Vector, kElement);
}

template<typename T>
inline int ListRemoveAll(std::list<T>& List, const T& kElement)
{
    return ContainerRemoveAll< T, std::list<T> >(List, kElement);
}

/** Returns the index of an element in a vector. Returns -1 if the element is not present. */
template<typename T>
int VectorFind(std::vector<T>& Vector, const T& kElement)
{
    for (int i=0; i<Vector.size(); i++)
    {
        if (Vector[i] == kElement)
        {
            return i;
        }
    }

    return -1;
}

/** Adds an element to a vector only if it is not already present. Returns the index of the element. */
template<typename T>
int VectorAddUnique(std::vector<T>& Vector, const T& kElement)
{
    for (int i=0; i<Vector.size(); i++)
    {
        if (Vector[i] == kElement)
        {
            return i;
        }
    }

    Vector.push_back(kElement);
    return Vector.size() - 1;
}

}

#endif // NBASICS_H
