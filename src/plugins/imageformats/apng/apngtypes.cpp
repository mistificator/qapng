#include "apngtypes_p.h"

ApngFrame::ApngFrame(const QImage &image, quint16 delay_num, quint16 delay_den) :
    QImage{image},
    _delay{static_cast<double>(delay_num) / static_cast<double>(delay_den)}
{}

double ApngFrame::delay() const
{
    return _delay;
}

int ApngFrame::delayMsec() const
{
    return qRound(_delay * 1000);
}
