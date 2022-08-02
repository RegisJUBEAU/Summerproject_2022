#include "mainwindow.h"
#include "ui_mainwindow.h"


QVector<int> colonne0;
QVector<int> colonne2;
QVector<int> SEGMENTATION2_L1;
QVector<int> AVERAGE2_L1;
QVector<int> PARTITION;
QVector<int> PARTITION2_L1;

QString file_name;






MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Item for choose a processor
    ui->comboBox_processor->addItem("");
    ui->comboBox_processor->addItem("ARM");
    ui->comboBox_processor->addItem("INTEL");

    //Item for choose the unit to the size
    ui->comboBox_unit->addItem("");
    ui->comboBox_unit->addItem("B");
    ui->comboBox_unit->addItem("kB");
    ui->comboBox_unit->addItem("MB");

    //Item for choose the unit to the size
    ui->comboBox_unit_part->addItem("");
    ui->comboBox_unit_part->addItem("B");
    ui->comboBox_unit_part->addItem("kB");
    ui->comboBox_unit_part->addItem("MB");

    //For my plot
    ui->customPlot->addGraph();
    ui->customPlot->addGraph();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::create_table(QString file_name, QVector<int> *colonne0, QVector<int> *colonne2)
{
    bool ok;

    QFile file(file_name);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream in(&file);
    int i = 0;
    while(!in.atEnd())
    {
        QString line = in.readLine();
        QStringList words = line.split("\t");
        foreach(QString word, words)
        {
            if (i == 0)
            {
                colonne0->append(word.toInt(&ok));
                i++;
            }
            else if (i == 1)
            {
                i++;
            }
            else if (i == 2)
            {
                colonne2->append(word.toFloat(&ok));
                i = 0;
            }
        }
    }
    file.close();
}

void MainWindow::find_segmentation(QVector<int> *tb, QVector<int> *segm)
{
    int avg = AVG(tb);
    int i;
    avg *= 0.8;

    for (i = 0; i < tb->size(); i++)
    {
        if (tb->at(i) < avg)
        {
            segm->append(i);
        }

    }
    segm->append(i);
}

int MainWindow::AVG(QVector<int> *tb)
{
    long int sum = 0;
    int i;


    for (i = 1; i < tb->size(); i++)
    {
        sum = sum + tb->at(i);

    }

    return sum/i;
}

void MainWindow::correction(QVector<int> *segm)
{
    for (int i = 0; i < segm->size()-2; i++)
    {
        if ((segm->at(i+1) - segm->at(i)) < 10)
        {
            if ((segm->at(i+2) - segm->at(i+1)) < 10)
            {
                segm->remove(i+1);
            }
            segm->remove(i);
            i--;
        }
    }
}


void MainWindow::avg_by_segmentation(QVector<int> *tb, QVector<int> *segm, QVector<int> *avg_tb)
{
    long int sum=0;
    int seg = 0;
    int size = segm->size();

    for (int n = 0; n < size; n++)
    {
        for (int l = seg; l < segm->at(n); l++)
        {
            sum += tb->at(l);

        }

        if (n == 0)
        {
            avg_tb->append(sum/segm->at(n));
        }
        else
        {
            avg_tb->append(sum/(segm->at(n)-segm->at(n-1)));
        }

        sum = 0;
        seg = segm->at(n);

    }
}

void MainWindow::partition_v2(QVector<int> *AVG, QVector<int> *PART, QVector<int> *tb_s, long nb_part)
{
    long int max = MAX(AVG);

    for (int i = 0; i < nb_part+1; i++)
    {
        tb_s->append(i * (max / nb_part));
    }

    for (int i = 0; i < AVG->size(); i++)
    {
        PART->append(tb_s->at(AVG->at(i)*nb_part/max));
    }
}

int MainWindow::MAX(QVector<int> *tb)
{
    int max = tb->at(0);

    for (int i = 1; i < tb->size(); i++)
    {
        if (tb->at(i) > max)
        {
            max = tb->at(i);
        }
    }
    return max;
}

void MainWindow::write_txt_part(QString Chemin_Fichier, QVector<int> *tb, QVector<int> *SEG, QVector<int> *AVG, QVector<int> *PART)
{
    QFile file(Chemin_Fichier);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream out(&file);

    int sg = 0;
    int b;
    int j = 0;
    int v = -1;
    do
    {
        v++;
        for (b = sg; b < SEG->at(v); b++)
        {
            out << tb->at(b) << "\t" << AVG->at(j) << "\t" << PART->at(j) << "\n";
        }
        j++;
        sg = SEG->at(v);
    }while (sg != tb->size());

    file.close();
}

void MainWindow::write_txt(QString Chemin_Fichier, QVector<int> *tb, QVector<int> *SEG, QVector<int> *AVG)
{
    QFile file(Chemin_Fichier);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(0, "error", file.errorString());
    }

    QTextStream out(&file);

    int sg = 0;
    int b;
    int j = 0;
    int v = -1;
    do
    {
        v += 1;
        for (b = sg; b < SEG->at(v); b++)
        {
            out << tb->at(b) << "\t" << AVG->at(j) << "\n";
        }
        j++;
        sg = SEG->at(v);
    }while (sg != tb->size());

    file.close();
}

int MainWindow::MIN(QVector<int> *tb)
{
    int min = tb->at(0);

    for (int i = 1; i < tb->size(); i++)
    {
        if (tb->at(i) < min)
        {
            min = tb->at(i);
        }
    }
    return min;
}

void MainWindow::plot(QVector<int> *x_, QVector<int> *y1_, QVector<int> *y2_, QVector<int> *SEG_)
{
    //legend
    QFont legendFont = font();  // start out with MainWindow's font..
    ui->customPlot->legend->setVisible(true);
    ui->customPlot->legend->setFont(legendFont);
    ui->customPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

    // add two new graphs and set their look:
    ui->customPlot->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
    ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlus, 1));
    ui->customPlot->graph(0)->setName("Partition of caches mises");
    ui->customPlot->graph(1)->setPen(QPen(Qt::red)); // line color red for second graph
    ui->customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
    ui->customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlus, 1));
    ui->customPlot->graph(1)->setName("Caches mises");
    // generate some points of data (y0 for first, y1 for second graph):
    QVector<double> x(x_->size()), y1(x_->size()), y2(x_->size()); // initialize with entries 0..100

    int sg = 0;
    int b;
    int j = 0;
    int v = -1;
    do
    {
        v++;
        for (b = sg; b < SEG_->at(v); b++)
        {
            x[b] = x_->at(b); // x goes from -1 to 1
            y1[b] = y1_->at(b);
            y2[b] = y2_->at(j); // let's plot a quadratic function
        }
        j++;
        sg = SEG_->at(v);
    }while (sg != x_->size());

    // configure right and top axis to show ticks but no labels:
    // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
    ui->customPlot->xAxis2->setVisible(true);
    ui->customPlot->xAxis2->setTickLabels(false);
    ui->customPlot->yAxis2->setVisible(true);
    ui->customPlot->yAxis2->setTickLabels(false);
    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));
    // pass data points to graphs:
    ui->customPlot->graph(0)->setData(x, y1);
    ui->customPlot->graph(1)->setData(x, y2);
    // let the ranges scale themselves so graph 0 fits perfectly in the visible area:
    ui->customPlot->graph(0)->rescaleAxes();
    // same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller than graph 0):
    ui->customPlot->graph(1)->rescaleAxes(true);
    // Note: we could have also just called customPlot->rescaleAxes(); instead
    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->customPlot->replot();
}

void MainWindow::correction_v2(QVector<int> *segm)
{
    for (int i = 0; i < segm->size()-1; i++)
    {
        if ((segm->at(i+1) - segm->at(i)) > 50)
        {
            segm->insert(i+1, (segm->at(i)+50));
        }
    }
}

void MainWindow::on_pushButton_select_file_clicked()
{
    if (!colonne0.isEmpty())
        colonne0.clear();
    if (!colonne2.isEmpty())
        colonne2.clear();

    file_name = QFileDialog::getOpenFileName(this, "Open a file", "/home");
    create_table(file_name, &colonne0, &colonne2);
    qDebug() << file_name << "\n";
}


void MainWindow::on_pushButton_calculate_clicked()
{
    if ((ui->comboBox_unit->currentText() == "")
            || (ui->comboBox_unit_part->currentText() == ""))
    {
        QMessageBox::warning(this, "title", "You did not complete everything");
    }
    else
    {
        SEGMENTATION2_L1.clear();
        AVERAGE2_L1.clear();
        PARTITION2_L1.clear();
        PARTITION.clear();

        find_segmentation(&colonne2, &SEGMENTATION2_L1);    //I look for values below 98.5% of my GPA to get my segmentations
        correction(&SEGMENTATION2_L1);                      //I check that my segmentations are not too close (about 20ms)
        correction_v2(&SEGMENTATION2_L1);
        avg_by_segmentation(&colonne2, &SEGMENTATION2_L1, &AVERAGE2_L1); //I take the average between each segmentation


        QString size = ui->lineEdit_size->text();
        QString size_part = ui->lineEdit_size_part->text();
        int resultat;
        bool ok;

        if (ui->comboBox_unit->currentText() == "kB")
        {
            size += "000";
        }
        else if (ui->comboBox_unit->currentText() == "MB")
        {
            size += "000000";
        }

        if (ui->comboBox_unit_part->currentText() == "kB")
        {
            size_part += "000";
        }
        else if (ui->comboBox_unit_part->currentText() == "MB")
        {
            size_part += "000000";
        }

        resultat = (size.toInt(&ok,10)/size_part.toInt(&ok,10));
        ui->label_part->setText(QString::number(resultat));

        partition_v2(&AVERAGE2_L1, &PARTITION2_L1, &PARTITION, resultat);


        plot(&colonne0, &colonne2, &PARTITION2_L1, &SEGMENTATION2_L1);

    }
}
