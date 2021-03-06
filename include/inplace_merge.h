#ifndef TIMSORT_INPLACE_MERGE_H
#define TIMSORT_INPLACE_MERGE_H

template <class RandomAccessIterator, class Compare>
RandomAccessIterator findGallopEnd(const RandomAccessIterator &first, const RandomAccessIterator &last, const RandomAccessIterator &val, Compare comp) {
    return std::upper_bound(first, last, *val, comp);
}

template <class RandomAccessIterator, class Compare>
void mergeNeighbours(const RandomAccessIterator &first, const RandomAccessIterator &second, const RandomAccessIterator &buffer, Compare comp, const size_t &minimumGallopSize) {
    size_t N = second - first;

    std::swap_ranges(first, second, buffer);

    RandomAccessIterator it1, it2, it, it1_end, it2_end;
    it1 = second;
    it1_end = it1 + N;
    it2 = buffer;
    it2_end = it2 + N;
    it = first;

    int lastElements = 0;
    RandomAccessIterator gallopStart, gallopFinish, gallopEnd;
    while (it1 != it1_end && it2 != it2_end) {
        if (sizeAbs(lastElements) == minimumGallopSize) {
            gallopStart = (lastElements > 0 ? it1 : it2);
            gallopEnd = (lastElements > 0 ? second : buffer) + N;

            gallopFinish = findGallopEnd(gallopStart, gallopEnd, lastElements > 0 ? it2 : it1, comp);
            std::swap_ranges(gallopStart, gallopFinish, it);

            (lastElements > 0 ? it1 : it2) += gallopFinish - gallopStart;
            it += gallopFinish - gallopStart;

            lastElements = 0;
            continue;
        }
        if (comp(*it1, *it2)) {
            std::iter_swap(it1++, it++);
            lastElements = (lastElements >= 0 ? lastElements + 1 : 0);
        } else {
            std::iter_swap(it2++, it++);
            lastElements = (lastElements <= 0 ? lastElements - 1 : 0);
        }
    }

    if (it1 != it1_end)
        std::swap_ranges(it1, it1_end, it);
    it += it1_end- it1;
    if (it2 != it2_end)
        std::swap_ranges(it2, it2_end, it);
}

template <class RandomAccessIterator, class Compare>
void inPlaceMerge(const RandomAccessIterator &first, const RandomAccessIterator &middle, const RandomAccessIterator &last, Compare comp, const size_t &minimumGallopSize) {

    size_t N = (size_t) (last - first);
    size_t len = sqrt(N);
    size_t cnt = N / len;

    RandomAccessIterator middleElementBlock =  first + len * (roundUp((size_t)(middle - first), len) - 1);
    RandomAccessIterator buffer = first + len * (cnt - 1);
    std::swap_ranges(middleElementBlock, middleElementBlock + len, buffer);

    RandomAccessIterator it1, it2, itMin;
    for (it1 = first; it1 != buffer; it1 += len) {
        itMin = it1;
        for (it2 = it1 + len; it2 <= buffer - len; it2 += len) {
            if (comp(*it2, *itMin) || (!comp(*it2, *itMin) && !comp(*itMin, *it2) && (comp(*(it2 + len - 1), *(itMin + len - 1)))))
                itMin = it2;
        }
        std::swap_ranges(itMin, itMin + len, it1);
    }

    for (it1 = first; it1 != buffer - len; it1 += len) {
        it2 = it1 + len;
        mergeNeighbours(it1, it2, buffer, comp, minimumGallopSize);
    }

    size_t S = N - len * (cnt - 1);
    insertionSort(last - 2 * S, last, last - 2 * S, comp);

    buffer = last - S;
    for (it1 = last -  2 * S; it1 >= first + S ; it1 -= S) {
        it2 = it1 - S;
        mergeNeighbours(it2, it1, buffer, comp, minimumGallopSize);
    }

    insertionSort(first, first + 2 * S, first, comp);
    insertionSort(last - S, last, last - S, comp);
}

#endif //TIMSORT_INPLACE_MERGE_H
