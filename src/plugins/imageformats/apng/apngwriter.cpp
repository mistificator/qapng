#include "apngwriter_p.h"
#include <QDebug>

ApngWriter::ApngWriter(QObject *parent) : QObject(parent)
{

}

ApngWriter::~ApngWriter()
{
    if(!_png)
    {
        return;
    }

    png_destroy_write_struct(&_png, &_info);
}

bool ApngWriter::addFrame(const QImage &img)
{
    if (!_allFrames.isEmpty())
    {
        if (_allFrames.last().size() != img.size() || _allFrames.last().format() != img.format())
        {
            return false;
        }
    }
    _allFrames << img;
    return true;
}

bool ApngWriter::init(QIODevice *device)
{
    if (_device == device) {
        return true;
    }

    _device = device;
    connect(_device, SIGNAL(aboutToClose()), this, SLOT(deviceAboutToClose()));

    //init png structs
    _png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if(!_png) {
        qCritical() << "failed to create png struct";
        return false;
    }

    _info = png_create_info_struct(_png);
    if(!_info) {
        qCritical() << "failed to create info struct";
        return false;
    }

    png_set_write_fn(_png, this, &ApngWriter::write_fn, nullptr);

    //set png jump position
    if (setjmp(png_jmpbuf(_png))) {
        qCritical() << "failed to create set callbacks";
        return false;
    }

    return true;
}

bool ApngWriter::writeFrames()
{
    if(!_png)
    {
        return false;
    }

    int bit_depth = 32;
    int color_type = PNG_COLOR_TYPE_RGB;
    switch (_allFrames[0].format())
    {
    case QImage::Format_RGB32:
        bit_depth = 32;
        color_type = PNG_COLOR_TYPE_RGB;
        break;
    case QImage::Format_ARGB32:
        bit_depth = 32;
        color_type = PNG_COLOR_TYPE_RGBA;
        break;
    case QImage::Format_Indexed8:
        bit_depth = 8;
        color_type = PNG_COLOR_TYPE_PALETTE;
        break;
    default:
        break;
    }
    png_set_IHDR(_png, _info, _allFrames[0].width(), _allFrames[0].height(),
                         bit_depth, color_type, PNG_INTERLACE_NONE,
                         PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    if (setjmp(png_jmpbuf(_png))) {
        qCritical() << "failed to set IHDR";
        return false;
    }

    if (_allFrames[0].colorCount() > 0)
    {
        QVector<png_color_struct> palette(_allFrames[0].colorCount());
        for (int i = 0; i < palette.count(); i++)
        {
            auto color = _allFrames[0].colorTable()[i];
            palette[i].red = qRed(color);
            palette[i].green = qGreen(color);
            palette[i].blue = qBlue(color);
        }
        png_set_PLTE(_png, _info, palette.constData(), _allFrames[0].colorCount());

        if (setjmp(png_jmpbuf(_png))) {
            qCritical() << "failed to set PLTE";
            return false;
        }
    }
    png_set_acTL(_png, _info, _allFrames.count(), 0);
    if (setjmp(png_jmpbuf(_png))) {
        qCritical() << "failed to set acTL";
        return false;
    }

    png_write_info(_png, _info);
    if (setjmp(png_jmpbuf(_png))) {
        qCritical() << "failed to write info";
        return false;
    }

    for (int i = 0; i < _allFrames.count(); i++)
    {
        QVector<png_bytep> rows(_allFrames[i].height());
        for (int r = 0; r < rows.count(); r++)
        {
            rows[r] = _allFrames[i].bits() + r * _allFrames[i].bytesPerLine();
        }
        png_write_frame_head(_png, _info, rows.data(),
                                        _allFrames[i].width(),  /* width */
                                        _allFrames[i].height(), /* height */
                                        0,       /* x offset */
                                        0,       /* y offset */
                                        1, 5,    /* delay numerator and denominator */
                                        PNG_DISPOSE_OP_NONE, /* dispose */
                                        PNG_BLEND_OP_SOURCE    /* blend */
                                       );

        png_write_image(_png, rows.data());
        png_write_frame_tail(_png, _info);

        if (setjmp(png_jmpbuf(_png))) {
            qCritical() << "failed to write frame";
            return false;
        }
    }

    png_write_end(_png, nullptr);
    png_destroy_write_struct(&_png, &_info);
    _png = nullptr;

    return true;
}

void ApngWriter::write_fn(png_structp png_ptr, png_bytep bytes_ptr, size_t sz)
{
    auto writer = reinterpret_cast<ApngWriter*>(png_get_io_ptr(png_ptr));
    writer->_device->write(reinterpret_cast<char *>(bytes_ptr), static_cast<qint64>(sz));
}

void ApngWriter::deviceAboutToClose()
{
    if (auto _device = dynamic_cast<QIODevice *>(sender()))
    {
        writeFrames();

        _device->disconnect(this);
        _device->close();
    }
}
