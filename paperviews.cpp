#include <paperviews.h>

#define LINE 0
#define ARROW 1
#define CIRCLE 2
#define RECTANGLE 3
#define FLATTEXT 4
#define POPUPTEXT 5
#define SPACING 5

MainScene::MainScene()
{
    document = new AutoDocument;
    QThread *thread = new QThread(this);
    document->moveToThread(thread);
    thread->start();
    QDesktopWidget *mydesk = QApplication::desktop();
    xres = mydesk->physicalDpiX();
    yres = mydesk->physicalDpiY();
    connect(document, &AutoDocument::imageCompleted, this, &MainScene::updateScene);
}

void MainScene::loadFile(const QString &addr)
{
    this->filename = addr;
    document->document = Poppler::Document::load(addr);
    if (!document || document->document->isLocked()) {
        delete document;
        return;
    }

    for(int i=0; i<document->document->numPages(); i++)
    {
        indexes.append(i);
    }

    document->document->setRenderHint(Poppler::Document::TextAntialiasing, 1);
    document->document->setRenderHint(Poppler::Document::Antialiasing, 1);

    /* set hidden */
    for(int pageidx=0; pageidx<document->document->numPages(); pageidx++)
    {
        for(int i=0;i<document->document->page(pageidx)->annotations().length();i++)
        {
            int type = document->document->page(pageidx)->annotations().at(i)->subType();
            if(type==1 || type==2 || type==3 || type==6)
            {
                document->document->page(pageidx)->annotations().at(i)->setFlags(Poppler::Annotation::Hidden);
            }
        }
    }

    QProgressDialog *progressDlg=new QProgressDialog(nullptr);
    progressDlg->setWindowModality(Qt::WindowModal);
    progressDlg->setMinimumDuration(5);
    progressDlg->setWindowTitle(tr("载入进度"));
    progressDlg->setLabelText(tr("正在加载"));
    progressDlg->setRange(0, document->document->numPages());
    QPushButton *button = new QPushButton(tr("取消"));
    button->setEnabled(false);
    progressDlg->setCancelButton(button);

    for(int i=0; i<document->document->numPages(); i++)
    {
        QImage *image = new QImage(document->document->page(i)->renderToImage(xres, yres, -1, -1, -1, -1));
        if(i==0)
        {
            width = image->width();
            height = image->height()*document->document->numPages();
        }
        document->images[i] = image;
        QImage *tmptmp = *(&(document->images[0]));
        pages.append(new PaperItem(i, (long)(&(document->images[0]))));
        pages.at(i)->setPos(0, image->height()*i+SPACING*i);
        addItem(pages.at(i));
        progressDlg->setValue(i+1);
    }

    setSceneRect(0, 0, width, height);
    document->startTimer();

    for(int pageidx=0; pageidx<document->document->numPages(); pageidx++)
    {
        for(int i=0;i<document->document->page(pageidx)->annotations().length();i++)
        {
            switch(document->document->page(pageidx)->annotations().at(i)->subType())
            {
            case 1:  // Text Annotation
            {
                Poppler::TextAnnotation *annotation = (Poppler::TextAnnotation *)(document->document->page(pageidx)->annotations().at(i));
                annotation->setFlags(Poppler::Annotation::Hidden);
                if(annotation->textType()==1)
                {
                    annotations.append(new PaperAnnotation::FlatTextAnnotation(pageidx, annotation, width, height/document->document->numPages(), scale));
                    this->addItem(annotations.at(annotations.length()-1));
                    connect(((PaperAnnotation::FlatTextAnnotation *)(annotations.last())),
                            &PaperAnnotation::FlatTextAnnotation::posChanged,
                            this, &MainScene::send_status_changed);
                    // 添加删除接口
                    connect((PaperAnnotation::FlatTextAnnotation *)(annotations.last()), &PaperAnnotation::FlatTextAnnotation::deleteSelf,
                            this, &MainScene::removeCertainItem);
                    //                    this->removeItem(tmprectitem);
                }
                else {
                    annotations.append(new PaperAnnotation::PopupTextAnnotation(pageidx, annotation, width, height/document->document->numPages(), scale));
                    this->addItem(annotations.at(annotations.length()-1));
                    connect((PaperAnnotation::PopupTextAnnotation *)(annotations.last()), &PaperAnnotation::PopupTextAnnotation::deleteSelf,
                            this, &MainScene::removeCertainItem);
                    //annotations.append(new PaperAnnotation::PreviewAnnotation(pageidx, annotation, width, height/document->document->numPages(), scale));
                    //this->addItem(annotations.last());
                }
                break;
            }
            case 3:  // Geom Annotation
            {
                Poppler::GeomAnnotation *annotation = (Poppler::GeomAnnotation *)(document->document->page(pageidx)->annotations().at(i));
                annotation->setFlags(Poppler::Annotation::Hidden);
                annotations.append(new PaperAnnotation::GeomAnnotation(pageidx, annotation, width, height/document->document->numPages(), scale));
                this->addItem(annotations.at(annotations.length()-1));
                connect(((PaperAnnotation::GeomAnnotation *)(annotations.last())),
                        &PaperAnnotation::GeomAnnotation::posChanged,
                        this, &MainScene::send_status_changed);
                // 添加删除接口
                connect((PaperAnnotation::GeomAnnotation *)(annotations.last()), &PaperAnnotation::GeomAnnotation::deleteSelf,
                        this, &MainScene::removeCertainItem);
                break;
            }
            case 2:  // LineAnnotation
            {
                Poppler::LineAnnotation *annotation = (Poppler::LineAnnotation *)(document->document->page(pageidx)->annotations().at(i));
                annotation->setFlags(Poppler::Annotation::Hidden);
                annotations.append(new PaperAnnotation::LineAnnotation(pageidx, annotation, width, height/document->document->numPages(), scale));
                this->addItem(annotations.at(annotations.length()-1));
                connect(((PaperAnnotation::LineAnnotation *)(annotations.last())),
                        &PaperAnnotation::LineAnnotation::posChanged,
                        this, &MainScene::send_status_changed);
                // 添加删除接口
                connect((PaperAnnotation::LineAnnotation *)(annotations.last()), &PaperAnnotation::LineAnnotation::deleteSelf,
                        this, &MainScene::removeCertainItem);

                QLinkedListIterator<QPointF> rwIterator(annotation->linePoints());
                startPoint = rwIterator.next();
                rwIterator.toBack();
                endPoint = rwIterator.previous();
                break;
            }
            case 6:  // Ink Annotation
            {
                Poppler::InkAnnotation *annotation = (Poppler::InkAnnotation *)(document->document->page(pageidx)->annotations().at(i));
                annotation->setFlags(Poppler::Annotation::Hidden);
                annotations.append(new PaperAnnotation::InkAnnotation(pageidx, annotation, width, height/document->document->numPages(), scale));
                this->addItem(annotations.at(annotations.length()-1));
                break;
            }
            }
        }
    }
}

void MainScene::updateSize()
{

}

void MainScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(isPressing)
    {
        event->accept();
        switch(shape)
        {
        case LINE:
        {
            endPoint = event->scenePos();
            tmplineitem->setLine(QLineF(startPoint-startPoint, endPoint-startPoint));
            tmplineitem->update();
            tmplineitem->show();
            break;
        }
        case CIRCLE:
        {
            endPoint = event->scenePos();
            tmpellipseitem->setRect(0, 0, abs(startPoint.x()-endPoint.x()), abs(startPoint.y()-endPoint.y()));
            tmpellipseitem->update();
            tmpellipseitem->show();
            break;
        }
        case RECTANGLE:
        {
            endPoint = event->scenePos();
            tmprectitem->setRect(0, 0, abs(startPoint.x()-endPoint.x()), abs(startPoint.y()-endPoint.y()));
            tmprectitem->update();
            tmprectitem->show();
            break;
        }
        default:
            break;
        }
    }
    else
    {
        QGraphicsScene::mouseMoveEvent(event);
    }
}

void MainScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(isDrawing && event->button() == Qt::LeftButton)
    {
        isPressing = true;
        event->accept();
        QPen pen;
        pen.setColor(QColor(0, 160, 230));
        pen.setWidth(2);
        startPoint = event->scenePos();
        endPoint = event->scenePos();
        switch(shape)
        {
        case LINE:
        {
            tmplineitem = new QGraphicsLineItem;
            tmplineitem->setPen(pen);
            tmplineitem->setLine(QLineF(startPoint, endPoint));
            tmplineitem->setPos(event->scenePos());
            addItem(tmplineitem);
            break;
        }
        case FLATTEXT:
        {
            QFont font;
            font.setPointSize(10);
            AnnotationDialog::FlatTextDialog *dialog = new AnnotationDialog::FlatTextDialog("", font, Qt::red);
            connect(dialog, &AnnotationDialog::FlatTextDialog::configUpdated, this, &MainScene::newFlatText);
            dialog->exec();
            isPressing = false;
            break;
        }
        case POPUPTEXT:
        {
            QFont font;
            font.setPointSize(10);
            AnnotationDialog::FlatTextDialog *dialog = new AnnotationDialog::FlatTextDialog("", font, Qt::red);
            connect(dialog, &AnnotationDialog::FlatTextDialog::configUpdated, this, &MainScene::newPopupText);
            dialog->exec();
            isPressing = false;
            break;
        }
        case CIRCLE:
        {
            tmpellipseitem = new QGraphicsEllipseItem;
            tmpellipseitem->setPen(pen);
            tmpellipseitem->setPos(event->scenePos());
            addItem(tmpellipseitem);
            break;
        }
        case RECTANGLE:
        {
            tmprectitem = new QGraphicsRectItem;
            tmprectitem->setPen(pen);
            tmprectitem->setPos(event->scenePos());
            addItem(tmprectitem);
            break;
        }
        default:
        {
            tmplineitem = new QGraphicsLineItem;
            startPoint = event->scenePos();
            endPoint = event->scenePos();
            tmplineitem->setPen(pen);
            tmplineitem->setLine(QLineF(startPoint, endPoint));
            tmplineitem->setPos(event->scenePos());
            addItem(tmplineitem);
            break;
        }
        }
    }
    else
    {
        QGraphicsScene::mousePressEvent(event);
    }
}

void MainScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(isPressing)
    {
        event->accept();
        isPressing = false;
        endPoint = event->scenePos();
        int imageheight = height/document->document->numPages();
        switch(shape)
        {
        case LINE:
        {
            int x1 = startPoint.x();
            int y1 = startPoint.y();
            int x2 = endPoint.x();
            int y2 = endPoint.y();

            int index = (int)(tmplineitem->pos().y()/imageheight);
            Poppler::LineAnnotation *lineannotation = new Poppler::LineAnnotation(Poppler::LineAnnotation::StraightLine);
//            QRectF boundary = QRectF(tmplineitem->pos().x()/width,
//                                     (((int)(tmplineitem->pos().y()))%(int)imageheight)/(double)imageheight,
//                                     tmplineitem->boundingRect().width()/width,
//                                     tmplineitem->boundingRect().height()/imageheight);
            QRectF boundary = QRectF(fmin(x1,x2)/width,
                                     ((int)fmin(y1, y2)%imageheight)/(double)imageheight,
                                     abs(x1-x2)/(double)width,
                                     abs(y1-y2)/(double)imageheight);
            lineannotation->setBoundary(boundary);
            Poppler::Annotation::Style style = Poppler::Annotation::Style(lineannotation->style());
            style.setWidth(2);
            lineannotation->setStyle(style);
            QLinkedList<QPointF> points;
            points.append(QPointF(x1/width, ((int)y1)%(int)imageheight/(double)imageheight));
            points.append(QPointF(x2/width, ((int)y2)%(int)imageheight/(double)imageheight));
            lineannotation->setLinePoints(points);
            lineannotation->style().setWidth(4);
            lineannotation->setFlags(Poppler::Annotation::Hidden);
            document->document->page(index)->addAnnotation(lineannotation);
            annotations.append(new PaperAnnotation::LineAnnotation(index, lineannotation, width/scale, imageheight/scale, scale));
            annotations.at(annotations.length()-1)->scale = scale;
            this->addItem(annotations.at(annotations.length()-1));
            emit this->status_changed(true);
            emit this->draw_completed();
            changeIsDrawing("circle", false);
            connect(((PaperAnnotation::LineAnnotation *)(annotations.last())),
                    &PaperAnnotation::LineAnnotation::posChanged,
                    this, &MainScene::send_status_changed);
            // 添加删除接口
            connect((PaperAnnotation::LineAnnotation *)(annotations.last()), &PaperAnnotation::LineAnnotation::deleteSelf,
                    this, &MainScene::removeCertainItem);
            this->removeItem(tmplineitem);
            break;
        }
        case CIRCLE:
        {
            qDebug() << "scale is" << scale;
            int index = (int)(tmpellipseitem->pos().y()/imageheight);
            Poppler::GeomAnnotation *geomannotation = new Poppler::GeomAnnotation();
            geomannotation->setGeomType(Poppler::GeomAnnotation::InscribedCircle);
            QRectF boundary = QRectF(tmpellipseitem->pos().x()/width, (((int)(tmpellipseitem->pos().y()))%(int)imageheight)/(double)imageheight,
                                     tmpellipseitem->boundingRect().width()/width, tmpellipseitem->boundingRect().height()/imageheight);
            geomannotation->setBoundary(boundary);
            Poppler::Annotation::Style style = Poppler::Annotation::Style(geomannotation->style());
            style.setWidth(2);
            geomannotation->setStyle(style);
            QLinkedList<QPointF> points;
            points.append(QPointF(startPoint.x()/scale/width, ((int)startPoint.y())%(int)imageheight/scale/imageheight));
            points.append(QPointF(endPoint.x()/scale/width, ((int)endPoint.y())%(int)imageheight/scale/imageheight));
            geomannotation->style().setWidth(4);
            geomannotation->setFlags(Poppler::Annotation::Hidden);
            document->document->page(index)->addAnnotation(geomannotation);
            annotations.append(new PaperAnnotation::GeomAnnotation(index, geomannotation, width/scale, imageheight/scale, scale));
            this->addItem(annotations.at(annotations.length()-1));
            emit this->status_changed(true);
            emit this->draw_completed();
            changeIsDrawing("circle", false);
            connect(((PaperAnnotation::GeomAnnotation *)(annotations.last())),
                    &PaperAnnotation::GeomAnnotation::posChanged,
                    this, &MainScene::send_status_changed);
            // 添加删除接口
            connect((PaperAnnotation::GeomAnnotation *)(annotations.last()), &PaperAnnotation::GeomAnnotation::deleteSelf,
                    this, &MainScene::removeCertainItem);
            this->removeItem(tmpellipseitem);
            break;
        }
        case RECTANGLE:
        {
            int index = (int)(tmprectitem->pos().y()/imageheight);
            Poppler::GeomAnnotation *geomannotation = new Poppler::GeomAnnotation();
            geomannotation->setGeomType(Poppler::GeomAnnotation::InscribedSquare);
            QRectF boundary = QRectF(tmprectitem->pos().x()/width, (((int)(tmprectitem->pos().y()))%(int)imageheight)/(double)imageheight,
                                     tmprectitem->boundingRect().width()/width, tmprectitem->boundingRect().height()/imageheight);
            geomannotation->setBoundary(boundary);
            Poppler::Annotation::Style style = Poppler::Annotation::Style(geomannotation->style());
            style.setWidth(2);
            geomannotation->setStyle(style);
            QLinkedList<QPointF> points;
            points.append(QPointF(startPoint.x()/scale/width, ((int)startPoint.y())%(int)imageheight/scale/imageheight));
            points.append(QPointF(endPoint.x()/scale/width, ((int)endPoint.y())%(int)imageheight/scale/imageheight));
            geomannotation->style().setWidth(4);
            geomannotation->setFlags(Poppler::Annotation::Hidden);
            document->document->page(index)->addAnnotation(geomannotation);
            annotations.append(new PaperAnnotation::GeomAnnotation(index, geomannotation, width/scale, imageheight/scale, scale));
            this->addItem(annotations.at(annotations.length()-1));
            annotations.at(annotations.length()-1)->scale = scale;
            emit this->status_changed(true);
            emit this->draw_completed();
            changeIsDrawing("circle", false);
            connect(((PaperAnnotation::GeomAnnotation *)(annotations.last())),
                    &PaperAnnotation::GeomAnnotation::posChanged,
                    this, &MainScene::send_status_changed);
            // 添加删除接口
            connect((PaperAnnotation::GeomAnnotation *)(annotations.last()), &PaperAnnotation::GeomAnnotation::deleteSelf,
                    this, &MainScene::removeCertainItem);
            this->removeItem(tmprectitem);
            break;
        }
        default:
        {

        }
        }
    }
    else
    {
        QGraphicsScene::mouseReleaseEvent(event);
    }
}

void MainScene::changeIsDrawing(const QString &text, bool isDrawing)
{
    qDebug() << "changejici";
    if(text==tr("箭头"))
    {
        shape = LINE;
    }
    else if(text==tr("箭头"))
    {
        shape = ARROW;
    }
    else if(text==tr("圆形"))
    {
        shape = CIRCLE;
    }
    else if(text==tr("方形"))
    {
        shape = RECTANGLE;
    }
    else if(text==tr("文字"))
    {
        shape = FLATTEXT;
    }
    else if(text==tr("注释"))
    {
        shape = POPUPTEXT;
    }
    else
    {
        shape = LINE;
    }
    this->isDrawing = isDrawing;
    if(isDrawing)
    {
        for(int i=0; i<pages.count();i++)
        {
            pages.at(i)->setCursor(Qt::CrossCursor);
        }
    }
    else
    {
        for(int i=0; i<pages.count();i++)
        {
            pages.at(i)->setCursor(Qt::ArrowCursor);
        }
    }
}

void MainScene::setCurrentShape(int i)
{
    shape = i;
}

void MainScene::newFlatText(const QString &text, QFont font, QColor color)
{
    int imageheight = height/document->document->numPages();
    int index = (int)(startPoint.y()/imageheight);
    Poppler::TextAnnotation *annotation = new Poppler::TextAnnotation(Poppler::TextAnnotation::InPlace);
    annotation->setTextColor(color);
    annotation->setContents(text);
    annotation->setTextFont(font);
    int totallength = annotation->textFont().pointSize()*annotation->contents().length();
    int hdistance = scale*width-startPoint.x();
    int vdistance = (index+1)*scale*imageheight-startPoint.y();
    annotation->setBoundary(QRectF(startPoint.x()/scale/width, (((int)(startPoint.y()))%(int)imageheight)/scale/imageheight,
                                   (totallength/hdistance>1?hdistance:totallength)/scale/width,
                                   ((totallength/hdistance+1)*annotation->textFont().pointSize()+20>vdistance?vdistance:(totallength/hdistance+1)*scale*annotation->textFont().pointSize()+20)/scale/imageheight));
    annotation->setFlags(Poppler::Annotation::Hidden);
    document->document->page(index)->addAnnotation(annotation);
    annotations.append(new PaperAnnotation::FlatTextAnnotation(index, annotation, width/scale, imageheight/scale, scale));
    this->addItem(annotations.at(annotations.length()-1));
    emit this->draw_completed();
    emit this->status_changed(true);
    changeIsDrawing("circle", false);
    connect(((PaperAnnotation::FlatTextAnnotation *)(annotations.last())),
            &PaperAnnotation::FlatTextAnnotation::posChanged,
            this, &MainScene::send_status_changed);
    // 添加删除接口
    connect((PaperAnnotation::FlatTextAnnotation *)(annotations.last()), &PaperAnnotation::FlatTextAnnotation::deleteSelf,
            this, &MainScene::removeCertainItem);
}

void MainScene::newPopupText(const QString &text, QFont font, QColor color)
{
    int imageheight = height/document->document->numPages();
    int index = (int)(startPoint.y()/imageheight);
    Poppler::TextAnnotation *annotation = new Poppler::TextAnnotation(Poppler::TextAnnotation::Linked);
    annotation->setTextColor(color);
    annotation->setContents(text);
    annotation->setTextFont(font);
    int totallength = annotation->textFont().pointSize()*annotation->contents().length();
    int hdistance = scale*width-startPoint.x();
    int vdistance = (index+1)*scale*imageheight-startPoint.y();
    annotation->setBoundary(QRectF(startPoint.x()/width, (((int)(startPoint.y()))%(int)imageheight)/(double)imageheight,
                                   30/(double)width*scale,
                                   30/(double)imageheight*scale));
    document->document->page(index)->addAnnotation(annotation);
    annotations.append(new PaperAnnotation::PopupTextAnnotation(index, annotation, width/scale, imageheight/scale, scale));
    this->addItem(annotations.at(annotations.length()-1));
    annotation->setFlags(Poppler::Annotation::Hidden);
    emit this->draw_completed();
    emit this->status_changed(true);
    changeIsDrawing("circle", false);
    connect(((PaperAnnotation::PopupTextAnnotation *)(annotations.last())),
            &PaperAnnotation::PopupTextAnnotation::posChanged,
            this, &MainScene::send_status_changed);
    // 添加删除接口
    connect((PaperAnnotation::PopupTextAnnotation *)(annotations.last()), &PaperAnnotation::PopupTextAnnotation::deleteSelf,
            this, &MainScene::removeCertainItem);
}

void MainScene::updateItem(int index)
{

}

void MainScene::updateScene()
{
    update();
}

void MainScene::removeCertainItem()
{
    qDebug() << "成功删除该项目";
    this->removeItem((PaperAnnotation::Annotation *)sender());
    for(int i=0; i<this->annotations.length();i++)
    {
        if(this->annotations.at(i)==sender())
        {
            this->annotations.remove(i);
        }
    }

    document->document->page(((PaperAnnotation::Annotation *)sender())->index)->removeAnnotation(((PaperAnnotation::Annotation *)sender())->return_annotation());

    delete sender();

    emit this->status_changed(true);
}

void MainScene::savePDF()
{
    //    regenerate_annotations();

    QString tmpfilename = "tmp_file.pdf";
    Poppler::PDFConverter *converter = document->document->pdfConverter();
    converter->setOutputFileName(tmpfilename);
    converter->setPDFOptions(Poppler::PDFConverter::WithChanges);
    bool conv_flag = converter->convert();
    if(conv_flag)
    {
        QFile::remove(this->filename);
        bool rename_flag = QFile::rename(tmpfilename, this->filename);
        if(rename_flag)
        {
            emit this->status_changed(false);
        }
        else
        {

        }
    }
    else
    {
    }
}

void MainScene::savePDFas()
{
    QString new_filename = QFileDialog::getSaveFileName(nullptr,
                                                        tr("请选择保存地址"),
                                                        "",
                                                        tr("PDF Files (*.pdf)"));
    this->filename = new_filename;

    regenerate_annotations();

    QString tmpfilename = "tmp_file.pdf";
    Poppler::PDFConverter *converter = document->document->pdfConverter();
    converter->setOutputFileName(tmpfilename);
    converter->setPDFOptions(Poppler::PDFConverter::WithChanges);
    bool conv_flag = converter->convert();
    if(conv_flag)
    {
        if(QFile::exists(new_filename))
        {
            QFile::remove(new_filename);
        }
        bool rename_flag = QFile::rename(tmpfilename, new_filename);
        if(rename_flag)
        {
            emit this->status_changed(false);
        }
        else
        {

        }
    }
    else
    {
    }
}

void MainScene::regenerate_annotations()
{
    for(int i=0; i<this->annotations.length(); i++)
    {
        document->document->page(annotations.at(i)->index)->addAnnotation(annotations.at(i)->return_annotation());
    }
    for(int i=0; i<document->document->numPages(); i++)
    {

    }
}

void MainScene::send_status_changed()
{
    emit this->status_changed(true);
}

void MainScene::printPDF()
{
    if(this->modified_status)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(nullptr, tr("提示"), tr("您的文档尚未保存，是否保存？"),
                                      QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Cancel);

        if(reply==QMessageBox::Yes)  // 文件保存，继续打开
        {
            emit this->status_changed(false);
        }
        else if(reply==QMessageBox::No)  // 文件未保存，继续打开
        {
            return;
        }
        else  // 操作取消
        {
            return;
        }
    }

    QDesktopWidget *mydesk = QApplication::desktop();
    int new_xres = mydesk->physicalDpiX();
    int new_yres = mydesk->physicalDpiY();

    QPrinter printer;
    QPrintDialog printDialog(&printer, nullptr);
    if (printDialog.exec()) {

        QProgressDialog *progressDlg=new QProgressDialog(nullptr);
        progressDlg->setWindowModality(Qt::WindowModal);
        progressDlg->setMinimumDuration(5);
        progressDlg->setWindowTitle(tr("打印进度"));
        progressDlg->setLabelText(tr("正在打印"));
        progressDlg->setCancelButtonText(tr("取消"));

        QPainter painter(&printer);

        int startPage, endPage;
        if(printer.printRange()==QPrinter::AllPages)
        {
            startPage = 0;
            endPage = document->document->numPages();
        }
        else if(printer.printRange()==QPrinter::PageRange)
        {
            startPage = printer.fromPage();
            endPage = printer.toPage()>=document->document->numPages()?
                        document->document->numPages():
                        printer.toPage()+1;
        }
        else
        {
            startPage = 0;
            endPage = document->document->numPages();
        }

        progressDlg->setRange(0, printer.copyCount()*(endPage-startPage));
        progressDlg->setValue(1);

        for(int idx=0; idx<printer.copyCount(); idx++)
        {
            for(int i=startPage;i<endPage;i++)
            {
                QImage *image = new QImage(document->document->page(i)->renderToImage(new_xres, new_yres, -1, -1, -1, -1));
                painter.drawImage(0, 0, *image);
                if(idx!=printer.copyCount()-1 || i!=(endPage-startPage-1))
                {
                    printer.newPage();
                }
                progressDlg->setValue(idx*(endPage-startPage)+i-startPage);
                if(progressDlg->wasCanceled())
                    return;
            }
        }
        progressDlg->close();
        painter.end();
    }
}

void MainScene::changePreview()
{
    this->isInPreview = !(this->isInPreview);
    qDebug() << "成功进入一次";
    if(this->isInPreview)
    {
        for(int i=annotations.length()-1; i>=0; i--)
        {
            if(QString(typeid(*(annotations.at(i))).name()).contains("Popup"))
            {

                annotations.append(new PaperAnnotation::PreviewAnnotation(
                                       annotations.at(i)->index,
                                       (Poppler::TextAnnotation *)(annotations.at(i)->return_annotation()),
                                       annotations.at(i)->width,
                                       annotations.at(i)->height,
                                       annotations.at(i)->scale));
                this->addItem(annotations.last());
                connect((PaperAnnotation::PreviewAnnotation *)(annotations.last()), &PaperAnnotation::PreviewAnnotation::needRefresh,
                        this, &MainScene::updateScene);
                this->removeItem(annotations.at(i));
                this->annotations.remove(i);
            }
        }
    }
    else
    {
        for(int i=annotations.length()-1; i>=0; i--)
        {
            if(QString(typeid(*(annotations.at(i))).name()).contains("Preview"))
            {
                annotations.append(new PaperAnnotation::PopupTextAnnotation(
                                       annotations.at(i)->index,
                                       (Poppler::TextAnnotation *)(annotations.at(i)->return_annotation()),
                                       annotations.at(i)->width,
                                       annotations.at(i)->height,
                                       annotations.at(i)->scale));
                this->addItem(annotations.last());
                this->removeItem(annotations.at(i));
                this->annotations.remove(i);
            }
        }
    }
}

MainFrame::MainFrame()
{
    graphicsview = new GraphicsView(this);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(graphicsview);
    setLayout(layout);

    graphicsview->setDragMode(QGraphicsView::ScrollHandDrag);
    graphicsview->setDragMode(QGraphicsView::RubberBandDrag);
    graphicsview->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    graphicsview->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

GraphicsView *MainFrame::view() const
{
    return graphicsview;
}

SideScene::SideScene()
{

}

GraphicsView::GraphicsView(MainFrame *frame) : QGraphicsView(), frame(frame)
{
    refreshtimer = new QTimer();
    this->setStyleSheet("background-color:gray;");
    this->setWindowFlags(Qt::FramelessWindowHint);
}

void GraphicsView::setScene(MainScene *scene)
{
    mainscene = scene;
    QGraphicsView::setScene(scene);
}

void GraphicsView::wheelEvent(QWheelEvent *ev)
{
    if(ev->modifiers() & Qt::ControlModifier)
    {
        if(ev->delta()>0 & scalefactor>2)
        {
            ev->ignore();
            return;
        }
        else if(ev->delta()<0 & scalefactor<0.5)
        {
            ev->ignore();
            return;
        }
        else
        {
            mainscene->document->indexes.clear();
            if(!refreshtimer->isActive())
            {
                oldCenterPoint = mapToScene(QPoint(width()/2, height()/2));
                currentScenePoint = mapToScene(ev->pos());
                lengthPoint = oldCenterPoint-currentScenePoint;
                mainscene->currentpage = (int)(currentScenePoint.y()/height()/scalefactor);
            }

            if(ev->delta()>0)
            {
                refreshtimer->stop();
                point = mapToScene(QPoint(this->x()*1.2+this->width()/2*1.2, this->y()*1.2+this->height()/2*1.2));
                scale(1.2, 1.2);
                scalefactor*=1.2;
                mainscene->scale = scalefactor;
                centerPoint = currentScenePoint+lengthPoint/scalefactor*oldscalefactor;
                centerOn(centerPoint);
                refreshtimer->start(500);
            }
            else
            {
                refreshtimer->stop();
                point = mapToScene(QPoint(this->x()/1.2+this->width()/2/1.2, this->y()/1.2+this->height()/2/1.2));
                scale(1/1.2, 1/1.2);
                scalefactor*=1/1.2;
                mainscene->scale = scalefactor;
                QPointF lengthPoint = oldCenterPoint-currentScenePoint;
                centerPoint = currentScenePoint+lengthPoint/scalefactor*oldscalefactor;
                centerOn(centerPoint);
                refreshtimer->start(500);
            }
            ev->accept();
        }
    }
    else
    {
        QGraphicsView::wheelEvent(ev);
    }
}

void GraphicsView::updateSize()
{
    refreshtimer->stop();
    mainscene->document->scale = scalefactor;
    mainscene->scale = scalefactor;

    //    for(int i=0; i<mainscene->annotations.length();i++)
    //    {
    //        mainscene->removeItem(mainscene->annotations.at(i));
    //        delete mainscene->annotations.at(i);
    //    }
    //    mainscene->annotations.clear();
    //    mainscene->annotations.clear();
    mainscene->document->indexes.clear();

    mainscene->document->timer->stop();

    for(int i=0; i<mainscene->annotations.length(); i++)
    {
        mainscene->annotations.at(i)->scale = mainscene->scale;
    }

    int idx = mainscene->currentpage;
    QImage *image1 = new QImage(mainscene->document->document->page(idx)->renderToImage(mainscene->xres*mainscene->scale, mainscene->yres*mainscene->scale, -1, -1, -1, -1));
    QImage *tmpimage = mainscene->document->images[idx];
    delete tmpimage;
    mainscene->document->images[idx] = image1;

    mainscene->width = image1->width();
    qDebug() << "width is" << mainscene->width;
    mainscene->height = image1->height()*mainscene->document->document->numPages();

    setSceneRect(-200, 0, mainscene->width+400, mainscene->height);

    update();
    mainscene->update();
    mainscene->pages.at(idx)->update();

    QMatrix matrix;
    matrix.scale(1, 1);
    matrix.rotate(0);
    setMatrix(matrix);
    centerPoint = currentScenePoint*scalefactor/oldscalefactor+lengthPoint;
    centerOn(centerPoint.x(), centerPoint.y());

    oldscalefactor = scalefactor;

    for(int i=0; i<mainscene->document->document->numPages(); i++)
    {
        mainscene->pages.at(i)->setPos(0, image1->height()*i+SPACING*i);
        if(i!=idx)
        {
            mainscene->document->indexes.append(i);
        }
    }

    for(int i=0; i<this->mainscene->annotations.length(); i++)
    {
        this->mainscene->annotations.at(i)->width = mainscene->width/scalefactor;
        this->mainscene->annotations.at(i)->height = mainscene->height/mainscene->document->document->numPages()/scalefactor;
        this->mainscene->annotations.at(i)->scale = scalefactor;
        this->mainscene->annotations.at(i)->resetpos();
        this->mainscene->annotations.at(i)->update();
    }

    for(int i=0; i<this->items().length(); i++)
    {
        if(QString(typeid(*(this->items().at(i))).name()).contains("Annotation"))
        {
            ((PaperAnnotation::Annotation *)(this->items().at(i)))->width = mainscene->width/scalefactor;
            ((PaperAnnotation::Annotation *)(this->items().at(i)))->height = mainscene->height/mainscene->document->document->numPages()/scalefactor;
            ((PaperAnnotation::Annotation *)(this->items().at(i)))->scale = scalefactor;
            if((QString(typeid(*(this->items().at(i))).name()).contains("Preview")))
            {
                ((PaperAnnotation::PreviewAnnotation *)(this->items().at(i)))->resetpos1();
            }
            else
            {
                ((PaperAnnotation::Annotation *)(this->items().at(i)))->resetpos();
            }
            ((PaperAnnotation::Annotation *)(this->items().at(i)))->update();
        }
    }

    connect(this, &GraphicsView::timerStarting, mainscene->document, &AutoDocument::updateImages);
    emit timerStarting();
    connect(this->mainscene->document, &AutoDocument::imageCompleted, this->mainscene, &MainScene::updateScene);
}

void GraphicsView::timeStopped()
{
    refreshtimer->stop();
}

QRectF PaperItem::boundingRect() const
{
    QImage **addr = (QImage **)(images + index*size);
    return QRectF(0, 0, (*addr)->width(), (*addr)->height());
}

void PaperItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QImage **addr = (QImage **)(images + index*size);
    painter->drawImage(0, 0, **addr);
}
