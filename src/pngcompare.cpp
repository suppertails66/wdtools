#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TStringConversion.h"
#include "util/ByteConversion.h"
#include "util/TTwoDArray.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using namespace BlackT;

int main (int argc, char* argv[]) {
  
  if (argc < 3) {
    return 0;
  }
  
  TGraphic first;
  TGraphic second;
  
  TPngConversion::RGBAPngToGraphic(string(argv[1]),
                                   first);
  TPngConversion::RGBAPngToGraphic(string(argv[2]),
                                   second);
                                   
  if ((first.w() != second.w())
      || (first.h() != second.h())) {
//    cout << "\"" << argv[1] << "\" \"" << argv[2] << "\"" << endl;
    return 0;
  }
                                   
  for (int j = 0; j < first.h(); j++) {
    for (int i = 0; i < first.w(); i++) {
      TColor col1 = first.getPixel(i, j);
      TColor col2 = second.getPixel(i, j);
      
      if ((col1.r() != col2.r())
          || (col1.g() != col2.g())
          || (col1.b() != col2.b())
          || (col1.a() != col2.a())) {
//        cout << "\"" << argv[1] << "\" \"" << argv[2] << "\"" << endl;
        return 0;
      }
    }
  }
        cout << "\"" << argv[1] << "\" \"" << argv[2] << "\"" << endl;
  
  return 0;
}
