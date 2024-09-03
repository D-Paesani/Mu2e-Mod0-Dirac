#pragma once

struct Geom {  //trasformare in singleton

private:

public:

  static const int cryNo = 51;
  static const int sidNo = 2;
  static const int chNo = 102;
  static const int rowNo = 7;
  static const int colNo = 9;

  static constexpr float cryL = 34.2; //check wrapping
  static constexpr float cryD = 200.0;
  static constexpr float widthX = cryL * 9;
  static constexpr float widthY =  cryL * 7;
  static constexpr int maxCol[colNo] = { 6, 7, 8, 9, 8, 7, 6 };

  using TypeCryMatrixDouble = double[cryNo][sidNo];
  using TypeCryMatrixInt = int[cryNo][sidNo];
  using TypeCryArrayDouble = double[cryNo];
  using TypeCryArrayInt = int[cryNo];

  static constexpr auto cryX = [] { // fillare -1 in celle non corrisp
    std::array<std::array<float, colNo>, rowNo>  vals{};
    
    float yMin = -cryL*((float)rowNo/2 - 0.5);
    for (int irow = 0; irow < rowNo; irow++) { 
      int maxcol = maxCol[irow];
      float xMin = -cryL*((float)maxcol/2 - 0.5);
      for (int icol = 0; icol < maxcol; icol++) { 
        vals[irow][icol] = xMin + icol*cryL;
        //cryY[irow][icol] = yMin + irow*cryL;
      }
    }
    return vals;
  }();

  static constexpr auto cryY = [] {
    std::array<std::array<float, colNo>, rowNo>  vals{};
    
    float yMin = -cryL*((float)rowNo/2 - 0.5);
    for (int irow = 0; irow < rowNo; irow++) { 
      int maxcol = maxCol[irow];
      float xMin = -cryL*((float)maxcol/2 - 0.5);
      for (int icol = 0; icol < maxcol; icol++) { 
        //cryX[irow][icol] = xMin + icol*cryL;
        vals[irow][icol] = yMin + irow*cryL;
      }
    }
    return vals;
  }();

  static constexpr auto cryRC = [] {
    std::array<std::array<int, colNo>, rowNo>  vals{};
    
    for (int icol = 0; icol < colNo; icol++) { 
      int val = icol;
      for (int irow = 0; irow < rowNo; irow++) { 
        vals[irow][icol] = val;
        val += maxCol[irow];
      }
    }
    return vals;
  }();

  static constexpr auto rowCry = [] {
    std::array<int, cryNo> vals{};
    
    for (int icol = 0; icol < colNo; icol++) { 
      int val = icol;
      for (int irow = 0; irow < rowNo; irow++) { 
        if(val < cryNo && icol < maxCol[irow]) { vals[val] = irow; }
        val += maxCol[irow];
      }
    }
    return vals;
  }();

  static constexpr auto colCry = [] {
    std::array<int, cryNo> vals{};
    
    for (int icol = 0; icol < colNo; icol++) { 
      int val = icol;
      for (int irow = 0; irow < rowNo; irow++) { 
        if(val < cryNo && icol < maxCol[irow]) { vals[val] = icol; }
        val += maxCol[irow];
      }
    }
    return vals;
  }();

  static int sideCha(int n) { return n%2; }
  static int cryCha(int n) { return floor(n/2); }
  static int chaCry(int c, int s) { return 2*c+s; } 












};
