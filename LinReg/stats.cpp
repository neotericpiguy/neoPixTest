#include <stdlib.h>

#include <chrono>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <thread>
#include <vector>

class Stats
{
public:
  struct StatsData {
    unsigned int xColumn;
    unsigned int yColumn;
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

  int open(const std::string& filename)
  {
    std::ifstream instream(filename.c_str());

    mFilename = filename;

    if (!instream.is_open())
    {
      std::cerr << "filename janky" << std::endl;
      return -1;
    }

    std::string line;
    while (!instream.eof())
    {
      double data;
      std::vector<double> datas;
      do
      {
        instream >> data;
        if (instream.eof())
          break;
        datas.push_back(data);
      } while (instream.get() != '\n');
      if (instream.eof())
        break;

      for (auto& pair : mStatPairs)
      {
        if (pair.second.xColumn > datas.size() || pair.second.yColumn > datas.size())
        {
          std::cerr << "invalid column: " << std::endl;
          return -1;
        }
        addPair(pair.first, datas[pair.second.xColumn - 1], datas[pair.second.yColumn - 1]);
      }
    }
    return 0;
  }

  int addStatPair(const std::string& key, unsigned int x, unsigned int y)
  {
    mStatPairs[key].xColumn = x;
    mStatPairs[key].yColumn = y;
    return 0;
  }

  int addPair(const std::string& key, double x, double y)
  {
    mStatPairs[key].Sx += x;
    mStatPairs[key].Sy += y;
    mStatPairs[key].Sxx += x * x;
    mStatPairs[key].Syy += y * y;
    mStatPairs[key].Sxy += x * y;
    mStatPairs[key].n++;

    return 0;
  }

  int LinRegCoefs(const std::string& key)
  {
    mStatPairs[key].m = (mStatPairs[key].n * mStatPairs[key].Sxy - mStatPairs[key].Sx * mStatPairs[key].Sy) / (mStatPairs[key].n * mStatPairs[key].Sxx - mStatPairs[key].Sx * mStatPairs[key].Sx);
    mStatPairs[key].b = mStatPairs[key].Sy / mStatPairs[key].n - mStatPairs[key].m * mStatPairs[key].Sx / mStatPairs[key].n;
    mStatPairs[key].r = (mStatPairs[key].n * mStatPairs[key].Sxy - mStatPairs[key].Sx * mStatPairs[key].Sy) / sqrt((mStatPairs[key].n * mStatPairs[key].Sxx - mStatPairs[key].Sx * mStatPairs[key].Sx) * (mStatPairs[key].n * mStatPairs[key].Syy - mStatPairs[key].Sy * mStatPairs[key].Sy));
    return 0;
  }

  int plot(const std::string& key, const std::string& outputFilename) const
  {
    std::ofstream outFile(outputFilename + ".plot");
    if (!outFile.is_open())
      return -1;

    const StatsData& statsData = mStatPairs.at(key);

    outFile << "set terminal png size 2000,800;\n";
    outFile << "set datafile separator \",\";\n";
    outFile << "set output \"" << outputFilename << "\";\n";

    if (statsData.r != 0)
    {
      outFile << "m = " << statsData.m << std::endl;
      outFile << "b = " << statsData.b << std::endl;
      outFile << "f(x) = m*x+b" << std::endl;
    }

    outFile << "plot \"" << mFilename << "\" using " << statsData.xColumn << ":"
            << statsData.yColumn << " title \"" << key << "\" with linespoints";

    if (statsData.r != 0)
    {
      outFile << ", f(x) with line title \"linreg\"";
    }

    outFile << std::endl;
    system(std::string("gnuplot -p " + outputFilename + ".plot").c_str());
    system(std::string("rm " + outputFilename + ".plot").c_str());
    return 0;
  }

  const StatsData& getStats(const std::string& key) const
  {
    static StatsData empty;
    if (mStatPairs.find(key) == mStatPairs.end())
    {
      return empty;
    }
    return mStatPairs.at(key);
  }

private:
  std::map<std::string, StatsData> mStatPairs;
  std::string mFilename;
};

int main(int argc, char** argv)
{
  if (argc < 2)
    return -1;

  std::string filename(argv[1]);
  std::cout << "Reading " << filename << std::endl;

  std::ifstream instream(filename.c_str());

  if (!instream.is_open())
  {
    std::cerr << "filename janky" << std::endl;
    return -1;
  }

  Stats data;
  data.addStatPair("data", 1, 2);
  data.open(filename);
  data.LinRegCoefs("data");
  const Stats::StatsData& linData = data.getStats("data");
  std::cout << linData.m << std::endl;
  std::cout << linData.b << std::endl;
  data.plot("data", "data.png");
  //  data.plotLinReg("file.plot");

  return 0;
}
