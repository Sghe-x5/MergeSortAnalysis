#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <chrono>
#include <iostream>
#include <fstream>
#include <functional>

using namespace std;

// Генератор массивов
class ArrayGenerator {
private:
    mt19937 rng;

public:
    ArrayGenerator() : rng(static_cast<unsigned>(time(nullptr))) {}

    // Случайный массив
    vector<int> generateRandomArray(int size, int minValue = 0, int maxValue = 6000) {
        uniform_int_distribution<int> dist(minValue, maxValue);
        vector<int> array(size);
        for (int &num : array) {
            num = dist(rng);
        }
        return array;
    }

    // Обратно отсортированный массив
    vector<int> generateReversedArray(int size) {
        vector<int> array(size);
        for (int i = 0; i < size; ++i) {
            array[i] = size - i;
        }
        return array;
    }

    // Почти отсортированный массив
    vector<int> generateAlmostSortedArray(int size, int swaps = 10) {
        vector<int> array(size);
        for (int i = 0; i < size; ++i) {
            array[i] = i;
        }
        shuffle(array.begin(), array.end(), rng);
        for (int i = 0; i < swaps; ++i) {
            int idx1 = rng() % size;
            int idx2 = rng() % size;
            swap(array[idx1], array[idx2]);
        }
        return array;
    }
};

// Тестирование сортировок
class SortTester {
public:
    long long measureSortTime(vector<int> array, function<void(vector<int>&)> sortFunction, int iterations = 5) {
        long long totalTime = 0;
        for (int i = 0; i < iterations; ++i) {
            vector<int> temp = array; // Копия массива для каждого замера
            auto start = chrono::high_resolution_clock::now();
            sortFunction(temp);
            auto elapsed = chrono::high_resolution_clock::now() - start;
            totalTime += chrono::duration_cast<chrono::microseconds>(elapsed).count(); // Микросекунды
        }
        return totalTime / iterations; // Среднее время
    }
};

// Функция слияния
void merge(vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<int> L(n1), R(n2);
    for (int i = 0; i < n1; ++i) L[i] = arr[left + i];
    for (int i = 0; i < n2; ++i) R[i] = arr[mid + 1 + i];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

// Сортировка вставками
void insertionSort(vector<int>& arr, int left, int right) {
    for (int i = left + 1; i <= right; ++i) {
        int key = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > key) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

// Стандартный Merge Sort
void mergeSort(vector<int>& arr, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

// Гибридный Merge + Insertion Sort
void hybridMergeSort(vector<int>& arr, int left, int right, int threshold) {
    if (right - left + 1 <= threshold) {
        insertionSort(arr, left, right);
        return;
    }
    int mid = left + (right - left) / 2;
    hybridMergeSort(arr, left, mid, threshold);
    hybridMergeSort(arr, mid + 1, right, threshold);
    merge(arr, left, mid, right);
}

int main() {
    ArrayGenerator generator;
    SortTester tester;

    vector<int> sizes;
    for (int size = 500; size <= 10000; size += 100) {
        sizes.push_back(size);
    }

    ofstream results("/Users/alexandraovsyannikova/Desktop/results.csv");
    results << "Size,Type,MergeSort,HybridMergeSort\n";

    for (int size : sizes) {
        auto randomArray = generator.generateRandomArray(size);
        auto reversedArray = generator.generateReversedArray(size);
        auto almostSortedArray = generator.generateAlmostSortedArray(size);

        vector<pair<string, vector<int>>> datasets = {
            {"Random", randomArray},
            {"Reversed", reversedArray},
            {"AlmostSorted", almostSortedArray}
        };

        for (auto& [type, array] : datasets) {
            long long mergeTime = tester.measureSortTime(array, [](vector<int>& arr) {
                mergeSort(arr, 0, arr.size() - 1);
            });

            long long hybridTime = tester.measureSortTime(array, [](vector<int>& arr) {
                hybridMergeSort(arr, 0, arr.size() - 1, 15);
            });

            results << size << "," << type << "," << mergeTime << "," << hybridTime << "\n";
        }
    }

    results.close();
    cout << "Results saved to results.csv\n";
    return 0;
}
