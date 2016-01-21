#if !defined(__MY_MyWindow_H__)
#define __MY_MyWindow_H__ 1


#include <QWidget>

class QPushButton;


class MyWindow : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal area_ READ getArea WRITE setArea NOTIFY areaChanged)

public:
    explicit MyWindow(QWidget *parent = NULL);
    virtual ~MyWindow();

public:
    qreal getArea() const { return area_; }
    void setArea(qreal area);

signals:
    void areaChanged();  // automatically generated by moc.
    void quitButtonClicked();  // automatically generated by moc.

public slots:
    void handAreaChanged();

private slots:
    void handleQuitButtonClicked();

private:
    QPushButton *button_;
    qreal area_;
};


#endif  // __MY_MyWindow_H__

