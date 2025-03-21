#include "Herotext.h"
#include "./ui_Herotext.h"
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>


HeroText::HeroText(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HeroText)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    connect(ui->action_new,&QAction::triggered,this,&HeroText::newFile);
    connect(ui->action_open,&QAction::triggered,this,&HeroText::open);
    connect(ui->action_save,&QAction::triggered,this,&HeroText::save);
    connect(ui->action_saveAs,&QAction::triggered,this,&HeroText::saveAs);

    //switch
    connect(ui->action_switch_hero,&QAction::triggered,[this]{
        ui->stackedWidget->setCurrentWidget(ui->page_hero);
    });
    connect(ui->action_switch_text,&QAction::triggered,[this]{
        ui->stackedWidget->setCurrentWidget(ui->page_text);
    });

    connect(ui->heroIcon,&QPushButton::clicked,this,[this]{
        QString iconpath = QFileDialog::getOpenFileName(this,"选择图片","C:/","图片 (*.jpg *.png *.webp)");
        ui->heroIcon->setIcon(QIcon(iconpath));
        ui->heroIcon->setIconSize(ui->heroIcon->size());
    });

}



/* new：
 * 1.询问新建文件还是英雄
 * 2.切换stackedwidget到指定页面
 */
void HeroText::newFile(){

    // 消息框
    QMessageBox *msgBox = new QMessageBox(QMessageBox::Question,"新建","你要新建英雄还是文本？");

    // 自定义按钮
    QPushButton *herobtn = new QPushButton("英雄",msgBox);
    QPushButton *textbtn = new QPushButton("文本",msgBox);
    msgBox->addButton(herobtn,QMessageBox::ActionRole);
    msgBox->addButton(textbtn,QMessageBox::ActionRole);

    // 显示消息框（模态）
    msgBox->exec();

    if (msgBox->clickedButton() == textbtn) {
        // 新建文本
        ui->stackedWidget->setCurrentWidget(ui->page_text);
        ui->textEdit->clear();
    } else{
        // 用户选择了新建英雄
        ui->stackedWidget->setCurrentWidget(ui->page_hero);
        ui->heroIcon->setIcon(QIcon());
        ui->lineEdit_heroName->clear();
        ui->lineEdit_age->clear();
        ui->lineEdit_atk->clear();
        ui->lineEdit_sex->clear();
        ui->lineEdit_skill->clear();
    }

};


/*open：
* 1.QFile 储存，文件dialog 打开.hero 或 .txt
* 2.文件：直接读取全部放到文本框上
* 3.英雄：读取icon，名字，年龄，性别，技能，攻击力到各个lineedit上
*/
void HeroText::open(){
    //文本文件
    if(ui->stackedWidget->currentWidget() == ui->page_text){
        current_file = QFileDialog::getOpenFileName(this,"选择文件","C:/","文本文件(*.txt)");
        QFile file(current_file);
        \
        //判断是否能正常打开
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QMessageBox::critical(this,"错误","文件无法或者没有打开！你的电脑有问题！");
            return;
        }

        //读取全部放到文本框上
        while(!file.atEnd()){
            ui->textEdit->setText(file.readAll());
        }
        file.close();

    }
    else if(ui->stackedWidget->currentWidget() == ui->page_hero){

        //hero 文件
        current_file = QFileDialog::getOpenFileName(this,"选择文件","C:/","hero (*.hero)");
        QFile file(current_file);

        //判断是否能正常打开
        if(!file.open(QIODevice::ReadOnly)){
            QMessageBox::critical(this,"错误","hero无法或者没有打开！你的电脑有问题！");
            return;
        }

        //读取icon，名字，年龄，性别，技能，攻击力到各个lineedit上
        QDataStream in(&file);
        QPixmap temp;
        in >> temp;
        in >> hero.name >> hero.age >> hero.sex >> hero.atk >> hero.skill;
        hero.icon = QIcon(temp);

        ui->lineEdit_heroName->setText(hero.name);
        ui->lineEdit_age->setText(QString::number(hero.age));
        ui->lineEdit_sex->setText(hero.sex);
        ui->lineEdit_atk->setText(QString::number(hero.atk));
        ui->lineEdit_skill->setText(hero.skill);

        ui->heroIcon->setIcon(hero.icon);
        ui->heroIcon->setIconSize(QSize(ui->heroIcon->size()));
        file.close();
    }
};

/*save
 * 当前页面为file：
 * 如果current_file为空，则调用saveAs。
 * 不为空，QString保存整个textedit的内容，QFile全部写入，关闭file
 *
 * 当前页面为hero
 * 每个lineedit的值存到.hero文件里
 */
void HeroText::save(){

    if(current_file.isEmpty()){
        saveAs();
        return;
    }
    QFile file(current_file);

    if(ui->stackedWidget->currentWidget() == ui->page_text){
        //文本
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
            QMessageBox::critical(this,"保存失败","你的文件有问题，建议把电脑摔碎以保护您的电脑。");
            return;
        }
        file.write(ui->textEdit->toPlainText().toUtf8());
    }
    else if(ui->stackedWidget->currentWidget() == ui->page_hero){
        //hero
        if(!file.open(QIODevice::WriteOnly)){
            QMessageBox::critical(this,"保存失败","你的文件有问题，建议把电脑摔碎以保护您的电脑。");
            return;
        }

        QDataStream out(&file);
        QPixmap pixmap = ui->heroIcon->icon().pixmap(ui->heroIcon->size());
        out << pixmap;
        out << ui->lineEdit_heroName->text();
        out << ui->lineEdit_age->text().toInt();
        out << ui->lineEdit_sex->text();
        out << ui->lineEdit_atk->text().toDouble();
        out << ui->lineEdit_skill->text();
    }
    file.close();
};



/*saveas
 * qfiledialog::getsavefilename 获取保存目标路径
 */
void HeroText::saveAs(){


    if(ui->stackedWidget->currentWidget() == ui->page_text){
        //文本
        current_file = QFileDialog::getSaveFileName(this,"保存","C:/","文本文件(*txt);;其它 (*)");
        QFile file(current_file);

        if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
            QMessageBox::critical(this,"保存失败","你的文件有问题，建议把电脑摔碎以保护您的电脑。");
            return;
        }
        file.write(ui->textEdit->toPlainText().toUtf8());
        file.close();

    }else if(ui->stackedWidget->currentWidget() == ui->page_hero){
        //hero
        current_file = QFileDialog::getSaveFileName(this,"保存","C:/","hero (*.hero)");
        QFile file(current_file);

        if(!file.open(QIODevice::WriteOnly)){
            QMessageBox::critical(this,"保存失败","你的文件有问题，建议把电脑摔碎以保护您的电脑。");
            return;
        }

        QDataStream out(&file);
        QPixmap pixmap = ui->heroIcon->icon().pixmap(QSize(48,48));
        out << pixmap;
        out << ui->lineEdit_heroName->text();
        out << ui->lineEdit_age->text().toInt();
        out << ui->lineEdit_sex->text();
        out << ui->lineEdit_atk->text().toDouble();
        out << ui->lineEdit_skill->text();
        file.close();
    }

};

HeroText::~HeroText()
{
    delete ui;
}
