



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

 QString csv_line;

 QString kind;
 QString address;
 QString comments;
 QString zip;
 QString city;
 QString name;
 QStringList refers;
 QStringList hours;
 r8 latitude;
 r8 longitude;

};


QString read_csv_file(QString path, QVector<Location_Data>& result, QString* for_header = nullptr)
{
 //QString contents = load_file(path);

 QFileInfo qfi(path);

 QString file_name = qfi.baseName();

 if(file_name.endsWith("-list"))
   file_name.chop(5);

 QStringList raw_lines;
 QList<QStringList> lines = QtCSV::Reader::readToList(path, &raw_lines);

 lines.takeFirst(); //  header

 if(for_header)
   *for_header = raw_lines.first();

 raw_lines.takeFirst();

 u2 row = 0;
 for(QStringList qsl : lines)
 {
  if(qsl.isEmpty())
    continue;

  Location_Data loc;
  loc.csv_line = raw_lines[row];
  ++row;

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
     QRegularExpression rx("(.+)\\((.*)\\)");
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
   case 7:
    {
     if(field.isEmpty() || field.toLower() == "all times")
       loc.hours = QStringList {"All times"};
     else if(field == "N/A")
       break;
     else
     {
      loc.hours = field.split(";", Qt::SkipEmptyParts);
      QStringList comments;
      for(QString& h_row : loc.hours)
      {
       QString h = h_row.simplified();
       static QMap<QString, QString> days {
         { "Mon", "Monday" },
         { "Tue", "Tuesday" },
         { "Wed", "Wednesday" },
         { "Thu", "Thursday" },
         { "Fri", "Friday" },
         { "Sat", "Saturday" },
         { "Sun", "Sunday" },
         { "M-F", "Monday to Friday" },
       };

       QString comment;
       {
        QRegularExpression rx("(\\S+)\\s*\\((.+)\\)");
        QRegularExpressionMatch match = rx.match(h);
        if(match.hasMatch())
        {
         h = match.captured(1);
         comment = match.captured(2);
        }
       }

       if(!comment.isEmpty())
         comments.push_back(comment);

       QString time_component;
       {
        QRegularExpression rx("(.*)/([^-]+)-([^-]+)");
        QRegularExpressionMatch match = rx.match(h);
        if(match.hasMatch())
        {
         h = match.captured(1);

         QString tc1, tc2;
         tc1 = match.captured(2);
         tc2 = match.captured(3);

         if(tc1.contains("."))
           tc1 = tc1.replace('.', ':') + "AM";
         else
           tc1 = tc1 + "PM";

         if(tc2.contains("."))
           tc2 = tc2.replace('.', ':') + "AM";
         else
           tc2 = tc2 + "PM";

         time_component = tc1 + "&ndash;" + tc2;
        }
       }

       if(days.contains(h))
         h = days[h];
       else
       {
        QStringList longs;
        static QByteArray days_short = "MTWRFSU"_qt.toLatin1();
        static QStringList days_long = "Monday Tuesday Wednesday "
          "Thursday Friday Saturday Sunday"_qt.split(' ');

        for(u1 i = 0; i < 7; ++i)
        {
         if(h.contains(QChar::fromLatin1(days_short[i])))
           longs.push_back(days_long[i]);
        }
        h = longs.join(", ");
       }

       h_row = h + " " + time_component;

      }
      if(comments.isEmpty())
        loc.hours.push_back("");
      else
        loc.hours.push_back("(%1)"_qt.arg(comments.join("; ")));
     }
    }
    break;

   default: break;
   }
  }

  result.push_back(loc);
 }

 return file_name;
}

void read_csv_files(QDir qd)
{
 QString csv_header;

 QStringList types;
 types << "*.csv";

 QMap<QString, QVector<Location_Data>> data_by_kind;

 QVector<QPair<const Location_Data*, QString>> composite_data;

 {
  QDirIterator it(qd.path(), types);
  while (it.hasNext())
  {
   QString path = it.next();

   QVector<Location_Data> data;

   QString kind = read_csv_file(path, data,
     csv_header.isEmpty()? &csv_header : nullptr);

   data_by_kind[kind] = data;

//   QVector<Location_Data>& ds = data_by_kind[kind];

//   for(Location_Data& loc : ds)
//   {
//    composite_data.push_back(QPair<const Location_Data*, QStringList>{&loc, {}});
//   }
  }
 }




 QMap<QString, QString> out_text;

 static QString name_line = "<div class='entry-name'>%1</div>";
 static QString address_line = "<div class='entry-address'>%1 (%2 %3)</div>";
 static QString comments_line = "<div class='entry-address-comments'>%1</div>";

 static QString instagram_refer_line = "<div class='entry-refer instagram'>Instagram: %1</div>";
 static QString facebook_refer_line = "<div class='entry-refer facebook'>"
   "Facebook: <a href='http://www.%1'>http://www.%1</a></div>";
 static QString twitter_refer_line = "<div class='entry-refer twitter'>"
   "Twitter: <a href='http://www.%1'>http://www.%1</a></div>";
 static QString web_refer_line = "<div class='entry-refer web'>"
   "Web Site: <a href='%1'>%1</a></div>";
 static QString other_refer_line = "<div class='entry-refer see'> See: %1</div>";

 static QString phone_refer_line = "<div class='entry-refer phone'> Phone: %1</div>";
 static QString email_refer_line = "<div class='entry-refer phone'> Email: %1</div>";

 static QString hours_start_line = "<div class='entry-hours'> Hours: %1";
 static QString hours_line = "\n<span class='entry-hours'>%1</span>";

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

    if(loc.name.startsWith("Community Cupboard"))
    {
     qDebug() << loc.name;
    }

    for(QString r : loc.refers)
    {
     if(r.startsWith("@"))
       lines << instagram_refer_line.arg(r);

     else if(r.startsWith("facebook.com"))
       lines << facebook_refer_line.arg(r);

     else if(r.startsWith("twitter.com"))
       lines << twitter_refer_line.arg(r);

     else if(r.startsWith("http"))
       lines << web_refer_line.arg(r);

     else if(r.startsWith("www"))
       lines << web_refer_line.arg("http://" + r);

//     else if(r.startsWith("http"))
//       lines << web_refer_line.arg(r);

     else if(r.startsWith("email:"))
       lines << email_refer_line.arg(r.mid(6));

     else if(r.startsWith("#"))
       lines << phone_refer_line.arg(r.mid(1));

     else
       lines << other_refer_line.arg(r);
    }

    QStringList qsl = loc.hours;
    QString hours_comment = qsl.last();
    if(hours_comment.isEmpty())
      qsl.takeLast();
    if(qsl.size() > 1)
    {
     lines << hours_start_line.arg("");
     for(QString h : qsl)
     {
      lines << hours_line.arg(h);
     }
     lines << "\n</div>";
    }
    else
      lines << hours_start_line.arg(qsl.first());

    QString html = html_wrap.arg(count).arg(lines.join("\n"));
    out_qts << html;

    composite_data.push_back({&loc, html});
   }

  }
 }

 std::sort(composite_data.begin(), composite_data.end(), [](auto& lhs, auto& rhs)
 {
  if(lhs.first->zip == rhs.first->zip)
  {
   if(lhs.first->city == rhs.first->city)
     return lhs.first->address < rhs.first->address;
   return lhs.first->city < rhs.first->city;
  }

  return lhs.first->zip < rhs.first->zip;
 });

 QString aggregate;

 {
  QTextStream qts(&aggregate);
  for(auto& pr : composite_data)
  {
   qts << pr.second;
  }
 }

 out_text["aggregate"] = aggregate;


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

 qd.cdUp();
 qd.cd("aggregate");
 {
  QString composite;
  QTextStream qts(&composite);
  qts << csv_header << "\n";
  for(auto& pr: composite_data)
  {
   if(pr.first->csv_line.isEmpty())
     qts << "?????\n";
   else
     qts << pr.first->csv_line << "\n";
  }
  save_file(qd.absoluteFilePath("aggregate-list.csv"), composite);
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


//  if(key == "pantry-data")
//    qDebug() << "key = " << key;

  QFileInfo qfi(qd, key + ".htm");
//  if(!qfi.exists())

  if(key.endsWith("-data"))
  {
   QDir csv_dir = qd;
   csv_dir.cdUp();
   csv_dir.cd("data");
   csv_dir.cd("csv-to-html");
   copy_file_to_folder(csv_dir.absoluteFilePath(key + ".htm"), qd.absolutePath());
  }

  QString insert = load_file(qfi.absoluteFilePath());

  if(insert.isEmpty())
  {
   if(qfi.exists())
     insert = " [empty file]: %1 "_qt.arg(qfi.fileName());
   else
     insert = " [missing file]: %1 "_qt.arg(qfi.fileName());
  }

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

  if(path.startsWith("refugee"))
    qDebug() << o_path;

  save_file(o_path, c);
 }

 return 0;
}

