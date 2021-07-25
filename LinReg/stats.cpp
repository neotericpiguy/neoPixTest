#include <chrono>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <vector>

namespace Reg {
struct LinRegData {
  double Sx;
  double Sy;
  double Sxx;
  double Sxy;
  double Syy;
  unsigned int n;
  double m;
  double b;
  double r;
};

struct LinRegData_xy {
  double x;
  double y;
  double Sx;
  double Sy;
  double Sxx;
  double Sxy;
  double Syy;
  unsigned int n;
  double m;
  double b;
  double r;
};

void LinReg(LinRegData& data, double x, double y)
{
  data.Sx += x;
  data.Sy += y;
  data.Sxx += x * x;
  data.Syy += y * y;
  data.Sxy += x * y;
  data.n++;
}

constexpr void LinRegCoefs(LinRegData& data)
{
  data.m = (data.n * data.Sxy - data.Sx * data.Sy) / (data.n * data.Sxx - data.Sx * data.Sx);
  data.b = data.Sy / data.n - data.m * data.Sx / data.n;
  data.r = (data.n * data.Sxy - data.Sx * data.Sy) / sqrt((data.n * data.Sxx - data.Sx * data.Sx) * (data.n * data.Syy - data.Sy * data.Sy));
}

constexpr void LinRegCoefs(LinRegData_xy& data)
{
  data.m = (data.n * data.Sxy - data.Sx * data.Sy) / (data.n * data.Sxx - data.Sx * data.Sx);
  data.b = data.Sy / data.n - data.m * data.Sx / data.n;
  data.r = (data.n * data.Sxy - data.Sx * data.Sy) / sqrt((data.n * data.Sxx - data.Sx * data.Sx) * (data.n * data.Syy - data.Sy * data.Sy));
}

void LinReg_xy(LinRegData_xy& data)
{
  data.Sx += data.x;
  data.Sy += data.y;
  data.Sxx += data.x * data.x;
  data.Syy += data.y * data.y;
  data.Sxy += data.x * data.y;
  data.n++;
}

}  // namespace Reg

void LinRegStatic(double& m, double& b, double& r, double x, double y)
{
  static double Sx = 0;
  static double Sy = 0;
  static double Sxx = 0;
  static double Syy = 0;
  static double Sxy = 0;
  static unsigned int n = 0;

  Sx += x;
  Sy += y;
  Sxx += x * x;
  Syy += y * y;
  Sxy += x * y;
  n++;

  m = (n * Sxy - Sx * Sy) / (n * Sxx - Sx * Sx);
  b = Sy / n - m * Sx / n;
  r = (n * Sxy - Sx * Sy) / sqrt((n * Sxx - Sx * Sx) * (n * Syy - Sy * Sy));
}

std::chrono::microseconds trial1(std::ifstream& instream)
{
  auto start = std::chrono::high_resolution_clock::now();
  Reg::LinRegData data = {0};
  double& m = data.m;
  double& b = data.b;
  double& r = data.r;
  while (!instream.eof())
  {
    double x;
    double y;

    instream >> x;
    instream.get();
    instream >> y;
    if (instream.eof())
      break;
    Reg::LinReg(data, x, y);
  }
  Reg::LinRegCoefs(data);
  auto stop = std::chrono::high_resolution_clock::now();

  instream.clear();                  // clear fail and eof bits
  instream.seekg(0, std::ios::beg);  // back to the start!

  return std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
}

std::chrono::microseconds trial2(std::ifstream& instream)
{
  auto start = std::chrono::high_resolution_clock::now();
  double m, b, r;
  while (!instream.eof())
  {
    double x;
    double y;

    instream >> x;
    instream.get();
    instream >> y;
    if (instream.eof())
      break;
    LinRegStatic(m, b, r, x, y);
  }
  auto stop = std::chrono::high_resolution_clock::now();

  instream.clear();                  // clear fail and eof bits
  instream.seekg(0, std::ios::beg);  // back to the start!

  return std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
}

std::chrono::microseconds trial3(std::ifstream& instream)
{
  auto start = std::chrono::high_resolution_clock::now();
  Reg::LinRegData_xy data = {0};
  while (!instream.eof())
  {
    instream >> data.x;
    instream.get();
    instream >> data.y;
    if (instream.eof())
      break;
    Reg::LinReg_xy(data);
  }
  Reg::LinRegCoefs(data);
  auto stop = std::chrono::high_resolution_clock::now();

  instream.clear();                  // clear fail and eof bits
  instream.seekg(0, std::ios::beg);  // back to the start!

  return std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
}

int main(int argc, char** argv)
{
  if (argc < 1)
    return -1;

  std::string filename(argv[1]);
  std::cout << "Reading " << filename << std::endl;

  std::ifstream instream(filename.c_str());

  if (!instream.is_open())
  {
    std::cerr << "filename janky" << std::endl;
    return -1;
  }

  const int repeats = 100;
  std::cout << "repeats: " << repeats << std::endl;

  const std::map<std::string, std::function<std::chrono::microseconds(std::ifstream&)>> funcMap = {
      {"data   ", trial1},
      {"static ", trial2},
      {"data_xy", trial3},
  };
  std::map<std::string, std::chrono::microseconds> durationMap;

  for (int i = 0; i < repeats; ++i)
  {
    for (const auto& funcPair : funcMap)
      durationMap[funcPair.first] += funcPair.second(instream);

    instream.clear();                  // clear fail and eof bits
    instream.seekg(0, std::ios::beg);  // back to the start!
  }

  for (const auto& durationPair : durationMap)
    std::cout << "\"" << durationPair.first << "\": " << durationPair.second.count() / repeats << "us" << std::endl;

  return 0;
}
