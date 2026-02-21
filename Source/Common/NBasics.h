#ifndef AXIO_NBASICS_H
#define AXIO_NBASICS_H

#include <algorithm>
#include <vector>

namespace NBasics
{

/** Remove an element from a container */
template <typename T, typename ContainerT>
bool RemoveOne(ContainerT& Container, const T& kElement)
{
    const auto iter = std::ranges::find(Container, kElement);
    if (iter == std::end(Container))
        return false;

    Container.erase(iter);
    return true;
}

/** Adds an element to a vector only if it is not already present. Returns the index of the element. */
template <typename T>
size_t VectorAddUnique(std::vector<T>& Vector, const T& kElement)
{
    const auto iter = std::ranges::find(Vector, kElement);
    if (iter != std::end(Vector))
        return size_t(std::distance(std::begin(Vector), iter));

    Vector.push_back(kElement);
    return Vector.size() - 1;
}

} // namespace NBasics

#endif // AXIO_NBASICS_H
