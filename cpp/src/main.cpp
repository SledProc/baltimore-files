



#include <QDir>
#include <QDirIterator>

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>

#include <QDebug>


#include "utils/textio.h"

//#include "utils/rapidcsv/rapidcsv.h"
//#include "qtcsv/qtcsv_global.h"

#include "qtcsv/stringdata.h"
#include "qtcsv/reader.h"
#include "qtcsv/writer.h"



struct Location_Data {

 QString kind;
 QString description;
 r8 latitude;
 r8 longitude;

};


QString read_csv_file(QString path, QVector<Location_Data>& result)
{
 //QString contents = load_file(path);

 QFileInfo qfi(path);

 QString file_name = qfi.baseName();

 if(file_name.endsWith("-list"))
   file_name.chop(5);

 QList<QStringList> lines = QtCSV::Reader::readToList(path);

 lines.takeFirst(); //  header

 for(QStringList qsl : lines)
 {
  if(qsl.isEmpty())
    continue;

  Location_Data loc;

  loc.kind = file_name;

  u2 col = 0;

  for(QString field : qsl)
  {
   ++col;

   if(col == 1)
   {
    QRegularExpression rx("POINT \\((\\S+) (\\S+)\\)");
    QRegularExpressionMatch match = rx.match(field);
    if(match.hasMatch())
    {
     QString lat = match.captured(1);
     QString lon = match.captured(2);

     loc.latitude = lat.toDouble();
     loc.longitude = lon.toDouble();
    }
   }
   if(col == 2)
     loc.description = field;
  }

  result.push_back(loc);
 }

 return file_name;
}

void read_csv_files(QDir qd)
{
 QStringList types;
 types << "*.csv";

 QMap<QString, QVector<Location_Data>> data_by_kind;

 QDirIterator it(qd.path(), types);
 while (it.hasNext())
 {
  QString path = it.next();

  QVector<Location_Data> data;

  QString kind = read_csv_file(path, data);

  data_by_kind[kind] = data;
 }

 qDebug() << data_by_kind.keys();
}


QString process_file(QDir qd, QString path)
{
 QString contents = load_file(path);

 QRegularExpression rx("%%([\\w-]+)%%");

 QRegularExpressionMatchIterator it(rx.globalMatch(contents));

 QVector<QPair<QString, QString>> inserts;

 while(it.hasNext())
 {
  QRegularExpressionMatch m = it.next();

  QString key = m.captured(1);

  qDebug() << "key = " << key;

  QString insert = load_file(qd.absoluteFilePath(key + ".htm"));

  inserts.push_back({m.captured(), insert});
 }

 for(QPair<QString, QString> pr : inserts)
 {
  contents.replace(pr.first, pr.second);
 }

 return contents;
}


int main(int argc, char *argv[])
{
 QDir qd(SITE_ROOT);

 read_csv_files(qd.absoluteFilePath("data"));

 QStringList types;
 types << "*.html" << "*.htm";

 QDir iqd(qd.absoluteFilePath("in"));
 QDir oqd(qd.absolutePath());

 QDirIterator it(iqd.path(), types);
 while (it.hasNext())
 {
  QString path = it.next();

  QString c = process_file(QDir(qd.absoluteFilePath("inserts")), path);

  QFileInfo qfi(path);

  QString o_path = oqd.absoluteFilePath(qfi.fileName());

  save_file(o_path, c);
 }

 return 0;
}

