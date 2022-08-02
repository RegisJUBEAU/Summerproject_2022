#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QDebug>
#include <QMessageBox>
#include <QTableWidget>
#include <QVector>
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <math.h>
#include <QFont>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void create_table(QString file_name, QVector<int> *colonne0, QVector<int> *colonne2);

    void find_segmentation(QVector<int> *tb, QVector<int> *segm);

    int AVG (QVector<int> *tb);

    void correction(QVector<int> *segm);

    void avg_by_segmentation(QVector<int> *tb, QVector<int> *segm, QVector<int> *avg_tb);

    void partition_v2(QVector<int> * AVG, QVector<int> * PART, QVector<int> * tb_s, long int nb_part);

    int MAX(QVector<int> *tb);

    void write_txt_part( QString Chemin_Fichier, QVector<int> * tb, QVector<int> * SEG, QVector<int> * AVG, QVector<int> * PART);

    void write_txt(QString Chemin_Fichier, QVector<int> * tb, QVector<int> * SEG, QVector<int> * AVG);

    int MIN(QVector<int> *tb);

    void plot(QVector<int> * x_, QVector<int> * y1_, QVector<int> * y2_, QVector<int> * SEG_);

    void correction_v2(QVector<int> * segm);

private slots:
    void on_pushButton_select_file_clicked();

    void on_pushButton_calculate_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
