#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <string>

using namespace std;

class ArrayGenerator {
 public:
  ArrayGenerator(int size, int maxVal, unsigned seed = 42) : size(size), maxVal(maxVal) {
    mt19937 gen(seed);
    uniform_int_distribution<int> dist(0, maxVal);

    randomArray.resize(size);
    for (int i = 0; i < size; ++i) {
      randomArray[i] = dist(gen);
    }

    reversedArray = randomArray;
    sort(reversedArray.begin(), reversedArray.end(), greater<int>());

    almostSorted = randomArray;
    sort(almostSorted.begin(), almostSorted.end());
    int swaps = size / 100;
    for (int k = 0; k < swaps; ++k) {
      int i = gen() % size;
      int j = gen() % size;
      swap(almostSorted[i], almostSorted[j]);
    }
  }

  vector<int> getRandom(int n) const {
    return vector<int>(randomArray.begin(), randomArray.begin() + n);
  }

  vector<int> getReversed(int n) const {
    return vector<int>(reversedArray.begin(), reversedArray.begin() + n);
  }

  vector<int> getAlmostSorted(int n) const {
    return vector<int>(almostSorted.begin(), almostSorted.begin() + n);
  }

 private:
  int size;
  int maxVal;
  vector<int> randomArray;
  vector<int> reversedArray;
  vector<int> almostSorted;
};

void insertionSort(vector<int>& arr, int left, int right) {
  for (int i = left + 1; i <= right; i++) {
    int key = arr[i];
    int j = i - 1;
    while (j >= left && arr[j] > key) {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
  }
}

void merge(vector<int>& arr, int left, int mid, int right) {
  int n1 = mid - left + 1;
  int n2 = right - mid;

  vector<int> leftArr(n1), rightArr(n2);
  for (int i = 0; i < n1; i++)
    leftArr[i] = arr[left + i];
  for (int j = 0; j < n2; j++)
    rightArr[j] = arr[mid + 1 + j];

  int i = 0, j = 0, k = left;
  while (i < n1 && j < n2) {
    if (leftArr[i] <= rightArr[j])
      arr[k++] = leftArr[i++];
    else
      arr[k++] = rightArr[j++];
  }
  while (i < n1)
    arr[k++] = leftArr[i++];
  while (j < n2)
    arr[k++] = rightArr[j++];
}

void mergeSort(vector<int>& arr, int left, int right) {
  if (left < right) {
    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
  }
}

void mergeSortHybrid(vector<int>& arr, int left, int right, int threshold) {
  int size = right - left + 1;
  if (size <= threshold) {
    insertionSort(arr, left, right);
    return;
  }
  int mid = left + (right - left) / 2;
  mergeSortHybrid(arr, left, mid, threshold);
  mergeSortHybrid(arr, mid + 1, right, threshold);
  merge(arr, left, mid, right);
}

class SortTester {
 public:
  template <typename Func>
  static long long timeOne(const vector<int>& data, Func sortFunc) {
    vector<int> a = data;
    auto start = chrono::high_resolution_clock::now();
    sortFunc(a);
    auto elapsed = chrono::high_resolution_clock::now() - start;
    return chrono::duration_casthrono::micrososeconds > (elapsed).count();
  }

  template <typename Func>
  static double timeAvg(const vector<int>& data, Func sortFunc, int runs) {
    long long sum = 0;
    for (int i = 0; i < runs; ++i) {
      sum += timeOne(data, sortFunc);
    }
    return static_cast<double>(sum) / runs;
  }
};

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  const int maxSize = 100000;
  const int minSize = 500;
  const int step = 100;
  const int runs = 5;
  const int maxValue = 6000;

  ArrayGenerator maker(maxSize, maxValue);

  ofstream file("results.csv");
  file << "type,n,algo,threshold,time" << endl;

  vector<string> types = {"random", "reversed", "almost"};
  vector<int> thresholds = {5, 10, 15, 20, 30, 50};

  for (const string& type : types) {
    for (int n = minSize; n <= maxSize; n += step) {
      vector<int> data;
      if (type == "random")
        data = maker.getRandom(n);
      else if (type == "reversed")
        data = maker.getReversed(n);
      else if (type == "almost")
        data = maker.getAlmostSorted(n);

      double timeStd = SortTester::timeAvg(data, [&](vector<int>& a) { mergeSort(a, 0, n - 1); }, runs);
      file << type << "," << n << ",standard,0," << timeStd << endl;

      for (int thr : thresholds) {
        double timeHyb = SortTester::timeAvg(data, [&](vector<int>& a) { mergeSortHybrid(a, 0, n - 1, thr); }, runs);
        file << type << "," << n << ",hybrid," << thr << "," << timeHyb << endl;
      }
    }
  }

  file.close();
  return 0;
}
