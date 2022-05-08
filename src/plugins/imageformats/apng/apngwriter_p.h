#ifndef APNGWRITER_P_H
#define APNGWRITER_P_H

#include <QObject>
#include <QIODevice>
#include <png.h>
#include "apngtypes_p.h"

class ApngWriter : public QObject
{
    Q_OBJECT
public:
    explicit ApngWriter(QObject *parent = nullptr);
    ~ApngWriter() override;

    bool init(QIODevice *device);
    bool addFrame(const QImage &);
    bool writeFrames();
private:
    QIODevice *_device = nullptr;

    png_structp _png = nullptr;
    png_infop _info = nullptr;

    QList<ApngFrame> _allFrames;

    static void write_fn(png_structp, png_bytep, size_t);
private slots:
    void deviceAboutToClose();
};

#endif // APNGWRITER_P_H
