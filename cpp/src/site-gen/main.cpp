



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
 QString address;
 QString comments;
 QString zip;
 QString city;
 QString name;
 QStringList refers;
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

   switch (col)
   {
   case 1:
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
    break;

   case 2:
    {
     QRegularExpression rx("(.+)\\(.*\\)");
     QRegularExpressionMatch match = rx.match(field);
     if(match.hasMatch())
     {
      loc.address = match.captured(1);
      loc.comments = match.captured(2);
     }
     else
       loc.address = field;
    }
    break;
   case 3: loc.zip = field; break;
   case 4: loc.city = field; break;
   case 5: loc.refers = field.split(";",
     Qt::SkipEmptyParts);
     for(QString& r : loc.refers)
       r = r.simplified();
     break;
   case 6: loc.name = field; break;
   default: break;
   }
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

 {
  QDirIterator it(qd.path(), types);
  while (it.hasNext())
  {
   QString path = it.next();

   QVector<Location_Data> data;

   QString kind = read_csv_file(path, data);

   data_by_kind[kind] = data;
  }
 }


 QMap<QString, QString> out_text;

 static QString name_line = "<div class='entry-name'>%1</div>";
 static QString address_line = "<div class='entry-address'>%1 (%2 %3)</div>";
 static QString comments_line = "<div class='entry-address-comments'>%1</div>";

 static QString instagram_refer_line = "<div class='entry-refer instagram'>Instagram: %1</div>";
 static QString facebook_refer_line = "<div class='entry-refer facebook'>"
   "Facebook: <a href='http://www.%1'>http://www.%1</a></div>";
 static QString web_refer_line = "<div class='entry-refer web'>"
   "Web Site: <a href='http://%1'>http://%1</a></div>";
 static QString other_refer_line = "<div class='entry-refer see'> See: %1</div>";


 static QString html_wrap = R"_(
   <!-- csv row %1 -->
   <section>
%2
   </section>
                       )_";

 {
  QMapIterator<QString, QVector<Location_Data>> it(data_by_kind);
  while(it.hasNext())
  {
   it.next();
   QTextStream out_qts(&out_text[it.key()]);

   u2 count = 0;

   for(const Location_Data& loc : it.value())
   {
    ++count;

    QStringList lines;
    if(!loc.name.isEmpty())
      lines += name_line.arg(loc.name);

    lines += address_line.arg(loc.address).arg(loc.city).arg(loc.zip);

    if(!loc.comments.isEmpty())
      lines += comments_line.arg(loc.comments);

    for(QString r : loc.refers)
    {
     if(r.startsWith("@"))
       lines << instagram_refer_line.arg(r);

     else if(r.startsWith("facebook.com"))
       lines << facebook_refer_line.arg(r);

     else if(r.startsWith("http"))
       lines << web_refer_line.arg(r);

     else
       lines << other_refer_line.arg(r);
    }

    out_qts << html_wrap.arg(count).arg(lines.join("\n"));
   }

  }
 }

 qd.cd("csv-to-html");
 {
  QMapIterator<QString, QString> it(out_text);
  while (it.hasNext())
  {
   it.next();

   QString outfile = qd.absoluteFilePath(it.key() + "-data.htm");
   save_file(outfile, it.value());
  }

 }

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

