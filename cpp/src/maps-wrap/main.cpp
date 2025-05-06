
//           Copyright Nathaniel Christen 2025.
//  Distributed under the Boost Software License, Version 1.0.
//     (See accompanying file LICENSE_1_0.txt or copy at
//           http://www.boost.org/LICENSE_1_0.txt)



#include "utils/rdsc-qh/qh-web-view-dialog.h"

#include <QApplication>



int main(int argc, char *argv[])
{
 QApplication qapp(argc, argv);

// QString url = "https://www.google.com/maps/d/u/0/viewer?mid=144OL4Q89OhRc2JInBCT-TMrvVjZ5zG4&ll=39.281772899999986%2C-76.6505279&z=16";
// QH_Web_View_Dialog dlg("https://www.google.com/mymaps");

 QString url = "https://www.google.com/maps/place/607+Ashburton+St,+Baltimore,+MD+21216/@39.2950837,-76.6612176,16z/data=!3m1!4b1!4m6!3m5!1s0x89c81b5f0923b6af:0x91f4d91b76d85c37!8m2!3d39.2950837!4d-76.6612176!16s%2Fg%2F11cpm1ftjm?entry=ttu&g_ep=EgoyMDI1MDQzMC4xIKXMDSoASAFQAw%3D%3D";


 QH_Web_View_Dialog dlg(url);

 dlg.show();

 return qapp.exec();
}
