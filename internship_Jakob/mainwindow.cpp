#include "mainwindow.h"
#include "ui_mainwindow.h"


QVector<QVector<double>> colonne0;
QVector<QVector<double>> colonne2;
QVector<QVector<double>> colonne1;

QVector<QVector<double>> segm;
QVector<QVector<double>> avg;
QVector<QVector<double>> part;
QVector<double> segmentation;

QString file_name;
QVector<QString> file_path;
int numberCore = 0;
int numberFile = 0;




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}



/********************************************************************************
 * Name : create_vector                                                         *
 *                                                                              *
 * Input variable: QVector<QString> *file_name                                  *
 *                 QVector<QVector<double>> *colonne0                           *
 *                 QVector<QVector<double>> *colonne1                           *
 *                 QVector<QVector<double>> *colonne2                           *
 *                                                                              *
 * Output variable: -                                                           *
 *                                                                              *
 * Summary of function: This function allows you to read text files that you    *
 *                      have previously selected. While reading the files,      *
 *                      it adds each value to its corresponding vector.         *
 ********************************************************************************/
void MainWindow::create_vector(QVector<QString> *file_name, QVector<QVector<double>> *colonne0,
                               QVector<QVector<double>> *colonne1, QVector<QVector<double>> *colonne2)
{
    QVector<double> index;
    QVector<double> instr;
    QVector<double> cache_mises;
    bool ok;
    for (int j = 0; j < file_name->size(); j++)
    {
        QFile file(file_name->at(j));
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::information(0, "error", file.errorString());
        }

        QTextStream in(&file);

        instr.clear();
        cache_mises.clear();
        index.clear();

        int i = 0;
        while(!in.atEnd())
        {
            QString line = in.readLine();
            QStringList words = line.split("\t");
            foreach(QString word, words)
            {
                if (i == 0)
                {
                    index.append(word.toDouble(&ok));
                    i++;
                }
                else if (i == 1)
                {
                    instr.append(word.toDouble(&ok));
                    i++;
                }
                else if (i == 2)
                {
                    cache_mises.append(word.toDouble(&ok));
                    i = 0;
                }
            }
        }
        colonne0->append(index);
        colonne1->append(instr);
        colonne2->append(cache_mises);
        file.close();
    }
}



/********************************************************************************
 * Name : find_segmentation                                                     *
 *                                                                              *
 * Input variable: QVector<QVector<double>> *tb                                 *
 *                 QVector<QVector<double>> *segm                               *
 *                                                                              *
 * Output variable: -                                                           *
 *                                                                              *
 * Summary of function: This function allows to find the segmentations from     *
 *                      the global average of a vector. To do this, it takes    *
 *                      each vector, calculates its average and as soon as a    *
 *                      value is less than 80% of the average, it creates a     *
 *                      segmentation.                                           *
 ********************************************************************************/
void MainWindow::find_segmentation(QVector<QVector<double>> *tb, QVector<QVector<double>> *segm)
{
    QVector<double> segmentation;
    QVector<double> colonne;
    for (int j = 0; j < tb->size(); j++)
    {
        colonne = tb->at(j);

        double avg = AVG(&colonne);
        int i;
        avg *= 0.8;

        for (i = 0; i < colonne.size(); i++)
        {
            if (colonne.at(i) < avg)
            {
                segmentation.append(i);
            }

        }
        colonne.clear();
        segmentation.append(i);
        segm->append(segmentation);
        segmentation.clear();
    }

}



/****************************************************************
 * Name : AVG                                                   *
 *                                                              *
 * Input variable: QVector<QVector<double>> *tb                 *
 *                                                              *
 * Output variable: double average                              *
 *                                                              *
 * Summary of function: This function calculates the average of *
 *                      a vector by summing each of its values  *
 *                      and dividing them by their number       *
 ****************************************************************/
double MainWindow::AVG(QVector<double> *tb)
{
    double sum = 0;
    int i;


    for (i = 1; i < tb->size(); i++)
    {
        sum = sum + tb->at(i);

    }

    return sum/i;
}



/********************************************************************
 * Name : DoubleToInt                                               *
 *                                                                  *
 * Input variable: double a                                         *
 *                                                                  *
 * Output variable: int b                                           *
 *                                                                  *
 * Summary of function: This function converts a double to an int.  *
 ********************************************************************/
int MainWindow::DoubleToInt(double a)
{
    int b = a;
    int reste = (a-b)*10;


    if (reste >= 5)
        return (b+1);
    else
        return b;
}



/********************************************************************************************
 * Name : correction_segm                                                                   *
 *                                                                                          *
 * Input variable: -                                                                        *
 *                                                                                          *
 * Output variable: -                                                                       *
 *                                                                                          *
 * Summary of function: This function checks if the values of the segmentations are not     *
 *                      too close on 2 consecutive values ( < 10). If this is the case,     *
 *                      then the lower segmentation is deleted.                             *
 *                      Furthermore, if the difference between each segmentation is too     *
 *                      great ( > 20) then it creates a new segmentation between the two    *
 *                      at +15 compared to the first one.                                   *
 ********************************************************************************************/
void MainWindow::correction_segm()
{
    for (int t = 0; t < segm.size(); t++)
    {
        for (int i = 0; i < segm[t].size()-2; i++)
        {
            if ((segm[t].at(i+1)-segm[t].at(i)) < 10)
            {
                if ((segm[t].at(i+2)-segm[t].at(i+1)) < 10)
                {
                    segm[t].remove(i+1);
                }
                segm[t].remove(i);
                i--;
            }
        }
    }

    for (int t = 0; t < segm.size(); t++)
    {
        for (int i = 0; i < segm[t].size()-1; i++)
        {
            if ((segm[t].at(i+1) - segm[t].at(i)) > 20)
                segm[t].insert(i+1, (segm[t].at(i)+15));
        }
    }
}



/********************************************************************
 * Name : avg_by_seg                                                *
 *                                                                  *
 * Input variable: QVector<double> *segm                            *
 *                 QVector<double> *colonne2                        *
 *                                                                  *
 * Output variable: QVector<double>                                 *
 *                                                                  *
 * Summary of function: This function calculates the average of     *
 *                      the vector between each segmentation of it. *
 ********************************************************************/
QVector<double> MainWindow::avg_by_seg(QVector<double> *segm, QVector<double> *colonne2)
{
    double sum;
    QVector<double> avg_tb;
    avg_tb.clear();
    int seg = 0;

    for(int i = 0; i < segm->size(); i++)
    {
        sum = 0;
        for (int l = seg; l < segm->at(i); l++)
        {
            sum += colonne2->at(l);
        }
        seg = segm->at(i);
        avg_tb.append(sum);
    }
    return avg_tb;
}



/********************************************************************************
 * Name : creation_table_to_comparison                                          *
 *                                                                              *
 * Input variable: QVector<double> *segm                                        *
 *                 QVector<double> *segmentation                                *
 *                                                                              *
 * Output variable: -                                                           *
 *                                                                              *
 * Summary of function: This function creates a new vector of all the combined  *
 *                      segmentations of the different selected text files      *
 ********************************************************************************/
void MainWindow::creation_table_to_comparison(QVector<QVector<double>> *segm, QVector<double> *segmentation)
{
    for(int i = 0; i < segm->size(); i++)
    {
        for (int j = 0; j < segm[i].size(); j++)
        {
            segmentation->append(segm[i].at(j));
        }
    }
}



/********************************************************************************
 * Name : tri_and_double                                                        *
 *                                                                              *
 * Input variable: QVector<double> *segmentation                                *
 *                                                                              *
 * Output variable: -                                                           *
 *                                                                              *
 * Summary of function: This function sorts the vector and removes duplicates   *
 ********************************************************************************/
void MainWindow::tri_and_double(QVector<double> *segmentation)
{
    int i,j,k;
    double c;
    QVector<double> essaie;

    for (int i = 0; i < segmentation->size(); i++)
        essaie.append(segmentation->at(i));



    for(i=1;i<essaie.size();i++)
    {
        if ( essaie.at(i) < essaie.at(i-1) )
        {
            j = 0;
            while ( essaie.at(j) < essaie.at(i) )
            {
                j++;
            }
            c = essaie.at(i);
            for( k = i-1 ; k >= j ; k-- )
            {
                essaie[k+1] = essaie[k];
            }
            essaie[j] = c;
        }
    }

    segmentation->clear();
    for (int i = 0; i < essaie.size(); i++)
        segmentation->append(essaie.at(i));



    for(int i = 0; i < (segmentation->size()-1); i++)
    {
        if (segmentation->at(i) == segmentation->at(i+1))
        {
            segmentation->remove(i);
            i--;
        }
    }

}



/********************************************************************************
 * Name : plot                                                                  *
 *                                                                              *
 * Input variable: QVector<double> colonne0                                     *
 *                 QVector<double> colonne2                                     *
 *                 QString file_name                                            *
 *                                                                              *
 * Output variable: -                                                           *
 *                                                                              *
 * Summary of function: This function creates an array that displays as         *
 *                      many graphs as we select files.                         *
 *                      After being created, this function displays the graphs  *
 *                      according to the values read in the files (here we just *
 *                      take the index and the caches put)                      *
 ********************************************************************************/
void MainWindow::plot(QVector<double> colonne0, QVector<double> colonne2, QString file_name)
{
    ui->tableWidget->insertRow(0);
    ui->tableWidget->setColumnCount(1);
    ui->tableWidget->setShowGrid(true);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "plot" );
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setRowHeight(0, 220);

    QCustomPlot *customPlot1 = new QCustomPlot(QCUSTOMPLOT_H);

    customPlot1->clearGraphs();

    // create graph and assign data to it:
    customPlot1->addGraph();

    customPlot1->graph(0)->setData(colonne0, colonne2);

    // let the ranges scale themselves so graph 0 fits perfectly in the visible area:
    customPlot1->graph(0)->rescaleAxes();

    // give the axes some labels
    customPlot1->xAxis->setLabel("Time");
    customPlot1->yAxis->setLabel("Cache misses");

    //legend
    QFont legendFont = font();  // start out with MainWindow's font..
    customPlot1->legend->setVisible(true);
    customPlot1->legend->setFont(legendFont);

    customPlot1->graph(0)->setLineStyle(QCPGraph::lsNone);
    customPlot1->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlus, 3));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    customPlot1->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

    // add two new graphs and set their look:
    customPlot1->graph(0)->setPen(QPen(Qt::red)); // line color blue for first graph
    customPlot1->graph(0)->setName("Caches misses " + file_name.section("/",-1,-1));

    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    customPlot1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    customPlot1->replot();

    ui->tableWidget->setCellWidget(0, 0, customPlot1);
}



/********************************************************************************
 * Name : create_first_row                                                      *
 *                                                                              *
 * Input variable: int column                                                   *
 *                 double value                                                 *
 *                                                                              *
 * Output variable: -                                                           *
 *                                                                              *
 * Summary of function: This function writes the double value passed            *
 *                      to it according to the column given on the first line   *
 ********************************************************************************/
void MainWindow::create_first_row(int column, double value)
{
    QLabel *in_case = new QLabel();
    in_case->setText(QString::number(value));
    ui->partition->setCellWidget(0, column, in_case);
}



/*******************************************************************************
 * Name : create_first_column                                                  *
 *                                                                             *
 * Input variable: int row                                                     *
 *                 QString name                                                *
 *                                                                             *
 * Output variable: -                                                          *
 *                                                                             *
 * Summary of function: This function writes the string that is passed         *
 *                      to it according to the given row in the first column   *
 *******************************************************************************/
void MainWindow::create_first_column(int row, QString name)
{
    QLabel *in_case = new QLabel();
    in_case->setText(name);
    ui->partition->setCellWidget(row, 0, in_case);
}



/********************************************************************
 * Name : put_value_table_partition                                 *
 *                                                                  *
 * Input variable: double value                                     *
 *                 int row                                          *
 *                 int column                                       *
 *                                                                  *
 * Output variable: -                                               *
 *                                                                  *
 * Summary of function: This function writes the double given to it *
 *                      according to the given column and row also  *
 ********************************************************************/
void MainWindow::put_value_table_partition(double value, int row, int column)
{
    QLabel *in_case = new QLabel();
    in_case->setText(QString::number(value));
    ui->partition->setCellWidget(row, column, in_case);
}



/************************************************************
 * Name : create_base_table_partition                       *
 *                                                          *
 * Input variable: -                                        *
 *                                                          *
 * Output variable: -                                       *
 *                                                          *
 * Summary of function: This function creates the basis     *
 *                      of the table for the partitions     *
 ************************************************************/
void MainWindow::create_base_table_partition()
{
    ui->partition->setRowCount(segm.size()+1);
    ui->partition->setColumnCount(segmentation.size()+1);
    ui->partition->setShowGrid(true);
    ui->partition->horizontalHeader()->setVisible(false);
    ui->partition->verticalHeader()->setVisible(false);
    ui->partition->setColumnWidth(0, 200);
}



/************************************************************************************
 * Name : create_segmentation_and_avg_total                                         *
 *                                                                                  *
 * Input variable: -                                                                *
 *                                                                                  *
 * Output variable: -                                                               *
 *                                                                                  *
 * Summary of function: This function creates a new vector of all the combined      *
 *                      segmentations of the different selected text files and      *
 *                      sorts them.                                                 *
 *                      In addition, it creates the new average vectors according   *
 *                      to the segmentations of each one which it puts in a single  *
 *                      vector                                                      *
 ************************************************************************************/
void MainWindow::create_segmentation_and_avg_total()
{
    for(int i = 0; i < segm.size(); i++)
    {
        for (int j = 0; j < segm[i].size(); j++)
        {
            segmentation.append(segm[i].at(j));
        }
    }

    for (int i = 0; i < segm.size(); i++)
    {
        avg.append(avg_by_seg(&segm[i], &colonne2[i]));
    }


    tri_and_double(&segmentation);
}



/********************************************************************************
 * Name : create_partition                                                      *
 *                                                                              *
 * Input variable: -                                                            *
 *                                                                              *
 * Output variable: -                                                           *
 *                                                                              *
 * Summary of function: This function creates the partitions according to each  *
 *                      segmentations and each average corresponding to these   *
 *                      segmentations.                                          *
 *                      Then, it adds these partitions into different vectors,  *
 *                      which are added into a vector of vectors.               *
 ********************************************************************************/
void MainWindow::create_partition()
{
    QVector<int> variable;
    QVector<int> part_tb;
    double sum;
    QVector<double> init;
    int l = 0;

    for (int i = 0; i < segm.size(); i++)
    {
        variable.append(0);
    }

    for (int i = 0; i < segmentation.size(); i++)
    {
        sum = 0;
        for (int j = 0; j < segm.size(); j++)
        {
            if(variable.at(j) == -1)
            {}
            else
                sum += avg[j].at(variable.at(j));
        }

        for(int k = 0; k < segm.size(); k++)
        {
            if (variable.at(k) == -1)
                part_tb.append(0);
            else if (avg[k].at(variable.at(k)) == 0)
                part_tb.append(0);
            else
                part_tb.append(DoubleToInt(16*(avg[k].at(variable.at(k))/sum)));
        }

        for(int row = 0; row < segm.size(); row++)
        {
            for(int colunm = 0; colunm < segm[row].size(); colunm++)
            {
                if (segm[row][colunm] == segmentation[i])
                {

                    if (variable[row] == (avg[row].size()-1))
                        variable[row] = -1;
                    else
                        variable[row] += 1;
                }
            }
        }

    }

    init.append(0);
    for (int i = 0; i < segm.size(); i++)
    {
        part.append(init);
    }

    for (int i = 0; i < (part_tb.size()/segm.size()); i++)
    {
        for(int j = 0; j < segm.size(); j++)
        {
            part[j].append(part_tb.at(i+j+l));
        }
        l += (segm.size()-1);
    }

    for (int i = 0; i < part.size(); i++)
    {
        part[i].remove(0);
    }
}



/********************************************************************************
 * Name : create_table_partition                                                *
 *                                                                              *
 * Input variable: -                                                            *
 *                                                                              *
 * Output variable: -                                                           *
 *                                                                              *
 * Summary of function: This function creates the partition table by entering   *
 *                      the file names in the first column, then the values of  *
 *                      each partition according to the files.                  *
 ********************************************************************************/
void MainWindow::create_table_partition()
{
    create_base_table_partition();

    QLabel *title = new QLabel();
    title->setText("Segmentation");
    ui->partition->setCellWidget(0, 0, title);

    for(int i = 1; i < (file_path.size()+1); i++)
    {
        create_first_column(i, file_path[i-1].section("/",-1,-1));
    }

    for(int column = 1; column < (segmentation.size()+1); column++)
    {

        create_first_row(column, segmentation.at(column-1));
    }

    for(int row = 1; row < (part.size()+1); row++)
    {
        for(int column = 1; column < (part[row-1].size()+1); column++)
        {
            put_value_table_partition(part[row-1].at(column-1), row, column);
        }
    }

}



/****************************************************************
 * Name : on_btn_select_file_clicked                            *
 *                                                              *
 * Input variable: A button called "OPEN" is connected          *
 *                 to this slot via a signal*                   *
 *                                                              *
 * Output variable: -                                           *
 *                                                              *
 * Summary of function: This button allows you to               *
 *                      retreve the paths to the various files. *
 ****************************************************************/
void MainWindow::on_btn_select_file_clicked()
{
    if (numberCore < QThread::idealThreadCount())
    {
        file_name = QFileDialog::getOpenFileName(this, "Open files", "/home/mdh/Desktop/Summerwork2022/new_small_tests");
        file_path.append(file_name);
        numberFile++;
        numberCore++;
        ui->lineEdit->setText(QString::number(numberFile));
    }
    else
    {
        QMessageBox::warning(this, "Alert", "You can't add more plots because your core count is " + QString::number(QThread::idealThreadCount(), 10));
    }
}



/****************************************************************
 * Name : on_btn_select_file_clicked                            *
 *                                                              *
 * Input variable: A button called "CALCULATE" is connected     *
 *                 to this slot via a signal                    *
 *                                                              *
 * Output variable: -                                           *
 *                                                              *
 * Summary of function: This button starts the calculation      *
 *                      of the partitions of the selected       *
 *                      files and their displays.               *
 ****************************************************************/
void MainWindow::on_pushButton_clicked()
{
    create_vector(&file_path, &colonne0, &colonne1, &colonne2);
    find_segmentation(&colonne2, &segm);
    correction_segm();
    create_segmentation_and_avg_total();


    create_partition();

    create_table_partition();

    for (int i = 0; i < segm.size(); i++)
    {
        plot(colonne0[i], colonne2[i], file_path[i]);
    }
}



/********************************************************************
 * Name : on_btn_select_file_clicked                                *
 *                                                                  *
 * Input variable: A button called "CLEAR" is connected             *
 *                 to this slot via a signal                        *
 *                                                                  *
 * Output variable: -                                               *
 *                                                                  *
 * Summary of function: This button clears all global variables     *
 *                      so that the entire program can be           *
 *                      restarted without restarting the software.  *               
 ********************************************************************/
void MainWindow::on_clear_clicked()
{
    colonne0.clear();
    colonne1.clear();
    colonne2.clear();
    segm.clear();
    avg.clear();
    part.clear();
    file_path.clear();
    numberCore = 0;
    numberFile = 0;
    ui->tableWidget->clear();
    ui->lineEdit->setText(QString::number(numberFile));
}

