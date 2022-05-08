#ifndef APNGTYPES_P_H
#define APNGTYPES_P_H

#include <QImage>
#include <png.h>

class ApngFrame : public QImage {
public:
    ApngFrame(const QImage &image = {}, quint16 delay_num = 0, quint16 delay_den = 10);

    double delay() const;
    int delayMsec() const;

private:
    double _delay;
};

struct Frame {
    quint32 x = 0;
    quint32 y = 0;
    quint32 width = 0;
    quint32 height = 0;
    quint32 channels = 0;

    quint16 delay_num = 0;
    quint16 delay_den = 1;
    quint8 dop = PNG_DISPOSE_OP_NONE;
    quint8 bop = PNG_BLEND_OP_SOURCE;

    quint64 rowbytes = 0;
    unsigned char * p = nullptr;
    png_bytepp rows = nullptr;
};

#endif // APNGTYPES_P_H
