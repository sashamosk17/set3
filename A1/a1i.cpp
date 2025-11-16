#include <iostream>
#include <random>
#include <cmath>
#include <iomanip>
#include <fstream>
using namespace std;

struct Circle {
  double x, y, r;
};

bool isInsideCircle(double px, double py, const Circle& c) {
  double dx = px - c.x;
  double dy = py - c.y;
  return (dx * dx + dy * dy) <= (c.r * c.r);
}

bool isInIntersection(double px, double py, const Circle& c1, const Circle& c2, const Circle& c3) {
  return isInsideCircle(px, py, c1) && isInsideCircle(px, py, c2) && isInsideCircle(px, py, c3);
}

double estimateArea(const Circle& c1, const Circle& c2, const Circle& c3, double minX, double maxX, double minY,
                    double maxY, int numSamples) {
  double rectArea = (maxX - minX) * (maxY - minY);

  mt19937 rng(42);
  uniform_real_distribution<double> distX(minX, maxX);
  uniform_real_distribution<double> distY(minY, maxY);

  int insideCount = 0;
  for (int i = 0; i < numSamples; ++i) {
    double x = distX(rng);
    double y = distY(rng);
    if (isInIntersection(x, y, c1, c2, c3)) {
      insideCount++;
    }
  }

  return rectArea * static_cast<double>(insideCount) / numSamples;
}

int main() {
  Circle c1{1.0, 1.0, 1.0};
  Circle c2{1.5, 2.0, sqrt(5.0) / 2.0};
  Circle c3{2.0, 1.5, sqrt(5.0) / 2.0};

  const double PI = 3.14159265358979323846;
  const double exact = 0.25 * PI + 1.25 * asin(0.8) - 1.0;
  const double wide_minX = 0.0, wide_maxX = 3.0;
  const double wide_minY = 0.0, wide_maxY = 3.0;
  double narrow_minX = max(c1.x - c1.r, max(c2.x - c2.r, c3.x - c3.r));
  double narrow_maxX = min(c1.x + c1.r, min(c2.x + c2.r, c3.x + c3.r));
  double narrow_minY = max(c1.y - c1.r, max(c2.y - c2.r, c3.y - c3.r));
  double narrow_maxY = min(c1.y + c1.r, min(c2.y + c2.r, c3.y + c3.r));



  ofstream fout("results_monte_carlo.csv");
  fout << "N,area_wide,relerr_wide,area_narrow,relerr_narrow\n";

  for (int N = 100; N <= 100000; N += 500) {
    double area_wide = estimateArea(c1, c2, c3, wide_minX, wide_maxX, wide_minY, wide_maxY, N);
    double area_narrow = estimateArea(c1, c2, c3, narrow_minX, narrow_maxX, narrow_minY, narrow_maxY, N);

    double relerr_wide = fabs(area_wide - exact) / exact;
    double relerr_narrow = fabs(area_narrow - exact) / exact;

    fout << N << "," << area_wide << "," << relerr_wide << "," << area_narrow << "," << relerr_narrow << "\n";

    cout << "N=" << N << " wide=" << area_wide << " narrow=" << area_narrow << endl;
  }

  fout.close();
  return 0;
}
