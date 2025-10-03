#include "mainwindow.h"

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QPainter>
#include <QHBoxLayout>
#include <vector>
#include <set>
#include <algorithm>
#include <cmath>

struct Edge {
    int i, j;
    bool operator<(const Edge &other) const {
        if (i == other.i) return j < other.j;
        return i < other.i;
    }
};

static double cross(const QPointF &a, const QPointF &b, const QPointF &c) {
    return (b.x()-a.x())*(c.y()-a.y()) - (b.y()-a.y())*(c.x()-a.x());
}

class DrawCanvas : public QWidget {
    Q_OBJECT
public:
    std::vector<QPointF> points;
    std::set<Edge> edges;
    QString title;
    int iterationCount;

    DrawCanvas(QString t, QWidget *parent=nullptr) : QWidget(parent), title(t), iterationCount(0) {
        setFixedSize(400,600);
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.fillRect(rect(), QColor(245,245,245));

        QPen pen(QColor(20,120,200));
        pen.setWidth(2);
        p.setPen(pen);
        for (auto &e : edges) {
            p.drawLine(points[e.i], points[e.j]);
        }

        for (int i=0; i<(int)points.size(); ++i) {
            QRectF r(points[i].x()-4, points[i].y()-4, 8, 8);
            p.setBrush(Qt::white);
            p.setPen(Qt::black);
            p.drawEllipse(r);
            p.drawText(points[i].x()+7, points[i].y()-7, QString::number(i));
        }

        p.setPen(Qt::black);
        p.drawText(10,20,title);
        p.drawText(10,35, QString("Iterations: %1").arg(iterationCount));
    }
};

class HullWindow : public QMainWindow {
    Q_OBJECT
public:
    DrawCanvas *slowCanvas;
    DrawCanvas *fastCanvas;

    HullWindow() {
        slowCanvas = new DrawCanvas("Slow Hull (Brute Force)");
        fastCanvas = new DrawCanvas("Fast Hull (Monotone Chain)");

        std::vector<QPointF> pts = {
            {100,100},
            {214,150},
            {303,80},
            {350,251},
            {200,300},
            {150,250},
            {200, 360},
            {120, 336}
        };

        slowCanvas->points = pts;
        fastCanvas->points = pts;

        computeSlowHull();
        computeFastHull();

        QWidget *central = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout();
        layout->addWidget(slowCanvas);
        layout->addWidget(fastCanvas);
        central->setLayout(layout);

        setCentralWidget(central);
        resize(820,640);
        setWindowTitle("Convex Hull — Side by Side");
    }

private:
    void computeSlowHull() {
        int n = (int)slowCanvas->points.size();
        int& count = slowCanvas->iterationCount;
        count = 0;

        for (int i=0;i<n;i++) {
            for (int j=i+1;j<n;j++) {
                bool pos=false, neg=false;
                for (int k=0;k<n;k++) {
                    count++;
                    if (k==i||k==j) continue;
                    double s = cross(slowCanvas->points[i], slowCanvas->points[j], slowCanvas->points[k]);
                    if (s>1e-9) pos=true;
                    else if (s<-1e-9) neg=true;
                    if (pos && neg) break;
                }
                if (!(pos && neg)) {
                    Edge e{std::min(i,j),std::max(i,j)};
                    slowCanvas->edges.insert(e);
                }
            }
        }
    }

    void computeFastHull() {
        int n = (int)fastCanvas->points.size();
        int& count = fastCanvas->iterationCount;
        count = 0;

        std::vector<int> idx(n);
        for (int i=0;i<n;i++) idx[i]=i;

        std::sort(idx.begin(), idx.end(), [&](int a,int b){
            if (fastCanvas->points[a].x()==fastCanvas->points[b].x())
                return fastCanvas->points[a].y()<fastCanvas->points[b].y();
            return fastCanvas->points[a].x()<fastCanvas->points[b].x();
        });

        std::vector<int> hull;
        for (int id: idx) {
            while (hull.size()>=2 && cross(
                                           fastCanvas->points[hull[hull.size()-2]],
                                           fastCanvas->points[hull[hull.size()-1]],
                                           fastCanvas->points[id]) <= 0) {
                count++;
                hull.pop_back();
            }
            if (hull.size()>=2) count++;
            hull.push_back(id);
        }

        int t = hull.size()+1;
        for (int i=n-2;i>=0;i--) {
            int id = idx[i];
            while (hull.size()>=t && cross(
                                           fastCanvas->points[hull[hull.size()-2]],
                                           fastCanvas->points[hull[hull.size()-1]],
                                           fastCanvas->points[id]) <= 0) {
                count++;
                hull.pop_back();
            }
            if (hull.size()>=t) count++;
            hull.push_back(id);
        }
        hull.pop_back();
        for (size_t i=0;i<hull.size();i++) {
            int a=hull[i], b=hull[(i+1)%hull.size()];
            Edge e{std::min(a,b), std::max(a,b)};
            fastCanvas->edges.insert(e);
        }
    }
};

#include "main.moc"

int main(int argc, char **argv) {
    QApplication app(argc,argv);
    HullWindow w;
    w.show();
    return app.exec();
}

