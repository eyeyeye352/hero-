#ifndef HEROTEXT_H
#define HEROTEXT_H

#include <QMainWindow>
#include <QMessageBox>
#include <QIcon>

QT_BEGIN_NAMESPACE
namespace Ui {
class HeroText;
}
QT_END_NAMESPACE

struct Hero{
    QString name;
    int age;
    QString sex;
    double atk;
    QString skill;
    QIcon icon;
};

class HeroText : public QMainWindow
{
    Q_OBJECT

public:
    HeroText(QWidget *parent = nullptr);
    ~HeroText();

public slots:
    void newFile();
    void open();
    void save();
    void saveAs();

private:
    Ui::HeroText *ui;
    QString current_file;
    Hero hero;
};
#endif // HEROTEXT_H
